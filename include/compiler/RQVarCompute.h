#pragma once
#include "define/RQInterDefine.h"
#include "define/RQASTDefine.h"
#include "RQPair.h"
/*
	�����ǽű������ļ������࣬�����ṩͳһ�����������

	���������������ͼ��������ƣ������㷽����Ϊ���ͣ���ֵ�ͣ��ַ��ͽ��м���
	���Ǽ�������ʵ����ʹ������Ϊllong��double��str��������
	�������λ�ú���ռ����λ����ͬ�磺
	!a����aӦ����rightλ��
*/
class RQVar;
///class RQPtrVal;

class RQVarCompute 
{
public:
	typedef void(*Fun_Type)(RQVar* left, RQVar* right, RQVar* save);

	/*
		�����ṩ������������ϲ���������������д洢
		left	[in]	������������Ҳ��Ϊ���α�����������ֻ��һ��������ʱ��ò���Ϊ���α���
		right	[in]	������Ҳ�����Ҳ��Ϊ���α���
		result	[out]	������������
		optType	[in]	������������������
	*/
	static void Compute(RQVar* left, RQVar* right, RQVar* save, int optType);

	/*
		��ʼ���ű�����������
	*/
	static void InitObj();
private:
	/*
		dd������double-double
		ll������long long -long long
		ss������str-str
		vv������void*-void*
		bb������bool-bool
	*/
	/*
	����left��right�����ݿռ��Լ�����
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
		���������ͽ�������
	*/
	static int _ElevateType(int leftType, int rightType = M_VAR_TYPE_INVALID);
private:
	static void _InvalidFun(RQVar* left, RQVar* right, RQVar* save);
	/*
		��ʼ������ͬһ�����ͽ��м���ı�������
		optType		[in]	�������������
		typeArray	[in]	���������������
		typeTotal	[in]	���������������Ԫ������
		fun			[in]	���ϲ��������Ĵ�����
	*/ 
	static void _InitObj_Fun(int optType,
		int* typeArray,int typeTotal,
		Fun_Type fun);		

	static void _InitObj();	

private:
	/*
		�ú�����Ҫ�ṩԤ�ڲ����ĸ������Ƚ�
		left	[in]	�����
		right	[in]	�Ҳ���
	*/
	static RQPair _DDComp(double left, double right);

private:
	/*
		����������������ά�Ƚ���������
		һά������������ֵ
		��ά������������
	*/
	static Fun_Type _funArray[M_SYMBOL_TOKEN_TOTAL][M_VAR_TYPE_TOTAL];
};
