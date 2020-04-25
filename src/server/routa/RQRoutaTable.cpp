#include "server/routa/RQRoutaTable.h"
#include "RQDBStack.h"
#include "RQSpinLock.h"
#include "RecycleBin.h"
#include "RQLoadNode.h"
#include "server/routa/RQRoutaRecycle.h"
#include "RQSys.h"
#include "RQSpinLock.h"
#include <iostream>

RQRoutaTable::RQRoutaTable()
{
	_saveList = new RQList(new RQSpinLock);
	_table_type = new RQMap<unsigned char, std::pair<RQList*, RQSpinLock>>(new RQSpinLock);
	_table_addr = new RQMap<int, RQMap <short, RQLoadNode*>>(new RQSpinLock);
}

RQRoutaTable::~RQRoutaTable()
{
	_ClearInAddrTable();
	_ClearInTypeTable();
	_ClearInList();

	delete _saveList->getLock();
	delete _saveList;

	delete _table_type;
	delete _table_addr;
}

RQRoutaNode * RQRoutaTable::GetBy(int ip, short port)
{
	RQRoutaNode* retNode = 0;
	auto iter_port = _table_addr->Find(ip);
	if (iter_port != _table_addr->end())
	{
		auto iter = iter_port->second.Find(port);
		if (iter != iter_port->second.end())
		{
			retNode = (RQRoutaNode*)iter->second->_obj;
		}
	}
	return retNode;
}

RQRoutaNode * RQRoutaTable::GetBy(char type)
{
	RQRoutaNode* retNode = 0;
	auto iter = _table_type->Find(type);				///查找type类型的网络节点集
	if (iter != _table_type->end())						///若该类型不存在于类型映射表中则不创建该类型集合
	{
		RQLoadNode* loadNode = (RQLoadNode*) iter->second.first->LoopVisit();		///拜访下一个该类型的路由节点
		if (loadNode)				///检查该类型是否还存有路由节点
		{
			retNode = (RQRoutaNode*) loadNode->_obj;	///有，则从挂载节点上获取路由节点信息
		}
	}
	return retNode;
}

RQRoutaNode * RQRoutaTable::CreateBy(int ip, short port)
{
	RQRoutaNode* retNode = GetBy(ip, port);
	if (!retNode)
	{
		retNode = RQRoutaRecycle().GetFreeRQRoutaNode();		///从回收站中获取一个路由节点
		retNode->_routaData.Set(ip, port, 0);
		(*_table_addr)[ip][port]= _SaveInList(retNode);			///将该节点存入路由表中，并创建网络地址映射表
	}
	return retNode;
}

void RQRoutaTable::SaveInTable(RQRoutaNode * routaNode)
{
	if (routaNode
		&& routaNode->_saveContainer == _saveList)				///检测该节点是否由节点本路由表对象创建
	{
		RQLoadNode* loadNode = routaNode->_loadNode;
		///---为该类型创建容器，防止空容器访问
		std::pair<RQList*, RQSpinLock>& pair = (*_table_type)[routaNode->_routaData._type];	///获取该类型的存储容器
		{
			RQAutoLock lock(&pair.second);
			if (!pair.first)
				pair.first = new RQList(new RQSpinLock);
		}

		if (pair.first != loadNode->_saveContainer)				///检查挂载节点是否已在本容器中，若在则不用存储
		{
			RQList* list = (RQList*)loadNode->_saveContainer;	///获取目标节点的源挂载存储容器
			if (list)
			{
				list->Remove(loadNode);							///将挂载节点从该类型中移除
			}
			pair.first->push_back(loadNode);					///将节点进行存储
		}
	}

}

void RQRoutaTable::Remove(int ip, short port)
{
	RQRoutaNode* node = GetBy(ip, port);
	Remove(node);
}

void RQRoutaTable::Remove(char type)
{
	RQRoutaNode* node = GetBy(type);
	Remove(node);
}

void RQRoutaTable::Remove(RQRoutaNode * routaNode)
{
	if (IsAttach(routaNode))
	{
		_RemoveInAddrTable(routaNode->_routaData._addr._ip,
			routaNode->_routaData._addr._port);					///将期望被移除的路由节点从地址映射表中移除
		_RemoveInTypeTable(routaNode);							///将期望被移除的路由节点从类型映射表中移除
		_RemoveInList(routaNode);								///将期望被移除的路由节点从存储容器中移除
	}
}

RQRoutaNode * RQRoutaTable::GetNext(RQRoutaNode * currentNode)
{
	return (RQRoutaNode*) _saveList->GetNext(currentNode);
}

bool RQRoutaTable::IsAttach(RQRoutaNode * node)
{
	return node && node->_saveContainer == _saveList;
}

void RQRoutaTable::Show()
{
	std::cout << "--------------------" << std::endl;
	RQNode* node = _saveList->GetNext();
	while (node)
	{
		_Show((RQRoutaNode*)node);
		node = _saveList->GetNext(node);
	}
	std::cout << std::endl;
}

void RQRoutaTable::_RemoveInTypeTable(RQRoutaNode* node)
{
	RQLoadNode* loadNode = node->_loadNode;					///获取存节点的挂载节点
	RQList* list = (RQList*)loadNode->_saveContainer;		///获取挂载节点所被存储的容器
	list->Remove(loadNode);									///将挂载节点从存储容器中移除
}

void RQRoutaTable::_ClearInTypeTable()
{
	_table_type->Clear();
}

void RQRoutaTable::_RemoveInAddrTable(int ip, short port)
{
	auto iter_portTable = _table_addr->Find(ip);		///根据ip在地址表中获取到端口表
	if (iter_portTable != _table_addr->end())
	{
		auto iter = iter_portTable->second.Find(port);
		iter_portTable->second.Remove(iter);
	}
}

void RQRoutaTable::_ClearInAddrTable()
{
	for (auto iter = _table_addr->begin();
		iter != _table_addr->end();
		iter++)
	{
		iter->second.Clear();
	}
}

void RQRoutaTable::_RemoveInList(RQRoutaNode * node)
{
	RecycleBin::RecycleNode(node->_loadNode);		///回收类型挂载节点
	_saveList->Remove(node);						///将路由节点从存储链表中移除

	RQRoutaRecycle().RecycleRQRoutaNode(node);			///回收路由节点
}

void RQRoutaTable::_ClearInList()
{
	RQRoutaNode* node = (RQRoutaNode*) _saveList->pop_back();
	while (node) {
		_RemoveInList(node);
		node = (RQRoutaNode*)_saveList->pop_back();
	}
}

RQLoadNode * RQRoutaTable::_SaveInList(RQRoutaNode * node)
{
	RQLoadNode* loadNode = RecycleBin().GetFreeRQLoadNode();			///为新节点所要一个挂载节点
	///将挂载节点与存储节点相互绑定
	loadNode->_obj = node;
	node->_loadNode = loadNode;

	_saveList->push_back(node);											///将存储节点存入存储容器中

	return loadNode;
}

void RQRoutaTable::_Show(RQRoutaNode * node)
{
	char buff[16];
	std::cout << "| " << Inet_addr(node->_routaData._addr._ip, buff, 16)
		<< " | " << RQNtoh(node->_routaData._addr._port)
		<< " | " << (int)node->_routaData._type << std::endl;
}
