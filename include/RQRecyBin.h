#pragma once
#include <map>
#include "RQSpinLock.h"
#include "RQDBStack.h"
#include "RQQueue.h"
/*
	本类是所有RQNode子节点的回收类，
*/

class RQRecyBin
{
public:

	//初始化回收类
	static void Init();

	//通过虚拟地址创建回收器
	static void Create(void* virAddr, unsigned int size, RQLock* lock);		
	//通过节点对象创建回收器，并回收节点
	static void Create(RQNode* node, unsigned int size, RQLock* lock);
	//创建一个暂存器
	static void* CreateTS();

	//将节点存入暂存器
	static void TemStorage(void* tsId, RQNode* node);

	//将废弃节点从暂存器中移除
	static void RemoveFromTs(void* tsId, RQNode* node);

	//回收指定暂存器中的存储节点
	static void RecycleTSNode(void* tsId);

	//回收暂存器
	static void RecycleTS(void* tsId);

	static void Recycle(RQNode* node);
	static RQNode* GetFree(void* virAddr);

private:
	//将tsId转换为队列对象
	static RQQueue* TsToQue(void* tsId);

private:
	static std::map<void*, RQDBStack*>* _recyBin;				//回收站表
	static RQList* _freeTemStorage;								//暂存器
	static RQDBStack* _loadNodeDbStack;							//加载节点回收站
	static RQSpinLock* _lock;									//防止创建暂存器时出现竞争
	static int _nextTsId;										//下一个暂存器id
};