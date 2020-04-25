#pragma once
#include "node/RQNode.h"
#include <string>
/*
	该节点是对任务的抽象，作为所有任务节点的基类
*/
class RQLoadNode;
class RQDivNode;
class RQTaskNode : public RQNode
{
public:
	friend class RQTaskManager;
	friend class RQTaskContainer;

	virtual void Create(void* param = 0);
	virtual void Clear();

	//参数为设备节点指针
	virtual void Execute(RQDivNode* param) = 0;	//若为C处理，则执行设置的对象任务处理
	virtual void Destroy(RQDivNode* param) = 0;	//若为C处理，则执行任务销毁行为（该函数应当必须填写）

	void SetOpterId(int opterId);
	int GetOpterId();

	void SetOpterType(int opterType);
	int GetOpterType();

	void SetTaskType(int taskType);
	int GetTaskType();

	void SetTimeOutMs(int timeOutMs);
	int GetTimeOutMs();

	void SetExeScriptName(const char* scriptName);
	void SetExeScriptName(const std::string& scriptName);
	const std::string& GetExeScriptName();

	void SetDestroyScriptName(const char* scriptName);
	void SetDestroyScriptName(const std::string& scriptName);
	const std::string& GetDestroyScriptName();

	bool IsRegister();

private:
	int _opterId;							//指定特定执行者id
	int _opterType;							//非指定执行者，但给予特定某类执行者
	///int _taskId;							//任务id，身份识别
	int _taskType;							//该任务的类型的描述
	unsigned long _timeOutMs;				//任务失效的超时时间
	std::string _exescriptName;				//若为脚本执行，则为执行脚本的名称
	std::string _destroyScriptName;			//若为脚本执行，则为执行销毁任务的脚本名称
	/*
		管理器注册标记变量，若任务注册那么该标签将被设置为true,
		此时除管理器外任务对任务节点信息的修改都将失败
	*/
	bool _isRegister;			
	RQLoadNode* _loadNode;					//挂载该任务节点的挂载节点
};