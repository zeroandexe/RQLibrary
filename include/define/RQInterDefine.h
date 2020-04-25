#pragma once

/*
	本文件是编译器的相关配置定义
*/

#define MAX_LINE_NUM_PER_SCRIPT	5120			//定义每个脚本的最大行数
#define MAX_STR_LEN_PER_LINE	2048			//定义每行允许的最大字符长度
#define MAX_VAR_NUM_PER_LIEN	512				//每行最大无效脚本变量数量
#define MAX_WORD_NUM_PER_LINE	512				//每行最大的元词数量
#define MAX_SENTENCE_NUM_PER_LINE (MAX_WORD_NUM_PER_LINE/2)		//每行代码拥有多少个子表达式
#define MAX_ASTNODE_NUM_PER_LIEN (MAX_WORD_NUM_PER_LINE+1)					//每行语句最大容纳的语法树节点个数
#define MAX_STR_LEN_PER_WORD	512				//每个单词的最大字符串长度
#define MAX_RUNSTATE_DEPTH		64				//允许if和while嵌套的深度
#define MAX_PARAM_PER_FUN		64				//在脚本中允许传入函数内部的最大参数个数,多出的一个表示空的结尾符
#define MAX_THRESHOLD_VAR_TO_SWAP	1024		//脚本变量执行交换操作时，若低于该值则使用拷贝，若高于该值才启动空间交换
#define M_PRECISION_FLOAT		100000			//在处理float时所使用的精度控制
#define M_PRECISION_DOUBLE		10000000000000	//在处理double类型时所使用的精度控制


//在函数参数数组中的各参数所在的下标位置
enum {
	M_VARARRAY_INDEX_EXP,						//寄存器下标
	M_VARARRAY_INDEX_ENVIMAP,					//环境变量表下标
	M_VARARRAY_INDEX_LOCALMAP,					//临时变量表下标
	M_VARARRAY_INDEX_PARAM_START,				//函数参数开始位置
};

#define M_NULL(type)		((type)0)			//在linux上提供空值

//脚本文件操作函数调用的对应id
enum {
	M_SCRIPT_OPT_INVALIDE,					//无效操作
	M_SCRIPT_OPT_GET_LINE,					//调用获取下一行数据
	M_SCRIPT_OPT_SET_ADD_LINE,				//调用增量偏移函数
	M_SCRIPT_OPT_SET_OFFSET_LINE,			//调用绝对偏移设置函数
	M_SCRIPT_OPT_JUMP_UP,					//调用括号上跳转函数
	M_SCRIPT_OPT_JUMP_DOWN,					//调用括号下跳转函数
	M_SCRIPT_OPT_TOTAL,						//文本文件函数操作的总个数
};

//变量类型
enum {
	M_VAR_TYPE_INVALID,					//无效变量类型
	M_VAR_TYPE_BOOL,					//考虑废弃
	M_VAR_TYPE_INT,						//4字节整型，将被替换 
	M_VAR_TYPE_FUN,						//函数类型
	M_VAR_TYPE_OBJ,						//变量是外部对象类型，在使用时，需要自定义一个增量并作为函数中的类型标识
	M_VAR_TYPE_LONG,					//变动字节位数
	M_VAR_TYPE_LLONG,					//8字节整型，将被使用
	M_VAR_TYPE_FLOAT,					//4字节浮点类型
	M_VAR_TYPE_DOUBLE,					//8字节浮点数类型
	//M_VAR_TYPE_OBJ,					//变量是一个指针	（暂未提供）
	//M_VAR_TYPE_CHAR,					//变量是一个字节	（不计划提供，不支持）

	M_VAR_TYPE_STR,						//字符串类型
	M_VAR_TYPE_TOTAL,					//变量类型总量
};

//声明解析状态
enum {
	M_WORD_TYPE_END,				//当前正在解析终止符（\r，\n，//或其他非法字符）
	M_WORD_TYPE_NAME,				//当前正在解析命名符（函数名，变量名）
	M_WORD_TYPE_BLANK,				//当前解析到了空符
	M_WORD_TYPE_VAL_STR,			//当前正在解析字符串字面量
	M_WORD_TYPE_VAL_INT,			//当前正在解析整数值字面量
	M_WORD_TYPE_SYMBOL,				//当前正在解析运算符
	M_WORD_TYPE_TOTAL,				//元词种类总数
};

//语法运行环境状态
enum {
	M_RUN_STATE_NATURAL,			//处于常规运行状态，无分支语句
	M_RUN_STATE_IF_TRUE,			//标记当前运行状态是if_true
	M_RUN_STATE_IF_TRUE_WILL_END,	//标记当前的if_true状态将要结束
	M_RUN_STATE_IF_TRUE_END,		//标记当前的if_true状态已经结束

	M_RUN_STATE_IF_FALSE,
	M_RUN_STATE_IF_FALSE_WILL_END,
	M_RUN_STATE_IF_FALSE_END,

	M_RUN_STATE_IF_FALSE_READY,
	M_RUN_STATE_IF_END,				//if状态结束
	M_RUN_STATE_WHILE,				//标记最近一次的操作为while
};

enum {
	M_BRACE_TYPE_NOT = 1,			//不是花括号
	M_BRACE_TYPE_LEFT,				//左花括号
	M_BRACE_TYPE_RIGHT,				//右花括号
};
