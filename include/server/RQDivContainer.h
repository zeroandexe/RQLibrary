#pragma once
#include "RQMap.h"
#include "RQSpinLock.h"
class RQList;
/*
	该对象为设备管理器的存储对象，用设备资源的保存和获取等操作，作为单例工厂处理
*/
class RQDivNode;

class RQDivContainer 
{
private:
	struct Three {
		RQSpinLock	_lock;
		RQMap<int, RQDivNode*> _divIdMap;	//设备id映射表
		RQList*		_divFreeList;				//设备存储链表
	};
private:
	friend class RQDivManager;
	/*
		将设备存入容器，此时并任务设备可用
		若该设备为重复id设备，则返回false
	*/
	static bool Insert(RQDivNode* divNode);
	static void Remove(RQDivNode* divNode);
	static RQDivNode* GetDivByTypeId(int divType, int divId);
	static RQDivNode* GetDivByType(int divType);
	static void ChangeSock(RQDivNode* divNode, int socket);
	/*
		通知存储器该设备执行任务结束，准备进入空闲设备队列
	*/
	static void TaskOver(RQDivNode* divNode);
	/*
		通知存储器，该设备将要执行任务，应当从空闲设备队列中离开
	*/
	static void WillRunTask(RQDivNode* divNode);
private:
	static RQDivNode* _GetDivByTypeId(int divType, int divId);
	static RQDivNode* _GetDivById(Three& third, int divId);

	/*
		将设备节点存入到设备类型表中，若该设备id被使用则将返回当前使用该id的设备
	*/
	static RQDivNode* _InsertInTypeMap(RQDivNode*divNode);		
	static void _InsertInSockMap(RQDivNode* divNode);		//将设备节点依照当前sock存入设备表中
	static void _RemoveFromTypeMap(RQDivNode* divNode);
	static void _RemoveFromSockMap(RQDivNode* divNode);
private:
	/*
	类型设备表，采用以设备类型为主键，设备实际存储数据结构为值的表
*/
	static RQMap<int, Three> _typeDivMap;
	/*
		socket-设备表，采用socket为主键设备节点为值的用于socket更新使用的表
	*/
	static RQMap<int, RQDivNode*> _sockDivMap;
};