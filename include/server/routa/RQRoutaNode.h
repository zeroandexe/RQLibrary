#pragma once
#include "node/RQNode.h"
#include "RQMap.h"
#include "RQRoutaData.h"
#include "RQRoutaDataEx.h"
#include "RQLoadNode.h"
struct RQRoutaNode : public RQNode
{
	RQRoutaData		_routaData;		//网络节点数据
	RQRoutaDataEx	_dataEx;		//对网络节点扩展的描述数据
	//RQLoadNode*		_addrLoad;		//地址挂在节点
	//RQLoadNode*		_typeLoad;		//类型挂在节点
	RQLoadNode*		_loadNode;		//挂载节点
	virtual void* Init(void* param = 0);
 };