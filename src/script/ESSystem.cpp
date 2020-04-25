#include <iostream>
#include "define/RQInterDefine.h"
#include "compiler/RQGrammar.h"
#include "RQCompiler.h"
#include "script/ESSystem.h"
#include "define/ESDefine.h"
#include "compiler/RQEnviVarTable.h"
#include "RecycleBin.h"
#include "RQSys.h"
#include "tool/RQMemory.h"
#include "RQRegex.h"
#include "RQStrTool.h"
#include "compiler/RQEnviVarTable.h"
#include "RQRigout.h"
#include "RQPack.h"
#include "RQThread.h"
#include "node/RQNullNode.h"
#include "RQRegularPool.h"

//extern RQEnviVarTable* _enviTable;
/*
	提供脚本打印函数，函数结束时不打印换行
*/
static void sys_print(RQVar ** varArray)
{
	int index = M_VARARRAY_INDEX_PARAM_START;			//获取参数起始下标
	while (varArray[index])					//检测下标所指的元素是否存在
	{
		switch (varArray[index]->GetType())
		{
		case M_VAR_TYPE_BOOL:
			std::cout << (varArray[index]->GetBool() ? "true" : "false") << std::endl;
			break;
		case M_VAR_TYPE_INT:
		case M_VAR_TYPE_LONG:
		case M_VAR_TYPE_LLONG:
			std::cout << varArray[index]->GetLLong();
			break;
		case M_VAR_TYPE_FUN:
		case M_VAR_TYPE_OBJ:
			std::cout << "0x" << std::ios_base::hex << varArray[index]->GetLLong();
			break;
		case M_VAR_TYPE_FLOAT:
		case M_VAR_TYPE_DOUBLE:
			std::cout << varArray[index]->GetDouble() << std::endl;
			break;
		case M_VAR_TYPE_STR:
			std::cout << varArray[index]->GetStr();
			break;
		default:
			std::cout << "**error: invalid RQVar type**";
		}
		index++;
	}
}

/*
	提供脚本打印函数，函数结束时打印换行
*/
static void sys_println(RQVar ** varArray)
{
	sys_print(varArray);						
	std::cout << std::endl;
}

/*
	函数参数，1个
	变量1：字符串类型变量，表示动态库的地址
	exp：返回动态库句柄（值由动态库函数返回的句柄值决定），类型（暂定为ptr）
*/
static void sys_load_dll(RQVar ** varArray)
{

}

/*
	设置寄存器变量，通常用于脚本调用对单参数的返回
	函数参数：1个
	变量1：变量名
*/
static void sys_set_exp(RQVar ** varArray)
{
	RQVar * exp = ES_EXP;						//获取寄存器变量
	RQVar * param = ES_PARAM(0);			//获取第一个参数
	if (param)			//检测参数是否有效,若有效则返回将_param的值赋给exp，若无效则返回整数-1
	{
		exp->Assign(param);
	}
	else {
		int retVal = -1;
		exp->SetInt(retVal);		//参数无效则，返回整数-1
	}
}

/*
	将传入的参数按照key-value的形式添加到局部变量表中
	参数0	[in]	做为key，值要求为字符串类型，并将成为变量名
	参数1	[in]	作为val，作为对应的被掉变量
*/
static void sys_add_local__keyVal(RQVar ** varArray)
{
	std::map<std::string, RQVar *>* localMap = (std::map<std::string, RQVar *>*) varArray[M_VARARRAY_INDEX_LOCALMAP];
	RQVar* key = ES_PARAM(0);
	RQVar* val = ES_PARAM(1);
	int isOk = !!key * !!val;
	if (isOk)
	{
		if (key->GetType() == M_VAR_TYPE_STR)
		{
			(*localMap)[key->GetStr()] = val;
		}
	}
}

/*
	将传入的参数按照key-value的形式添加到全局变量表中
	参数0	[in]	做为key，值要求为字符串类型，并将成为变量名
	参数1	[in]	作为val，作为对应的被掉变量
*/
static void sys_add_envi_keyVal(RQVar ** varArray)
{
	RQVar* key = ES_PARAM(0);
	RQVar* val = ES_PARAM(1);
	int isOk = !!key * !!val;
	if (isOk)
	{
		if (key->GetType() == M_VAR_TYPE_STR)
		{
			RQVar* val = RQEnviVarTable::GetVar(key->GetStr());			///获取环境变量
			val->Assign(val);											///将期望的值拷贝入环境变量中

		}
	}
}

/*
	按照传入的变量名，从环境变量表中移除对应变量
	输入：
		所有输入都为变量名
*/
static void sys_remove_envi(RQVar ** varArray)
{
	///std::map<std::string, RQVar *>* localMap = (std::map<std::string, RQVar *>*) ES_LOCAL;
	
	for(int index = M_VARARRAY_INDEX_PARAM_START;varArray[index];index++)
	{
		///_enviTable->RemoveVar(varArray[index]->GetStr());
		RQEnviVarTable::RemoveVar(varArray[index]->GetStr());
	}
}

/*
	提供脚本嵌套调用
	函数参数：1个
	变量1：脚本名称
*/
static void sys_run_script(RQVar ** varArray)
{
	RQVar * exp = ES_EXP;
	RQVar * filePaht = ES_PARAM(0);

	if (!filePaht)
	{
		return;
	}

	RQCompiler* inter = new RQCompiler();						//创建一个编译器对象
	inter->RunScript(filePaht->GetStr());				//执行解释器，并获取脚本执行结果
	exp->Assign(inter->GetExp());								//将解释器执行的脚本结果传递到上层解释器
	delete inter;												//当前解释器无效，销毁解释器
}

/*
	提供脚本的if调用
*/
static void sys_if(RQVar ** varArray)
{
	RQVar * exp = ES_EXP;
	unsigned int lastParamIndex = M_VARARRAY_INDEX_PARAM_START;
	bool retBol = false;				//默认检测结果为false

	while (varArray[lastParamIndex])lastParamIndex++;			//找到最后一个参数的下一个下标
	lastParamIndex -=1;							//调整下标值，定位到最后一个有效参数所在的元素下标

	//调整参数下标，放置参数下标越界
	lastParamIndex = lastParamIndex < M_VARARRAY_INDEX_PARAM_START ? M_VARARRAY_INDEX_PARAM_START : lastParamIndex;

	if (varArray[lastParamIndex])			//检测是否具有参数，放置if的空参数调用
	{
		retBol = varArray[lastParamIndex]->GetBool();
	}
	exp->SetBool(retBol);					//将if的运算结果传入寄存器变量
}

//由于if和while的处理并没有任何差异，所以可以直接调用if的处理
static void sys_while(RQVar ** varArray)
{
	sys_if(varArray);
}

//else，仅仅起到代码块跳转的作用，所以可以不给与任何实现
static void sys_else(RQVar ** varArray)
{
}

/*
	提供将脚本变量添加到环境变量的操作
	将脚本中的数据变量拷贝到环境变量表中
	参数个数：多个
*/
static void sys_push_var(RQVar ** varArray)
{
	///遍历传入参数，将将所传入参数向环境变量表中填写
	for (int index = M_VARARRAY_INDEX_PARAM_START; varArray[index]; index++)
	{
		const char* name = varArray[index]->GetName();
		///RQVar* var = _enviTable->GetVar(name);			///获取该名称下的脚本变量对象，防止因存入同名脚本对象导致内存泄流
		RQVar* var = RQEnviVarTable::GetVar(name);
		var->Assign(varArray[index]);		///将当前的数据值赋予给环境变量表中
	}
}

/*
	从环境变量表中获取对应名称的脚本变量，并拷贝其值
	参数个数：多个
*/
static void sys_pull_var(RQVar ** varArray)
{
	///遍历传入参数，按照传入的参数从环境变量表中获取数据
	for (int index = M_VARARRAY_INDEX_PARAM_START; varArray[index]; index++)
	{
		///RQVar * enviVar = _enviTable->GetVar(varArray[index]->GetName());		///获取参数所指定的环境变量
		RQVar* enviVar = RQEnviVarTable::GetVar(varArray[index]->GetName());
		if (enviVar)	///检查变量是否存在
		{
			varArray[index]->Assign(enviVar);	///存在则将环境变量的值赋予传入参数
		}
		else {
			varArray[index]->SetInt(0);			///不存在则将参数置0
		}
	}
}

/*
	将传入的句柄组进行回收
	输入：多个
		_param 0~n：传入将被回收的句柄
*/
static void sys_clear_handle(RQVar** varArray)
{
	for (int index = M_VARARRAY_INDEX_PARAM_START; varArray[index]; index++)
	{
		RecycleBin::RecycleNode(varArray[index]);
	}
}


/*
	提供delete 操作
	输入：n个
*/
static void sys_delete(RQVar** varArray)
{
	for (int index = M_VARARRAY_INDEX_PARAM_START ; varArray[index]; index++)
	{
		delete varArray[index]->GetObj();
	}
}

/*
	提供毫秒级睡眠
	输入：1个
		_param	[int]	毫秒数
*/
static void sys_sleep_ms(RQVar** varArray)
{
	RQSleepMs(ES_PARAM(0)->GetInt());
}

/*
	提供秒级睡眠
	输入：1个
		_param	[int]	秒数
*/
static void sys_sleep_s(RQVar** varArray)
{
	RQSleepSec(ES_PARAM(0)->GetInt());
}


/*
	将传入变量的值转换为函数所写类型，并返回
	输入：1个
		_param	[in]	数据源
	输出：1个
		exp		[out]	转换后的值
*/
static void sys_var_to_bool(RQVar** varArray)
{
	ES_EXP->SetBool(ES_PARAM(0)->GetBool());
}

static void sys_var_to_int(RQVar** varArray)
{
	ES_EXP->SetInt(ES_PARAM(0)->GetInt());
}
static void sys_var_to_long(RQVar** varArray)
{
	ES_EXP->SetLong(ES_PARAM(0)->GetLong());
}

static void sys_var_to_llong(RQVar** varArray)
{
	ES_EXP->SetLLong(ES_PARAM(0)->GetLLong());
}

static void sys_var_to_float(RQVar** varArray)
{
	ES_EXP->SetFloat(ES_PARAM(0)->GetFloat());
}

static void sys_var_to_double(RQVar** varArray)
{
	ES_EXP->SetDouble(ES_PARAM(0)->GetDouble());
}

static void sys_var_to_obj(RQVar** varArray)
{
	ES_EXP->SetObj(ES_PARAM(0)->GetObj());
}

static void sys_var_to_fun(RQVar** varArray)
{
	ES_EXP->SetFun(ES_PARAM(0)->GetFun());
}

static void sys_var_to_str(RQVar** varArray)
{
	ES_EXP->SetStr(ES_PARAM(0)->GetStr());
}

/*
	将传入变量的值转换为目标类型并存储，支持多参数
	输入：N个
		_param	[in]	数据源
*/


#define ES_FUN_TURN_TYPE(SetFun,GetFun)		\
{											\
	int i = -1;								\
	while (ES_PARAM((++i)))					\
	{										\
		ES_PARAM(i)->SetFun(ES_PARAM(i)->GetFun());		\
	}										\
}

static void sys_var_turn_bool(RQVar** varArray)
{
	ES_FUN_TURN_TYPE(SetBool, GetBool);
}

static void sys_var_turn_int(RQVar** varArray)
{
	ES_FUN_TURN_TYPE(SetInt, GetInt);
}
static void sys_var_turn_long(RQVar** varArray)
{
	ES_FUN_TURN_TYPE(SetLong, GetLong);
}

static void sys_var_turn_llong(RQVar** varArray)
{
	ES_FUN_TURN_TYPE(SetLLong, GetLLong);
}

static void sys_var_turn_float(RQVar** varArray)
{
	ES_FUN_TURN_TYPE(SetFloat, GetFloat);
}

static void sys_var_turn_double(RQVar** varArray)
{
	ES_FUN_TURN_TYPE(SetDouble, GetDouble);
}

static void sys_var_turn_obj(RQVar** varArray)
{
	ES_FUN_TURN_TYPE(SetObj, GetObj);
}

static void sys_var_turn_fun(RQVar** varArray)
{
	ES_FUN_TURN_TYPE(SetFun, GetFun);
}

static void sys_var_turn_str(RQVar** varArray)
{
	ES_FUN_TURN_TYPE(SetStr, GetStr);
}

/*
	将传入变量的类型作为字符串并进行返回
	输入：1个
		_param	[in]	将被获取类型值的脚本变量
	输出：1个
		exp		[out]	返回变量类型字符串
*/
static void sys_var_type_str(RQVar** varArray)
{
	static const char* typeStr[] = { "invalid","bool","int","static void(RQVar**)","static void*","long","long long","float","double","char*" };
	ES_EXP->SetStr(typeStr[ES_PARAM(0)->GetType()]);
}

/*
	将传入变量的类型作为数值并进行返回
	输入：1个
		_param	[in]	将被获取类型值的脚本变量
	输出：1个
		exp		[out]	返回变量类型数值
*/
static void sys_var_type_int(RQVar** varArray)
{
	ES_EXP->SetInt(ES_PARAM(0)->GetType());
}

/*
	按照给定的表达式对源字符串进行截取、调整，
	将所有目标子串拼接为一个字符串，返回获取的字节数
	输入（偏移）：
	0:	sourStr			[in]	含有目标子串的源字符串
	:	sourStrLen		[in]	源字符串长度
	1:	formCopy		[in]	描述对源字符串的拷贝格式
	2:	replaceParrent	[in]	检测源字符串中将要被替换的目标子串
	3:	replaceStr		[in]	用于替换的字符串
	4:	searchParrent	[in]	用于截取目标子串的表达式
	5:	adjustParrent	[in]	调整被截取下来的子串表达式
	6:	outBuff			[out]	存放目标字符串的内存空间
	7:	buffLen			[in]	存放空间的字节大小
	exp			[out]	本次获取得到的字节数
*/
static void sys_regex_str(RQVar** varArray)
{
	///---获取临时内存空间
	int newSize = ES_PARAM(0)->GetDataLenght() + 4;
	RQMemory* memory = RecycleBin::GetFreeRQMemory();
	char* tempStr = 0;

	///---按照指定格式拷贝源字符数据
	char* formCopy = ES_PARAM(1)->GetStr();
	if (formCopy&&formCopy[0])
	{
		memory->Resize(newSize);
		tempStr = (char*)memory->GetAddr();
		
		formCopy = formCopy ? formCopy : (char*)"%s";
		sprintf(tempStr, formCopy, ES_PARAM(0)->GetStr());
	}
	else
	{
		tempStr = ES_PARAM(0)->GetStr();
	}
	
	///---替换目标子串
	char* replaceParrent = ES_PARAM(2)->GetStr();
	if (replaceParrent&&replaceParrent[0])
	{
		RQRegex_replace(tempStr, replaceParrent, ES_PARAM(3)->GetStr(), tempStr);
	}
	///调整结果的空间大小和访问类型
	ES_PARAM(6)->Resize(newSize);
	ES_PARAM(6)->SetStr("");
	///---截取-调整并拼接目标子串
	
	int writeBytes = RQASRegex_match_loop(tempStr,RQStrlen(tempStr),
		ES_PARAM(4)->GetStr(), ES_PARAM(5)->GetStr(),
		(char*)ES_PARAM(6)->GetAddr(), newSize);

	///---校准存储空间的数据长度
	ES_PARAM(6)->SetDataLength(writeBytes);
	///---回收不再使用的内存对象
	RecycleBin::RecycleNode(memory);
	///---将结果字符串的字节数返回
	ES_EXP->SetInt(writeBytes);
}



/*
	数据包解包函数，在解包时保证数据包包头在之前已被解析过,故在解析前会越过解析长度
	1、数据包对象
	2、数据包中用户数据包头长度或数据包偏移量
	3、参数1（解包后存放数据的脚本对象）
	4、....
	exp：解析所使用的字符串长度
*/
static void Script_ParesePack(RQVar** varArray)
{
	RQPack* pack = (RQPack*)ES_PARAM(0)->GetObj();							///获取数据段地址
	int offset = ES_PARAM(1)->GetInt();										///获取解析的数据偏移量
	int writeBytes = RQRigout::AnalyzeBody(((char*)pack->GetDataPtr())+ offset,				///获取数据段地址
		pack->GetDataLen()- offset,
		&ES_PARAM(2));
	ES_EXP->SetInt(writeBytes);												///将被解析出的数据数量交付给脚本
}


/*
	组装包头：
	传入参数为：
	0、组包节点
	1、数据包长度
	2、目标协议簇代号
	3、目标操作代号

*/
static void Script_RigOut_Head(RQVar** varArray)
{
	RQPack * pack = (RQPack*)ES_PARAM(0)->GetObj();
	RQPackHead* head = pack->GetPackHead();
	pack->SetDateLen(ES_PARAM(1)->GetInt());				///设置数据段长度
	
	head->_optProtocol = ES_PARAM(2)->GetInt();				///设置操作协议簇
	head->_optDecorate = ES_PARAM(3)->GetInt();				///设置操作协议号
}

/*
	组装数据包包体
	传入参数：
	1、被写入的数据包
	2、数据写入偏移量
	3、参数1
	...、...
*/
static void Script_RigOut_Body(RQVar** varArray)
{
	RQPack* pack = (RQPack*)ES_PARAM(0)->GetObj();			///获取数据包
	char* dataPtr = (char*) pack->GetDataPtr();				///获取数据写入首地址（包头用户包头）
	dataPtr += ES_PARAM(1)->GetInt();						///调整数据写入下标

	int writeBytes = RQRigout::RigoutBody(dataPtr, varArray + 2);			///将数据包中的后续脚本变量写入数据包中
	ES_EXP->SetInt(writeBytes);								///将写入数据长度交付于脚本
}

/*
	常规组包，传入数据为字符串，数值等
	0、组包节点
	1、目标协议簇代号
	2、目标操作代号
	3、参数1
	...
*/
static void Script_RigOut_Com(RQVar** varArray)
{
	RQPack* pack = (RQPack*)ES_PARAM(0)->GetObj();
	RQPackHead* head = pack->GetPackHead();

	///设置包头
	head->_optProtocol = ES_PARAM(1)->GetInt();
	head->_optDecorate = ES_PARAM(2)->GetInt();
	
	///设置包体
	int writeBytes = RQRigout::RigoutBody((char*)pack->GetDataPtr(), &ES_PARAM(3));
	int headSize = sizeof(RQPackHead);
	pack->SetDateLen(writeBytes);
	ES_EXP->SetInt(writeBytes);
}

/*
	获取一个发送数据包对象
	1、数据包空间大小(默认为8K)
*/
static void Script_GetFreePack(RQVar** varArray)
{
	RQPack* pack = RecycleBin::GetFreeRQPack();
	int packSize = ES_PARAM(0) ? ES_PARAM(0)->GetInt() : 8 * 1024;
	pack->Resize(packSize);					///根据参数开辟数据存放内存空间
	ES_EXP->SetObj(pack);
}

/*
	回收数据包对象
	1、参数1
	2、...
*/
static void Script_RecyclePack(RQVar** varArray)
{
	for (int i = 0; ES_PARAM(i); i++)
	{
		RecycleBin::RecycleNode((RQNode*)ES_PARAM(i));
	}
}

/*
	加载动态库
	1、动态库路径
	2、动态库模式（RTLD_LAZY 、RTLD_NOW，默认为：RTLD_NOW ）
	exp：被加载的动态库句柄
*/
static void Script_LoadDll(RQVar** varArray)
{
	void* dll = 0;
	if (ES_PARAM(0)
		&& ES_PARAM(0)->GetStr())
	{
		int modl = ES_PARAM(1) ? ES_PARAM(1)->GetInt() : 0;
		dll = LoadDll(ES_PARAM(0)->GetStr(), modl);
	}
	ES_EXP->SetObj(dll);
}

/*
	从动态库中加载函数，并将加载的函数存入全局表
	1、动态库句柄
	2、函数名称
	exp：被加载的函数地址
*/
static void Script_LoadFunFromDll(RQVar** varArray)
{
	void* funAddr = LoadFunFromDll(ES_PARAM(0)->GetObj(), ES_PARAM(1)->GetStr());
	if (funAddr)
	{
		///将加载的函数地址注册入全局变量表
		RQEnviVarTable::SetVar(ES_PARAM(1)->GetStr(), funAddr, sizeof(void*), M_VAR_TYPE_FUN);	
	}
	ES_EXP->SetFun(funAddr);
}

//卸载指定的动态库
//1、动态库句柄值
static void Script_UnloadDll(RQVar** varArray)
{
	UnloadDll(ES_PARAM(0)->GetObj());
}

//提供线程函数执行的外壳函数
static void _RunThreadFun(void* param)
{
	typedef void(*ThreadFun_Type)(RQVar** varArray);
	RQNullNode* nnode = (RQNullNode*)param;
	RQThread* t = (RQThread*)nnode->GetPtr(0);
	RQVar** varArray = (RQVar**)nnode->GetPtr(1);

	((ThreadFun_Type)ES_PARAM(0))(varArray);				//利用新线程执行脚本委托的函数，并交付期望的参数

	///线程函数执行完毕，回收资源
	delete varArray;
	RecycleBin::RecycleNode(nnode);
	delete t;
	
}

///根据传入的变量数组，申请同样大小的空间并拷贝数据
static RQVar** _NewMem_CopyVal(RQVar** varArray)
{
	int i = 0;
	for (int i = M_VARARRAY_INDEX_PARAM_START; varArray[i]; i++);
	RQVar** newVarArray = new RQVar*[i];
	RQCopy(newVarArray, varArray, sizeof(i) * sizeof(RQVar*));
	return newVarArray;
}


///定时回调的外壳函数
static void _RunTimeCallBack(void* param)
{
	typedef void(*CallbackdFun_Type)(RQVar** varArray);
	RQVar** varArray = (RQVar**)param;
	((CallbackdFun_Type)ES_PARAM(0)->GetFun())(varArray);

	if (!ES_PARAM(3)->GetInt())											///若本次回调不是循环回调，那么就回收参数数组内存
	{
		delete varArray;
	}
}



//添加函数接口
static void Init_ESSystem_Fun()
{
	ES_ADD_FUN("sys_print", sys_print);
	ES_ADD_FUN("sys_println", sys_println);
	ES_ADD_FUN("sys_set_exp", sys_set_exp);
	ES_ADD_FUN("sys_add_local__keyVal", sys_add_local__keyVal);
	ES_ADD_FUN("sys_add_envi_keyVal", sys_add_envi_keyVal);
	ES_ADD_FUN("sys_remove_envi", sys_remove_envi);
	ES_ADD_FUN("sys_run_script", sys_run_script);
	ES_ADD_FUN("if", sys_if);
	ES_ADD_FUN("while", sys_while);
	ES_ADD_FUN("else", sys_else);
	ES_ADD_FUN("sys_push_var", sys_push_var);
	ES_ADD_FUN("sys_pull_var", sys_pull_var);
	ES_ADD_FUN("sys_clear_handle", sys_clear_handle);
	ES_ADD_FUN("sys_delete", sys_delete);
	ES_ADD_FUN("sys_sleep_ms", sys_sleep_ms);
	ES_ADD_FUN("sys_sleep_s", sys_sleep_s);

	ES_ADD_FUN("sys_var_to_bool", sys_var_to_bool);
	ES_ADD_FUN("sys_var_to_int", sys_var_to_int);
	ES_ADD_FUN("sys_var_to_long", sys_var_to_long);
	ES_ADD_FUN("sys_var_to_llong", sys_var_to_llong);
	ES_ADD_FUN("sys_var_to_float", sys_var_to_float);
	ES_ADD_FUN("sys_var_to_double", sys_var_to_double);
	ES_ADD_FUN("sys_var_to_obj", sys_var_to_obj);
	ES_ADD_FUN("sys_var_to_fun", sys_var_to_fun);
	ES_ADD_FUN("sys_var_to_str", sys_var_to_str);

	ES_ADD_FUN("sys_var_turn_bool", sys_var_turn_bool);
	ES_ADD_FUN("sys_var_turn_int", sys_var_turn_int);
	ES_ADD_FUN("sys_var_turn_long", sys_var_turn_long);
	ES_ADD_FUN("sys_var_turn_llong", sys_var_turn_llong);
	ES_ADD_FUN("sys_var_turn_float", sys_var_turn_float);
	ES_ADD_FUN("sys_var_turn_double", sys_var_turn_double);
	ES_ADD_FUN("sys_var_turn_obj", sys_var_turn_obj);
	ES_ADD_FUN("sys_var_turn_fun", sys_var_turn_fun);
	ES_ADD_FUN("sys_var_turn_str", sys_var_turn_str);

	ES_ADD_FUN("sys_var_type_str", sys_var_type_str);
	ES_ADD_FUN("sys_var_type_int", sys_var_type_int);

	ES_ADD_FUN("sys_regex_str",	sys_regex_str);

	ES_ADD_FUN("sys_paresePack", Script_ParesePack);
	ES_ADD_FUN("sys_rigout_head", Script_RigOut_Head);
	ES_ADD_FUN("sys_rigout_body", Script_RigOut_Body);
	ES_ADD_FUN("sys_rigout_com", Script_RigOut_Com);
	ES_ADD_FUN("sys_getFreePack", Script_GetFreePack);
	ES_ADD_FUN("sys_recyclePack", Script_RecyclePack);

	///---动态库
	ES_ADD_FUN("sys_loadDll", Script_LoadDll);
	ES_ADD_FUN("sys_loadFunFromDll", Script_LoadFunFromDll);
	ES_ADD_FUN("sys_unloadDll", Script_UnloadDll);
}

static void Init_ESSystem_Var()
{
	int val = 0;
	RQEnviVarTable::SetVar("NULL", &val, sizeof(int), M_VAR_TYPE_INT);
	RQEnviVarTable::SetVar("false", &val, sizeof(int), M_VAR_TYPE_BOOL);

	val = 1;
	RQEnviVarTable::SetVar("true", &val, sizeof(int), M_VAR_TYPE_BOOL);
}

void Init_ESSystem()
{
	Init_ESSystem_Var();
	Init_ESSystem_Fun();
}