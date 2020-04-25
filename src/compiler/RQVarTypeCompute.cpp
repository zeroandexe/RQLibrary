#include "compiler/RQVarTypeCompute.h"
#include "RQPtrVal.h"
#include "RQVar.h"
#include "tool/RQMemory.h"
#include "RQStrTool.h"

typedef RQVarTypeCompute::Fun_Type Fun_Type;
Fun_Type RQVarTypeCompute::_funArray[M_VAR_TYPE_TOTAL][M_VAR_TYPE_TOTAL];	//���庯������

template<typename Ti, typename To>
inline RQPtrVal RQVarTypeCompute::_Ti_To_To(RQVar * val)
{
	Ti * data = (Ti *)val->_data->GetAddr();
	RQPtrVal ptrVal((To)*data);
	return static_cast<RQPtrVal&&>(ptrVal);
}

RQPtrVal RQVarTypeCompute::Compute(RQVar * val, int targetType)
{
	return _funArray[targetType][val->_type](val);		///�˴����ӱ�����������Ч����������ֱ�ӱ���
}

void RQVarTypeCompute::InitObj()
{
	_InitObj_Integer();			///��ʼ��ԴΪ���������ʹ���
	_InitObj_Float();			///��ʼ��ԴΪ4�ֽڸ��������ʹ���
	_InitObj_Double();			///��ʼ��ԴΪ8�ֽڸ���������  
	_InitObj_Str();				///��ʼ��ԴΪ�ַ������ʹ���
 }


RQPtrVal RQVarTypeCompute::_AllToBool(RQVar * val)
{
	RQPtrVal ptrVal(*(long long*)val->_data->GetAddr());
	return static_cast<RQPtrVal&&>(ptrVal);
}

RQPtrVal RQVarTypeCompute::_LLongToLLong(RQVar * val)
{
	return _Ti_To_To<long long, long long>(val);
}

RQPtrVal RQVarTypeCompute::_FloatToLLong(RQVar * val)
{
	return _Ti_To_To<float, long long>(val);
}

RQPtrVal RQVarTypeCompute::_DoubleToLLong(RQVar * val)
{
	return _Ti_To_To<double, long long>(val);
}

RQPtrVal RQVarTypeCompute::_StrToLLong(RQVar * val)
{
	long long data=RQStrToInt10((char*)val->_data->GetAddr());
	RQPtrVal ptrVal(data);
	return static_cast<RQPtrVal&&>(ptrVal);
}



RQPtrVal RQVarTypeCompute::_LLongToDouble(RQVar * val)
{
	return _Ti_To_To<long long, double>(val);
}

RQPtrVal RQVarTypeCompute::_FloatToDouble(RQVar * val)
{
	return _Ti_To_To<float, double>(val);
}

RQPtrVal RQVarTypeCompute::_DoubleToDouble(RQVar * val)
{
	return _Ti_To_To<double, double>(val);
}

RQPtrVal RQVarTypeCompute::_StrToDouble(RQVar * val)
{
	double data = RQStrToDouble((char*)val->_data->GetAddr(),val->_data->GetSize());
	RQPtrVal ptrVal(data);
	return static_cast<RQPtrVal&&>(ptrVal);
}

RQPtrVal RQVarTypeCompute::_LLongToFloat(RQVar * val)
{
	return _Ti_To_To<long long, float>(val);
}

RQPtrVal RQVarTypeCompute::_FloatToFloat(RQVar * val)
{
	return _Ti_To_To<float, float>(val);
}

RQPtrVal RQVarTypeCompute::_DoubleToFloat(RQVar * val)
{
	return _Ti_To_To<double, float>(val);
}

RQPtrVal RQVarTypeCompute::_StrToFloat(RQVar * val)
{
	float data = (float) RQStrToDouble((char*)val->_data->GetAddr(), val->_data->GetSize());
	RQPtrVal ptrVal(data);
	return static_cast<RQPtrVal&&>(ptrVal);
}

RQPtrVal RQVarTypeCompute::_LLongToStr(RQVar * val)
{
	val->_buff->Resize(128);	///��չ�ڴ�ռ����ڴ��llong���ݵ��ַ���
	long long data = *(long long *)val->_data->GetAddr();		///��ȡ����
	///������ת��Ϊ�ַ��������д洢
	RQIntegerToString(data, (char* )val->_buff->GetAddr(), val->_buff->GetSize());
	RQPtrVal ptrVal(val->_buff->GetAddr());		///�����������ַ��������浽�˻����������Խ��������ڴ��ַ����		
	return static_cast<RQPtrVal&&>(ptrVal);
}

RQPtrVal RQVarTypeCompute::_FloatToStr(RQVar * val)
{
	val->_buff->Resize(128);	///��չ�ڴ�ռ����ڴ��llong���ݵ��ַ���
	float data = *(float *)val->_data->GetAddr();		///��ȡ����
	///������ת��Ϊ�ַ��������д洢
	RQFloatToStr(data, (char*)val->_buff->GetAddr(), val->_buff->GetSize(),M_PRECISION_FLOAT);
	RQPtrVal ptrVal(val->_buff->GetAddr());		///�����������ַ��������浽�˻����������Խ��������ڴ��ַ����		
	return static_cast<RQPtrVal&&>(ptrVal);
}

RQPtrVal RQVarTypeCompute::_DoubleToStr(RQVar * val)
{
	val->_buff->Resize(128);	///��չ�ڴ�ռ����ڴ��llong���ݵ��ַ���
	double data = *(double *)val->_data->GetAddr();		///��ȡ����
	///������ת��Ϊ�ַ��������д洢
	RQDoubleToStr(data, (char*)val->_buff->GetAddr(), val->_buff->GetSize(),M_PRECISION_DOUBLE);
	RQPtrVal ptrVal(val->_buff->GetAddr());		///�����������ַ��������浽�˻����������Խ��������ڴ��ַ����		
	return static_cast<RQPtrVal&&>(ptrVal);
}

RQPtrVal RQVarTypeCompute::_StrToStr(RQVar * val)
{
	RQPtrVal ptrVal(val->_data->GetAddr());
	return static_cast<RQPtrVal&&>(ptrVal);
}

/*
	---------------------��ʼ������
*/
void RQVarTypeCompute::_InitObj_Integer()
{
	///��ʼ�����ֲ�
	int type[] = { M_VAR_TYPE_BOOL,
		M_VAR_TYPE_INT ,
		M_VAR_TYPE_LONG,
		M_VAR_TYPE_LLONG,
		M_VAR_TYPE_FUN,
		M_VAR_TYPE_OBJ };


	for (int i = 0; i < sizeof(type) / sizeof(int); i++)
	{
		_funArray[type[i]][M_VAR_TYPE_BOOL] = _LLongToLLong;
		_funArray[type[i]][M_VAR_TYPE_INT] = _LLongToLLong;
		_funArray[type[i]][M_VAR_TYPE_LONG] = _LLongToLLong;
		_funArray[type[i]][M_VAR_TYPE_LLONG] = _LLongToLLong;
		_funArray[type[i]][M_VAR_TYPE_FUN] = _LLongToLLong;
		_funArray[type[i]][M_VAR_TYPE_OBJ] = _LLongToLLong;
		_funArray[type[i]][M_VAR_TYPE_FLOAT] = _FloatToLLong;
		_funArray[type[i]][M_VAR_TYPE_DOUBLE] = _DoubleToLLong;
		_funArray[type[i]][M_VAR_TYPE_STR] = _StrToLLong;
	}

	///����ֵ���������������ͣ��������ڴ����ݷ�0���
	_funArray[M_VAR_TYPE_BOOL][M_VAR_TYPE_STR] = _LLongToLLong;
	_funArray[M_VAR_TYPE_BOOL][M_VAR_TYPE_FLOAT] = _LLongToLLong;
	_funArray[M_VAR_TYPE_BOOL][M_VAR_TYPE_DOUBLE] = _LLongToLLong;
	_funArray[M_VAR_TYPE_BOOL][M_VAR_TYPE_INVALID] = _LLongToLLong;
}

void RQVarTypeCompute::_InitObj_Float()
{
	_funArray[M_VAR_TYPE_FLOAT][M_VAR_TYPE_BOOL] = _LLongToFloat;
	_funArray[M_VAR_TYPE_FLOAT][M_VAR_TYPE_INT] = _LLongToFloat;
	_funArray[M_VAR_TYPE_FLOAT][M_VAR_TYPE_LONG] = _LLongToFloat;
	_funArray[M_VAR_TYPE_FLOAT][M_VAR_TYPE_LLONG] = _LLongToFloat;
	_funArray[M_VAR_TYPE_FLOAT][M_VAR_TYPE_FUN] = _LLongToFloat;
	_funArray[M_VAR_TYPE_FLOAT][M_VAR_TYPE_OBJ] = _LLongToFloat;
	_funArray[M_VAR_TYPE_FLOAT][M_VAR_TYPE_FLOAT] = _FloatToFloat;
	_funArray[M_VAR_TYPE_FLOAT][M_VAR_TYPE_DOUBLE] = _DoubleToFloat;
	_funArray[M_VAR_TYPE_FLOAT][M_VAR_TYPE_STR] = _StrToFloat;
}

void RQVarTypeCompute::_InitObj_Double()
{
	_funArray[M_VAR_TYPE_DOUBLE][M_VAR_TYPE_BOOL] = _LLongToDouble;
	_funArray[M_VAR_TYPE_DOUBLE][M_VAR_TYPE_INT] = _LLongToDouble;
	_funArray[M_VAR_TYPE_DOUBLE][M_VAR_TYPE_LONG] = _LLongToDouble;
	_funArray[M_VAR_TYPE_DOUBLE][M_VAR_TYPE_LLONG] = _LLongToDouble;
	_funArray[M_VAR_TYPE_DOUBLE][M_VAR_TYPE_FUN] = _LLongToDouble;
	_funArray[M_VAR_TYPE_DOUBLE][M_VAR_TYPE_OBJ] = _LLongToDouble;
	_funArray[M_VAR_TYPE_DOUBLE][M_VAR_TYPE_FLOAT] = _FloatToDouble;
	_funArray[M_VAR_TYPE_DOUBLE][M_VAR_TYPE_DOUBLE] = _DoubleToDouble;
	_funArray[M_VAR_TYPE_DOUBLE][M_VAR_TYPE_STR] = _StrToDouble;
}


void RQVarTypeCompute::_InitObj_Str()
{
	///��ʼ�����ֲ���
	_funArray[M_VAR_TYPE_STR][M_VAR_TYPE_BOOL] = _LLongToStr;
	_funArray[M_VAR_TYPE_STR][M_VAR_TYPE_INT] = _LLongToStr;
	_funArray[M_VAR_TYPE_STR][M_VAR_TYPE_LONG] = _LLongToStr;
	_funArray[M_VAR_TYPE_STR][M_VAR_TYPE_LLONG] = _LLongToStr;
	_funArray[M_VAR_TYPE_STR][M_VAR_TYPE_FUN] = _LLongToStr;
	_funArray[M_VAR_TYPE_STR][M_VAR_TYPE_OBJ] = _LLongToStr;
	_funArray[M_VAR_TYPE_STR][M_VAR_TYPE_FLOAT] = _FloatToStr;
	_funArray[M_VAR_TYPE_STR][M_VAR_TYPE_DOUBLE] = _DoubleToStr;
	_funArray[M_VAR_TYPE_STR][M_VAR_TYPE_STR] = _StrToStr;
}
