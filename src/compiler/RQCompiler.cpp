#include "RQCompiler.h"
#include "RQVar.h"
#include "define/RQInterDefine.h"
#include "compiler/RQScriptTable.h"
#include "compiler/RQScriptFile.h"
#include "RecycleBin.h"
#include "compiler/RQActuator.h"
#include "compiler/RQEnviVarTable.h"
#include "compiler/RQScriptSour.h"
#include "compiler/RQResolver.h"
#include "RQMutex.h"


RQCompiler::RQCompiler()
{
	_actuator = new RQActuator;								///创建执行器
	_scriptFile = RecycleBin::GetFreeRQScriptFile();
	_mutex = new RQMutex;
}

RQCompiler::~RQCompiler()
{
	delete _actuator;
	RecycleBin::RecycleNode(_scriptFile);
}

int RQCompiler::RunScript(const char * filePath)
{
	RQScriptSour* scriptSour = RQScriptTable::GetScriptFile(filePath);		///从脚本变量表中获取filePath所指定的脚本
	int retInt = RunScript(scriptSour->_scriptFile);
	RQScriptTable::ReleaseScriptFile(scriptSour);				///脚本资源使用完毕，释放控制权
	return retInt;
}

int RQCompiler::RunScript(RQScriptFile * scriptFile)
{
	if (!scriptFile)	///检查脚本是否被成功加载
	{
		return -1;		///未被加载，返回错误码-1
	}
	_actuator->RunScriptFile(scriptFile);
	_exp = scriptFile->GetExp();
	return 0;
}

RQVar * RQCompiler::RunCmd(const char * cmdStr)
{
	RQVar* retVar = 0;
	if (cmdStr)
	{
		RQAutoLock lock(_mutex);
		if (RQResoler::AnalyzeSentence(cmdStr, _scriptFile))	///解析传入的脚本代码并检测是否解析成功
		{
			_actuator->RunScriptFile(_scriptFile);				///执行刚解析的代码
			retVar = _scriptFile->GetExp();						///获取执行结果，如果有的话
		}
		_scriptFile->Reset();									///语句对象执行完成，回收脚本中的所有数据，并将脚本恢复到可用状态
	}

	return retVar;
}

void RQCompiler::ClearCmd()
{
	RQAutoLock lock(_mutex);
	_scriptFile->Clear();
}

void RQCompiler::SetEnviVar(const char * name, void * addr, int size, int type)
{
	RQEnviVarTable::SetVar(name, addr, size, type);
}

void RQCompiler::SetEnviVar(const char * name, RQVar * var)
{
	RQEnviVarTable::SetVar(name, var);
}

RQVar* RQCompiler::GetEnviVar(const char * name)
{
	return RQEnviVarTable::GetVar(name);
}

void RQCompiler::RemoveEnviVar(const char * name)
{
	return RQEnviVarTable::RemoveVar(name);
}

RQVar * RQCompiler::GetLocalVar(const char * name)
{
	return (*_scriptFile->GetLocalMap())[name];
}

RQVar* RQCompiler::GetExp()
{
	return _exp;
}

RQVar* RQCompiler::GetFreeVar()
{
	return RecycleBin::GetFreeRQVar();
}

void RQCompiler::RecycleFreeVar(RQVar* var)
{
	RecycleBin::RecycleNode(var);
}
