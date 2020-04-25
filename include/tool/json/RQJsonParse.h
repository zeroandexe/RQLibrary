#pragma once
#include "RQVar.h"
#include "RecycleBin.h"
#include "RQJsonMap.h"
#include "RQJsonArray.h"
#include "RQList.h"

/*
	json的解析对象，提供json生成
*/
class RQJsonParse
{
public:
	/*
		创建json的解析对象
		buff	[in]		过滤字符的起始地址
		offset	[in/out]	从当前位置到解析位置的偏移量（会被更新）
		buffLen	[in]		从当前起始地址到字符串尾的总长度
	*/
	RQJsonParse(const char* buff, int buffLen = 0);
	~RQJsonParse();


	/*
		开始解析所设置的json数据（该函数为解析入口）
		return	[out]	解析成功返回解析所得到的json对象，
						失败则返回空对象
	*/
	RQJson* ParseJson();

protected:
	/*
		获取临时空闲的对象，该对象不能保存在任何对象中
	*/
	RQVar* _GetFreeRQVar();

	/*
		解析map风格的json
		return	[out]	解析成功返回map对象
						解析失败放回-1
	*/
	RQJson* _ParseMap();
	/*
		解析array风格的json
	*/
	RQJson* _ParseArray();

	/*
		从键值对中的键开始解析（仅解析一个变量）
		return [out]	成功解析返回0
						失败返回-1
	*/
	int _ParseKeyVal(RQJsonMap* json);


	/*
		该函数假设key已被解析，故仅解析作为值的数据
	*/
	RQJson* _ParseVal();
protected:

	/*
		根据传入的字符串，过滤不可显示字符
		return	[out]		解析成功返回第一个有效字符
							若失败则返回0
	*/
	char _FilterSpaceChar();

	/*
		从双引号开始解析，获取被解析字符串
		return	[out]		若成功解析则拷贝缓存中的字符串
							返回存储字符串副本的动态变量
							失败返回空
	*/
	RQVar* _ParseStr();
	/*
		从偏移量所确定的当卡位置开始解析，并获取解析所得到的浮点数
		return	[out]	若解析成功则将该值存入脚本变量中
						若失败则返回空
	*/
	RQVar* _ParseDigit();
	/*
		从偏移量所确定的当卡位置开始解析
		并将所得到的关键字在关键字表中进行解析和匹配，
		return	[out]	匹配成功则返回关键字数据副本
						匹配失败则返回空
	*/
	RQVar* _ParseKeyWord();

	/*
		从偏移量所确定的当卡位置开始解析
		获取关键字字符串以及对关键字尾':'字符进行校验。
		return	[out]	校验成功，返回关键字对象
						校验失败则返回空
	*/
	RQVar* _ParseKey();

	/*
		记录当前所设置的偏移量
	*/
	void _RememberOffset(int offset);
	/*
		将按照被记录的偏移量将分段数据拷贝到var中
	*/
	void _CopyStrByOffset(RQVar* var);

protected:
	void* _varItemTs;			//脚本资源暂存器

	char* _buff;			//将被解析的字符串
	int _buffLen;				//被解析字符串的长度
	int _offset;				//被解析字符串当前的偏移量
	RQList _offsetList;			//字符串被转义时的分段偏移记录
};