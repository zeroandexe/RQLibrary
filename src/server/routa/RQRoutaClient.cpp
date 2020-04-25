#include "server/routa/RQRoutaClient.h"
#include "RQSys.h"
#include "RQCommunication.h"
#include "define/RQdefine.h"
#include "RQRegularPool.h"
#include "RQPack.h"
#include "define/RQParam.h"

RQRoutaClient::RQRoutaClient(RQRoutaData_Addr * serverAddr, RQRoutaData * localData, int timeOutS)
	:RQRouta(localData,timeOutS)
{
	_serverIp = serverAddr->_ip;
	_serverPort = serverAddr->_port;
	_activeLoad._obj = this;
	_copyLoad._obj = this;
}

void RQRoutaClient::LoginRoutaNet()
{
	_SendRoutaInsert(_localData._addr._ip, _localData._addr._port, _localData._type);
	_SendRoutaCopy();

	///若在期望时限内本路由节点未收到任何可处理的数据包则向路由节点发送注册报告，通知本路由节点可以接收数据
	RQRegularPool::Register(&_activeLoad, &_activeLoad, _Active, M_ROUTA_DELAY_TIMES*1000, true, true);
	///注册定时获取路由表的服务
	RQRegularPool::Register(&_copyLoad, &_copyLoad, _CopyTable, M_ROUTA_COPY_TIMES * 1000, true, true);
}

void RQRoutaClient::LogoutRoutaNet()
{
	_SendRouta(_serverIp, _serverPort, _localData._addr._ip, _localData._addr._port, 0,
		M_ROUTA_PROTOCOL, M_ROUTA_PROT::REMOVE);				///将本机从路由网络中移除

	RQRegularPool().Remove(&_activeLoad);								///移除本机的激活行为
	RQRegularPool().Remove(&_copyLoad);								///移除本机的激活行为
}

void RQRoutaClient::DelayRoutaNet()
{
	RQRegularPool().Active(this);								///延迟本机进行路由注册
}

void RQRoutaClient::_SendRoutaInsert(int insertIp, short insertPort, int insertOptType)
{
	_SendRouta(_serverIp, _serverPort, insertIp, insertPort, insertOptType,
		M_ROUTA_PROTOCOL, M_ROUTA_PROT::INSERT);
}


void RQRoutaClient::_SendRoutaRemove(int removeIp, short removePort)
{
	_SendRouta(_serverIp, _serverPort, removeIp, removePort, 0,
		M_ROUTA_PROTOCOL, M_ROUTA_PROT::REMOVE);
}

void RQRoutaClient::_SendRoutaError(int errorIp, short errorPort)
{
	_SendRouta(_serverIp, _serverPort, errorIp, errorPort, 0,
		M_ROUTA_PROTOCOL, M_ROUTA_PROT::ERROR);
}

void RQRoutaClient::_SendRoutaCopy()
{
	RQRoutaData_Addr addr;

	RQSession* client = RQCommunication().Connect(_serverIp, _serverPort);
	char buff[16];

	if (client)
	{
		addr.Set(_localData._addr._ip, _localData._addr._port);

		int sendBytes = client->Send(&addr, sizeof(addr), M_ROUTA_PROTOCOL, M_ROUTA_PROT::COPY);
		client->SetCallFun_Recv(_CopyRoutaTable, this);
		RQCommunication().RegisterSessionPool(client, _timeOutS,false);
	}
}

void RoutaTable_Null(int ip, short port, int type, RQRoutaTable* table)
{
}

void RoutaTable_Insert(int ip, short port, int type, RQRoutaTable* table)
{
	RQRoutaNode* node = table->CreateBy(ip, port);
	node->_routaData._type = type;
	table->SaveInTable(node);
}

void RoutaTable_Remove(int ip, short port, int type, RQRoutaTable* table)
{
	table->Remove(ip, port);
}

void RQRoutaClient::RoutaPack(RQSession * session, RQPack * pack)
{
	///---定义协议处理函数
	typedef void(*Fun_Type)(int ip, short port, int type, RQRoutaTable* table);
	static Fun_Type Fun_Array[M_ROUTA_PROT::TOTAL] = {
		RoutaTable_Null,
		RoutaTable_Insert ,		//insert
		RoutaTable_Remove,
		RoutaTable_Null,
		RoutaTable_Null
	};
	RQPackHead* head = pack->GetPackHead();

	if (M_ROUTA_PROTOCOL == head->_optProtocol								///检查是否是路由协议
		&& M_ROUTA_PROT::TOTAL > (unsigned int)head->_optDecorate)		///检查是否是在有效操作协议内
	{
		char* ptr = (char*)pack->GetDataPtr();								///获取数据首地址
		Fun_Type callFun = Fun_Array[head->_optDecorate];

		RQRoutaTable* table = &_routaTable;

		for (int offset = sizeof(RQRoutaData_Addr);
			offset < pack->GetDataLen();
			offset += sizeof(RQRoutaData))									///采用数据长度和偏移量来计算节点个数
		{
			RQRoutaData* data = (RQRoutaData*)(ptr + offset);				///获取当前网络数据
			callFun(data->_addr._ip, data->_addr._port, data->_type, table);		///按照协议指定的操作方
		}

		_routaTable.Show();
	}
}

void RQRoutaClient::_CopyRoutaTable(RQSession * session, RQPack * pack,RQPackHead* head, void * param)
{
	RQRoutaClient* client = (RQRoutaClient*)param;
	client->RoutaPack(session, pack);
}

void RQRoutaClient::_Active(void * param)
{
	RQRoutaClient* routa = (RQRoutaClient*)((RQLoadNode*)param)->_obj;
	routa->_SendRouta(routa->_serverIp, routa->_serverPort,
		routa->_localData._addr._ip, routa->_localData._addr._port, routa->_localData._type,
		M_ROUTA_PROTOCOL, M_ROUTA_PROT::INSERT);				///第一次登陆向路由中心发送注册报告
}

void RQRoutaClient::_CopyTable(void * param)
{
	RQRoutaClient* routa = (RQRoutaClient*)((RQLoadNode*)param)->_obj;
	routa->_SendRoutaCopy();									///向网络发出路由表拷贝请求
}
