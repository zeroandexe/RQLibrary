#pragma once
#include <string>
#include "node/RQNode.h"
#include "RQVar.h"
#include "RecycleBin.h"
#include <map>

class RQJsonArray;
class RQJsonMap;
class RQJsonVal;
/*
	Json模块中作为Json相关变量的父类
	json ::= ‘{’ [{(express ‘,’)],0,}express] ’}’

	express ::= <str>’:’<obj_val | array_val | val>
	
	obj_val ::= json
	array_val ::= ‘[’ json |val’]’
	val ::= str|digit|key|
	
	str ::=” \”…\” ”
	digiti ::= {number,1,}[‘.’ {number,1,}]
	number ::= 0|1|2|3|4|5|6|7|8|9
	key ::= true,false,null

*/

class RQJson : public RQNode
{
public:
	RQJson();
	virtual ~RQJson();
	virtual void Create(void* param = 0);
	virtual void* Init(void* param = nullptr);	
	virtual void Clear();
	/*
		对所传入的字符串进行json解析，并返回解析后的json对象
		buff	[in]	被解析的字符串（可以没有c结尾符）
		buffLen	[in]	字符串长度
		return	[out]	被解析出的对象
						若解析失败则返回空
	*/
	static RQJson* ParseJson(char* buff, int buffLen);

	/*
		回收不再使用的Json对象，并释放json对象
	*/
	static void ReleaseJson(RQJson* jsonObj);

	virtual void SetInt(int val) {}
	virtual void SetDouble(double val) {}
	virtual void SetStr(const char* str, unsigned int strlen = -1) {}
	virtual void SetLLong(long long val) {}
	
	virtual int asInt() { return 0; }
	virtual double asDouble() { return .0; }
	virtual const char* asStr() { return ""; }
	virtual long long asLLong() { return 0; }
	virtual RQJson& operator[](const char* keyName);
	virtual RQJson& operator[](int index);
	virtual int Count() { return 0; }
	virtual operator bool() { return true; };
	virtual void Refurbish() {}					//json对象进行刷新，具体含义由各子类自定义
protected:
	/*
		获取作为无效值使用的json数据对象
	*/
	RQJson* _InvalidJson();

private:
	RQJsonVal* _invalidVal;
};

