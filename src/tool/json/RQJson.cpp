#include "RQJsonHead.h"
#include "tool/json/RQJsonParse.h"
#include "RecycleBin.h"


RQJson::RQJson()
{
	Create();
}

RQJson::~RQJson()
{
	Clear();
}

void RQJson::Create(void * param)
{
	_invalidVal = 0;
}

void* RQJson::Init(void * param)
{
	_invalidVal =_invalidVal? _invalidVal: RecycleBin::GetFreeRQJsonVal();
	return nullptr;
}

void RQJson::Clear()
{
	RecycleBin::RecycleNode(_invalidVal);
	_invalidVal = 0;
}

RQJson * RQJson::ParseJson(char * buff, int buffLen)
{
	RQJsonParse parse(buff,buffLen);				///创建json解析器
	return parse.ParseJson();						///开始解析json
}

void RQJson::ReleaseJson(RQJson * jsonObj)
{
	RecycleBin::RecycleNode(jsonObj);				///回收json对象
}

RQJson * RQJson::_InvalidJson()
{
	_invalidVal->SetLLong(0);
	return _invalidVal;
}

RQJson& RQJson::operator[](const char* keyName)
{
	return *_invalidVal;
}
 RQJson& RQJson::operator[](int index)
{
	 return *_invalidVal;
}