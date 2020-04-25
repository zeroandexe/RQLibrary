#include "compiler/RQLexical.h"
#include "define/RQInterDefine.h"
#include "define/RQdefine.h"
#include "RecycleBin.h"
#include "compiler/RQSentence.h"
#include "compiler/RQWord.h"
#include <string.h>

static std::map<char, int>* _charMap;			//解释器使用的字码表
typedef int(*Analysz_FunType)(const char* buff, RQWord* word);			//子识别函数类型
static Analysz_FunType _analyszFun[M_WORD_TYPE_TOTAL];			//解析元词的字码表

#define M_TRA_MEAN_NUM 256						//转义字符传输长度
static char traMeanArray[M_TRA_MEAN_NUM];


//空符分析，用于' '和'\t'过滤
int Analysz_Blank(const char* buff, RQWord* word)
{
	int index = 0;
	for (; buff[index]; index++)
	{
		int isBlank = !(' '-buff[index])+!('\t'-buff[index]);		///检查当前符号是否是空符
		if (!isBlank)			///不是空符，退出当前状态
		{
			break;
		}
	}
	return index;
}

//字符串字面量分许，用于存储" sd fds33r4r "中双引号内部的所有字符串，并去掉'\"'
int Analysz_Str(const char* buff, RQWord* word)
{
	int index = 1;												///跳过导致进入函数的字符'\"'
	word->_buff[0] = 0;
	for (; buff[index]; index++)			///跳过末尾的'\"'字符
	{
		if ('\"' == buff[index])
		{
			index++;
			break;
		}

		if ('\\' == buff[index] && traMeanArray[buff[index+1]])
		{
			word->_buff[word->_len++] = traMeanArray[buff[++index]];
		}
		else {
			word->_buff[word->_len++] = buff[index];
		}
	}
	return index;
}


int _Analysz_Num(const char* buff, RQWord* word)
{
	int index = 0;
	for (; buff[index]; index++)
	{
		int isInt = ('0' <= buff[index])*('9' >= buff[index]);
		if (!isInt)
		{
			break;
		}
		word->_buff[word->_len++] = buff[index];
	}
	return index;
}

//数字字面量分析，用于存储156465这些数字字符
int Analysz_Int(const char* buff, RQWord* word)
{
	int index = _Analysz_Num(buff, word);
	if ('.' == buff[index])
	{
		word->_buff[word->_len++] = buff[index++];
		index += _Analysz_Num(buff+index, word);
	}
	return index;

	//int index = 0;
	//for (; buff[index]; index++)
	//{
	//	int isInt = ('0' <= buff[index])*('9' >= buff[index]);
	//	if (!isInt)
	//	{
	//		break;
	//	}
	//	word->_buff[word->_len++] = buff[index];
	//}
	//return index;
}

//运算符解析，用于存储运算符
int Analysz_Symbol(const char* buff, RQWord* word)
{
	int index = 1;

	int isNote = !(buff[0] - buff[1]) * !(buff[0] - '/');		//检查该行是否是注释行
	if (isNote)
	{
		index = 0;
		word->_len = 0;
		((char*)buff)[0] = 0;
		return index;
	}

	word->_buff[word->_len++] = buff[0];

	int isOk = (!('=' - buff[1])		//检测目标是否是等号
		+ !(buff[0] - buff[1]))		//检查当前字符是否都相等
		*(')' - buff[0]);			//并且第一个符号不能是')'

	if (isOk)		//检查第二个运算符是否为'='，若是则表示该符号是一个组合符号
	{
		word->_buff[word->_len++] = buff[1];
		index = 2;
	}
	return index;
}

//命名符解析，用于存储变量命名如_7asdf
int Analysz_Name(const char* buff, RQWord* word)
{
	int index = 0;
	int curChType;
	for (; buff[index]; index++)
	{
		curChType = (*_charMap)[buff[index]];			///获取当前字符的类别
		int isOk = !(M_WORD_TYPE_NAME - curChType) + !(M_WORD_TYPE_VAL_INT - curChType);	///检测当前字符是否是命名符
		if (!isOk)		///当前字符不属于命名规则的字符
		{
			break;
		}
		word->_buff[word->_len++] = buff[index];
	}
	return index;
}

int Analysz_End(const char* buff, RQWord* word)
{
	if (buff[0])
	{
		((char*)buff)[0] = 0;			///由于检测到了非法字符，所以将该字符置空，用于退出循环
	}
	return 0;
}


static void InitTraMenAry()
{
	memset(traMeanArray, 0, sizeof(traMeanArray));
	traMeanArray['b'] = '\b';
	traMeanArray['f'] = '\f';
	traMeanArray['n'] = '\n';
	traMeanArray['r'] = '\r';
	traMeanArray['t'] = '\t';
	traMeanArray['"'] = '\"';
	traMeanArray['\\'] = '\\';
}

void RQLexical::InitObj()
{
	static bool hasInited = false;
	if (hasInited)
	{
		return;
	}
	hasInited = true;

	InitTraMenAry();
	_charMap = new std::map<char, int>;			///创建字码表空间
	//初始化数字字符
	for (char ch = '0'; ch <= '9'; ch++)		///将所有数字全部标识为数字元词类型
	{
		(*_charMap)[ch] = M_WORD_TYPE_VAL_INT;
	}
	
	//初始化命名符
	for (char ch = 'a'; ch <= 'z'; ch++)
	{
		(*_charMap)[ch] = M_WORD_TYPE_NAME;
	}
	for (char ch = 'A'; ch <= 'Z'; ch++)
	{
		(*_charMap)[ch] = M_WORD_TYPE_NAME;
	}
	(*_charMap)['_'] = M_WORD_TYPE_NAME;
	//初始化字面量符
	(*_charMap)['\"'] = M_WORD_TYPE_VAL_STR;
	//初始化运算符
	(*_charMap)['!'] = M_WORD_TYPE_SYMBOL;
	(*_charMap)['%'] = M_WORD_TYPE_SYMBOL;
	(*_charMap)['&'] = M_WORD_TYPE_SYMBOL;
	(*_charMap)['|'] = M_WORD_TYPE_SYMBOL;
	(*_charMap)[','] = M_WORD_TYPE_SYMBOL;
	(*_charMap)['+'] = M_WORD_TYPE_SYMBOL;
	(*_charMap)['-'] = M_WORD_TYPE_SYMBOL;
	(*_charMap)['*'] = M_WORD_TYPE_SYMBOL;
	(*_charMap)['/'] = M_WORD_TYPE_SYMBOL;
	(*_charMap)['='] = M_WORD_TYPE_SYMBOL;
	(*_charMap)['<'] = M_WORD_TYPE_SYMBOL;
	(*_charMap)['>'] = M_WORD_TYPE_SYMBOL;
	(*_charMap)['('] = M_WORD_TYPE_SYMBOL;
	(*_charMap)[')'] = M_WORD_TYPE_SYMBOL;
	(*_charMap)['{'] = M_WORD_TYPE_SYMBOL;
	(*_charMap)['}'] = M_WORD_TYPE_SYMBOL;
	(*_charMap)['.'] = M_WORD_TYPE_SYMBOL;
	//初始化空符
	(*_charMap)[' '] = M_WORD_TYPE_BLANK;
	(*_charMap)['\t'] = M_WORD_TYPE_BLANK;
	//初始化终结符(隐性终结符包含“//”)
	(*_charMap)['\r'] = M_WORD_TYPE_END;
	(*_charMap)['\n'] = M_WORD_TYPE_END;
	(*_charMap)[';'] = M_WORD_TYPE_END;
	(*_charMap)['\0'] = M_WORD_TYPE_END;
	//初始化环境变量引用符
	(*_charMap)['@'] = M_WORD_TYPE_SYMBOL;

	//-------------------初始化解析函数
	_analyszFun[M_WORD_TYPE_BLANK] = &Analysz_Blank;
	_analyszFun[M_WORD_TYPE_END] = &Analysz_End;
	_analyszFun[M_WORD_TYPE_NAME] = &Analysz_Name;
	_analyszFun[M_WORD_TYPE_VAL_INT] = &Analysz_Int;
	_analyszFun[M_WORD_TYPE_VAL_STR] = &Analysz_Str;
	_analyszFun[M_WORD_TYPE_SYMBOL] = &Analysz_Symbol;
}


//获取当前可用的元词对象，并将元词存入暂存器等待回收
#define GET_WORD(word,tsId)					\
{											\
	word = RecycleBin::GetFreeRQWord();		\
	RecycleBin::TemStorage(tsId,word);		\
}

//传入当前准备识别的字符和当前正在识别的元词所处类型
unsigned int RQLexical::Analysz(const char* buff, RQWord** wordArray, void* junkTsId)
{
	int wordCount = 0;
	RQWord* curWord;
	GET_WORD(curWord, junkTsId);					///获取一个空元词用于存储当前解析字符
	int lastWordType = M_WORD_TYPE_BLANK;			///一开始默认上个状态为空符
	int curChType;									///当前字符的状态
	int readIndex = 0;								///从第一个字符开始读取
	char curCh;										///当前解析字符
	int braceType = 0;								///花括号特征变量

	while (M_WORD_TYPE_END != lastWordType)
	{
		curCh = buff[readIndex];					///获取最新一个用于检测字符
		curChType =(*_charMap)[curCh];				///检测该字符所属元词类别

		if (M_WORD_TYPE_BLANK != lastWordType 
			&&(curWord->_len
				||M_WORD_TYPE_VAL_STR == lastWordType))		///检查上次的元词是否不是空符，不是空符则元词应当并存储
		{
			curWord->_type = lastWordType;			///保存上次元词的类型解析结果
			curWord->_buff[curWord->_len] = 0;		///自作c标准字符
			wordArray[wordCount++] = curWord;		///将成功解析的元词存入
			GET_WORD(curWord, junkTsId);
		}
		lastWordType = curChType;					///存储当前元词类别

		readIndex += _analyszFun[curChType](buff + readIndex, curWord);		///对当前字符进行识别并更新当前偏移量
	}

	///检查元词元词组的属性
	if (wordCount)			//检测本次是否获取到了元词
	{
		char lastChar = wordArray[wordCount - 1]->_buff[0];		///获取元词组中最后一个元词中的第一个字符（花括号是独立元词）
		braceType = !('}' - lastChar)*M_BRACE_TYPE_RIGHT + !('{' - lastChar)*M_BRACE_TYPE_LEFT; ///计算或花括号的特性
	}
	wordArray[wordCount] = 0;	///将元词结束位置空标识结束

	return braceType;		///本行语句的花括号特性
}