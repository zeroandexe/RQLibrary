#pragma once
#include "RQIterator.h"
/*
	本内是数据存储结构的父类，用于对子类的存储进行统一化
	有效节点：除作为头尾的标记节点外的其他节点
*/
class RQNode;				//声明节点类型
class RQLock;

class RQLineStorage
{
public:
	RQLineStorage();
	virtual ~RQLineStorage();

	//不带锁尾端操作
	virtual void push_back(RQNode*);
	virtual RQNode* pop_back() ;

	//不带锁，前端操作
	virtual void push_front(RQNode*);
	virtual RQNode* pop_front() ;
	
	//清除容器
	virtual void Clear();

	//获取当前有效元素个数
	virtual unsigned int GetCount() ;

	//从存储容器中移除目标node，但是不会回收node的内存空间
	virtual void Remove(RQNode*);
	//virtual void remove(RQIterator*) = 0;

	//将容器的所有有效节点移除，但是不会回收节点的内存空间
	virtual void removeAll();

	//获取节点中的锁对象
	virtual RQLock* getLock();

	virtual RQIterator begin() = 0;
	virtual RQIterator end() = 0;

	//将迭代器向前向前/后偏移一个节点单位
	virtual void AddIteator(RQIterator* iterator) = 0;
	virtual void SubIterator(RQIterator* iterator) = 0;

	//消除迭代器所指向的节点，并同时将迭代器向前偏移
	virtual void Erase(RQIterator* iterator) = 0;
protected:
	RQIteratorProxy* _proxy;
};