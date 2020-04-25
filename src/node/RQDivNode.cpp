#include "node/RQDivNode.h"
#include "define/RQdefine.h"

void RQDivNode::Create(void * param)
{
	Clear();
}

void RQDivNode::Clear()
{
	_divId = 0;
	_divType = 0;
	_divStatus = M_DIV_STATUS_INVALID;
	_sock = 0;
	_isRegister = false;
	_loadNode = 0;
}

void RQDivNode::SetDivId(int divId)
{
	if (!_isRegister)
	{
		_divId = divId;
	}
}

int RQDivNode::GetDivId()
{
	return _divId;
}

void RQDivNode::SetDivType(int divType)
{
	if (!_isRegister)
	{
		_divType = divType;
	}
}

int RQDivNode::GetDivType()
{
	return _divType;
}

void RQDivNode::SetTimeOutMs(unsigned long timeOutMs)
{
	if (!_isRegister)
	{
		_timeOutMs = timeOutMs;
	}
}

unsigned long RQDivNode::GetTimeOutMs()
{
	return _timeOutMs;
}

int RQDivNode::GetDivStatus(int status)
{
	return _divStatus;
}

void RQDivNode::SetSock(int sock)
{
	if (!_isRegister)
	{
		_sock = sock;
	}
}

int RQDivNode::GetSock()
{
	return _sock;
}

bool RQDivNode::IsRegister()
{
	return _isRegister;
}
