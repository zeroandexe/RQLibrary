#pragma once
#include "define/RQInterDefine.h"
#include "define/RQASTDefine.h"
#include "RQPair.h"
/*
	该类是脚本变量的计算器类，用于提供统一的运算符处理。

	方法计算器和类型计算器类似，将计算方法分为整型，数值型，字符型进行计算
	于是计算器其实最终使用类型为llong、double、str三种类型
	参数存放位置和所占符号位置相同如：
	!a，则a应传入right位置
*/
class RQVar;
///class RQPtrVal;

class RQVarCompute 
{
public:
	typedef void(*Fun_Type)(RQVar* left, RQVar* right, RQVar* save);

	/*
		方法提供对象期望的组合操作，并将结果进行存储
		left	[in]	运算符左操作，也称为主参变量；当操作只有一个操作数时则该参数为主参变量
		right	[in]	运算符右操作，也称为符参变量
		result	[out]	运算符操作结果
		optType	[in]	期望操作的特征代号
	*/
	static void Compute(RQVar* left, RQVar* right, RQVar* save, int optType);

	/*
		初始化脚本变量计算器
	*/
	static void InitObj();
private:
	/*
		dd：代表double-double
		ll：代表long long -long long
		ss：代表str-str
		vv：代表void*-void*
		bb：代表bool-bool
	*/
	/*
	交换left和right的数据空间以及类型
	*/
	static void _Swap_aa(RQVar* left, RQVar* right, RQVar* save);

	// =
	static void _Assign_aa(RQVar* left, RQVar* right, RQVar* save);

	// +
	static void _Add_dd(RQVar* left, RQVar* right, RQVar* save);
	static void _Add_ll(RQVar* left, RQVar* right, RQVar* save);
	static void _Add_ss(RQVar* left, RQVar* right, RQVar* save);
	
	// - 
	static void _Sub_dd(RQVar* left, RQVar* right, RQVar* save);
	static void _Sub_ll(RQVar* left, RQVar* right, RQVar* save);

	// *
	static void _Mutil_dd(RQVar* left, RQVar* right, RQVar* save);
	static void _Mutil_ll(RQVar* left, RQVar* right, RQVar* save);

	// /
	static void _Div_dd(RQVar* left, RQVar* right, RQVar* save);
	static void _Div_ll(RQVar* left, RQVar* right, RQVar* save);

	// +=
	static void _Add_Equal_dd(RQVar* left, RQVar* right, RQVar* save);
	static void _Add_Equal_ll(RQVar* left, RQVar* right, RQVar* save);
	static void _Add_Equal_ss(RQVar* left, RQVar* right, RQVar* save);

	// -=
	static void _Sub_Equal_dd(RQVar* left, RQVar* right, RQVar* save);
	static void _Sub_Equal_ll (RQVar* left, RQVar* right, RQVar* save);

	// *=
	static void _Mutil_Equal_dd(RQVar* left, RQVar* right, RQVar* save);
	static void _Mutil_Equal_ll(RQVar* left, RQVar* right, RQVar* save);

	// /=
	static void _Div_Equal_dd(RQVar* left, RQVar* right, RQVar* save);
	static void _Div_Equal_ll(RQVar* left, RQVar* right, RQVar* save);

	// >
	static void _More_ss(RQVar* left, RQVar* right, RQVar* save);
	static void _More_ll(RQVar* left, RQVar* right, RQVar* save);
	static void _More_dd(RQVar* left, RQVar* right, RQVar* save);

	// %
	static void _Mold_ll(RQVar* left, RQVar* right, RQVar* save);

	// %=
	static void _Mold_Equal_ll(RQVar* left, RQVar* right, RQVar* save);

	// <
	static void _Less_ss(RQVar* left, RQVar* right, RQVar* save);
	static void _Less_ll(RQVar* left, RQVar* right, RQVar* save);
	static void _Less_dd(RQVar* left, RQVar* right, RQVar* save);

	// !
	static void _Non_dd(RQVar* left, RQVar* right, RQVar* save);

	// ==
	static void _Equal_Equal_ss(RQVar* left, RQVar* right, RQVar* save);
	static void _Equal_Equal_ll(RQVar* left, RQVar* right, RQVar* save);
	static void _Equal_Equal_dd(RQVar* left, RQVar* right, RQVar* save);

	// >=
	static void _More_Equal_ss(RQVar* left, RQVar* right, RQVar* save);
	static void _More_Equal_ll(RQVar* left, RQVar* right, RQVar* save);
	static void _More_Equal_dd(RQVar* left, RQVar* right, RQVar* save);

	// <=
	static void _Less_Equal_ss(RQVar* left, RQVar* right, RQVar* save);
	static void _Less_Equal_ll(RQVar* left, RQVar* right, RQVar* save);
	static void _Less_Equal_dd(RQVar* left, RQVar* right, RQVar* save);

	// !=
	static void _Non_Equal_ss(RQVar* left, RQVar* right, RQVar* save);
	static void _Non_Equal_ll(RQVar* left, RQVar* right, RQVar* save);
	static void _Non_Equal_dd(RQVar* left, RQVar* right, RQVar* save);

	/*
		对运算类型进行提升
	*/
	static int _ElevateType(int leftType, int rightType = M_VAR_TYPE_INVALID);
private:
	static void _InvalidFun(RQVar* left, RQVar* right, RQVar* save);
	/*
		初始化可用同一种类型进行计算的变量操作
		optType		[in]	运算符特征代码
		typeArray	[in]	左操作数类型数组
		typeTotal	[in]	左操作数类型数组元素总数
		fun			[in]	符合操作特征的处理函数
	*/ 
	static void _InitObj_Fun(int optType,
		int* typeArray,int typeTotal,
		Fun_Type fun);		

	static void _InitObj();	

private:
	/*
		该函数主要提供预期操作的浮点数比较
		left	[in]	左参数
		right	[in]	右参数
	*/
	static RQPair _DDComp(double left, double right);

private:
	/*
		函数操作处理共三个维度进行索引。
		一维：操作符特征值
		二维：计算结果类型
	*/
	static Fun_Type _funArray[M_SYMBOL_TOKEN_TOTAL][M_VAR_TYPE_TOTAL];
};
