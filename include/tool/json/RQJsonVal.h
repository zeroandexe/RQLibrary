#pragma once
#include "RQVar.h"
#include "RecycleBin.h"
#include "RQJson.h"

class RQJsonVal : public RQJson
{
public:
	RQJsonVal();
	~RQJsonVal();
	virtual void Create(void* param = nullptr);
	virtual void* Init(void* param = nullptr);						//数据节点创建
	virtual void Clear();

	/*
		交换对象和传入变量的内存空间，数据类型、长度等
	*/
	void Swap(RQVar* var);
	/*
		采用var的数据为对象赋值，则对象为var的副本
	*/
	void Set(RQVar* var);

	void SetInt(int val);
	void SetDouble(double val);
	void SetStr(const char* str, unsigned int strlen = -1);
	void SetLLong(long long val);

	int asInt();
	double asDouble();
	const char* asStr() ;
	long long asLLong() ;

	virtual operator bool();
private:
	RQVar* _var;
};

