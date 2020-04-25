#pragma once
#include "define/RQdefine.h"
#include "RQList.h"

class RQLock;
/*
	该类是一个队列，提供数据的先入后出的操作操作
*/

class RQQueue
{
public:
	RQQueue(RQLineStorage* lineStorage);
	~RQQueue();

	void push_front(RQNode* node);		
	void push_back(RQNode* node);

	RQNode* pop_front();				
	RQNode* pop_back();

	void Clear();

	RQLineStorage* GetStorage();

	void Remove(RQNode* node);

	static RQQueue* Create_list(RQLock* lock=M_NULL(RQLock*));				//创建用链表作为数据存储结构的队列

	static void Destroy(RQQueue* queue);

	unsigned int GetCount();
private:
	RQLineStorage * _lineStorage;			//数据结构存储容器
};