#pragma once
#include "RQPtrVal.h"
#include "define/RQdefine.h"
#include "define/RQInterDefine.h"

/*
	该对象为脚本变量的类型计算器，用于提供类型转换后的数据输出，
	以及计算时类型提升处理
*/

class RQVar;
class RQVarTypeCompute {
public:
	typedef RQPtrVal  (*Fun_Type)(RQVar*);
	/*
		将传入的脚本变量的数据转换为目标类型的输出
		val			[in]	数据源
		targetType	[in]	数据源目标类型
	*/
	static RQPtrVal Compute(RQVar* val,int targetType);
	/*
		初始化类型计算器的操作
	*/
	static void InitObj();
private:

	/*
		由于布尔值判定是对内存值非0的检测
	*/
	static RQPtrVal _AllToBool(RQVar* val);
	/*
		所有类型数据转换为整数类型
	*/
	static RQPtrVal _LLongToLLong(RQVar* val);
	static RQPtrVal _FloatToLLong(RQVar* val);
	static RQPtrVal _DoubleToLLong(RQVar* val);
	static RQPtrVal _StrToLLong(RQVar* val);
	/*
		所有类型的数据转换为浮点数类型
	*/
	static RQPtrVal _LLongToDouble(RQVar* val);
	static RQPtrVal _FloatToDouble(RQVar* val);
	static RQPtrVal _DoubleToDouble(RQVar* val);
	static RQPtrVal _StrToDouble(RQVar* val);

	static RQPtrVal _LLongToFloat(RQVar* val);
	static RQPtrVal _FloatToFloat(RQVar* val);
	static RQPtrVal _DoubleToFloat(RQVar* val);
	static RQPtrVal _StrToFloat(RQVar* val);

	/*
		所有数据类型转换为字符串
	*/
	static RQPtrVal _LLongToStr(RQVar* val);
	static RQPtrVal _FloatToStr(RQVar* val);
	static RQPtrVal _DoubleToStr(RQVar* val);
	static RQPtrVal _StrToStr(RQVar* val);
private:
	static void _InitObj_Integer();		//初始化数据源为整数的处理
	static void _InitObj_Float();		//初始化数据源为浮点数的处理，这里的float不是指float类型
	static void _InitObj_Str();
	static void _InitObj_Double();
	///static void _InitObj_Bool();

	template<typename Ti,typename To>
	static RQPtrVal _Ti_To_To(RQVar* val);

private:
	/*
		该数组为类型转换处理函数数组，
		第一维：表示输出数据源类型
		第二维：表示输入数据源类型
	*/
	static Fun_Type _funArray[M_VAR_TYPE_TOTAL][M_VAR_TYPE_TOTAL];	//定义函数数组
};
