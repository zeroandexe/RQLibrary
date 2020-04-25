#include "server/routa/RQRoutaRecycle.h"
#include "server/routa/RQRoutaNode.h"
#include "RQSpinLock.h"

RQDBStack* RQRoutaRecycle::_dbStack = 0;

RQRoutaRecycle::RQRoutaRecycle()
{
	if (!_dbStack)
	{
		RQRoutaNode* node = new RQRoutaNode;
		_dbStack = new RQDBStack(new RQSpinLock, sizeof(RQRoutaNode), node);
	}
}

RQRoutaNode * RQRoutaRecycle::GetFreeRQRoutaNode()
{
	return (RQRoutaNode*) _dbStack->pop();
}

void RQRoutaRecycle::RecycleRQRoutaNode(RQRoutaNode * node)
{
	_dbStack->push(node);
}
