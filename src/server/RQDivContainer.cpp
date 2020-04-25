#include "server/RQDivContainer.h"
#include "node/RQDivNode.h"
#include "define/RQdefine.h"
#include "RQList.h"


 RQMap<int, RQDivContainer::Three> RQDivContainer::_typeDivMap;
 RQMap<int, RQDivNode*> RQDivContainer::_sockDivMap;


bool RQDivContainer::Insert(RQDivNode * divNode)
{
	divNode->_isRegister = true;		///设备节点由于可以进入管理器，所以不再允许外部对设备节点进行修改。
	divNode->_divStatus = M_DIV_STATUS_FREE;				///标记任务处于任务空闲状态等待任务处理

	bool retBol = false;
	RQDivNode* saveDiv = _InsertInTypeMap(divNode);			///将设备存入设备类型表中
	if (saveDiv == divNode)				///检查欲添加设备是否存储成功（若不同则标识欲添加设备是重复id设备）
	{
		_InsertInSockMap(divNode);		///将设备存入sock映射表中
		retBol = true;
	}
	return retBol;
}

void RQDivContainer::Remove(RQDivNode * divNode)
{
	_RemoveFromTypeMap(divNode);				///将该设备从设备存储容器中移除
	_RemoveFromSockMap(divNode);				///将该设备从sock映射表中移除
	divNode->_isRegister = false;			///释放节点，终止管理器对设备的管理
}

RQDivNode * RQDivContainer::GetDivByTypeId(int divType, int divId)
{
	RQDivNode* retDiv = 0;
	Three& third = _typeDivMap[divType];
	if (third._divFreeList)						///查看该类型是否存在设备存入容器，若不存在则表示没有改类型设备
	{
		RQDivNode* divNode = _GetDivById(third, divId);		///针对输入的id查找对应的任务节点
		if (divNode && M_DIV_STATUS_FREE == divNode->_divStatus)
		{
			third._divFreeList->Remove(divNode);					///由于节点已被外界获取，那么对于管理器来说该设备已加入一项工作
			divNode->_divStatus = M_DIV_STATUS_WORK;
			retDiv = divNode;
		}
	}
	return retDiv;
}

RQDivNode * RQDivContainer::GetDivByType(int divType)
{
	RQDivNode* retDiv = 0;
	Three& third = _typeDivMap[divType];
	if (third._divFreeList)						///查看该类型是否存在设备存入容器，若不存在则表示没有改类型设备
	{
		RQDivNode* divNode = (RQDivNode*)third._divFreeList->pop_front();			///针对输入的id查找对应的任务节点
		if (divNode)
		{
			divNode->_divStatus = M_DIV_STATUS_WORK;
			retDiv = divNode;
		}
	}
	return retDiv;
}

void RQDivContainer::ChangeSock(RQDivNode * divNode, int socket)
{
	if (divNode)
	{
		_RemoveFromSockMap(divNode);
		divNode->_sock = socket;					///更新传入设备的socket信息
		_InsertInSockMap(divNode);					///将当前已更新过sock的设备存入sock表中
	}
}

void RQDivContainer::TaskOver(RQDivNode * divNode)
{
	if (M_DIV_STATUS_WORK == divNode->_divStatus)		///并检查当前设备是否为正常工作结束
	{
		divNode->_divStatus = M_DIV_STATUS_FREE;		///通过检测，表明设备的工作已结束，恢复为空闲状态
		_typeDivMap[divNode->_divType]._divFreeList->push_front(divNode);	///任务匹配失败，将空闲的设备存储待用
	}
}

void RQDivContainer::WillRunTask(RQDivNode * divNode)
{
	if (M_DIV_STATUS_WORK == divNode->_divStatus)	///检查设备是否正在工作中
	{
		_typeDivMap[divNode->_divType]._divFreeList->Remove(divNode);		///将设备从空闲容器中取出，表示设备已有任务
		divNode->_divStatus = M_DIV_STATUS_WORK;			///设备节点得到任务，因此标记为工作中，
	}
}

RQDivNode * RQDivContainer::_GetDivByTypeId(int divType, int divId)
{
	auto& idMap = _typeDivMap[divType]._divIdMap;
	auto iter = idMap.Find(divId);
	RQDivNode* divNode = 0;
	if (iter != idMap.end())
	{
		divNode = iter->second;
	}
	return divNode;
}

RQDivNode * RQDivContainer::_GetDivById(Three & third, int divId)
{
	RQDivNode* divNode = 0;
	auto iter = third._divIdMap.Find(divId);
	if (iter != third._divIdMap.end())
	{
		divNode = iter->second;
	}
	return divNode;
}

RQDivNode* RQDivContainer::_InsertInTypeMap(RQDivNode * divNode)
{
	Three& third = _typeDivMap[divNode->_divType];				///获取设备该设备类型在所在的id存储容器
	{
		RQAutoLock lock(&third._lock);							///防止链表存储容器创建泄漏
		if (!third._divFreeList)
		{
			third._divFreeList = new RQList(new RQSpinLock);	///创建链表存储容器
		}
	}

	RQDivNode* oldDiv = _GetDivById(third,divNode->_divId);		///获取当前id所对应的设备，防止同id设备多次插入
	if (!oldDiv)
	{
		third._divIdMap[divNode->_divId] = divNode;				///将设备存入设备id映射表中
		third._divFreeList->push_front(divNode);				///将设备存入空闲设备存储容器中
		oldDiv = divNode;
	}
	return oldDiv;
}

void RQDivContainer::_InsertInSockMap(RQDivNode * divNode)
{
	int newDivSock = divNode->_sock;			///记录传入节点的sock值，防止同一节点被传入两次
	RQDivNode* oldDiv = _sockDivMap[divNode->_sock];
	if (oldDiv)
	{
		oldDiv->_sock = 0;						///由于sock被抢占，所以旧设备节点的sock将设置为无效
	}
	divNode->_sock = newDivSock;
	_sockDivMap[divNode->_sock] = divNode;		///保存设备到填写的旧设备节点存储位置中 
}

void RQDivContainer::_RemoveFromTypeMap(RQDivNode * divNode)
{
	Three& third = _typeDivMap[divNode->_divType];
	if (third._divFreeList&&divNode->_saveContainer == third._divFreeList)	///检查该类型的容器是否存在，且是否为该容器
	{
		third._divFreeList->Remove(divNode);
	}
	RQDivNode* saveDiv = _GetDivById(third, divNode->_divId);
	if (saveDiv == divNode)						///检查该设备id是否属于传入的设备节点
	{
		third._divIdMap.Remove(divNode->_divId);	///从设备id映射表中移除该设备
	}
}

void RQDivContainer::_RemoveFromSockMap(RQDivNode * divNode)
{
	auto iter = _sockDivMap.Find(divNode->_sock);
	RQDivNode* saveDiv = iter != _sockDivMap.end() ? iter->second : 0;	///获取该sock对应的设备节点
	if (saveDiv == divNode)
	{
		_sockDivMap.Remove(divNode->_sock);
	}
}
