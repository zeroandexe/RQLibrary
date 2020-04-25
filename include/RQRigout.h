#pragma once
#include "define/RQInterDefine.h"
/*
	该对象用于数据包的封装和组装处理，对于不同的包头可从该类继承，获取该类用于包体组装的处理
	版本：v2
	当前采用的格式不再是12n，而是14n
*/

class RQVar;
class RQRigout {
public:
	typedef int(*Fun_Type)(char*, RQVar*);	
	enum {
		M_OPT_RIGOUT,		//组包操作
		M_OPT_ANALYZE,		//解包操作
		M_OPT_TOTAL			//操作总数
	};
	/*
		将传入的脚本对象数组中的每个脚本对象的数据存入缓冲区中，并返回本次组包所消耗掉的字节长度
		buff		[out]	数据缓冲区
		varArray	[in]	作为数据单元的脚本变量对象数组
		varTotal	[in]	脚本对象在数组中的总个数，若默认则由数组访问到空元素时停止
		return		[out]	组包写入的数据长度
		注意：这里不检测缓冲区是否会产生越界，并将崩溃视为一种提醒
	*/
	static int RigoutBody(char* buff, RQVar** varArray, unsigned int varTotal = -1);
	static int RigoutBody(char* buff, RQVar* var);			///组装单个元素

	/*
		将传入的数据包包体进行解包操作，并返回本次获取所使用的字节长度
		buff	[in]	传入的数据包包体内存空间
		length	[in]	数据包包体字节长度
		varArray	[out]	依次获取数据包中的数据段
		dataCount	[out]	返回本此共获取了多少数据段（未解析出数据则返回0）
		varTotal	[in]	期望获取的数据字段数量，若未设置则直到数据子被获取完或者脚本变量被使用完
		return	[out]	本次获取使用的总数据长度(若未成功解析，则返回0)
	*/
	static int AnalyzeBody(char* buff, unsigned int length,
		RQVar** varArray, unsigned int* dataCount=0, unsigned varTotal = -1);
	//仅解析一个变量
	static int AnalyzeBody(char* buff, RQVar* var);
	/*
		初始化脚本对象
	*/
	static void InitObj();
	/*
		输入完整字段首地址，获取本字段的数据段地址
	*/
	static char* ToData(char* buff);
	/*
		获取字段头长度
	*/
	static int DateHeadSize();
public:
	static int RigoutBool(char* buff, RQVar* var);
	static int RigoutInt(char* buff, RQVar* var);
	static int RigoutFun(char* buff, RQVar* var);
	static int RigoutObj(char* buff, RQVar* var);
	static int RigoutLong(char* buff, RQVar* var);
	static int RigoutLLong(char* buff, RQVar* var);
	static int RigoutFloat(char* buff, RQVar* var);
	static int RigoutDouble(char* buff, RQVar* var);
	static int RigoutStr(char* buff, RQVar* var);

	static int Rigout(char* buff, void* data, int dataLen, int dataType);

public:
	static int AnalyzeBool(char* buff, RQVar* var);
	static int AnalyzeInt(char* buff, RQVar* var);
	static int AnalyzeFun(char* buff, RQVar* var);
	static int AnalyzeObj(char* buff, RQVar* var);
	static int AnalyzeLong(char* buff, RQVar* var);
	static int AnalyzeLLong(char* buff, RQVar* var);
	static int AnalyzeFloat(char* buff, RQVar* var);
	static int AnalyzeDouble(char* buff, RQVar* var);
	static int AnalyzeStr(char* buff, RQVar* var);

	static int Analyze(char* buff, RQVar* var,void* data, int dataType);

	/*
		该接口用于在内部的非RQLibrary的C函数调用接口
		buff		[in]	数据段头部内存地址
		addr		[out]	用于存储数据段的内存地址
		addrSize	[in]	存储内存地址的字节大小
		dataType	[out]	该数据的数据类型
		return		[out]	该数据段的整个大小
	*/
	//static int Analyze(char* buff, void* addr,int addrSize , int* dataType);
private:

	/*
		组解包函数处理数组
		一维：组、解操作代号
		二维：操作类型
	*/
	static Fun_Type _funArray[M_OPT_TOTAL][M_VAR_TYPE_TOTAL];
};

