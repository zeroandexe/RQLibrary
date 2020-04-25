#pragma once
#include "define/RQdefine.h"
#include "RQStack.h"

class RQLock;

class RQDBStack
{
public:
	//创建样板节点node来获取大小和虚拟地址
	RQDBStack(RQLock* lock, unsigned int size,RQNode* node);
	//根据传入的大小和虚拟地址进行设定
	RQDBStack(RQLock* lock, unsigned int size, void* virAddr);
	~RQDBStack();

	void push(RQNode* node);			//数据对象入栈操作
	RQNode* pop();					//数据对象出栈操作，当容器中没有对象时，构建对象

	void clear();
	RQStack* GetStorage();
	
	unsigned int GetCount();

private:
	RQNode * _createNode();

private:
	void* _virAddr;				//设置数据对象的虚拟地址
	unsigned int _size;			//设置数据对象需要的内存大小
	RQStack* _stack;	//设置本类的数据结构形式
};