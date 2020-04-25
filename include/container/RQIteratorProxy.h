#pragma once
#include "RQSpinLock.h"
#include "RQLoadNode.h"

/*
	该对象为迭代器和容器之间相互作用的中介，即维护迭代器的正确性，由提供了迭代器的正确处理

	注意，由于目前容器中还未添加非线性存储结构，所以这里仅将线性容器作为存储结构
*/

class RQLineStorage;
class RQIterator;

class RQIteratorProxy {
public:
	RQIteratorProxy(RQLineStorage* storage);
	~RQIteratorProxy();
	/*
		将容器所拥有的迭代器加入到存储链表中
	*/
	virtual void Insert(RQIterator* iterator);
	/*
		从链表中移除指定的迭代器
	*/
	virtual void Erase(RQIterator* iterator);
	/*
		增加迭代器一个单元偏量，具体实现由迭代器所归属的容器决定
	*/
	void AddIterator(RQIterator* iterator);
	/*
		减少迭代器一个单元偏量，同上
	*/
	void SubIterator(RQIterator* iterator);

private:
	RQSpinLock* _lock;				//防止多线程操作带来的数据损坏
	RQLineStorage* _storage;		//被代理的线性存储容器
	RQLoadNode* _iteratorList;		//迭代器存储链
};