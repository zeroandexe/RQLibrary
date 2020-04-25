#pragma once
#include "container/RQLineStorage.h"
#include "container/RQIterator.h"
/*
	该类为链表数据结构提供，临界区的加锁方式
*/

class RQNode;
class RQLock;
///class RQIterator;
///class RQIteratorList;

class RQList : public RQLineStorage
{
public:
	RQList(RQLock* lock=0);
	~RQList();

	virtual void push_back(RQNode*);
	virtual RQNode* pop_back();

	virtual void push_front(RQNode*);
	virtual RQNode* pop_front();

	virtual RQNode* GetNext(RQNode* node = 0);			///获取传入节点的下一个节点

	virtual void Clear();				///回收元素
	virtual void Destroy();				///销毁元素
	virtual unsigned int GetCount();

	virtual void Remove(RQNode*);		///删除除标签节点外的所有数据节点

	virtual RQLock* getLock();

	/*
		对targetNode进行排序处理，并在cmpFun返回true的位置进行插入
		cmpFun：第一个参数为基点，第二参数为欲存入点
	*/
	virtual void InsertByCmp(RQNode* targetNode, bool(*cmpFun)(RQNode*, RQNode*));

	/*
		提供迭代器边界
	*/
	virtual RQIterator begin();
	virtual RQIterator end();

	/*
		将迭代器向头或者向尾移动一个节点单元
	*/
	virtual void AddIteator(RQIterator* iterator);
	virtual void SubIterator(RQIterator* iterator);

	/*
		将迭代器中的节点回收，注意该操作不会处理节点中的数据
	*/
	virtual void Erase(RQIterator* iterator);

	/*
		对内部节点进行循环访问，仅在容器没有元素时返回为空，
		并且被访问节点不会从链表中移除
	*/
	RQNode* LoopVisit();

private:
	///建立prev和next的连接
	///void _Connect(RQNode* prev, RQNode* next);
	void _Insert(RQNode* prev, RQNode* node, RQNode* next);
	void _Remove(RQNode* prev, RQNode* node, RQNode* next);
private:
	RQNode* _visitNode;			//当前被访问的节点
	RQNode* _head;
	RQNode* _end;
	unsigned int _count;
	RQLock* _lock;
	///RQIteratorList* _iter;
};