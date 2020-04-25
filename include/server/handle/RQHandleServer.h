#pragma once

/*
	该类作为所有期望提供句柄服务的父类，提供句柄的清理接口
*/
class RQHandle;
class RQHandleServer {
public:
	virtual void DestrayHandle(RQHandle* handle) = 0;	//定义句柄回收的通用接口 
};