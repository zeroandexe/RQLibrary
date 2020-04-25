#include "compiler/RQCSC.h"
#include "compiler/RQLine.h"
#include "define/RQASTDefine.h"
#include "define/RQInterDefine.h"
#include "RQVar.h"

typedef RQCSC::Type_KeyLabelFun Type_KeyLabelFun;
Type_KeyLabelFun* RQCSC::_keyLabelFunArray;	//关键字函数数组

RQCSC::RQCSC()
{
	int* start = new int[MAX_RUNSTATE_DEPTH + 1];
	_runStateArray = start + 1;			//由于在检测状态时，经常会检测上一个状态，为了避免使用过量的使用if，所以设置为数组一个元素的下溢出
	Reset();
}


RQCSC::~RQCSC()
{
	int * end = _runStateArray - 1;
	delete[]end;
}

unsigned int RQCSC::KeyLabelFun_Invalid(bool result)
{
	return M_SCRIPT_OPT_GET_LINE;
}

unsigned int RQCSC::Control(unsigned int cscLabel, bool result)
{
	return (this->*_keyLabelFunArray[cscLabel])(result);
}

//处理if语句带来的脚本操作
//true：执行if之下的代码块，即获取下一行代码
//false：跳过if当前代码块，执行后续代码（else或其他代码）
//无论if是成功还是失败，都需要记录判断状态
unsigned int RQCSC::KeyLabelFun_If(bool result)
{
	int is = !(M_RUN_STATE_IF_FALSE_READY - _runStateArray[_runStateCount - 1]) //检测前一个状态是否是if失败状态，并且该if没有else代码处理块
		+ !(M_RUN_STATE_IF_END - _runStateArray[_runStateCount - 1]);		//检测之前的if是否已被完全处理

	if (is)		//根据计算值检测当前状态是否无效
	{
		_runStateCount--;
	}

	int writeIndex = _runStateCount++;						//记录写入下标
	_runStateArray[writeIndex] = M_RUN_STATE_IF_TRUE;				//默认if条件成功
	unsigned int reply_opt = M_SCRIPT_OPT_GET_LINE;

	if (!result)			//检测if条件是否失败，
	{
		reply_opt = M_SCRIPT_OPT_JUMP_DOWN;	//由于if失败，将跳过if所在的代码块
		_runStateArray[writeIndex] = M_RUN_STATE_IF_FALSE_READY;	//并将if状态设置为if_false用于else对语句进行判断
	}
	return reply_opt;
}

/*
对于while的处理，需要检测while的判断结果，
true：while成功，则解释器进入while循环状态，获取下一行代码继续执行
false：while失败，while的代码块不被执行，即不进入while循环状态，while状态不记录
*/
unsigned int RQCSC::KeyLabelFun_While(bool result)
{
	int is = !(M_RUN_STATE_IF_FALSE_READY - _runStateArray[_runStateCount - 1]) //检测前一个状态是否是if失败状态，并且该if没有else代码处理块
		+ !(M_RUN_STATE_IF_END - _runStateArray[_runStateCount - 1]);		//检测之前的if是否已被完全处理

	if (is)		//根据计算值检测当前状态是否无效
	{
		_runStateCount--;
	}

	unsigned int reply_opt = M_SCRIPT_OPT_JUMP_DOWN;				//默认while失败；
	if (result)				//检测while判断条件是否成功
	{
		reply_opt = M_SCRIPT_OPT_GET_LINE;			//由于判断成功则获取下一行语句继续执行
		_runStateArray[_runStateCount++] = M_RUN_STATE_WHILE;		//标记当前已进入while循环状态
	}
	return reply_opt;
}

/*
else代码处理，检查当前状态
if_end：跳过else的代码块并且取消除if状态
if_false：继续向下执行
*/
unsigned int RQCSC::KeyLabelFun_Else(bool result)
{
	unsigned int reply_opt = M_SCRIPT_OPT_GET_LINE;
	switch (_runStateArray[_runStateCount - 1])
	{
	case M_RUN_STATE_IF_FALSE_READY:					//检测if的判断是否是失败,
		_runStateArray[_runStateCount - 1] = M_RUN_STATE_IF_FALSE;
		reply_opt = M_SCRIPT_OPT_GET_LINE;
		break;
	case M_RUN_STATE_IF_END:
		reply_opt = M_SCRIPT_OPT_JUMP_DOWN;			//向下跳过else代码块
		_runStateCount--;								//消除if状态
		break;
	}
	return reply_opt;
}


/*
右括号处理需要区分当前代码块存在的情况
处于while：程序将向上会跳到while判断语句行
处于if_true或者if_false：程序将修改if_true状态为if_end，继续获取下一行执行
*/
unsigned int RQCSC::KeyLabelFun_BigRightParentheses(bool result)
{
	int is = !(M_RUN_STATE_IF_FALSE_READY - _runStateArray[_runStateCount - 1]) //检测前一个状态是否是if失败状态，并且该if没有else代码处理块
		+ !(M_RUN_STATE_IF_END - _runStateArray[_runStateCount - 1]);		//检测之前的if是否已被完全处理

	if (is)		//根据计算值检测当前状态是否无效
	{
		_runStateCount--;
	}
	unsigned int reply_opt = M_SCRIPT_OPT_GET_LINE;

	int lastStateIndex = _runStateCount - 1;				//最新状态元素下标
	switch (_runStateArray[lastStateIndex])
	{
		//while循环处理
	case M_RUN_STATE_WHILE:
		reply_opt = M_SCRIPT_OPT_JUMP_UP;				//跳回while语句继续判断
		_runStateCount--;								//消除while循环状态
		break;

		//if代码块处理
	case M_RUN_STATE_IF_FALSE:
	case M_RUN_STATE_IF_TRUE:
		_runStateArray[lastStateIndex] = M_RUN_STATE_IF_END;			//标记if代码块结束
		break;
	}
	return reply_opt;
}


void RQCSC::InitObj()
{
	if (_keyLabelFunArray)
	{
		return;
	}

	_keyLabelFunArray = new Type_KeyLabelFun[M_KEY_LABEL_TOTAL];
	_keyLabelFunArray[M_KEY_LABEL_INVALID] = &RQCSC::KeyLabelFun_Invalid;
	_keyLabelFunArray[M_KEY_LABEL_IF] = &RQCSC::KeyLabelFun_If;
	_keyLabelFunArray[M_KEY_LABEL_WHILE] = &RQCSC::KeyLabelFun_While;
	_keyLabelFunArray[M_KEY_LABEL_ELSE] = &RQCSC::KeyLabelFun_Else;
	_keyLabelFunArray[M_KEY_LABEL_BIG_RIGHT_PARENTHESES] = &RQCSC::KeyLabelFun_BigRightParentheses;
}

void RQCSC::Reset()
{
	_runStateCount = 0;
	_runStateArray[-1] = M_RUN_STATE_NATURAL;				//溢出元素的运行状态为常规
	_runStateArray[0] = M_RUN_STATE_NATURAL;
}