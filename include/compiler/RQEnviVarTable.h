#pragma once 
///#include <map>
#include <string>
#include <RQMap.h>
#include "RQSpinLock.h"
///#include "RQSpinLock.h"
#include "RQVar.h"

/*
	该类为编译器的环境变量对象，为单例对象
*/
class RQEnviVarTable {
public:

	static void InitObj();

	/*
		保存脚本变量到环境变量表中
		name	[in]	脚本变量新命名
		addr	[in]	将被存储的数据/地址
		size	[in]	数据的总字节数
		type	[in]	数据的对应类型
	*/
	static void SetVar(const char * name, void * addr, int size, int type);

	//该函数不要使用，准备废弃
	static void SetVar(const char* name, RQVar* var);

	//根据输入的脚本变量名称，获取该变量，若不存在该变量将创建该变量
	static RQVar* GetVar(const char* name);

	//将指定名称的变量重环境变量表中删除
	static void RemoveVar(const char* name);

	//根据输入的脚本变量名称，获取该变量，若不在则返回空
	static RQVar* FindVar(const char* name);

private:
	///std::map<std::string, RQVar*> _enviTable;			//环境变量对象
	static RQSpinLock _lock;								//保卫环境变量读写的自旋锁
	static RQMap<std::string, RQVar*>* _enviTable;
};