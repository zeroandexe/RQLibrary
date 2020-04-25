#include "RQHandle.h"
#include "server/handle/RQHandleServer.h"

void RQHandle::Create(void* param)
{
	Clear();
}

void RQHandle::Clear()
{
	_handlServer = 0;
	_handle._long = 0;

	if (_handlServer)			///检查当前句柄是否具有句柄服务
	{
		_handlServer->DestrayHandle(this);
	}
}