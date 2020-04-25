#include "RQVar.h"
#include "RQStrTool.h"
#include "tool/RQMemory.h"
#include <string.h>
#include "RQPtrVal.h"
#include "define/RQInterDefine.h"
#include "define/RQdefine.h"
#include "compiler/RQVarTypeCompute.h"
#include "compiler/RQVarCompute.h"
#include "RecycleBin.h"

RQVar::RQVar()
{
	Create();
	Init();
}

void RQVar::operator=(const RQVar & other)
{
	Assign((RQVar*) &other);
}

void RQVar::Create(void* param)
{
	
	_tsId = RecycleBin::CreateTS();			///创建一个回收器构建变量
	_memberMap = new std::map<std::string, RQVar*>;
}

void * RQVar::Init(void * param)
{
	_name = RecycleBin::GetFreeRQMemory();
	_name->Resize(64);

	_data = RecycleBin::GetFreeRQMemory();
	_data->Resize(sizeof(long long));

	_buff = RecycleBin::GetFreeRQMemory();

	_type = M_VAR_TYPE_INT;
	return nullptr;
}


void RQVar::Clear()
{
	_type = M_VAR_TYPE_INT;

	RecycleBin::RecycleNode(_name);
	RecycleBin::RecycleNode(_data);
	RecycleBin::RecycleNode(_buff);

	_name = 0;
	_data = 0;
	_buff = 0;

	_memberMap->clear();					///清除所有成员变量
	RecycleBin::RecycleTemStorage(_tsId);	///将成员变量回收
}

void RQVar::RunOpt(RQVar * left, RQVar * right, RQVar * save, int optType)
{
	RQVarCompute::Compute(left, right, save, optType);
}

int RQVar::GetInt()
{
	return RQVarTypeCompute::Compute(this, M_VAR_TYPE_INT)._int;
}

long RQVar::GetLong()
{
	return RQVarTypeCompute::Compute(this, M_VAR_TYPE_LONG)._long;
}

long long RQVar::GetLLong()
{
	return RQVarTypeCompute::Compute(this, M_VAR_TYPE_LLONG)._llong;
}

double RQVar::GetDouble()
{
	return RQVarTypeCompute::Compute(this, M_VAR_TYPE_DOUBLE)._double;
}

float RQVar::GetFloat()
{
	return RQVarTypeCompute::Compute(this, M_VAR_TYPE_FLOAT)._float;
}

bool RQVar::GetBool()
{
	return RQVarTypeCompute::Compute(this, M_VAR_TYPE_BOOL)._llong;;
}

void * RQVar::GetFun()
{
	return RQVarTypeCompute::Compute(this, M_VAR_TYPE_FUN)._ptr;
}

void * RQVar::GetObj()
{
	return RQVarTypeCompute::Compute(this, M_VAR_TYPE_OBJ)._ptr;
}

char * RQVar::GetStr()
{
	return (char*) RQVarTypeCompute::Compute(this, M_VAR_TYPE_STR)._ptr;
}

unsigned int RQVar::GetStrLength()
{
	unsigned int length;
	if (M_VAR_TYPE_STR == _type)
	{
		length = _data->GetLength();
	}
	else {
		length = _buff->GetLength();
	}
	return length;
}

/*
	----------------数据存储接口
*/
void RQVar::SetInt(int val, const char * name)
{
	SetLLong(val, name);
	_type = M_VAR_TYPE_INT;
}

void RQVar::SetLong(long val, const char * name)
{
	SetLLong(val, name);
	_type = M_VAR_TYPE_LONG;
}

void RQVar::SetLLong(long long val, const char * name)
{
	_data->Assign(&val, sizeof(long long));
	_name->Assign((void*)name, RQStrlen(name));
	_type = M_VAR_TYPE_LLONG;
}

void RQVar::SetDouble(double val, const char * name)
{
	_data->Assign(&val, sizeof(double));
	_name->Assign((void*)name, RQStrlen(name));
	_type = M_VAR_TYPE_DOUBLE;
}

void RQVar::SetFloat(float val, const char * name)
{
	_data->Assign(&val, sizeof(float));
	_name->Assign((char*)name, RQStrlen(name));
	_type = M_VAR_TYPE_FLOAT;
}

void RQVar::SetBool(bool val, const char * name)
{
	SetLLong(val, name);
	_type = M_VAR_TYPE_BOOL;
}

void RQVar::SetFun(void * val, const char * name)
{
	RQPtrVal ptrVal;
	ptrVal._ptr = val;
	SetLLong(ptrVal._llong, name);
	_type = M_VAR_TYPE_FUN;
}

void RQVar::SetObj(void * val, const char * name)
{
	SetFun(val, name);
	_type = M_VAR_TYPE_OBJ;
}

void RQVar::SetStr(const char * val, unsigned int length,const char * name)
{
	if (-1 == length)
	{
		length = RQStrlen(val);
	}
	_data->Assign((void*)val, length);
	_name->Assign((void*)name, RQStrlen(name));
	_type = M_VAR_TYPE_STR;
}

void RQVar::AppendData(void * val, unsigned int length)
{
	_data->Append(val, length);
}


void RQVar::Set(void * val,int type, unsigned int length, const char * name)
{
#define M_CASE(m_case,member,type)	\
case m_case:	\
	ptrVal.member = *(type*)val;	\
	break;

	RQPtrVal ptrVal;
	switch (type)
	{
		M_CASE(M_VAR_TYPE_BOOL, _int, bool);
		M_CASE(M_VAR_TYPE_DOUBLE, _double, double);
		M_CASE(M_VAR_TYPE_FLOAT, _float, float);
		//M_CASE(M_VAR_TYPE_FUN, _ptr, void*);
		M_CASE(M_VAR_TYPE_INT, _int, int);
		M_CASE(M_VAR_TYPE_LLONG, _llong, long long);
		M_CASE(M_VAR_TYPE_LONG, _long, long);
	case M_VAR_TYPE_FUN:
	case M_VAR_TYPE_OBJ:
		ptrVal._ptr = val;
		break;
	default:
		if (-1 == length)
		{
			length = RQStrlen((const char*)val);
		}
		_data->Assign(val, length);
		_name->Assign((void*)name, RQStrlen(name));
		_type = type;
		return;
	}
	_data->Assign(&ptrVal, sizeof(long long));		///至少已8字节存储
	_name->Assign((void*)name, RQStrlen(name));
	_type = type;
}

/*
	-----------------------基本功能提供
*/
void RQVar::Assign(RQVar * other)
{
	if (other != this)
	{
		AssignData(other);			///拷贝传入对象的数据
		AssignMember(other);		///深度拷贝对象的所有成员
	}
}

void RQVar::AssignMember(RQVar* other)
{
	if (other != this)
	{
		RecycleBin::RecycleTemStorage(_tsId);		///回收旧成员变量
		_memberMap->clear();			///释放所有为成员分配的存储位置
		for (auto iter = other->_memberMap->begin(); iter != other->_memberMap->end(); iter++)
		{
			RQVar* newMember = _CreateMember(iter->second->GetName());	///创建新成员
			newMember->AssignData(iter->second);	///拷贝目标对象数据
		}
	}
}


void RQVar::Resize(int size, bool holdData)
{
	_data->Resize(size, holdData);
}

void RQVar::SwapData(RQVar * other)
{
	if (other != this)
	{
		if (other->_data->GetLength() > MAX_THRESHOLD_VAR_TO_SWAP)
		{
			int tempType;
			_data->Swap(other->_data);
			M_SWAP(_type, other->_type, tempType);
		}
		else {
			AssignData(other);
		}
	}
}

void RQVar::SwapName(RQVar * other)
{
	if (other != this)
	{
		_name->Swap(other->_name);
	}
}

void RQVar::SwapMember(RQVar* other)
{
	std::map<std::string, RQVar*>* tempMap=0;
	M_SWAP(tempMap, _memberMap, other->_memberMap);
}

void RQVar::SetName(const char * name)
{
	_name->Assign((void*)name, RQStrlen(name));
}

char * RQVar::GetName()
{
	return (char*)_name->GetAddr();
}

int RQVar::GetType()
{
	return _type;
}

unsigned int RQVar::GetSize()
{
	return _data->GetSize();
}
void* RQVar::GetAddr()
{
	return _data->GetAddr();
}

unsigned int RQVar::GetDataLenght()
{
	return _data->GetLength();
}
void RQVar::SetDataLength(unsigned int dataLength)
{
	return _data->ResetLength(dataLength);
}

RQVar* RQVar::Member(const char* memberName)
{
	RQVar* member = 0;
	if (memberName&&memberName[0])				///要求成员对象必须有名称
	{
		member = (*_memberMap)[memberName];
		if (!member)
		{
			member = _CreateMember(memberName);
		}
	}
	return member;
}
RQVar* RQVar::Member(const std::string& memberName)
{
	return Member(memberName.c_str());
}

RQVar * RQVar::operator[](const char * memberName)
{
	return Member(memberName);
}

RQVar * RQVar::operator[](const std::string & memberName)
{
	return Member(memberName);
}

void RQVar::AssignData(RQVar* other)
{
	if (other != this)
	{
		_data->Assign(other->_data->GetAddr(), other->_data->GetLength());
		_type = other->_type;
	}
}


RQVar* RQVar::_CreateMember(const char* memberName)
{
	RQVar* newMember = RecycleBin::GetFreeRQVar();		///索要一个新成员
	(*_memberMap)[memberName] = newMember;					///将成员存储到变量表中
	RecycleBin::TemStorage(_tsId, newMember);			///将成员存储到成员表中
	newMember->SetName(memberName);						///设置新成员名称
	return newMember;
}

void RQVar::InitObj()
{
	RQVarTypeCompute::InitObj();
	RQVarCompute::InitObj();
}
