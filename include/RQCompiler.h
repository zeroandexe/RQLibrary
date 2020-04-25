#pragma once
#include <map>
#include <string>
#include <vector>

class RQGrammar;					//语法分析器
class RQVar;						//脚本变量
class RQScriptTable;				//脚本表
class RQSentence;					//语句对象
class RQActuator;					//执行器
class RQScriptFile;					//脚本对象
class RQMutex;
class RQCompiler
{
public:
	/*
		enviMap：传入外部环境变量
		elementManager：元素管理器 
	*/
	RQCompiler();
	~RQCompiler();
	
	/*
		filePath	[in]	脚本文件的路径
		return		[out]	返回0表示该脚本已执行
							返回-1表示该脚本不能执行
	*/
	int RunScript(const char* filePath);			//加载并运行脚本

	/*
		scriptFile	[in]	运行已加载过的脚本文件
		return		[out]	返回0表示该脚本已执行
							返回-1表示该脚本不能执行
	*/
	int RunScript(RQScriptFile* scriptFile);

	/*
		cmdStr	[in]	一行脚本代码
	*/
	RQVar* RunCmd(const char* cmdStr);

	/*
		清除使用cmd所产生的资源
	*/
	void ClearCmd();
	/*
		name：在脚本中变量的名称
		var：脚本对象地址
	*/
	static void SetEnviVar(const char* name, RQVar* var);	//把变量存入环境变量表中
	static RQVar* GetEnviVar(const char* name);				//按照输入名在环境变量中获取对象的变量对象
	static void RemoveEnviVar(const char* name);			//按照输入将指定名称的变量从环境变量表中清除

	/*
		获取该编译器中cmd使用的变量
	*/
	RQVar* GetLocalVar(const char* name);
		/*
		name：即将在脚本中出现的命名字符串
		addr：字符串对应的内存数据地址
		size：变量对象的孔家大小
	*/
	static void SetEnviVar(const char* name, void* addr, int size, int type);	//按照输入在环境变量表中加入一个键值对

	//返回最近一次脚本执行的返回值
	RQVar* GetExp();

	static RQVar* GetFreeVar();								//获取一个空闲可用的对象
	static void RecycleFreeVar(RQVar* var);					//回收未被使用的变量对象,如果变量被使用，那么就不应该回收
	
private:
	RQVar* _exp;					//编译器的寄存器变量
	RQActuator* _actuator;			//脚本执行器
	RQScriptFile* _scriptFile;		//被执行的脚本对象
	RQMutex* _mutex;				//多线程下避免对编译器资源产生竞争的互斥锁
};