#include "node/RQNetParamNode.h"

void RQNetParamNode::Create(void * param)
{
	Clear();
}

void RQNetParamNode::Clear()
{
	_routaRole = 0;
	_pack = 0;
	_optProt = 0;
	_optDecor = 0;
	_recvFun = 0;
	_recvParam = 0;
	_destoryFun = 0;
	_destoryParam = 0;
	_sessTimeOutSec = 0;
}

