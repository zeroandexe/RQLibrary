#pragma once
#include "node/RQAliveNode.h"
/*
	数据活跃栈，将按照数据的活跃性进行排序，
	按照pop要求输出的活跃节点永远是匹配要求中最新的活跃点
*/

class RQDBStack;
class RQList;
class RQLock;

class RQAliveStack
{
public:
	RQAliveStack();
	~RQAliveStack();


	/*
		获取还未达到死亡时间的节点，被获取的节点将离开该容器
		aliveTimeMs	[in]	活跃时间毫秒，
				若为0则表示不在乎当前节点是否死亡
		return		[out]	按照活跃要求，没有活跃节点则返回空
	*/
	RQAliveNode* pop_alive(unsigned int aliveTimeSec);
	/*
		获取以及死亡的节点，被获取的节点将离开该容器
		aliveTimeMs	[in]	活跃时间毫秒，
				若为0则表示不在乎当前节点是否活跃
		return		[out]	按照活跃要求，没有死亡节点则返回空
	*/
	RQAliveNode* pop_dead(unsigned int aliveTimeSec);

	/*
		保存活跃节点的最新状态
	*/
	void push(RQAliveNode* node);
	/*
		将节点从活跃栈中移除
	*/
	void Remove(RQAliveNode* node);

	/*
		获取当前存入节点的数量统计
	*/
	int Count();

	/*
		所要一个空闲的活跃节点用于存储
	*/
	static RQAliveNode* GetFreeRQAliveNode();

	/*
		回收活跃栈中的所有节点
	*/
	void RecycleRQAliveStack(RQAliveStack* aliveStack);

	/*
		回收不再使用的活跃节点
	*/
	void RecyclRQAliveNode(RQAliveNode* node);

private:
	RQLock* _lock;
	RQList* _saveList;					///活跃节点存储链表
};