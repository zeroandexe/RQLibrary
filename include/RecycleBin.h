#pragma once
/*
	该对象是回收站对象，用于回收整个系统中的所有对象和对象的分配
	被回收站管理的所有对象都必须具备虚表地址

	（主要用于容纳那些用于通用容器的节点对象
		或者用于容纳混合回收的节点对象）
*/

class RQPair;				//多类型变量对
class RQNode;				//基类节点对象
class RQWord;				//词源对象
class RQVar;				//脚本变量对象
class RQScriptFile;			//脚本文件对象
class RQSentence;			//脚本语句行对象
class RQASTNode;			//语法树节点
class RQCodeSet;			//代码集对象
class RQLine;				//代码集行对象
class RQExpression;			//表达式对象
class RQKeyNode;			//关键字节点
class RQLoadNode;			//挂载节点
class RQEpollInfo;			//epoll中使用的数据节点
class RQFunNode;			//函数节点对象
class RQBuffNode;			//缓冲区节点
class RQHandle;				//句柄对象
class RQMutex;				//互斥锁对象
class RQIterator;			//迭代器
class RQRegularNode;		//定时器节点对象
class RQMemory;				//内存对象
class RQSession;			//连接会话对象
class RQHandlePool;			//句柄池
class RQPack;				//数据包对象
class RQSessNode;			//会话数据包对象
class RQAliveNode;			//活跃节点
class RQNetParamNode;		//路由网络参数节点

class RQJsonVal;			//json的数据对象
class RQJsonMap;			//json的表容器对象
class RQJsonArray;			//json的数组容器对象
class RQNullNode;			//空节点对象

class RQSmartNode;			//智能指针对象

class RecycleBin
{
public:
	
	static void InitObj();

	//回收节点对象，并对node执行clear操作
	static void RecycleNode(RQNode* node);

	//索要一个可用的元词对象
	static RQWord* GetFreeRQWord();
	static void* GetVirAddrRQWord();

	//获取一个空闲的挂载节点
	static RQLoadNode* GetFreeRQLoadNode();
	static void* GetVirAddrRQLoadNode();

	//索要一个可用的脚本变量对象
	static RQVar* GetFreeRQVar();
	static void* GetVirAddrVar();


	//索要一个函数节点
	static RQFunNode* GetFreeRQFunNode();
	static void* GetVirAddrRQFunNode();

	//所要一个函数节点
	static RQHandlePool* GetFreeRQHandlePool();
	static void* GetVirAddrRQHandlePool();

	//向回收栈索要你个句柄对象
	static RQHandle* GetFreeRQHandle();
	static void* GetVirAdrRQHandle();
	
	//索要一个pair节点
	static RQPair* GetFreeRQPair();
	static void* GetVirAddrRQPair();
	
	//索要一个定时器节点
	static RQRegularNode* GetFreeRQRegularNode();
	static void* GetVirAddrRQRegularNode();

	//创建暂存器并获取暂存器id
	static void* CreateTS();

	//回收不使用的暂存器
	static void RecycleTS(void* tsId);

	//暂存器，用于存储不需要但是又暂时需要的变量
	//tsId，暂存器的识别id，不同的id就是不同的暂存器
	static void TemStorage(void* tsId, RQNode* node);

	//从暂存器中移除被存储的废弃节点
	static void RemoveFromTs(void* tsId, RQNode* node);

	//根据给定的暂存器识别id，回收该暂存器中的所有存储元素
	static void RecycleTemStorage(void* tsId);

	//索要一个脚本文件对象
	static RQScriptFile* GetFreeRQScriptFile();

	//索要一个语句对象
	static RQSentence* GetFreeRQSentence();

	//索要一个语法树节点
	static RQASTNode* GetFreeRQASTNode();

	//索要一个可用的代码集对象
	static RQCodeSet* GetFreeRQCodeSet();

	//索要一个代码行对象
	static RQLine* GetFreeRQLine();

	//索要一个表达式对象
	static RQExpression* GetFreeRQExpression();

	//索要一个内存对象
	static RQMemory* GetFreeRQMemory();

	//索要一个会话对象
	static RQSession* GetFreeRQSession();

	//索要一个数据包对象
	static RQPack* GetFreeRQPack();

	//索要一个会话数据存储对象
	static RQSessNode* GetFreeRQSessNode();
	//索要一个活跃站节点
	static RQAliveNode* GetFreeRQAliveNode();
	//索要一个路由网络参数节点
	static RQNetParamNode* GetFreeRQNetParamNode();
	//获取一个json的键值对对象
	static RQJsonVal* GetFreeRQJsonVal();
	//获取一个Json的map对象
	static RQJsonMap* GetFreeRQJsonMap();
	//获取一个Json的数组对象
	static RQJsonArray* GetFreeRQJsonArray();
	//索要一个无类型数据节点
	static RQNullNode* GetFreeRQNullNode();
	//索要一个智能指针节点
	static RQSmartNode* GerFreeRQSmartNode();
};