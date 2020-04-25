#pragma once
#include "RQNode.h"

/*
	该节点被称为空节点，用于提供变长容量的空类型数据存储
*/
class RQMemory;
struct RQNullNode : public RQNode
{
	RQNullNode(int newArySize = 0);
	~RQNullNode();

	virtual void Create(void* param = nullptr);						//数据节点创建
	virtual void Clear();

	void** _ptrAry;					//void*数组地址
	unsigned int _arySize;					//当前数组能容纳void*变量的个数

	/*
		扩展空节点的容量
	*/
	void Resize(int newArySize);

	/*
		根据传入的下标和接口获取对应元素，并转换为特定的值
		若设置的下标超过了当前容量将放弃该操作（返回值为0）
	*/

	void* GetPtr(unsigned int index);
	long GetLong(unsigned int index);

	void SetPtr(unsigned int index, void* val);
	void SetLong(unsigned int index, long val);

private:
	RQMemory* _memory;
};