#pragma once
#include "container/RQLineStorage.h"

/*
	该类为数组类，提供数组相关操作
	删除容器并不会删除里面的元素
	若在数组容日中提供超过容量大小的元素个数将造成泄漏
*/
class RQNode;
class RQLock;

class RQArray : public RQLineStorage
{
public:
	RQArray(unsigned int size, RQLock* lock = 0);
	virtual ~RQArray();
	
	virtual void push_back(RQNode* node);			//将元素添加到尾部
 	virtual RQNode* pop_back();						//将元素从尾部取出

	virtual unsigned int Count();					//查看当前元素的个数

	virtual RQIterator begin();
	virtual RQIterator end();

	//将迭代器向前向前/后偏移一个节点单位
	virtual void AddIteator(RQIterator* iterator);
	virtual void SubIterator(RQIterator* iterator);

	//消除迭代器所指向的节点，并同时将迭代器向前偏移
	virtual void Erase(RQIterator* iterator);

private:
	unsigned int _size;			//数组大小
	RQLock* _lock;				
	RQNode** _array;			//元素存储数组
	unsigned int _headIndex;	//头存入下标
	unsigned int _endIndex;		//尾存入下标
	unsigned int _count;		//容器中元素个数
};

