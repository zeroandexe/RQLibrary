#pragma once 
#include "node/RQNode.h"
/*
	该对象为设备的基类对象，用于提供设备管理器使用的约定参数,子类通过继承使用
*/
class RQLoadNode;

class RQDivNode : public RQNode
{
public:
	friend class RQDivManager;
	friend class RQDivContainer;

	virtual void Create(void* param = 0);
	virtual void Clear();  

	virtual void Destory(void* param = 0) = 0;		//设备销毁函数，用于设备未活跃超时设备管理器对设备的超时处理

	void SetDivId(int divId);
	int GetDivId();

	void SetDivType(int divType);
	int GetDivType();

	void SetSock(int sock);
	int GetSock();

	void SetTimeOutMs(unsigned long timeOutMs);
	unsigned long GetTimeOutMs();


	int GetDivStatus(int status);
	bool IsRegister();

private:
	int _divId;					//设备id
	int _divType;				//设备类型
	int _divStatus;				//设备状态
	int _sock;					//设备socket
	bool _isRegister;			//是否已注册入设备管理器
	RQLoadNode* _loadNode;		//任务节点的挂载节点
	unsigned long _timeOutMs;	//任务心跳/活跃超时时间
};