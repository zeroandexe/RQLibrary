#pragma once
#include "node/RQNode.h"
/*
	该对象为迭代器对象，用于提供RQ中容器的遍历操作
*/

class RQIteratorProxy;
class RQNode;

class RQIterator {
public:
	/*
		无归属的迭代器不进入任何迭代器
	*/
	RQIterator();
	/*
		指定迭代器的归属，并同时指定该迭代器的位置
	*/
	RQIterator(RQIteratorProxy* proxy,RQNode* node);
	/*
		若该迭代器有归属则，从代理中注销该迭代器
	*/
	~RQIterator();

	RQIterator(const RQIterator& other);
	//RQIterator(RQIteratorProxy&& proxy);


	virtual void* Init(void* param = 0);
	virtual void Clear();

	void operator =(const RQIterator& other);
	void operator = (RQIterator&& other);
	bool operator == (const RQIterator& other);
	bool operator == (RQIterator&& other);
	bool operator != (const RQIterator& other);
	bool operator != (RQIterator&& other);

	bool isNull();

	/*
		迭代器位移的抽象增量处理，
		对于离散型迭代器则由节点其内部的链接指针控制
		对于连续性迭代器则对内存地址进行指针长度偏移
	*/
	void operator ++();
	void operator --();
	virtual RQNode*& operator *();

	/*
		传入代理进行认证，检测迭代器是否属于对应的归属代理
	*/
	bool IsWon(RQIteratorProxy* proxy);
protected:
	RQNode* _node;				//由于RQ容器为节点为核心，所以将迭代对象设置为节点对象
	RQIteratorProxy* _proxy;			//该迭代器所归于的容器代理
};