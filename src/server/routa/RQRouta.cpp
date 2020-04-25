#include "server/routa/RQRouta.h"
#include "RQCommunication.h"
#include "define/RQdefine.h"
#include "RQRegularPool.h"
#include "RQSys.h"

RQRouta::RQRouta(RQRoutaData * localData, int timeOutS)
{
	_localData = *localData;
	_timeOutS = timeOutS;
}

void RQRouta::Insert(int ip, short port, char optType)
{
	RQRoutaNode* node = _routaTable.CreateBy(ip, port);

	if (node->_routaData._type != optType)			///检查是否是新路由节点
	{
		_routaTable.SaveInTable(node);				///将该地址最新的路由信息存入表中
		node->_routaData._type = optType;
		_SendRoutaInsert(ip, port, optType);		///通知网络有新节点增加
	}
}

void RQRouta::Insert(RQRoutaData * data)
{
	if (data
		&& (data->_addr._ip!=_localData._addr._ip
			|| data->_addr._port != _localData._addr._port))	///本机不将自身添加入本地路由表中
	{
		Insert(data->_addr._ip, data->_addr._port, data->_type);
	}
}

void RQRouta::Remove(int ip, short port)
{
	RQRoutaNode* node = _routaTable.GetBy(ip, port);
	Remove(node);
}

void RQRouta::Remove(RQRoutaNode * node)
{
	if (_routaTable.IsAttach(node))
	{
		_SendRoutaRemove(node->_routaData._addr._ip, node->_routaData._addr._port);			///通知网络有节点需要从网络中移除
		_routaTable.Remove(node);						///将节点从本地路由表总移除
	}
}

void RQRouta::Error(int ip, short port)
{
	RQRoutaNode* node = _routaTable.GetBy(ip, port);
	if (node)
	{
		Error(node);
	}
}

void RQRouta::Error(RQRoutaNode * node)
{
	if (_routaTable.IsAttach(node))
	{
		_SendRoutaError(node->_routaData._addr._ip, node->_routaData._addr._port);			///通告网络本地出现异常节点
		_routaTable.Remove(node);						///将异常节点从本地移除
	}
}

void RQRouta::CopyTable()
{
	_SendRoutaCopy();									///请求路由信息
}

RQRoutaNode * RQRouta::GetNodeBy(char optType)
{
	return _routaTable.GetBy(optType);
}

RQRoutaNode * RQRouta::GetNodeBy(int ip, short port)
{
	return _routaTable.GetBy(ip,port);
}

void RQRouta::LoginRoutaNet()
{

}

void RQRouta::LogoutRoutaNet()
{
}

void RQRouta::DelayRoutaNet()
{
}

int RQRouta::GetTimeOutS()
{
	return _timeOutS;
}

int RQRouta::GetLocalIp()
{
	return _localData._addr._ip;
}

short RQRouta::GetLocalPort()
{
	return _localData._addr._port;
}

void RQRouta::_SendRoutaRemove(int removeIp, short removePort)
{
}

void RQRouta::_SendRoutaCopy()
{
}

void RQRouta::_SendRouta(int remoteIp, short remotePort,
	int ip, short port, char type,
	int protocol, int decorate)
{
#pragma pack(push,1)
	struct {
		RQRoutaData_Addr _sourAddr;
		RQRoutaData		_data;
	}data;
#pragma pack(pop)

	RQSession* client = RQCommunication().Connect(remoteIp, remotePort);
	if (client)
	{
		data._sourAddr.Set(_localData._addr._ip, _localData._addr._port);
		data._data.Set(ip, port, type);

		client->Send(&data, sizeof(data), protocol, decorate);
		RQRegularPool::Register(client,client, _Destory, _timeOutS * 1000);
	}
}

void RQRouta::_Destory(void * param)
{
	RQSession* client = (RQSession*)param;
	RQCommunication::RecycleSession(client);
}
