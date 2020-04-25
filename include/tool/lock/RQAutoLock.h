#pragma once

class RQLock;

class RQAutoLock
{
public:
	RQAutoLock(RQLock * lock,bool isRead = true);
	void UnLock();
	~RQAutoLock();
private:
	RQLock * _lock;
	bool _isRead;
	bool _isLock;
};