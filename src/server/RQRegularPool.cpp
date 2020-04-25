#include "RQRegularPool.h"
#include "RQSpinLock.h"
#include "RQHandlePool.h"
#include "RQThread.h"
#include "RQList.h"
#include "RecycleBin.h"
#include "node/RQRegularNode.h"
#include "RQTime.h"
#include "RQSys.h"
#include "define/RQdefine.h"
#include "RQThreadPool.h"
#include "node/RQFunNode.h"

RQThread*	RQRegularPool::_checkThread;					//检测线程
RQList* RQRegularPool::_saveList;							//已存入数据存储容器
RQMap<void*, RQRegularNode*>* RQRegularPool::_idMap;		//数据地址-存储id映射表
RQSpinLock* RQRegularPool::_lock;
time_t		RQRegularPool::_checkStartTime;					//开始检测时

///---节点不在池中
#define M_STATUS_NULL		0
///---节点当前自由、未被占用
#define M_STATUS_FREE		1
///---节点当前正在执行移除
#define M_STATUS_REMOVE	-1
///---节点当前正在执行销毁
#define M_STATUS_DESTORY	-2
///---节点当前期望恢复自由
#define M_STATUS_NEED_FREE	2


void RQRegularPool::InitObj()
{
	static bool needInit = true;
	if (needInit)
	{
		RQThreadPool::CreatePool(0);
		_idMap = new RQMap<void*, RQRegularNode*>(new RQSpinLock);
		_saveList = new RQList(new RQSpinLock);
		_checkThread = new RQThread;

		_checkThread->SetFun(_ThreadFun, 0);	///注册时间监听函数
		_checkThread->Weak();					///唤醒线程，开始执行

		needInit = false;
	}
}

void RQRegularPool::Register(void* label,void * param,
	RQRegularPool::CallbackFun_Type fun, unsigned int timeOutMs,
	bool allowActive, bool isLoop)
{
	if (param&&fun)
	{
		bool needSave = false;
		RQRegularNode*& regularNode = (*_idMap)[label];
		if (!regularNode)
		{
			regularNode = RecycleBin::GetFreeRQRegularNode();
			regularNode->_label = label;
			needSave = true;
		}

		///更新节点设置数据
		regularNode->_status = M_STATUS_FREE;

		regularNode->_funAddr = fun;
		regularNode->_timeOutMs = regularNode->_timeOutMs = timeOutMs;
		regularNode->_isLoop = isLoop;
		regularNode->_allowActive = allowActive;
		regularNode->_registerTimeMs = RQTime().GetMSecVal();				///记录当前注册/修改时间
		regularNode->_param = param;

		if(needSave)
			_saveList->push_back(regularNode);
	}
}

void * RQRegularPool::Remove(void * label)
{
	void* funAddr = 0;
	auto iter = _idMap->Find(label);					///查找并获取定时节点
	RQRegularNode* regularNode = iter != _idMap->end() ? iter->second : 0;
	if (regularNode)		///检测数据对应的定时节点有效并且还未被回收
	{
		int status = regularNode->_status;				
		regularNode->_status = M_STATUS_REMOVE;			///标识节点已准备回收（防止执行欲回收节点）
		regularNode->_isLoop = false;					///取消任务的循环定时处理
		_idMap->Remove(iter);							///将数据从记录表中移除
	}
	return funAddr;
}

void RQRegularPool::Active(void * label)
{
	if (label)
	{
		auto iter = _idMap->Find(label);				///查找并获取定时节点
		if (iter != _idMap->end() 
			&& iter->second
			&& iter->second->_allowActive)				///检查是否存在该元素，并检测该元素是否允许被激活
		{
			iter->second->_registerTimeMs = RQTime().GetMSecVal();		///更新数据注册时间
		}
	}
}

void RQRegularPool::_ThreadFun(void * param)
{
	RQRegularNode* regularNode;
	int isOk;
	RQTime nowTime;
	int count = 0;						///当前存储容器中被监事的回调个数
	while (true) 
	{
		count = _saveList->GetCount();
		nowTime.Now();
		for (unsigned int i = 0; i < count; i++)
		{
			regularNode = (RQRegularNode*)_saveList->pop_front();		///获取第一数据存储节点
			if (!regularNode)								///防止由于多线程使用导致的链表中节点个数不一致
			{
				break;										///防止空访问避免资源浪费
			}
			if (M_STATUS_REMOVE != regularNode->_status)	///检测节点是否未被通知已经无效，需要回收处理节点
			{
				isOk = (regularNode->_registerTimeMs + regularNode->_timeOutMs)
					<= nowTime.GetMSecVal();				///计算节点是否超时
				if (isOk)									///检查当前节点是否可执行，
				{
					///存入线程池
					RQThreadPool::Push(_CallFun, regularNode);			///交付给线程池进行执行
				}
				else {
					_saveList->push_back(regularNode);		///当前节点还未超时，将节点退回
				}
			}
			else {
				RecycleBin::RecycleNode(regularNode);
			}
		}
		RQSleepMs(M_REGULAR_STIME_MS);
	}
}

void RQRegularPool::_CallFun(void * param)
{
	RQRegularNode* regularNode = (RQRegularNode*) param;
	if (regularNode->_funAddr)								///检查是否期望执行回收操作
	{
		regularNode->_funAddr(regularNode->_param);			///执行定时任务
	}

	if (M_STATUS_REMOVE != regularNode->_status)
	{
		if (regularNode->_isLoop)							///检查是否需要重复轮入
		{
			regularNode->_registerTimeMs = RQTime().GetMSecVal();							///更新节点活跃时间 
			_saveList->push_back(regularNode);				///存入节点进行监听
			return;
		}
		else {
			_idMap->Remove(regularNode->_label);			///由于该数据未被用户移除，但已过时故自动移除
		}
	}
	RecycleBin::RecycleNode(regularNode);
}
