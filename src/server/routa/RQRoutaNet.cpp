#include "RQRoutaNet.h"
#include "RQSys.h"
#include "RQPack.h"
#include "RecycleBin.h"

RQRoutaNet::RQRoutaNet(RQRouta * routa, RQSession::CallFun_Type recvProtFun,void* funParam)
{
	_routa = routa;
	_userRecvFun = recvProtFun;
	_listenSession = 0;
	_userParam = funParam;
}

RQRoutaNet::~RQRoutaNet()
{
	LogoutNet();
	delete _routa;
}

int RQRoutaNet::LoginNet(int serviceNum)
{
	int retInt = -1;
	short localPort = _routa->GetLocalPort();
	if (localPort)
	{
		retInt = _ListenService(localPort, serviceNum);			///开启监听服务
		if (!retInt)						///检测监听服务是否成功开启
		{
			_routa->LoginRoutaNet();				///激活路由节点在网络中的活跃值
			retInt = 0;
		}
	}
	return retInt;
}

void RQRoutaNet::LogoutNet()
{
	RQCommunication::RecycleSession(_listenSession);						///回收监听服务对象，取消对外界的功能提供
	_listenSession = 0;
	_routa->LogoutRoutaNet();												///将本机从路由网络中移除
}

int RQRoutaNet::OptNet(int optType, RQPack * pack, int optProtocol, int optDecorate,
	RQSession::CallFun_Type recvFun,void* recvFunParam,
	RQSession::CallFun_Type destroyFun,void* destroyFunParam,
	int timeOutS)
{
	int retInt = -1;
	if (_listenSession) {					///检测是否开启了监听服务，开启了监听服务则表示本机已进入路由网络
		RQRoutaNode* node = _routa->GetNodeBy(optType);						///按照期望的节点类型获取提供处理的服务节点
		if (node){							///检查是否存在这种服务
			///连接目标服务节点
			RQSession* client = RQCommunication().Connect(node->_routaData._addr._ip,node->_routaData._addr._port);
			if (client
				&& -2!= client->Send(pack, optProtocol, optDecorate))					///将数据发往目标服务主机
			{
				client->SetCallFun_Destory(destroyFun, destroyFunParam);
				client->SetCallFun_Recv(recvFun, recvFunParam);
				RQCommunication().RegisterSessionPool(client, timeOutS, false);
				retInt = 0;
			}
			else {
				///无法建立连接，向网络报告目标机器可能存在问题
				_routa->Error(node);
			}
		}
	}
	if (-1 == retInt)											///发送失败，准备调用会话销毁处理
	{
		if (destroyFun)											///是否进行会话销毁
		{
			destroyFun(0, pack,pack->GetPackHead(), destroyFunParam);				///执行用户期望的回收处理
		}
		RecycleBin::RecycleNode(pack);							///回收未被保存的数据包对象
	}

	return retInt;
}

int RQRoutaNet::_ListenService(short port, int waitNum)
{
	int retInt = -1;
	if (!_listenSession)
	{
		_listenSession = RQCommunication().Listen(0, port, waitNum);		///开启网络服务监听
		if (_listenSession)
		{
			///设置监听服务所执行的连接接收函数，额外参数为本网络对象
			_listenSession->SetCallFun_Accept(_AcceptFun, this);	
			RQCommunication().RegisterSessionPool(_listenSession, 0);		///将服务监听加入会话管理器
			retInt = 0;
		}
	}
	return retInt;
}

void RQRoutaNet::_RecvFun(RQSession * session, RQPack * pack,RQPackHead* head, void * param)
{
	RQRoutaNet* net = (RQRoutaNet*)param;
	net->_routa->DelayRoutaNet();

	if (M_ROUTA_PROTOCOL == head->_optProtocol)		///检查是否是路由协议
	{
		net->_routa->RoutaPack(session, pack);		///将数据包交由路由模块处理
	}
	else if(net->_userRecvFun) {					///检测用户是否设置了自定义处理函数
		net->_userRecvFun(session, pack,head, net->_userParam);			///将会话对象与数据包都交予用户处理
	}
}

void RQRoutaNet::_AcceptFun(RQSession * session, void * param,SockAddr_In& addr)
{
	RQRoutaNet* net = (RQRoutaNet*)param;
	session->SetCallFun_Recv(_RecvFun, param);
	RQCommunication().RegisterSessionPool(session, net->_routa->GetTimeOutS(),false);
}
