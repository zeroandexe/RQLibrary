#include "RQRecyBin.h"
#include "RQSpinLock.h"
#include "RecycleBin.h"
//其他公用模块
#include "tool/RQMemory.h"

//编译器模块
#include "compiler/RQWord.h"
#include "RQVar.h"
#include "compiler/RQScriptFile.h"
#include "compiler/RQSentence.h"
#include "compiler/RQASTNode.h"
#include "compiler/RQCodeSet.h"
#include "compiler/RQLine.h"
#include "compiler/RQExpression.h"
#include "RQLoadNode.h"
#include "node/RQFunNode.h"
#include "RQPair.h"
#include "RQHandle.h"
#include "RQMutex.h"
#include "node/RQRegularNode.h"
#include "RQHandlePool.h"
//通讯模块
#include "RQSession.h"
#include "RQPack.h"
#include "node/RQSessNode.h"
#include "node/RQAliveNode.h"
#include "node/RQNetParamNode.h"

//json
#include "tool/json/RQJsonVal.h"
#include "tool/json/RQJsonMap.h"
#include "tool/json/RQJsonArray.h"

#include "node/RQNullNode.h"
#include "node/RQSmartNode.h"

static void* memoryVirAddr		= 0;
static void* sessionVirAddr		= 0;
static void* wordVirAddr		= 0;
static void* varVirAddr			= 0;
static void* scriptFileVirAddr	= 0;
static void* sentenceVirAddr	= 0;
static void* astNodeVirAddr		= 0;
static void* codeSetVirAddr		= 0;
static void* lineVirAddr		= 0;
static void* expressionVirAddr	= 0;
static void* loadNodeVirAddr	= 0;
static void* funNodeVirAddr		= 0;
static void* buffNodeVirAddr	= 0;
static void* pairVirAddr		= 0;
static void* handleVirAddr		= 0;
static void* regularNodeVirAddr = 0;
static void* handlePoolVirAddr	= 0;
static void* sessNode			= 0;
static void* aliveNode			= 0;
static void* netParamNode		= 0;

static void* jsonVal			= 0;
static void* jsonMap			= 0;
static void* jsonArray			= 0;

static void* packVirAddr		= 0;

static void* nullNode			= 0;
static void* smartNode			= 0;

//---创建对应类型的回收站
#define GET_VIR(varVirAddr,type,varNode)	\
{											\
	varNode=  new type();					\
	varNode->Init();						\
	varVirAddr = *(void**)varNode;			\
	RQRecyBin::Create(varVirAddr,sizeof(type),new RQSpinLock);	\
}

#define GET_OBJ(type,virAddr)	(type*)RQRecyBin::GetFree(virAddr)

//---return 一个对应类型的空闲变量
#define RETURN_FREE_VAR(varVirAddr,type)			\
{													\
	type* retNode;									\
	if (varVirAddr)									\
	{												\
		return GET_OBJ(type, varVirAddr);			\
	}												\
	else {											\
		GET_VIR(varVirAddr, type, retNode);			\
		return retNode;								\
	}												\
}



void RecycleBin::InitObj()
{
	RQRecyBin::Init();
}

void RecycleBin::RecycleNode(RQNode * node)
{
	RQRecyBin::Recycle(node);
}

RQWord * RecycleBin::GetFreeRQWord()
{
	RETURN_FREE_VAR(wordVirAddr, RQWord);
}

void * RecycleBin::GetVirAddrRQWord()
{
	return wordVirAddr;
}

RQLoadNode * RecycleBin::GetFreeRQLoadNode()
{
	RETURN_FREE_VAR(loadNodeVirAddr, RQLoadNode);
}

void * RecycleBin::GetVirAddrRQLoadNode()
{
	return loadNodeVirAddr;
}

RQVar * RecycleBin::GetFreeRQVar()
{
	RETURN_FREE_VAR(varVirAddr, RQVar);
}

void * RecycleBin::GetVirAddrVar()
{
	return varVirAddr;
}


RQFunNode * RecycleBin::GetFreeRQFunNode()
{
	RETURN_FREE_VAR(funNodeVirAddr, RQFunNode);
}

void * RecycleBin::GetVirAddrRQFunNode()
{
	return funNodeVirAddr;
}

RQHandlePool * RecycleBin::GetFreeRQHandlePool()
{
	RETURN_FREE_VAR(handlePoolVirAddr, RQHandlePool);
}

void * RecycleBin::GetVirAddrRQHandlePool()
{
	return handlePoolVirAddr;
}


RQHandle * RecycleBin::GetFreeRQHandle()
{
	RETURN_FREE_VAR(handleVirAddr, RQHandle);
}

void * RecycleBin::GetVirAdrRQHandle()
{
	return handleVirAddr;
}

RQPair * RecycleBin::GetFreeRQPair()
{
	RETURN_FREE_VAR(pairVirAddr, RQPair);
}

void * RecycleBin::GetVirAddrRQPair()
{
	return pairVirAddr;
}

RQRegularNode * RecycleBin::GetFreeRQRegularNode()
{
	RETURN_FREE_VAR(regularNodeVirAddr, RQRegularNode);
}

void * RecycleBin::GetVirAddrRQRegularNode()
{
	return regularNodeVirAddr;
}

void* RecycleBin::CreateTS()
{
	return RQRecyBin::CreateTS();
}

void RecycleBin::RecycleTS(void* tsId)
{
	RQRecyBin::RecycleTS(tsId);
}

void RecycleBin::TemStorage(void* tsId, RQNode * node)
{
	RQRecyBin::TemStorage(tsId, node);
}

void RecycleBin::RemoveFromTs(void * tsId, RQNode * node)
{
	RQRecyBin::RemoveFromTs(tsId, node);
}

void RecycleBin::RecycleTemStorage(void* tsId)
{
	RQRecyBin::RecycleTSNode(tsId);
}

RQScriptFile * RecycleBin::GetFreeRQScriptFile()
{
	RETURN_FREE_VAR(scriptFileVirAddr, RQScriptFile);
}

RQSentence * RecycleBin::GetFreeRQSentence()
{
	RETURN_FREE_VAR(sentenceVirAddr, RQSentence);
}

RQASTNode * RecycleBin::GetFreeRQASTNode()
{
	RETURN_FREE_VAR(astNodeVirAddr, RQASTNode);
}

RQCodeSet * RecycleBin::GetFreeRQCodeSet()
{
	RETURN_FREE_VAR(codeSetVirAddr, RQCodeSet);
}

RQLine * RecycleBin::GetFreeRQLine()
{
	RETURN_FREE_VAR(lineVirAddr, RQLine);
}

RQExpression * RecycleBin::GetFreeRQExpression()
{
	RETURN_FREE_VAR(expressionVirAddr, RQExpression);
}

RQMemory * RecycleBin::GetFreeRQMemory()
{
	RETURN_FREE_VAR(memoryVirAddr, RQMemory);
}

RQSession * RecycleBin::GetFreeRQSession()
{
	RETURN_FREE_VAR(sessionVirAddr, RQSession);
}

RQPack * RecycleBin::GetFreeRQPack()
{
	RETURN_FREE_VAR(packVirAddr,RQPack);
}

RQSessNode * RecycleBin::GetFreeRQSessNode()
{
	RETURN_FREE_VAR(sessNode, RQSessNode);
}

RQAliveNode * RecycleBin::GetFreeRQAliveNode()
{
	RETURN_FREE_VAR(aliveNode, RQAliveNode);
}

RQNetParamNode * RecycleBin::GetFreeRQNetParamNode()
{
	RETURN_FREE_VAR(netParamNode, RQNetParamNode);
}

RQJsonVal * RecycleBin::GetFreeRQJsonVal()
{
	RETURN_FREE_VAR(jsonVal, RQJsonVal);
}

RQJsonMap * RecycleBin::GetFreeRQJsonMap()
{
	RETURN_FREE_VAR(jsonMap, RQJsonMap);
}

RQJsonArray * RecycleBin::GetFreeRQJsonArray()
{
	RETURN_FREE_VAR(jsonArray, RQJsonArray);
}

RQNullNode* RecycleBin::GetFreeRQNullNode()
{
	RETURN_FREE_VAR(nullNode, RQNullNode);
}

RQSmartNode * RecycleBin::GerFreeRQSmartNode()
{
	RETURN_FREE_VAR(smartNode, RQSmartNode);
}
