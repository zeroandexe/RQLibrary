#include "compiler/RQVarCompute.h"
#include "RQVar.h"
#include "RQStrTool.h"
#include "RecycleBin.h"

///#include "RQPtrVal.h"
typedef RQVarCompute::Fun_Type Fun_Type;
Fun_Type RQVarCompute::_funArray[M_SYMBOL_TOKEN_TOTAL][M_VAR_TYPE_TOTAL]{ _InvalidFun };


void RQVarCompute::Compute(RQVar * left, RQVar * right, RQVar * save, int optType)
{
	///根据操作类型和结果类型来进行计算
	int leftType = left ? left->GetType() : M_VAR_TYPE_INVALID;
	int rightType = right ? right->GetType() : M_VAR_TYPE_INVALID;

	//此处若崩溃则表示脚本尝试对两个无效变量进行计算，或将无效变量转换为其他变量
	_funArray[optType][_ElevateType(leftType, rightType)](left, right, save);
}

void RQVarCompute::InitObj()
{
	_InitObj();
}


void RQVarCompute::_Swap_aa(RQVar* left, RQVar* right, RQVar* save)
{
	left->SwapData(right);
}


void RQVarCompute::_Assign_aa(RQVar * left, RQVar * right, RQVar * save)
{
	left->Assign(right);
}

//处理 +
void RQVarCompute::_Add_dd(RQVar * left, RQVar * right, RQVar * save)
{
	save->SetDouble(left->GetDouble() + right->GetDouble());
}

void RQVarCompute::_Add_ll(RQVar * left, RQVar * right, RQVar * save)
{
	save->SetLLong(left->GetLLong() + right->GetLLong());
}

void RQVarCompute::_Add_ss(RQVar * left, RQVar * right, RQVar * save)
{
	///获取数据源
	char* leftStr = left->GetStr();
	char* rightStr = right->GetStr();
	///计算空间大小需求
	int leftLen = RQStrlen(leftStr);
	int rightLen = RQStrlen(rightStr);

	///RQVar* temp = RecycleBin::GetFreeRQVar();	///获取临时存储变量
	RQVar* temp = new RQVar;
	temp->Resize(leftLen + rightLen);			///扩建存储空间
	temp->SetStr(leftStr, leftLen);				///拷贝第一元素数据
	temp->AppendData(rightStr, rightLen);		///将后续数据拷贝入内存
	save->SwapData(temp);						///通过交换
}


//处理 -
void RQVarCompute::_Sub_dd(RQVar * left, RQVar * right, RQVar * save)
{
	double leftVal = left ? left->GetDouble() : 0;
	save->SetDouble(leftVal - right->GetDouble());
}

void RQVarCompute::_Sub_ll(RQVar * left, RQVar * right, RQVar * save)
{
	long long leftVal = left?left->GetLLong():0;
	save->SetLLong(leftVal - right->GetLLong());
}


// 处理 *
void RQVarCompute::_Mutil_dd(RQVar * left, RQVar * right, RQVar * save)
{
	save->SetDouble(left->GetDouble()*right->GetDouble());
}

void RQVarCompute::_Mutil_ll(RQVar * left, RQVar * right, RQVar * save)
{
	save->SetLLong(left->GetLLong()*right->GetLLong());
}


//处理 /
void RQVarCompute::_Div_dd(RQVar * left, RQVar * right, RQVar * save)
{
	save->SetDouble(left->GetDouble() / right->GetDouble());
}

void RQVarCompute::_Div_ll(RQVar * left, RQVar * right, RQVar * save)
{
	save->SetLLong(left->GetLLong() / right->GetLLong());
}

//处理 +=
void RQVarCompute::_Add_Equal_dd(RQVar * left, RQVar * right, RQVar * save)
{
	_Add_dd(left, right, left);
}

void RQVarCompute::_Add_Equal_ll(RQVar * left, RQVar * right, RQVar * save)
{
	_Add_ll(left, right, left);
}

void RQVarCompute::_Add_Equal_ss(RQVar * left, RQVar * right, RQVar * save)
{
	_Add_ss(left, right, left);
}

// -=
void RQVarCompute::_Sub_Equal_dd(RQVar * left, RQVar * right, RQVar * save)
{
	_Sub_dd(left, right, left);
}

void RQVarCompute::_Sub_Equal_ll(RQVar * left, RQVar * right, RQVar * save)
{
	_Sub_ll(left, right, left);
}

// *=
void RQVarCompute::_Mutil_Equal_dd(RQVar * left, RQVar * right, RQVar * save)
{
	_Mutil_dd(left, right, left);
}

void RQVarCompute::_Mutil_Equal_ll(RQVar * left, RQVar * right, RQVar * save)
{
	_Mutil_ll(left, right, left);
}

// /=
void RQVarCompute::_Div_Equal_dd(RQVar * left, RQVar * right, RQVar * save)
{
	_Div_dd(left, right, left);
}

void RQVarCompute::_Div_Equal_ll(RQVar * left, RQVar * right, RQVar * save)
{
	_Div_ll(left, right, left);
}

//处理 >
void RQVarCompute::_More_ss(RQVar * left, RQVar * right, RQVar * save)
{
	bool result = RQStrComp(left->GetStr(), right->GetStr())>0;
	save->SetBool(result);
}

void RQVarCompute::_More_ll(RQVar * left, RQVar * right, RQVar * save)
{
	bool result = left->GetLLong() > right->GetLLong();
	save->SetBool(result);
}
		

void RQVarCompute::_More_dd(RQVar * left, RQVar * right, RQVar * save)
{
	RQPair ptr = _DDComp(left->GetDouble(), right->GetDouble());
	bool ret = ptr._first._llong>0;		///检查整数部分是否left>right
	if (!ptr._first._llong)				///检查左右参数在整数位是否相等
	{
		ret = ptr._second._llong > 0;
	}
	save->SetBool(ret);
}

void RQVarCompute::_Mold_ll(RQVar * left, RQVar * right, RQVar * save)
{
	long long result = left->GetLLong() % right->GetLLong();
	save->SetLLong(result);
}

void RQVarCompute::_Mold_Equal_ll(RQVar * left, RQVar * right, RQVar * save)
{
	_Mold_ll(left, right, left);
}

//处理 <
void RQVarCompute::_Less_ss(RQVar * left, RQVar * right, RQVar * save)
{
	bool result = RQStrComp(left->GetStr(), right->GetStr()) < 0;
	save->SetBool(result);
}

void RQVarCompute::_Less_ll(RQVar * left, RQVar * right, RQVar * save)
{
	bool result = left->GetLLong() < right->GetLLong();
	save->SetBool(result);
}

void RQVarCompute::_Less_dd(RQVar * left, RQVar * right, RQVar * save)
{
	RQPair ptr = _DDComp(left->GetDouble(), right->GetDouble());
	bool ret = ptr._first._llong < 0;
	if (!ptr._first._llong)		///检查左右参数在整数位是否相等，或者已得到确认答案
	{
		ret = ptr._second._llong < 0;
	}
	save->SetBool(ret);

}

//处理 !
void RQVarCompute::_Non_dd(RQVar * left, RQVar * right, RQVar * save)
{
	bool result = !right->GetBool();
	save->SetBool(result);
}

//处理 ==
void RQVarCompute::_Equal_Equal_ss(RQVar * left, RQVar * right, RQVar * save)
{
	bool result = RQStrComp(left->GetStr(), right->GetStr()) == 0;
	save->SetBool(result);
}

void RQVarCompute::_Equal_Equal_ll(RQVar * left, RQVar * right, RQVar * save)
{
	bool result = left->GetLLong() == right->GetLLong();
	save->SetBool(result);
}

void RQVarCompute::_Equal_Equal_dd(RQVar * left, RQVar * right, RQVar * save)
{
	RQPair ptr = _DDComp(left->GetDouble(), right->GetDouble());
	bool ret = false;
	if (!ptr._first._llong)				///检查左右参数在整数位是否相等
	{
		ret = !ptr._second._llong;		///检车小数部分是否相等
	}
	save->SetBool(ret);
}

//处理 >=
void RQVarCompute::_More_Equal_ss(RQVar * left, RQVar * right, RQVar * save)
{
	bool result = RQStrComp(left->GetStr(), right->GetStr()) >= 0;
	save->SetBool(result);
}

void RQVarCompute::_More_Equal_ll(RQVar * left, RQVar * right, RQVar * save)
{
	bool result = left->GetLLong() >= right->GetLLong();
	save->SetBool(result);
}

void RQVarCompute::_More_Equal_dd(RQVar * left, RQVar * right, RQVar * save)
{
	RQPair ptr = _DDComp(left->GetDouble(), right->GetDouble());
	bool ret = ptr._first._llong>0;			///检查left>right是否成立,若成立则不再进行比较
	if (!ptr._first._llong&&!ret)			///检查左右参数在整数位是否相等,该值若
	{
		ret = ptr._second._llong>=0;		///检车小数部分是否相等
	}
	save->SetBool(ret);
}

//处理 <=
void RQVarCompute::_Less_Equal_ss(RQVar * left, RQVar * right, RQVar * save)
{
	bool result = RQStrComp(left->GetStr(), right->GetStr()) <= 0;
	save->SetBool(result);
}

void RQVarCompute::_Less_Equal_ll(RQVar * left, RQVar * right, RQVar * save)
{
	bool result = left->GetLLong() <= right->GetLLong();
	save->SetBool(result);
}

void RQVarCompute::_Less_Equal_dd(RQVar * left, RQVar * right, RQVar * save)
{
	RQPair ptr = _DDComp(left->GetDouble(), right->GetDouble());
	bool ret = ptr._first._llong < 0;		///检查left<right是否成立,若成立则不再进行比较
	if (!ptr._first._llong && !ret)			///检查左右参数在整数位是否相等,该值若
	{
		ret = ptr._second._llong <= 0;		///检车小数部分是否相等
	}
	save->SetBool(ret);
}

//处理 !=
void RQVarCompute::_Non_Equal_ss(RQVar * left, RQVar * right, RQVar * save)
{
	bool result = RQStrComp(left->GetStr(), right->GetStr()) != 0;
	save->SetBool(result);
}

void RQVarCompute::_Non_Equal_ll(RQVar * left, RQVar * right, RQVar * save)
{
	bool result = left->GetLLong() != right->GetLLong();
	save->SetBool(result);
}

void RQVarCompute::_Non_Equal_dd(RQVar * left, RQVar * right, RQVar * save)
{
	RQPair ptr = _DDComp(left->GetDouble(), right->GetDouble());
	bool ret = ptr._first._llong;			///检查left>right是否成立,若成立则不再进行比较
	if (!ptr._first._llong && !ret)			///检查左右参数在整数位是否相等,该值若
	{
		ret = ptr._second._llong;		///检车小数部分是否相等
	}
	save->SetBool(ret);
}


int RQVarCompute::_ElevateType(int leftType, int rightType)
{
	int retType = leftType > rightType ? leftType : rightType;				///由于在定义宏时已确定类型等级
	if (M_VAR_TYPE_INVALID != retType)
	{
		retType = retType < M_VAR_TYPE_LLONG ? M_VAR_TYPE_LLONG : retType;		///将所有整型提升到LLong
		retType = retType == M_VAR_TYPE_FLOAT ? M_VAR_TYPE_DOUBLE : retType;	///若为浮点数则提升到double
	}

	return retType;
}

void RQVarCompute::_InvalidFun(RQVar * left, RQVar * right, RQVar * save)
{
}

void RQVarCompute::_InitObj_Fun(int optType,
	int* typeArray, int typeTotal,
	Fun_Type fun)
{
	for (int index = 0; index < typeTotal; index++)
	{
		_funArray[optType][typeArray[index]] = fun;
	}
}

void RQVarCompute::_InitObj()
{
#define M_REGISTER_FUN(optType,llFun,dd_Fun,_ssFun)		\
{														\
	_funArray[optType][M_VAR_TYPE_LLONG] = llFun;		\
	_funArray[optType][M_VAR_TYPE_DOUBLE] = dd_Fun;		\
	_funArray[optType][M_VAR_TYPE_STR] = _ssFun;		\
}
	///初始化 = 操作
	M_REGISTER_FUN(M_SYMBOL_TOKEN_EQUAL, _Assign_aa, _Assign_aa, _Assign_aa);

	///初始化 + 操作
	M_REGISTER_FUN(M_SYMBOL_TOKEN_ADD, _Add_ll, _Add_dd, _Add_ss);
	///初始化 - 操作
	M_REGISTER_FUN(M_SYMBOL_TOKEN_SUB, _Sub_ll, _Sub_dd, _InvalidFun);
	///初始化 * 操作
	M_REGISTER_FUN(M_SYMBOL_TOKEN_MULIT, _Mutil_ll, _Mutil_dd, _InvalidFun);
	///初始化 / 操作
	M_REGISTER_FUN(M_SYMBOL_TOKEN_DIV, _Div_ll, _Div_dd, _InvalidFun);

	///初始化 += 操作
	M_REGISTER_FUN(M_SYMBOL_TOKEN_ADD_EQUAL, _Add_Equal_ll, _Add_Equal_dd, _Add_Equal_ss);
	///初始化 -= 操作
	M_REGISTER_FUN(M_SYMBOL_TOKEN_SUB_EQUAL, _Sub_Equal_ll, _Sub_Equal_dd, _InvalidFun);
	///初始化 *= 操作
	M_REGISTER_FUN(M_SYMBOL_TOKEN_MULIT_EQUAL, _Mutil_Equal_ll, _Mutil_Equal_dd, _InvalidFun);
	///初始化 /= 操作
	M_REGISTER_FUN(M_SYMBOL_TOKEN_DIV_EQUAL, _Div_Equal_ll, _Div_Equal_dd, _InvalidFun);

	///初始化 % 操作
	M_REGISTER_FUN(M_SYMBOL_TOKEN_MOLD, _Mold_ll, _Mold_ll, _Mold_ll);
	///初始化 % 操作
	M_REGISTER_FUN(M_SYMBOL_TOKEN_MOLD_EQUAL, _Mold_Equal_ll, _Mold_Equal_ll, _Mold_Equal_ll);

	///初始化 > 操作
	M_REGISTER_FUN(M_SYMBOL_TOKEN_MORE, _More_ll, _More_dd, _More_ss);
	///初始化 < 操作
	M_REGISTER_FUN(M_SYMBOL_TOKEN_LESS, _Less_ll, _Less_dd, _Less_ss);
	///初始化 ！操作
	M_REGISTER_FUN(M_SYMBOL_TOKEN_NON, _Non_dd, _Non_dd, _Non_dd);

	///初始化 == 操作
	M_REGISTER_FUN(M_SYMBOL_TOKEN_EQUAL_EQUAL, _Equal_Equal_ll, _Equal_Equal_dd, _Equal_Equal_ss);
	///初始化 >= 操作
	M_REGISTER_FUN(M_SYMBOL_TOKEN_MORE_EQUAL, _More_Equal_ll, _More_Equal_dd, _More_Equal_ss);
	///初始化 <= 操作
	M_REGISTER_FUN(M_SYMBOL_TOKEN_LESS_EQUAL, _Less_Equal_ll, _Less_Equal_dd, _Less_Equal_ss);
	///初始化 != 操作
	M_REGISTER_FUN(M_SYMBOL_TOKEN_NON_EQUAL, _Non_Equal_ll, _Non_Equal_dd, _Non_Equal_ss);

	///初始化赋值操作-数据交换
	M_REGISTER_FUN(M_SYMBOL_TOKEN_OTHER_SWAP, _Swap_aa, _Swap_aa, _Swap_aa);
}

RQPair RQVarCompute::_DDComp(double left, double right)
{								
	long long leftIntegre = (long long)left;		
	long long rightIntegre = (long long)right;	
	RQPair pair;
	pair._first._llong = leftIntegre - rightIntegre;		///进行初次计算并获取符号值
							
	leftIntegre = ((long long )(left - leftIntegre))*M_PRECISION_DOUBLE;
	rightIntegre = ((long long)(right - rightIntegre))*M_PRECISION_DOUBLE;

	pair._second._llong = leftIntegre - rightIntegre;

	return pair;
}
