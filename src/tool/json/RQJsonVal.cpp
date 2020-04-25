#include "RQJsonHead.h"
#include "RecycleBin.h"

RQJsonVal::RQJsonVal()
{
	Init();
}

RQJsonVal::~RQJsonVal()
{
	Clear();
}

void RQJsonVal::Create(void * param)
{
}

void * RQJsonVal::Init(void * param)
{
	_var = RecycleBin::GetFreeRQVar();
	return nullptr;
}

void RQJsonVal::Clear()
{
	RecycleBin::RecycleNode(_var);
	_var = 0;
}

void RQJsonVal::Swap(RQVar * var)
{
	_var->SwapData(var);
}

void RQJsonVal::Set(RQVar * var)
{
	_var->AssignData(var);
}

void RQJsonVal::SetInt(int val)
{
	_var->SetInt(val);
}

void RQJsonVal::SetDouble(double val)
{
	_var->SetDouble(val);
}

void RQJsonVal::SetStr(const char * str, unsigned int strlen)
{
	_var->SetStr(str, strlen);
}

void RQJsonVal::SetLLong(long long val)
{
	_var->SetLLong(val);
}

int RQJsonVal::asInt()
{
	return _var->GetInt();
}

double RQJsonVal::asDouble()
{
	return _var->GetDouble();
}

const char * RQJsonVal::asStr()
{
	return _var->GetStr();
}

long long RQJsonVal::asLLong()
{
	return _var->GetLLong();
}

RQJsonVal::operator bool()
{
	return _var;
}
