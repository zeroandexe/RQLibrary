#include "RQJsonHead.h"
#include "RecycleBin.h"
#include <string.h>
#include "RQStrTool.h"
#include "tool/json/RQJsonParse.h"
#include "RQLoadNode.h"

RQJsonParse::RQJsonParse(const char * buff, int buffLen)
{
	_varItemTs = RecycleBin::CreateTS();			///获取一个暂存器保存解析时所使用的临时变量
	_buff = (char*) buff;
	_buffLen = buffLen?buffLen:RQStrlen(buff);
	_offset = 0;
}

RQJsonParse::~RQJsonParse()
{
	RecycleBin::RecycleTemStorage(_varItemTs);		///回收临时分配的数据对象
	RecycleBin::RecycleTS(_varItemTs);				///释放暂存器
	_varItemTs = 0;
	_buff = 0;
	_buffLen = 0;
}

RQJson * RQJsonParse::ParseJson()
{
	char first = _FilterSpaceChar();				///获取第一个非空字符
	RQJson* retJson = 0;
	if ('{' == first)
	{
		retJson = _ParseMap();						///更具json格式的特性，从map开始解析
	}

	return retJson;
}

RQVar * RQJsonParse::_GetFreeRQVar()
{
	RQVar* retVar = RecycleBin::GetFreeRQVar();
	RecycleBin::TemStorage(_varItemTs, retVar);
	return retVar;
}

RQJson* RQJsonParse::_ParseMap()
{
	RQJsonMap* jsonMap = RecycleBin::GetFreeRQJsonMap();
	char first;
	if ('}' != _buff[_offset])								///检查元素是否不是空表
	{
		do {
			if (_ParseKeyVal(jsonMap))						///检测解析是否失败
			{
				first = 0;
				break;
			}
			first = _FilterSpaceChar();						///获取结尾符号
		} while (',' == first);

		if ('}' != first)									///检测json是否有正确的结尾
		{
			///---解析失败
			RecycleBin::RecycleNode(jsonMap);				///回收json表对象
			jsonMap = 0;
		}
	}
	else {
		_offset++;
	}

	return jsonMap;
}

///---偏移量位于数组起始符号'['+1的偏移量
RQJson * RQJsonParse::_ParseArray()
{
	RQJsonArray* jsonArray = RecycleBin::GetFreeRQJsonArray();
	RQJson* val;

	char first;											///获取第一个非空字符
	if (']' != _buff[_offset])							///检测元素不是空数组
	{
		do {
			val = _ParseVal();								///解析表中的值
			if (!val)
			{
				break;
			}
			jsonArray->push(val);						///将解析的结果存入数组中
			first = _FilterSpaceChar();						///获取表的结束符（,或]），用以确认是否已完成数组解析
		} while (',' == first && val);						///检查是否还有元素需要解析

		if (']' != first || !val)							///检查解析是否具有正确的结尾，val的解析是否成功
		{
			RecycleBin::RecycleNode(jsonArray);
			RecycleBin::RecycleNode(val);
			jsonArray = 0;
		}
	}
	else {
		_offset++;
	}
	jsonArray->Refurbish();								///刷新json数组内部的索引
	return jsonArray;
}

int RQJsonParse::_ParseKeyVal(RQJsonMap * json)
{
	int retInt = -1;
	RQVar* key = _ParseKey();							///获取key
	if (key)											///检测对key的解析是否成功
	{
		RQJson* val = _ParseVal();						///获取所对应的值
		if (val)
		{
			json->SetVal(key, val);						///值获取成功，将该值保存入jsonMap中
			retInt = 0;
		}
	}

	return retInt;
}



///---创建用于辅助过滤空字符操作的数组创建函数
static char* _InitFilterArray()
{
	static char* chArray = 0;
	if (!chArray)
	{
		chArray = new char[256];
		memset(chArray, 1, 256);					///默认所有字符都是合法字符

		///---设置不合法字符
		chArray[' '] = 0;
		chArray['\t'] = 0;
		chArray['\r'] = 0;
		chArray['\n'] = 0;
	}
	return chArray;
}

///---执行后_offset则是该可显示字符的下标+1
char RQJsonParse::_FilterSpaceChar()
{
	static char* chArray = _InitFilterArray();
	char retCh = 0;

	for (; _offset < _buffLen 
		&& _buff[_offset];)			///从当前偏移量继续遍历字符串（防止越界，同时防止长度不准确）
	{
		char curCh = _buff[_offset++];
		if (chArray[curCh])
		{
			retCh = curCh;
			break;
		}
	}

	return retCh;
}

///--进入该函数时要求：已确认为字符串，偏移量下标为\"后一个字符
RQVar * RQJsonParse::_ParseStr()
{
	RQVar* retVar = 0;
	int isOk = 0;
	_RememberOffset(_offset);					///记录拷贝起点

	for (; _offset < _buffLen
		&& _buff[_offset]; _offset++)
	{
		if ('"' == _buff[_offset])				///达到字符串尾，并且字符串尾部未转义
		{
			_RememberOffset(_offset - 1);		///记录拷贝终点以及
			if ('\\' != _buff[_offset - 1])
			{
				isOk = true;
				_offset++;
				break;
			}
		}
	}
	if (isOk)									///检测是否成功获取到完整字符串
	{
		retVar = _GetFreeRQVar();
		_CopyStrByOffset(retVar);
	}
	_offsetList.Clear();						///清除本机偏移记录
	return retVar;
}

///---要求已确认是数字，并且该偏移量为数字字符串的第二个字符
#define M_IS_NUM(ch) ('0' <= (ch) && '9' >= (ch))
RQVar * RQJsonParse::_ParseDigit()
{
	RQVar* retVar = 0;

	int oldOffset = _offset-1;
	int pointTimes = 0;

	for (; _offset < _buffLen
		&& _buff[_offset]; _offset++)
	{
		if (!M_IS_NUM(_buff[_offset]))			///检测是否不是数字
		{
			if ('.' == _buff[_offset] && !pointTimes)
			{
				pointTimes = 1;
			}
			else {
				break;
			}
		}
	}

	retVar = _GetFreeRQVar();
	if (pointTimes)						///出现过小数点，则按照浮点数存储
	{
		retVar->SetDouble(RQStrToDouble(_buff + oldOffset, _offset - oldOffset));
	}
	else {								///没有出现过小数点，则按照整数存储
		retVar->SetInt(RQStrToInt10(_buff + oldOffset, _offset - oldOffset));
	}
	return retVar;
}

///---要求已确认是数字，并且该偏移量为数字字符串的第二个字符
#define M_IS_LETTER(ch) (('A'<= (ch) && 'Z'>=(ch))||('a'<=(ch)&&'z'>=(ch)))
RQVar * RQJsonParse::_ParseKeyWord()
{
	RQVar* retVar = 0;

	int isOk = 0;
	int oldOffset = _offset-1;
	for (; _offset < _buffLen
		&& _buff[_offset]; _offset++)
	{
		if (!M_IS_LETTER(_buff[_offset]))
		{
			isOk = true;
			break;
		}
	}
	retVar = RQJsonKeyMap::SearchKeyWord(_buff + oldOffset, _offset - oldOffset);		///按照所查获到的关键字
	return retVar;
}

//---当前偏移量指向字符串的第一个字节
RQVar * RQJsonParse::_ParseKey()
{
	RQVar* keyStr = 0;				///获取到目标关键字
	char first = _FilterSpaceChar();
	if ('"' == first)
	{
		keyStr = _ParseStr();
		if (keyStr)
		{
			char first = _FilterSpaceChar();		///获取下一个有效字符
			if (':' != first)
			{
				keyStr = 0;
			}
		}
	}

	return keyStr;
}

void RQJsonParse::_RememberOffset(int offset)
{
	RQLoadNode* loadNode = RecycleBin::GetFreeRQLoadNode();
	loadNode->_obj = (void*)offset;
	_offsetList.push_back(loadNode);
}

void RQJsonParse::_CopyStrByOffset(RQVar * var)
{
	RQLoadNode* startLoad = (RQLoadNode*)_offsetList.pop_front();	///获取起始点
	RQLoadNode* endLoad = (RQLoadNode*)_offsetList.pop_back();		///获取中止偏移量

	int startIndex = *(int*)&startLoad->_obj;
	int endIndex = *(int*)&endLoad->_obj;
	RQLoadNode* other = (RQLoadNode*) _offsetList.pop_front();
	var->SetStr(0, 0);
	var->Resize(endIndex - startIndex+1);
	while (other)
	{
		int otherIndex = *(int*)&other->_obj;
		var->AppendData(_buff + startIndex, otherIndex - startIndex);
		startIndex = otherIndex + 1;
		RecycleBin::RecycleNode(other);
		other = (RQLoadNode*)_offsetList.pop_front();
	}
	var->AppendData(_buff + startIndex, endIndex - startIndex+1);
	
	RecycleBin::RecycleNode(startLoad);
	RecycleBin::RecycleNode(endLoad);
}

typedef RQJson*(*ParseVal_FunType)();

RQJson * RQJsonParse::_ParseVal()
{
	char first = _FilterSpaceChar();
	RQJson* json = 0;

	if ('{' == first) {							///检测目标字符是否是表解析
		json = _ParseMap();
	}
	else if ('[' == first) {							///检测目标是否是数组
		json = _ParseArray();
	}
	else {
		RQVar* var = 0;
		if ('"' == first)
		{
			var = _ParseStr();
		}
		else if (M_IS_NUM(first)
			|| '-' == first)						///检测目标字符是否是数值
		{
			var = _ParseDigit();
		}
		else if (M_IS_LETTER(first))				///检测目标字符是否是字母/关键字
		{
			var = _ParseKeyWord();
		}

		if (var)										///检测是否获取到数据
		{
			RQJsonVal* jsonVal = RecycleBin::GetFreeRQJsonVal();
			jsonVal->Set(var);
			json = (RQJson*)jsonVal;
		}
	}

	return json;
}
