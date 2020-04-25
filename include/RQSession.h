#pragma once
#include "node/RQNode.h"
#include "node/RQSessNode.h"
#include "RQPair.h"
#include "RQSock.h"
#include "RQSpinLock.h"
#include <atomic>
#include "RQPack.h"

class RQFunNode;
class RQHandlePool;
/*
	该会话对象为已以连接上的网络会话对象
	在接收信息时会对接收的数据进行检测，仅在成功时返回成功，否则将对远端发送错误信息通知数据包
	
	设置设备信息，为的是避免使用会话对象时过多的参与数据包头的处理

*/
class RQSession : public  RQNode
{
public:
	/*
		回调函数的类型定义，
		session	[in]	本次产生数据的会话对象
		memroy	[in]	本次沟通远端所发送来的数据，若本次为accept回调则该内存空间的数据为垃圾数据
		param	[in]	设置回调时所一同设置的相关参数
	*/
	typedef void(*CallFun_Type)(RQSession* session, RQPack* pack,RQPackHead* head, void* param);
	typedef void(*AcceptFun_Type)(RQSession* session, void* param, SockAddr_In& addr);

	RQSession();
	virtual ~RQSession();

	virtual void Create(void* param = 0);
	virtual void* Init(void* param = 0);
	virtual void Clear();


	void SetSock(int sock,bool isListen = false);		//为会话对象设置网络资源句柄，默认为非监听会话
	RQSock& GetSock();			//返回socket值
	void SetDivInfo(unsigned short divType, unsigned int divId);	//设置本对话所归属的设备

	void SetCallFun_Destory(CallFun_Type destroyFun, void* param = 0);	//注册会话销毁时执行的销毁函数
	void SetCallFun_Recv(CallFun_Type recvFun, void* param = 0);		//注册会话接收到处理函数时的信息处理函数
	void SetCallFun_Accept(AcceptFun_Type acceptFun, void* param = 0);	//注册会话检测到具有新连接时执行的新连接处理函数
	
	bool IsListen();			//反馈该会话是否是监听会话

	/*
		将memroy中的数据发送给该会话所连接的远端对象，在内部将自动处理包头数据的添加
		若指定序列号，则数据不会被缓存 
		memroy		[in]	包含发送给远端的内存数据和数据长度的内存对象，已包含包头长度
		optProtocol	[in]	对数据处理的要求处理协议
		optDecorate	[in]	对所设置协议的辅助说明处理
		seriNum		[in]	数据包对应序列号，外部使用通常为0（用于内部对数据包的记录）
		return		[out]	返回发送的字节长度，即原接口send的返回值
							返回-2表示缓存池已满暂时无法缓存，故无法发送数据包
	*/
	int Send(RQPack* pack, unsigned int optProtocol, unsigned int optDecorate, unsigned int seriNum=0);

	/*
		将data指向的内存空间并将指定长度的数据组装入数据包中，
		并通过会话对象将数据发送，若数据被指定序列号则该数据将不会在意回应，并且不会被缓存
		data		[in]	内存数据的内存首地址
		dateLen		[in]	内存首地址长度
		optProtocol	[in]	本次数据要求的操作协议
		optDecorate	[in]	操作协议辅助说明协议
		seriNum		[in]	数据包对应序列号，外部使用通常为0（用于内部对数据包的记录
		return		[out]	返回发送的字节长度，即原接口send的返回值
	*/
	int Send(void* data, unsigned int dataLen, unsigned int optProtocol, unsigned int optDecorate, unsigned int seriNum = 0);
	
	/*
		当有新连接接入时将会回调该函数，
		该函数将采用当前监听会话中的连接创建新的会话对象，
		并且将所采用的会话对象作为参数调用外界注册的accept回调函数
	*/
	static void Accept(void* param);
	/*
		执行网络数据获取操作，并对数据包的完整性和序列性进行检测
		若判定该数据为无效数据包，则由会话对象将会发送对应的错误报告对会话远端，并取消本次接收
		若判定成功，则将获取到网络数据到内存存储对象中，并更新对象对数长度的计数
		最后将调用外界所注册的Recv回调函数
		（当Recv执行完毕后，若本次数据为通讯模块回应数据，则程序将会回收接收序列号所对应的数据包，
		若需要将数据包保存，则因提前取出）
	*/
	static void Recv(void* session);
	/*
		销毁时将清理会话对象所产生的数据并回收当前所使用的会话资源，
		若外界有设置destory回调函数，那么最后调用外界所注册的销毁函数
	*/
	static void Destory(void* param);

	/*
		根据会话对象对外发送时使用的序列号，获取该序列号的数据包，该内存对象不会从会话对象中移除
		（一旦发出的数据包得到回应后，该内存对象将在接收回掉函数接收后被回收）
		seriNum		[in]	发送时使用的序列号
		return		[out]	返回序列号所对应的内存对象（数据包）
	*/
	RQPack* GetPackBySeriNum(unsigned int seriNum);

	/*
		功能同GetMemoryBySeriNum，但是会话对象会交出对内存对象的占有权，及会话对象不再拥有该内存对象		
		seriNum		[in]	发送时使用的序列号
		return		[out]	返回序列号所对应的内存对象（数据包）
	*/
	RQPack* RemovePackBySeriNum(unsigned int seriNum);

	/*
		根据传入的起始序列号，下一个正在被使用的序列号
		startSeriNum	[in]	起始检测序列号
		return			[out]	起始号后第一个正在被序列号，为0则表是当前序从当前序列号开始到结束都没有数据包存在
	*/
	unsigned int GetSeriNumOfSendPack(unsigned int startSeriNum);
	/*
		返回会话判断，该会话是否正在被使用
	*/
	bool IsUsing();
	/*
		使用该会话,若占用成功则返回true，否则返回false
	*/
	bool Using();
	/*
		不再使用该会话
	*/
	void NotUsing();
private:
	static int _RecvPackHead(RQSession* session, RQPack* memory);		//接收数据包头，并进行校验，返回错误码
	static int _RecvPackBody(RQSession* session, RQPack* memory);		//接收数据包体，并进行校验，返回错误码
	
	void _RigOutHeadPack(RQPackHead* packHead, unsigned int dateLen,
		int optProtocol, int optDecorate);								//组装数据包头

	unsigned int _SavePack(RQPack* pack);	///将传入的数据包存入被容器中，并占有该数据包的归属标识
	RQPack* _RemovePack(unsigned int handle);	///根据传入的句柄获取对应的数据包，并移除数据包的标识

	void _RecyclePack();					///回收该会话未被取走的数据包
	void _SetZero();						///将对象数据置空
private:
	RQSock* _sock;							//本次连接所产生的网络资源句柄
	unsigned int _divType;					//当前会话所归属设备的类型
	unsigned int _divId;					//当前会话所归属设备的id
	///unsigned int _NextSqn;					//网络数据包下次发送的接收序列号
	RQSpinLock* _lock;

	std::atomic_bool	_isUsing;			//当前会话是否被使用
	RQPair _callDestory;					//会话销毁调用函数
	RQPair _callAccept;						//会话创建调用函数
	RQPair _callRecv;						//会话接收网络数据调用函数
	RQHandlePool* _sendedPackPool;			//已发送的数据包存储池
	int _errPackTimes;						//数据包错误次数
	bool _isListen;							//当前会话是否是监听会话标志
};

