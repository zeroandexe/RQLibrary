#include "RQCommunication.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "RQSys.h"
#include "RQSession.h"
#include "RQRegularPool.h"
#include "RQThreadPool.h"
#include "RQThread.h"
#include "RecycleBin.h"
#include "node/RQFunNode.h"
#include "define/RQdefine.h"

#include "RQHandlePool.h"

int RQCommunication::_epollFd = 0;			//提供文件句柄检测使用epoll句柄
RQThread* RQCommunication::_epollThread;	//执行epoll_wait的等待线程
epoll_event* RQCommunication::_eventArray;		///epoll获取活跃标识符事件数组


/*
	通讯模块的初始化,
*/
void RQCommunication::InitObj(int cpuCoreNum)
{
	static bool needInit = true;
	if (needInit)
	{
		RQThreadPool::CreatePool(cpuCoreNum);			///创建线程池
		RQRegularPool::InitObj();						///启动定时池

		_epollFd = epoll_create(M_SESSION_TOTAL);		///申请epoll资源
		_eventArray = new epoll_event[M_SESSION_TOTAL];
		_epollThread = 0;
		needInit = false;
	}
}

RQSession * RQCommunication::Connect(const char * ip, unsigned short port, int tryTimes)
{
	return Connect(inet_addr(ip), RQHton(port), tryTimes);
}

RQSession * RQCommunication::Connect(int ip, short port, int tryTimes)
{
	RQSession* newSession = 0;					///定义本次连接所产生的新会话对象
	RQSock sock;
	sock.socket();
	if (sock.IsOk())
	{
		int retVal = 0;
		do {
			retVal = sock.Connect(ip, port);	///连接目标主机
		} while (retVal && tryTimes--);			///进行失败尝试

		if (!retVal)							///检测是否成功连接，若成功连接则可生成会话对象
		{
			sock.ToAsy();
			newSession = RecycleBin::GetFreeRQSession();
			newSession->SetSock(sock);			///连接成功，创建会话对象
		}
		else {
			sock.Close();						///链阶失败清理sock
		}
	}
	return newSession;
}

RQSession * RQCommunication::Listen(const char * ip, unsigned short port, unsigned int listenSize)
{
	unsigned int ipVal = INADDR_ANY;
	if (ip)
	{
		ipVal = inet_addr(ip);
	}
	return Listen(ipVal, RQHton(port), listenSize);
}

RQSession * RQCommunication::Listen(int ip, short port, unsigned int listenSize)
{
	RQSession* listenSession = 0;			///监听会话对象

	RQSock sock;
	sock.socket();
	if (sock.IsOk()){
		sock.ToReusePort();					///标记该监听可重复使用监听端口
		if (!sock.Listen(ip, port, listenSize)){
			sock.ToAsy();
			listenSession = RecycleBin::GetFreeRQSession();
			///将创建号的sock对象赋予会话，并标识该会话为监听会话
			listenSession->SetSock(sock, true);			
		}
		else{
			sock.Close();
		}
	}
	return listenSession;
}

int RQCommunication::RegisterSessionPool(RQSession * session, unsigned int timeOutS, bool allowActive)
{
	epoll_event epollEvent;
	epollEvent.events = EPOLLIN; ///| EPOLLET		///标记为epoll注册事件，

	epollEvent.data.ptr = session;				///将本次会话作为传递参数进行携带

	int retErr =epoll_ctl(_epollFd, EPOLL_CTL_ADD, session->GetSock(), &epollEvent);	///将会话对象和会话文件标识符注册入epoll中

	if (!retErr&&timeOutS)						///会话在epoll中注册成功，并且该会话为定时活跃会话
	{
		///按照给定的超时时间将会话以及会话清理函数注册入定时池中，最总的销毁由会话对象的销毁处理函数执行
		RQRegularPool::Register(session,session, RQSession::Destory, timeOutS*1000, allowActive);
	}
	return retErr;
}

void RQCommunication::ClearSession(RQSession * session)
{
	RQRegularPool::Remove(session);				///将会话从定时池中移除

	epoll_event epollEvent;
	///epollEvent.events = EPOLLET; ///|

	epoll_ctl(_epollFd, EPOLL_CTL_DEL, ((RQSession*)session)->GetSock(), &epollEvent);		///将未被移除的会话从epoll中移除
}

void RQCommunication::RecycleSession(RQSession * session)
{
	RecycleBin::RecycleNode(session);
}

void RQCommunication::RecyclePack(RQPack * pack)
{
	RecycleBin::RecycleNode((RQNode*)pack);
}

RQPack * RQCommunication::GetFreeRQPack()
{
	return RecycleBin::GetFreeRQPack();
}

void RQCommunication::Run(bool newThread)
{
	if (newThread && !_epollThread)						///检测是否需要创建新线程进行执行监听程序
	{
		_epollThread = new RQThread();
		_epollThread->SetFun(_EpollThreadFun, 0);

		_epollThread->Weak();							///唤醒线程并进行执行
	}
	else
	{
		_EpollThreadFun(0);								///不需要新线程，则占用当前线程执行
	}
}

void RQCommunication::_EpollThreadFun(void * param)
{
	void(*callFun)(void*);
	while (true)
	{
		int aliveNum = epoll_wait(_epollFd, _eventArray, M_SESSION_TOTAL, -1);	///等待epoll反应，并获取当前活跃的会话数量
		for (int i = 0; i < aliveNum; i++)
		{
			RQSession* session = (RQSession*)_eventArray[i].data.ptr;			///获取处于活跃状态下的会话对象

			if (session->Using())						///尝试占用会话，并检测是否占用成功
			{
				callFun = RQSession::Recv;				///默认为该会话操作为recv操作
				if (session->IsListen())			 	///检查该会话是否是监听会话
				{
					callFun = RQSession::Accept;							///若该会话是监听则改为调用accept操作
				}
				RQThreadPool::Push(callFun,session);			///将设置好的函数节点存入线程池中执行
			}
			RQRegularPool::Active(session);				///将会话对象从定时池中激活一次
		}
	}
}
