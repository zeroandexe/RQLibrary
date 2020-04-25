#pragma once
#include "RQMap.h"
#include "RQSpinLock.h"
#include "node/RQDivNode.h"
#include "node/RQTaskNode.h"

class RQList;
/*
	该对象为设备管理器为单例工厂模式，
	1、提供外界设备的统一存储和检索和设备对任务的匹配
	2、提供连接设备和socket的对应
	3、提供连接设备活跃性的失效超时设置和激活处理
*/
class RQDivManager
{
private:
	struct Three {
		RQSpinLock	_lock;
		RQMap<int, RQDivNode*> _divIdMap;	//设备id映射表
		RQList*		_divFreeList;				//设备存储链表
	};
public:
	/*
		根据设备节点中设置的设备id和设备Type对设备进行存储
		divType和divId	：作为设备存储、检索和任务匹配时的主要依据
		socket			：作为以socket查找和存储设备时的主键
		timeOutMs		：作为设备是否存入定时池的依据
							>0则存入设备池
							<=0则表示该设备永久有效，不进行有效检测
		return [out]	-1	：传入的设备的id已经被使
						-2	：参数为空或者已设备节点已存在存储容器
						0	：操作成功
		注意：
		1、若设备节点进入管理器，那么设备节点将不可被外界修改   
		2、第一次进入设备管理器的节点都将视为无任务节点，即处于M_DIV_STATUS_FREE状态。
		
	*/
	static void Insert(RQDivNode* divNode);					//根据设备节点中的参数将存储设备节点

	/*
		将设备从任务管理器中移除，若设备存在定时检测那么也同样取消对设备的检测
		并取消设备管理器的管制（即，外界可对设备管进行修改）
		移除操作不会产生设备的销毁调用，也不会更改设备内置数据
	*/
	static void Remove(RQDivNode* divNode);
	static void Remove(int divType, int divId);

	/*
		根据传入的设备参数获取设备节点，但不修改设备的当前状态，同时该设备将从设备管理器中移除
		divType	[in]	期望获取的设备类型
		divId	[in]	期望获取的指定设备，若不填写则随机获取该类型的一个设备
		return	[out]	容器中还有这样的设备，那就返回该设备若没有则返回空
	*/
	static RQDivNode* GetDivByTypeId(int divType, int divId);			
	static RQDivNode* GetDivByType(int divType);
	/*
		更换指定设备的socket值，若该socket存在旧设备，那么旧设备的socket将被置空
	*/
	static void ChangeSock(RQDivNode* divNode, int socket);
	static void ChangeSock(int divType, int divId,int socket);
	/*
		针对于需要激活的设备节点,若设备不属于设备管理器将不进行处理
		激活不会更改设备的超时设置，仅重置设备无效倒计时
	*/
	static void ActiveDivNode(RQDivNode* divNode);
	static void ActiveDivNode(int divType, int divId);

	/*
		通知设备管理器任务执行完成，设备可恢复到可执行状态
	*/
	static void TaskOver(RQDivNode* divNode);
	static void TaskOver(int divType, int divId);

private:
	static RQTaskNode* _FindTask(int divType, int divId);				//传入设备的基本参数获取符合该设备的任务
	static bool _MatchTask(RQDivNode* divNode);				//根据该任务参数在任务管理器中匹配任务
	static void _ActiveDivNode(RQDivNode* saveDiv, RQDivNode* targetDiv);
private:
	static void _DestoryDiv(void* param);		//设备销毁接口函数
};