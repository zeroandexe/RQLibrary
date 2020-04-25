#pragma once
#include "compiler/RQEnviVarTable.h"
#include "define/RQInterDefine.h"
/*
	该文件用于环境变量脚本，提供利于编写代码的宏定义
*/

//定义脚本函数添加宏操作
#define ES_ADD_FUN(name,addr)	RQEnviVarTable::SetVar(name, (void*)addr,sizeof(void*),M_VAR_TYPE_FUN)

#define ES_ADD_VAR(name,addr,size,type) RQEnviVarTable::SetVar(name,(void*)addr,size,type)

#define ES_ADD_INT(name,val) {int copyVal = val;ES_ADD_VAR(name,&copyVal,sizeof(int),M_VAR_TYPE_INT);}

//定义寄存器变量提取宏
#define ES_EXP							(varArray[M_VARARRAY_INDEX_EXP])

/*
	定义脚本函数参数提取宏
	offset	[in]	参数相对第一个参数的偏移量
*/
#define ES_PARAM(offset)				(varArray[M_VARARRAY_INDEX_PARAM_START+offset])

#define ES_LOCAL						(varArray[M_VARARRAY_INDEX_LOCALMAP])

//检查参数是否是一个有效函数
#define ES_PARAM_IS_FUN(offset)					\
(ES_PARAM(offset)								\
	&&M_VAR_TYPE_FUN == ES_PARAM(offset)->GetType()		\
	&& ES_PARAM(offset)->GetObj())				