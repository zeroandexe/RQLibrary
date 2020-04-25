#include "RQInit.h"
#include "RecycleBin.h"
#include "compiler/RQLexical.h"
#include "compiler/RQScriptTable.h"
#include "compiler/RQCSC.h"
#include "compiler/RQExpressionCompute.h"
#include "compiler/RQGrammar.h"
#include "compiler/RQEnviVarTable.h"
#include "RQVar.h"
#include "script/ESInit.h"
#include "RQCommunication.h"
#include "RQRigout.h"
#include "RQRegularPool.h"

//初始化全局资源
void RQInit_Globle()
{
	RecycleBin::InitObj();
}

//初始化编译器
void RQInit_Compiler()
{
	RQEnviVarTable::InitObj();			//初始化环境变量表
	RQLexical::InitObj();				//初始化词法器
	RQCSC::InitObj();					//初始化流程控制器
	RQExpressionCompute::InitObj();		//初始化表达式计算器
	RQGrammar::InitObj();				//初始化词法器
	RQVar::InitObj();
}

//初始化网络部分
void RQInit_Net()
{
	RQRigout::InitObj();				//初始化封包解包器
	RQCommunication::InitObj();				//初始化通讯模块
}

void RQInit()
{
	static bool needInit = true;
	if (needInit)
	{
		RQInit_Globle();				//初始化全局资源		
		RQInit_Compiler();				//初始化编译器
		RQInit_Es();					//初始化脚本接口配置
		RQInit_Net();					//初始化网络相关模块
		RQRegularPool::InitObj();
		needInit = false;
	}
}
