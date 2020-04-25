#include "RQRigout.h"
#include "RQSys.h"
#include "RQVar.h"
#include "RQStrTool.h"


typedef RQRigout::Fun_Type Fun_Type;
Fun_Type RQRigout::_funArray[RQRigout::M_OPT_TOTAL][M_VAR_TYPE_TOTAL];

#define M_INIT_OPT(opt,type,fun)		_funArray[opt][type]=fun
#define M_INIT_RIGOUT_OPT(type,fun)		M_INIT_OPT(M_OPT_RIGOUT,type,fun)
#define M_INIT_ANALYZE_OPT(type,fun)	M_INIT_OPT(M_OPT_ANALYZE,type,fun)

//在数据包中数据长度的数据类型
#define M_DATA_LEN_TYPE		int


#pragma pack (push,1)
struct PackData_Head {
	unsigned char _type;
	unsigned M_DATA_LEN_TYPE _dataLen;
};
#pragma pack(pop)


void RQRigout::InitObj()
{
	//初始化组包操作
	M_INIT_RIGOUT_OPT(M_VAR_TYPE_BOOL, &RigoutBool);
	M_INIT_RIGOUT_OPT(M_VAR_TYPE_INT, &RigoutInt);
	M_INIT_RIGOUT_OPT(M_VAR_TYPE_LONG, &RigoutLong);
	M_INIT_RIGOUT_OPT(M_VAR_TYPE_LLONG, &RigoutLLong);
	M_INIT_RIGOUT_OPT(M_VAR_TYPE_FLOAT, &RigoutFloat);
	M_INIT_RIGOUT_OPT(M_VAR_TYPE_DOUBLE, &RigoutDouble);
	M_INIT_RIGOUT_OPT(M_VAR_TYPE_FUN, &RigoutFun);
	M_INIT_RIGOUT_OPT(M_VAR_TYPE_OBJ, &RigoutObj);
	M_INIT_RIGOUT_OPT(M_VAR_TYPE_STR, &RigoutStr);

	M_INIT_ANALYZE_OPT(M_VAR_TYPE_BOOL, &AnalyzeBool);
	M_INIT_ANALYZE_OPT(M_VAR_TYPE_INT, &AnalyzeInt);
	M_INIT_ANALYZE_OPT(M_VAR_TYPE_LONG, &AnalyzeLong);
	M_INIT_ANALYZE_OPT(M_VAR_TYPE_LLONG, &AnalyzeLLong);
	M_INIT_ANALYZE_OPT(M_VAR_TYPE_FLOAT, &AnalyzeFloat);
	M_INIT_ANALYZE_OPT(M_VAR_TYPE_DOUBLE, &AnalyzeDouble);
	M_INIT_ANALYZE_OPT(M_VAR_TYPE_FUN, &AnalyzeFun);
	M_INIT_ANALYZE_OPT(M_VAR_TYPE_OBJ, &AnalyzeObj);
	M_INIT_ANALYZE_OPT(M_VAR_TYPE_STR, &AnalyzeStr);
}


int RQRigout::RigoutBody(char * buff, RQVar ** varArray, unsigned int varTotal)
{
	int writeLen = 0;
	for (unsigned int index = 0; index < varTotal&&varArray[index]; index++)
	{
		writeLen+=RigoutBody(buff + writeLen, varArray[index]);
	}

	return writeLen;
}

int RQRigout::RigoutBody(char * buff, RQVar * var)
{
	return _funArray[M_OPT_RIGOUT][var->GetType()](buff, var);
}


/*
	----------------------解包调用入口
*/

int RQRigout::AnalyzeBody(char * buff, unsigned int length, RQVar ** varArray, unsigned int * dataCount, unsigned varTotal)
{
	unsigned int temp_dataCount;
	unsigned int* count = dataCount ? dataCount : &temp_dataCount;		///获取有效计数空间
	*count = 0;
	unsigned int readBytes = 0;
	int isOk = (readBytes < length)		///获取条件要求数据缓冲区的获取不越界
		* !!varArray[*count]				///当前有脚本变量可以存储缓冲区数据段数据
		* (*count < varTotal);			///还剩有可使用的缓冲区
	while (isOk)
	{
		 readBytes += AnalyzeBody(buff + readBytes, varArray[*count]);	///获取下一节数据段数据
		++(*count);						///调整计数下标，准备进行下一轮获取
		isOk = (readBytes < length)* !!varArray[*count]*(*count < varTotal);
	}

	while (varArray[*count] && *count < varTotal)
	{
		varArray[(*count)++]->SetLLong(0);			///将未被赋予数据段的脚本变量置空
	}
	return readBytes;
}

int RQRigout::AnalyzeBody(char * buff, RQVar * var)
{
	return _funArray[M_OPT_ANALYZE][buff[0]](buff,var);
}

/*
	----------------------分类型组包
*/
#define M_CALL_RIGOUT(TYPE,typeId,val)	\
{											\
	TYPE data = val;				\
	return Rigout(buff,&data,sizeof(TYPE),typeId);	\
}

char * RQRigout::ToData(char * buff)
{
	if (buff)
	{
		buff += 1 + sizeof(M_DATA_LEN_TYPE);
	}
	return buff;
}

int RQRigout::DateHeadSize()
{
	return sizeof(PackData_Head);
}

int RQRigout::RigoutBool(char * buff, RQVar * var)
{
	M_CALL_RIGOUT(bool, M_VAR_TYPE_BOOL, var->GetBool());
}

int RQRigout::RigoutInt(char* buff, RQVar* var)
{
	M_CALL_RIGOUT(int, M_VAR_TYPE_INT, RQHton(var->GetInt()));
}

int RQRigout::RigoutFun(char * buff, RQVar * var)
{
	M_CALL_RIGOUT(void*, M_VAR_TYPE_FUN, var->GetFun());
}

int RQRigout::RigoutObj(char * buff, RQVar * var)
{
	M_CALL_RIGOUT(void*, M_VAR_TYPE_OBJ, var->GetObj());
}

int RQRigout::RigoutLong(char * buff, RQVar * var)
{
	M_CALL_RIGOUT(long, M_VAR_TYPE_LONG, RQHton(var->GetLong()));
}

int RQRigout::RigoutLLong(char * buff, RQVar * var)
{
	M_CALL_RIGOUT(long long, M_VAR_TYPE_LLONG, RQHton(var->GetLLong()));
}

int RQRigout::RigoutFloat(char * buff, RQVar * var)
{
	M_CALL_RIGOUT(float, M_VAR_TYPE_FLOAT, RQHton(var->GetFloat()));
}

int RQRigout::RigoutDouble(char * buff, RQVar * var)
{
	M_CALL_RIGOUT(double, M_VAR_TYPE_DOUBLE, var->GetDouble());
}

int RQRigout::RigoutStr(char * buff, RQVar * var)
{
	int writeBytes = Rigout(buff, var->GetStr(),var->GetStrLength(), M_VAR_TYPE_STR);
	buff[writeBytes] = 0;
	return writeBytes;
}



int RQRigout::Rigout(char * buff, void * addr, int dataLen, int dataType)
{
	PackData_Head* data = (PackData_Head*)buff;
	data->_type = dataType;
	data->_dataLen = RQHton((M_DATA_LEN_TYPE)dataLen);
	RQCopy(buff + sizeof(PackData_Head), addr, dataLen);
	return sizeof(PackData_Head)+dataLen;
}


/*
	------------------解包处理
*/

#define M_CALL_ANALYZE(TYPE,typeId)		\
{										\
	TYPE data = *(TYPE*)(buff+sizeof(PackData_Head));	\
	data = RQNtoh(data);								\
	return Analyze(buff,var,&data,typeId);				\
}

int RQRigout::AnalyzeBool(char * buff, RQVar * var)
{
	M_CALL_ANALYZE(bool, M_VAR_TYPE_BOOL);
}

int RQRigout::AnalyzeInt(char * buff, RQVar * var)
{
	M_CALL_ANALYZE(int, M_VAR_TYPE_INT);
}

int RQRigout::AnalyzeFun(char * buff, RQVar * var)
{
	M_CALL_ANALYZE(void*, M_VAR_TYPE_FUN);
}

int RQRigout::AnalyzeObj(char * buff, RQVar * var)
{
	M_CALL_ANALYZE(void*, M_VAR_TYPE_FUN);
}

int RQRigout::AnalyzeLong(char * buff, RQVar * var)
{
	M_CALL_ANALYZE(long, M_VAR_TYPE_LONG);
}

int RQRigout::AnalyzeLLong(char * buff, RQVar * var)
{
	M_CALL_ANALYZE(long long, M_VAR_TYPE_LLONG);
}

int RQRigout::AnalyzeFloat(char * buff, RQVar * var)
{
	M_CALL_ANALYZE(float, M_VAR_TYPE_FLOAT);
}

int RQRigout::AnalyzeDouble(char * buff, RQVar * var)
{
	M_CALL_ANALYZE(double, M_VAR_TYPE_DOUBLE);
}

int RQRigout::AnalyzeStr(char * buff, RQVar * var)
{
	PackData_Head* head = (PackData_Head*)buff;
	int dataLen = RQNtoh((M_DATA_LEN_TYPE)head->_dataLen);
	var->SetStr(buff + sizeof(PackData_Head), dataLen);
	return dataLen + sizeof(PackData_Head);
}

int RQRigout::Analyze(char * buff,RQVar* var, void * data,int dataType)
{
	PackData_Head* head = (PackData_Head*)buff;
	var->Set(data, dataType);
	return RQNtoh((M_DATA_LEN_TYPE)head->_dataLen)+sizeof(PackData_Head);
}
