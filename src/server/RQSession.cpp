#include "RQSession.h"
#include "RQSock.h"
#include "RecycleBin.h"
#include "node/RQFunNode.h"
#include "RQCommunication.h"
#include "define/RQdefine.h"
#include "RQHandlePool.h"

#include <iostream>

RQSession::RQSession()
{
	Create(); 
	Init();
}


RQSession::~RQSession()
{
	RecycleBin::RecycleNode(_sendedPackPool);
	delete _sock;
	delete _lock;
}

void RQSession::Create(void * param)
{
	_sock = new RQSock;					///创建sock对象
	_sendedPackPool = RecycleBin::GetFreeRQHandlePool();
	_lock = new RQSpinLock();
}

void * RQSession::Init(void * param)
{
	_sendedPackPool->Resize(M_SESSION_POOL_SIZE);		///扩展已发送数据包的存储池
	_SetZero();
	return nullptr;
}

void RQSession::_RecyclePack()
{
	int handle = GetSeriNumOfSendPack(0);				///获取第一个待回应的数据包句柄
	while (handle)
	{
		RQPack* pack = RemovePackBySeriNum(handle);		///归还句柄资源，获取存放的数据包对象
		RecycleBin::RecycleNode(pack);					///回收未被处理的数据包
		handle = GetSeriNumOfSendPack(handle);
	}
}

void RQSession::_SetZero()
{
	_divType = 0;
	_divId = 0;
	_isUsing = 0;
	_callDestory._first._llong = 0;
	_callDestory._second._llong = 0;
	_callAccept._first._llong = 0;
	_callAccept._second._llong = 0;
	_callRecv._first._llong = 0;
	_callRecv._second._llong = 0;
	_errPackTimes = 0;
	_isListen = 0;
}

void RQSession::Clear()
{
	_sock->Close();										///关闭sock，回收网络资源
	///---回收未被处理的数据包
	_RecyclePack();
}

void RQSession::SetSock(int sock,bool isListen)
{
	_sock->SetSock(sock);
	_isListen = isListen;
}

RQSock& RQSession::GetSock()
{
	return *_sock;
}

void RQSession::SetDivInfo(unsigned short divType, unsigned int divId)
{
	_divId = divId;
	_divType = divType;
}

void RQSession::SetCallFun_Destory(CallFun_Type destroyFun, void * param)
{
	_callDestory._first._ptr = (void*)destroyFun;
	_callDestory._second._ptr = param;
}

void RQSession::SetCallFun_Recv(CallFun_Type recvFun, void * param)
{
	_callRecv._first._ptr = (void*)recvFun;
	_callRecv._second._ptr = param;
}

void RQSession::SetCallFun_Accept(AcceptFun_Type acceptFun, void * param)
{
	_callAccept._first._ptr = (void*)acceptFun;
	_callAccept._second._ptr = param;
}

bool RQSession::IsListen()
{
	return _isListen;
}

int RQSession::Send(RQPack * pack, unsigned int optProtocol, unsigned int optDecorate, unsigned int seriNum)
{
	int sendBytes = -2;
	
	///---获取数据包包头对象
	RQPackHead* packHead = pack->GetPackHead();

	///---设置数据包数据长度、主协议、辅助说明协议以及本会话配置
	_RigOutHeadPack(packHead, pack->GetPackLen(), optProtocol, optDecorate);

	///将本次数据包的内存对象存入发送数组中，并获取数据包所对应的序列号，若已有序列号则不存入发送池中
	unsigned int saveHandle = seriNum? seriNum : _SavePack(pack);
	if (saveHandle)			///检测该会话的容量已超量
	{
		packHead->_serialNumber = saveHandle;			///将本包的存储句柄通知远端，

		RQPackHead::AdjustHeadChar(packHead);			///填写包头效验数据
		RQPackHead::AdjustCheckData(packHead);			///设置包头尾端效验数据
		RQPackHead::AdjustCheckSum(packHead);			///设置包头效验和参数
		RQPackHead::Hton(packHead);						///将包头数据转换为大端模式
		sendBytes = _sock->Send((char*)packHead, pack->GetPackLen());		///将数据发送向远端
	}

	RecycleBin::RecycleNode(pack);						///用户指定序列号时，或者会话容器已满时都将回收数据包

	return sendBytes;
}  

int RQSession::Send(void * data, unsigned int dataLen, unsigned int optProtocol, unsigned int optDecorate, unsigned int seriNum)
{
	RQPack* pack = RecycleBin::GetFreeRQPack();			///所要一个数据包
	pack->Resize(dataLen);								///按照传入的数据大小分配空间
	pack->WriteData(data, dataLen);						///将数据写入数据包的数据段

	return Send(pack, optProtocol, optDecorate, seriNum);	///无论该pack是否指定了序列号，被调函数都会将pack对象处理调
}

void RQSession::Accept(void * param)
{
	RQSession* listenSession = (RQSession*)param;	///获取监听会话对象
	AcceptFun_Type acceptFun = (AcceptFun_Type)listenSession->_callAccept._first._ptr;		///获取外界注册的接收处理函数

	SockAddr_In addr;
	int newSock = listenSession->_sock->Accept(addr); ///accept(*listenSession->_sock, 0, 0);
	if (newSock > 0)
	{
		RQSession* newSession = RecycleBin::GetFreeRQSession();
		newSession->SetSock(newSock);
		newSession->GetSock().ToAsy();						///将sock切换至异步模式
		///调用监听会话所注册的监听函数
		acceptFun(newSession,listenSession->_callAccept._second._ptr,addr);			///执行外界所注册的新连接接收处理函数
	}
	listenSession->NotUsing();
}


void RQSession::Recv(void * param)
{
	int optProtocol = 0;				///本次数据包的协议
	int serialNumber = -1;				///本次数据包的认证序列号，
	
	RQSession* session = (RQSession*)param;
	RQPack* pack = RecycleBin::GetFreeRQPack();

	int errCode = _RecvPackHead(session, pack);		///获取数据包包头数据

	if (M_COMCA_ERR::OK == errCode)			///检测包头是否正确
	{
		RQPackHead* packHead = pack->GetPackHead();
		serialNumber = packHead->_serialNumber;		///获取数据包协议号
		serialNumber = serialNumber ? serialNumber : -1;

		errCode = _RecvPackBody(session, pack);		///获取数据包包体数据

		if (M_COMCA_ERR::OK == errCode)
		{
			packHead = pack->GetPackHead();
			optProtocol = packHead->_optProtocol;		///获取处理协议
			pack->SetPackLen(packHead->_dataLen);		///调整数据包对象的存储长度
			///---无论是错误包，还是回应包，以及数据包都通过协议和接收回掉函数交余接收函数
			CallFun_Type callFun = (CallFun_Type)session->_callRecv._first._ptr;
			if (callFun)			///查看是否没有设置接收回调，若有则调用
			{
				callFun(session, pack, packHead, session->_callRecv._second._ptr);
				if (pack->_saveContainer)				///检测用户是否已经截取数据包
				{
					pack = RecycleBin::GetFreeRQPack();
				}
			}
			///---查看本次收到的数据包是否为接收回应，若为接收回应则回收回应所指向的数据包
			if (M_COMCA_PROTOCOL == packHead->_optProtocol				///是通讯协议
				 && M_COMCA_PROT::M_COMCA_PROT_RESPOND == packHead->_optDecorate)		///并且是通讯终的路由协议
			{
				RecycleBin::RecycleNode(session->RemovePackBySeriNum(serialNumber));			///将已得到回应的数据包（内存对象从未回应池中移除	
			}
			else {
				pack->Clear();
				///若本次的数据包不是回应数据包，则回应对方数据包处理完成
				session->Send(pack, M_COMCA_PROTOCOL, M_COMCA_PROT::M_COMCA_PROT_RESPOND, serialNumber);	///告知远端，本次处理完成
			}
		}
	}

	if (M_COMCA_ERR::OK != errCode
		&& M_COMCA_ERR::NOT_DATA != errCode
		&& M_COMCA_ERR::DISCONNECT != errCode)
	{
		pack->WriteData(&errCode, sizeof(errCode));
		session->Send(pack, M_COMCA_PROTOCOL, M_COMCA_PROT::M_COMCA_PROT_ERR, serialNumber);	///将当前出现的问题通知给推讯远端
	}
	if (M_COMCA_ERR::DISCONNECT == errCode)
	{
		Destory(session);
	}
	else {
		session->NotUsing();						///设置该会话已被释放可再次接受数据
		RecycleBin::RecycleNode(pack);				///回收内存对
	}
}

//通常由定时池进行回调
void RQSession::Destory(void * param)
{
	RQSession* session = (RQSession*)param;
	RQCommunication::ClearSession(session);					///将会话从监听中移除，防止后续会话数据包的产生
	///RQPack* pack = RecycleBin::GetFreeRQPack();
	CallFun_Type destoryFun = (CallFun_Type)session->_callDestory._first._ptr;	///在会话销毁前先调用外界注册的回调销毁函数
	
	if (destoryFun)			///检查外界是否需要调用销毁函数
	{
		destoryFun(session, 0,0, session->_callDestory._second._ptr);
	}
	///RecycleBin::RecycleNode(pack);						///回收请求的内存对象
	RQCommunication::RecycleSession(session);			///回收会话对象
}

RQPack * RQSession::GetPackBySeriNum(unsigned int seriNum)
{
	return (RQPack*)_sendedPackPool->GetSour(seriNum);
}

RQPack * RQSession::RemovePackBySeriNum(unsigned int seriNum)
{
	RQPack* pack =  (RQPack*)_RemovePack(seriNum);
	if (pack)
	{
		RQPackHead* head = pack->GetPackHead();
		RQPackHead::Ntoh(head);
		head->_serialNumber = 0;
	}
	return pack;
}

unsigned int RQSession::GetSeriNumOfSendPack(unsigned int startSeriNum)
{
	return _sendedPackPool->GetUsedHandle(startSeriNum);
}

bool RQSession::IsUsing()
{
	return _isUsing;
}

bool RQSession::Using()
{
	bool retBol = false;
	RQAutoLock lock(_lock);
	if (!_isUsing)
	{
		_isUsing = true;
		retBol = true;
	}
	return retBol;
}

void RQSession::NotUsing()
{
	RQAutoLock lock(_lock);
	_isUsing = false;
}

int RQSession::_RecvPackHead(RQSession* session, RQPack* pack)
{
	int errNum = M_COMCA_ERR::HEAD_SHORT;			///默认错误为数据包头不全

	pack->Resize(sizeof(RQPackHead));					///默认初次获取的数据长度仅为包头长度
	RQPackHead* packHead = pack->GetPackHead();				///获取存储空间内存地址

	int recvBytes = session->GetSock().Recv((char*)packHead, sizeof(RQPackHead));		///获取包头数据
	if (recvBytes == sizeof(RQPackHead))					///检查是否获取到完整包头数据
	{
		RQPackHead::Ntoh(packHead);							///将包头转换为本地字节序
		errNum = M_COMCA_ERR::CHECK_HEAD;
		if (RQPackHead::CheckHeadIsOk(packHead))			///检测包头标记是否正常
		{
			errNum = M_COMCA_ERR::CHECK_SUM;
			if (RQPackHead::CheckSumIsoK(packHead))			///检测包头校验和
			{
				errNum = M_COMCA_ERR::OK;					///包头通过检测
			}
		}
	}
	else if(!recvBytes 
		|| -1 == recvBytes)
	{
		errNum = M_COMCA_ERR::DISCONNECT;
	}

	return errNum;
}

int RQSession::_RecvPackBody(RQSession * session, RQPack * pack)
{
	int errNum = M_COMCA_ERR::DISCONNECT;;		///默认错误为网络断链
	RQPackHead* packHead = pack->GetPackHead();			///获取存储空间内存地址

	pack->Resize(packHead->_dataLen, true);				///根据数据包中数据的大小调整数据缓存区大小
	packHead = pack->GetPackHead();						///获取调整后的内存地址
	if (packHead->_dataLen - sizeof(RQPackHead))
	{
		int recvBytes = session->GetSock().Recv(((char*)packHead) + sizeof(RQPackHead), packHead->_dataLen - sizeof(RQPackHead));	///获取后续数据，通过了第一次检测，所以这里不再检测
		if (recvBytes >= 0)									///检查当前资源是否可读
		{
			errNum = M_COMCA_ERR::PACK_SHORT;
			if (packHead->_dataLen - sizeof(RQPackHead) == recvBytes)	///检查数据包是否具有正确的长度
			{
				///---开始检测包尾数据部分
				errNum = M_COMCA_ERR::CHECK_DATA;				///（通过了包头标签字节，那么暂定为包尾数据效验错误）
				if (RQPackHead::CheckDataIsOk(packHead))					///检查数据包的数据尾部是否通过效验
				{
					errNum = M_COMCA_ERR::OK;					///通过了所有检测，即该数据表为完整且有效的数据包
				}
			}
		}
	}
	else
	{
		errNum = M_COMCA_ERR::OK;								///链接正常，则本次获取认为成功
	}

	errNum = session->GetSock().IsConnecting() ?		///检测socket是否正处于链接状态
		errNum :										///那么按照当前错误判定进行处理
		M_COMCA_ERR::DISCONNECT;							///链接无效，那么socket已经断开，返回网络断开错误

	return errNum;
}

void RQSession::_RigOutHeadPack(RQPackHead * packHead,unsigned int dateLen,int optProtocol,int optDecorate)
{
	///设置数据包长度
	packHead->_dataLen = dateLen;
	///设置相关协议
	packHead->_optProtocol = optProtocol;
	packHead->_optDecorate = optDecorate;
}

unsigned int RQSession::_SavePack(RQPack * pack)
{
	unsigned int seriNum = _sendedPackPool->SaveSour(pack);
	if (seriNum)
	{
		pack->_saveContainer = (RQLineStorage*) this;			///占有该数据包防止数据包被外界占用
	}
	return seriNum;
}

RQPack * RQSession::_RemovePack(unsigned int seriNum)
{
	RQPack* pack = (RQPack*)_sendedPackPool->RecyclHandle(seriNum);
	if (pack)
	{
		pack->_saveContainer = 0;								///归还数据包的自由，使其可被其他容器使用
	}
	return pack;
}
