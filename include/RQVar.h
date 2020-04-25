#pragma once
//#include "node/RQNode.h"
#include "define/RQASTDefine.h"
#include "define/RQInterDefine.h"
#include "RQPair.h"
#include <map>
/*
	动态变量V2版，已进行内存分离、变量类型管理、操作计算等提纯，
	由于整型、4字节浮点数、8字节浮点数、字符串在数据获取上存在差异，
	所以这里将其作为4中不同的基础类型进行区分处理

*/
class RQMemory;
class RQVar : public RQNode
{
public:
	friend class RQVarTypeCompute;
	typedef void(*RQVarFun_Type)(RQVar**);		//定义外界脚本函数类型

	RQVar();
	void operator = (const RQVar& other);

	virtual void Create(void* param = 0);
	virtual void* Init(void* param = 0);
	virtual void Clear();

	/*
		根据输入计算操作结果
		left	[in]	操作符主/左参变量
		right	[in]	操作符右参变量
		save	[out]	结果保存对象
		optType	[in]	本次将被执行的操作
	*/
	static void RunOpt(RQVar* left, RQVar* right, RQVar* save, int optType);

	//根据指定类型获取数据，将内存数据当作对应的类型获取
	int GetInt();
	long GetLong();
	long long GetLLong();
	double GetDouble();
	float GetFloat();				//将内存数据当作float类型获取
	bool GetBool();					//该操作较为特殊，输出为鉴定后的布尔值
	void* GetFun();					//获取函数地址
	void* GetObj();					//获取对象指针
	char* GetStr();					//获取字符串类型
	unsigned int GetStrLength();	//获取字符串数据长度


	//根据传入名称和内容  
	void SetInt(int val, const char* name = 0);
	void SetLong(long val, const char* name = 0);
	void SetLLong(long long val, const char* name = 0);
	void SetDouble(double val, const char* name = 0);
	void SetFloat(float val, const char* name = 0);
	void SetBool(bool val, const char* name = 0);
	void SetFun(void* val, const char* name = 0);
	void SetObj(void* val, const char* name = 0);
	void SetStr(const char* val,unsigned int length = -1, const char* name = 0);	//length为-1则表示由程序计算长度
	/*
		将val追加保存到当前的内存空间中，并且追加不改变数据类型
		val		[in]	被追加数据的内存地址
		length	[in]	被追加数据的字节长度
	*/
	void AppendData(void* val,unsigned int length);

	/*
		根据输入参数设定脚本变量
		val		[in]	数据内存地址
		type	[in]	输入数据类型
		length	[in]	数据长度
		name	[in]	脚本变量名称
		(目前该函数仅用于旧函数替换，新程序应尽量不使用)
	*/
	void Set(void* val, int type, unsigned int length = -1, const char*name = 0);

	//基本设置功能
	void Assign(RQVar* other);			//拷贝另一对象数据以及成员对象
	void AssignData(RQVar* other);		//拷贝另一个对象的数据
	void AssignMember(RQVar* other);	//拷贝另一个对象成员的所有数据及成员（目前成员变量只有一层且不可引用外界成员变量）
	void Resize(int size, bool holdData = false);	//重置大小,并决定是否保留数据（默认不保留）

	void SwapData(RQVar* other);		//交换动态变量的数据
	void SwapMember(RQVar* other);		//将换动态变量的成员表

	void SetName(const char* name);		//更换变量的名称
	char* GetName();					//获取变量名称

	int GetType();						//获取变量类型
	unsigned int GetSize();				//获取当前数据存储空间大小

	void* GetAddr();					//获取数据的存储地址
	unsigned int GetDataLenght();		//获取数据的长度
	void SetDataLength(unsigned int dataLength);	//设置数据长度
	/*
		初始化脚本变量以及脚本变量相关的处理器
	*/
	static void InitObj();
	/*
		根据成员名称获取属于该脚本变量的成员,若没有改成员将生成对应成员
	*/
	RQVar* Member(const char* memberName);
	RQVar* Member(const std::string& memberName);
	RQVar* operator[](const char* memberName);
	RQVar* operator[](const std::string& memberName);

private:
	RQVar* _CreateMember(const char* memberName);	//在对象成员表中创建一个成员对象
private:
	std::map<std::string, RQVar*>* _memberMap;		//属于该对象的成员表；
	void* _tsId;		//缓存器id
	RQMemory* _name;	//脚本变量名称
	RQMemory* _data;	//数据存储内存
	RQMemory* _buff;	//用于临时缓存数据的存储区
	int _type;
	
private:		//暂时不提供的操作
	void SwapName(RQVar* other);		//交换动态变量的名称
};