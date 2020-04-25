#pragma once
#include "define/RQdefine.h"
#include "node/RQNode.h"

class RQLock : public RQNode
{
public:
	RQLock() {};
	virtual ~RQLock() {};
	/*
		加锁函数，参数仅为读锁/或写锁提供，若锁为读写锁则该参数无效
	*/
	virtual void Lock(bool isRead = true) = 0;
	/*
		加锁函数，参数仅为读锁/或写锁提供，若锁为读写锁则该参数无效
	*/
	virtual void Unlock(bool isRead = true) = 0;
};