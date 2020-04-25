#include "RQArray.h"

RQArray::RQArray(unsigned int size,RQLock* lock)
{
	_size = size;
	_lock = lock;
	_array = new RQNode*[_size];
	_headIndex = 0;
	_endIndex = 0;
	_count = 0;
}

RQArray::~RQArray()
{
	delete[] _array;
}

void RQArray::push_back(RQNode * node)
{
	int isOk = (!!node)*(_endIndex - _size);
	if (isOk)			///检查当前是否还有可写入空间
	{
			_array[_endIndex++] = node;
			_count++;
	}
}

RQNode* RQArray::pop_back()
{
	RQNode* retNode = 0;
	if (_endIndex>0)						///检测元素是否存在，若存在元素则调整尾写入下标
	{
		retNode = _array[--_endIndex];
		_count--;
	}
	return retNode;
}

unsigned int RQArray::Count()
{
	return _count;
}


//-----------------------
RQIterator RQArray::begin()
{
	return RQIterator(_proxy, 0);
}
RQIterator RQArray::end()
{
	return RQIterator(_proxy, 0);
}

//将迭代器向前向前/后偏移一个节点单位
void RQArray::AddIteator(RQIterator* iterator)
{

}
void RQArray::SubIterator(RQIterator* iterator)
{

}

void RQArray::Erase(RQIterator* iterator)
{

}