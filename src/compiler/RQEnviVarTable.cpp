#include "compiler/RQEnviVarTable.h"
#include "RecycleBin.h"
#include "RQVar.h"

///#include "RQSpinLock.h"

RQMap<std::string, RQVar*>* RQEnviVarTable::_enviTable = 0;
RQSpinLock RQEnviVarTable::_lock;


void RQEnviVarTable::InitObj()
{
	static volatile bool needInit = true;
	if (needInit)
	{
		needInit = false;
		_enviTable = new RQMap<std::string, RQVar*>(new RQSpinLock);
	}
}

void RQEnviVarTable::SetVar(const char* name, RQVar* var)
{
	int isOk = !!name * !!var;		///计算传入参数是否不为空
	if (isOk)
	{
		RQAutoLock lock(&_lock);
		var->SetName(name);
		(*_enviTable)[name] = var;
	}
}

void RQEnviVarTable::SetVar(const char * name, void * addr, int size, int type)
{
	int isOk = !!name*size;			///计算传入条件是否满足输入
	if (isOk)
	{
		RQVar* enviVar = GetVar(name);						///获取环境变量表中的脚本变量
		RQAutoLock lock(&_lock);
		enviVar->Set(addr, type, size, name);				///更新变量内容
	}
}

RQVar * RQEnviVarTable::GetVar(const char * name)
{
	RQAutoLock lock(&_lock);
	RQVar*& enviVar = (*_enviTable)[name];				///获取标识当前名称的用于存储脚本变量地址的变量
	if (!enviVar)										///检测该变量是否存在不存在
	{
		enviVar = RecycleBin::GetFreeRQVar();			///由于变量不存在则赋予新变量地址
		enviVar->SetName(name);
	}
	return enviVar;
}

void RQEnviVarTable::RemoveVar(const char * name)
{
	//RQAutoLock lock(&_lock);
	auto iter = _enviTable->Find(name);
	RecycleBin::RecycleNode((RQNode*)iter->second);		///回收变量对象
	//_enviTable.erase(iter);								///将对象从表中移除
	_enviTable->Remove(iter);
}

RQVar * RQEnviVarTable::FindVar(const char * name)
{
	RQAutoLock lock(&_lock);
	auto iter = _enviTable->Find(name);					///获取标识当前名称的用于存储脚本变量地址的变量
	RQVar* enviVar = 0;
	if (iter != _enviTable->end())
	{
		enviVar = iter->second;
	}
	return enviVar;
}
