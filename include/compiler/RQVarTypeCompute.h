#pragma once
#include "RQPtrVal.h"
#include "define/RQdefine.h"
#include "define/RQInterDefine.h"

/*
	�ö���Ϊ�ű����������ͼ������������ṩ����ת��������������
	�Լ�����ʱ������������
*/

class RQVar;
class RQVarTypeCompute {
public:
	typedef RQPtrVal  (*Fun_Type)(RQVar*);
	/*
		������Ľű�����������ת��ΪĿ�����͵����
		val			[in]	����Դ
		targetType	[in]	����ԴĿ������
	*/
	static RQPtrVal Compute(RQVar* val,int targetType);
	/*
		��ʼ�����ͼ������Ĳ���
	*/
	static void InitObj();
private:

	/*
		���ڲ���ֵ�ж��Ƕ��ڴ�ֵ��0�ļ��
	*/
	static RQPtrVal _AllToBool(RQVar* val);
	/*
		������������ת��Ϊ��������
	*/
	static RQPtrVal _LLongToLLong(RQVar* val);
	static RQPtrVal _FloatToLLong(RQVar* val);
	static RQPtrVal _DoubleToLLong(RQVar* val);
	static RQPtrVal _StrToLLong(RQVar* val);
	/*
		�������͵�����ת��Ϊ����������
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
		������������ת��Ϊ�ַ���
	*/
	static RQPtrVal _LLongToStr(RQVar* val);
	static RQPtrVal _FloatToStr(RQVar* val);
	static RQPtrVal _DoubleToStr(RQVar* val);
	static RQPtrVal _StrToStr(RQVar* val);
private:
	static void _InitObj_Integer();		//��ʼ������ԴΪ�����Ĵ���
	static void _InitObj_Float();		//��ʼ������ԴΪ�������Ĵ��������float����ָfloat����
	static void _InitObj_Str();
	static void _InitObj_Double();
	///static void _InitObj_Bool();

	template<typename Ti,typename To>
	static RQPtrVal _Ti_To_To(RQVar* val);

private:
	/*
		������Ϊ����ת�����������飬
		��һά����ʾ�������Դ����
		�ڶ�ά����ʾ��������Դ����
	*/
	static Fun_Type _funArray[M_VAR_TYPE_TOTAL][M_VAR_TYPE_TOTAL];	//���庯������
};
