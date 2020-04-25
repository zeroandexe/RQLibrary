#pragma once

#include <sys/epoll.h>
#include "RQSession.h"
#include "define/RQParam.h"
/*
	该类基于TCP提供网络节点双方c/s的RP/C通讯方式，
*/

///class RQSession;
class RQThread; 
class RQPack;
class RQCommunication
{
public:
	/*
		初始化通讯模块，并设置通讯所使用的执行线程数量
	*/
	static void InitObj(int cpuCoreNum = 0);
	/*
		同步执行连接目标服务器
		ip		[in]	目标服务器网络地址
		port	[in]	服务器目标进程端口
		tryTimes	[in]	连接所进行的失败尝试次数
		return	[out]	返回本次连接所产生的会话对象
	*/
	static RQSession* Connect(const char* ip, unsigned short port,int tryTimes = 1);			//连接目标服务器
	//同上，ip和port为大端
	static RQSession* Connect(int ip, short port, int tryTimes = 1);

	/*
		同步注册监听，若有新连接存在
		ip		[in]	指定被监听的目标主机地址，若地址为空则表示接收任意地址
		port	[in]	提供给外界的连接的网络端口
		listenNum	[in]	为监听端口所提供的监听队列容量
	*/
	static RQSession* Listen(const char* ip,unsigned short port,unsigned int listenNum);	
	//同上，ip和port为大端
	static RQSession* Listen(int ip, short port, unsigned int listenNum);
	/*
		将会话加入到会话池中，并根据传入的超时事件决定是否将会话加入到定时池中检测超时行为
		session		[in]	将被加入池中的连接会话
		timeOutS	[in]	在池中每次激活后的有效活跃时间，若设置为0则表示会话长期有效不会应为超时未使用导致销毁
		isListen	[in]	该会话是否是一个监听会话
		allowActive	[in]	该会话是否可以在获取交互数据时重置当前会话的有效时间（默认为允许）
		return		[out]	执行是否成功
	*/
	static int RegisterSessionPool(RQSession* session,unsigned int timeOutS,bool allowActive = true);

	/*
		会话已结束清理会话资源，取消对会话的定时检测
		不执行会话对象的回收
	*/
	static void ClearSession(RQSession* session);

	/*
		回收不使用的会话对象
	*/
	static void RecycleSession(RQSession* session);
	/*
		回收不使用的数据包对象
	*/
	static void RecyclePack(RQPack* pack);
	/*
		获取一个空闲的数据包对象
	*/
	static RQPack* GetFreeRQPack();
	/*
		启动会话模块，并决定会话模块是否占用当前线程
		newThread	[in]	默认启动新线程管理会话，若为false则占用当前线程来执行通讯管理
	*/
	static void Run(bool newThread = true);

protected:
	static void _EpollThreadFun(void* param);			//epoll_wait的执行函数，处理epoll中的线程检测
private:
	static int _epollFd;					//提供文件句柄检测使用epoll句柄
	static RQThread* _epollThread;			//执行epoll_wait的等待线程
	static epoll_event* _eventArray;		//epoll获取活跃标识符事件数组，数组大小由配置宏决定
};

