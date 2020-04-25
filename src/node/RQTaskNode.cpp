#include "node/RQTaskNode.h"

void RQTaskNode::Create(void * param)
{
	Clear();
}

void RQTaskNode::Clear()
{
	_opterId = 0;
	_opterType = 0;
	_timeOutMs = 0;
	_taskType = 0;
	_isRegister = false;
	_loadNode = 0;

	_exescriptName.clear();
	_destroyScriptName.clear();
}

void RQTaskNode::SetOpterId(int opterId)
{
	if (!_isRegister)
	{
		_opterId = opterId;
	}
}

int RQTaskNode::GetOpterId()
{
	return _opterId;
}

void RQTaskNode::SetOpterType(int opterType)
{
	if (!_isRegister)
	{
		_opterType = opterType;
	}
}

int RQTaskNode::GetOpterType()
{
	return _opterType;
}

void RQTaskNode::SetTaskType(int taskType)
{
	if (!_isRegister)
	{
		_taskType = taskType;
	}
}

int RQTaskNode::GetTaskType()
{
	return _taskType;
}

void RQTaskNode::SetTimeOutMs(int timeOutMs)
{
	if (!_isRegister)
	{
		_timeOutMs = timeOutMs;
	}
}

int RQTaskNode::GetTimeOutMs()
{
	return _timeOutMs;
}

void RQTaskNode::SetExeScriptName(const char * scriptName)
{
	if (!_isRegister&&scriptName)
	{
		_exescriptName = scriptName;
	}
}

void RQTaskNode::SetExeScriptName(const std::string & scriptName)
{
	if (!_isRegister)
	{
		_exescriptName = scriptName;
	}
}

const std::string & RQTaskNode::GetExeScriptName()
{
	return _exescriptName;
}

void RQTaskNode::SetDestroyScriptName(const char * scriptName)
{
	if (!_isRegister&&scriptName)
	{
		_destroyScriptName = scriptName;
	}
}

void RQTaskNode::SetDestroyScriptName(const std::string & scriptName)
{
	if (!_isRegister)
	{
		_destroyScriptName = scriptName;
	}
}

const std::string & RQTaskNode::GetDestroyScriptName()
{
	return _destroyScriptName;
}

bool RQTaskNode::IsRegister()
{
	return _isRegister;
}
