#include "server/routa/RQRoutaCenter.h"
#include "RQCommunication.h"
#include "RQPack.h"
#include "RecycleBin.h"


RQRoutaCenter::RQRoutaCenter(RQRoutaData * localData, int timeOutS)
	:RQRouta(localData,timeOutS)
{
}

void RQRoutaCenter::Error(RQRoutaNode * node)
{
	int ip = node->_routaData._addr._ip;
	short port = node->_routaData._addr._port;
	
	RQSession* client = RQCommunication().Connect(ip, port);			///尝试连接异常地址
	if (!client)							///检查目标异常地址是否无法连接
	{
		RQRouta::Remove(node);				///由于目标无法连接，将信息从路由表中移除
	}
	RQCommunication::RecycleSession(client);
}

void RQRoutaCenter::_SendRoutaInsert(int insertIp, short insertPort, int insertOptType)
{
	RQRoutaNode* node = _routaTable.GetNext(0);

	while (node)
	{
		_SendRouta(node->_routaData._addr._ip, node->_routaData._addr._port, insertIp, insertPort, insertOptType,
			M_ROUTA_PROTOCOL, M_ROUTA_PROT::INSERT);
		node = _routaTable.GetNext(node);
	}
}

void RQRoutaCenter::_SendRoutaRemove(int removeIp, short removePort)
{
	RQRoutaNode* node = _routaTable.GetNext(0);

	while (node)
	{
		_SendRouta(node->_routaData._addr._ip, node->_routaData._addr._port, removeIp, removePort, 0,
			M_ROUTA_PROTOCOL, M_ROUTA_PROT::REMOVE);
		node = _routaTable.GetNext(node);
	}
}

void RQRoutaCenter::_SendRoutaError(int errorIp, short errorPort)
{
	_SendRoutaRemove(errorIp, errorPort);
}

void RoutaTable_Null(int ip, short port, int type, RQRoutaCenter* center, RQRoutaData_Addr* sourAddr)
{
}

void RoutaTable_Insert(int ip, short port, int type, RQRoutaCenter* center, RQRoutaData_Addr* sourAddr)
{
	center->Insert(ip, port, type);
}

void RoutaTable_Remove(int ip, short port, int type, RQRoutaCenter* center, RQRoutaData_Addr* sourAddr)
{
	center->Remove(ip, port);
}

void RoutaTable_Error(int ip, short port, int type, RQRoutaCenter* center, RQRoutaData_Addr* sourAddr)
{
	center->RQRouta::Error(ip, port);
}

void RQRoutaCenter::RoutaPack(RQSession * session, RQPack * pack)
{
	///---定义协议处理函数
	RQPackHead* head = pack->GetPackHead();

	if (M_ROUTA_PROTOCOL == head->_optProtocol									///检查是否是路由协议
		&& M_ROUTA_PROT::TOTAL > (unsigned int)head->_optDecorate)			///检查是否是在有效操作协议内
	{
		switch (head->_optDecorate)
		{
		case M_ROUTA_PROT::COPY:
			ProtFun_Copy(session);		///处理路由网络中的路由表拷贝协议
			break;
		default:
			ProtFun_Other(pack->GetDataPtr(), pack->GetDataLen(), head);		///处理其他可同参数、行为的协议
		}

		_routaTable.Show();			///显示当前路由表信息
	}
}

void RQRoutaCenter::ProtFun_Copy(RQSession * session)
{
	RQPack* pack = RQCommunication().GetFreeRQPack();
	///写入本机地址
	RQRoutaData_Addr localAddr;
	localAddr.Set(GetLocalIp(), GetLocalPort());				
	pack->WriteData(&localAddr, sizeof(RQRoutaData_Addr));

	RQRoutaNode* node = _routaTable.GetNext(0);						///获取第一个路由节点
	while (node)
	{
		pack->AppendData((void*)&node->_routaData, sizeof(RQRoutaData));
		node = _routaTable.GetNext(node);
	}
	session->Send(pack, M_ROUTA_PROTOCOL, M_ROUTA_PROT::INSERT);	///将数据回发给目标网络对象
}

void RQRoutaCenter::ProtFun_Other(void * dataAddr, int dataLen,RQPackHead* head)
{
	typedef void(*Fun_Type)(int ip, short port, int type, RQRoutaCenter* center, RQRoutaData_Addr* sourAddr);
	static Fun_Type Fun_Array[M_ROUTA_PROT::TOTAL] = {
		RoutaTable_Null,
		RoutaTable_Insert ,
		RoutaTable_Remove,
		RoutaTable_Error,
		RoutaTable_Null
	};

	Fun_Type callFun = Fun_Array[head->_optDecorate];

	char buff[16];
	for (int offset = sizeof(RQRoutaData_Addr);
		offset < dataLen;
		offset += sizeof(RQRoutaData))										///采用数据长度和偏移量来计算节点个数
	{
		RQRoutaData* data = (RQRoutaData*)(((char*)dataAddr) + offset);					///获取当前网络数据
		callFun(data->_addr._ip, data->_addr._port, data->_type, this, (RQRoutaData_Addr*)dataAddr);		///按照协议指定的操作方
	}
}
