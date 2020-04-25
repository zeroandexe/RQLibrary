#pragma once
#include <string>
/*
	该文件主要用于提供字符处理相关的功能函数（宽窄字符），例如：拷贝、比较、转换等
*/

//det：存储目标地址
//sour：样板目标地址
//len：期望拷贝多好个字符
//return：返回真是拷贝的字节个数
//unsigned int RQCopy(wchar_t* det, const wchar_t* sour,unsigned int len=-1);
unsigned int RQCopy(void* det, const void* sour,unsigned int len=-1);
unsigned int RQCopyStr(char* det, const char* sour, unsigned int len = -1);	///由于字符串的拷贝，通常在尾部添加一个空作为结束符，所以这里对无类型拷贝进行区别

//统计字符的字节数
//str：被计算的子字符串
//str为NULL则返回0
//计算以\0作为结尾标识
unsigned int RQStrlen(const wchar_t* str);
unsigned int RQStrlen(const char* str);


//窄字符转换为宽字符
//det：目标宽字符存储区域
//sour：窄字符读取区域
//成功执行转换返回实际转换字节数（>0）
//转换未执行则返回0
unsigned int RQCharToWchar(wchar_t* det, const char* sour);
/*
	将宽字符串转换为窄字符（若被转换字符为ascii则转为单字节，若为非ascii则不予转换）
	det		[out]	目标字符，要求传入前已开好空间
	sour	[in]	原字符（应该是以0x00 00 结尾）
	return	[out]	窄字符的字节数
*/
unsigned int RQWcharToChar(char* det, const wchar_t* sour);


//传入希望转换的整数，和用于存储转换后字符的存储空间
//计算完成后返回内存空真是写入的字符个数
//integer：将被转换的数
//buffer：用于存储目转换后的数字字符串
//size：缓冲区大小
int RQIntegerToString(unsigned long long integer, char* buffer, unsigned int buffLen);
int RQIntegerToString(long long integer, char* buffer, unsigned int buffLen);
///int RQIntegerToString(unsigned int integer, char* buffer, unsigned int buffLen);
///int RQIntegerToString(int integer, char* buffer, unsigned int buffLen);

//将字符串转换为数值（10进制）
long long RQStrToInt10(const char* str,unsigned int len = -1,int* offset=0);
//long long RQStrToInt10(const std::string& str, unsigned int len = -1);

//将浮点数转换为字符串
int RQDoubleToStr(double data, char* buffer, unsigned int buffLen, long long precision = 1000000000000000);
int RQFloatToStr(float data, char* buffer, unsigned int buffLen, long long precision = 10000000);
/*
	按照传入精度，将浮点数转换为字符串
	data		[in]	输入字符串
	precision	[in]	精度倍数如：10倍，1000倍等
	buffer		[in]	缓存空间地址
	buffLen		[in]	缓冲空间字节大小
*/
int RQFloatToStr(double data, long long precision, char* buffer, unsigned int buffLen);
//将字符串转换为浮点数
double RQStrToDouble(const char* str, unsigned int len = -1);


//窄字符之间的比较。
//strLeft < strRight = 正数 
//strLeft == strRight = 0
//strLeft > strRight = 负数
//compNum表示检测的长度，compNum不为0（如：3，则只比较前三个元素）
//若需要匹配所有长度则compNum=-1
int RQStrComp(const char* strLeft, const char* strRight,unsigned int compNum = -1);
int RQWStrComp(const wchar_t* strLeft, const wchar_t* strRight, unsigned int compNum = -1);

//根据样板在父字符串中找到子字符串
//返回字字符串在父字符串中的起始下标
int RQSearchStr(const char* sour, const char* model);

//将字符串的大写字母转换为小写字母，并返回转换的个数
int RQCharUpToLow(char* str, unsigned int len);

//将字符串的小写字母转换为大写字母，并返回转换的个数
int RQCharLowToUp(char* str, unsigned int len);

/*
	将小数点作为原点进行左右移动，直到全部转化为浮点数或者整数
	将整数大小不变转为小数，如：1234 => 0.1234
	同样将小数转为整数：如：0.1234 => 1234
*/
double RQOriginMove(long long integer);
long long RQOleftMove(double floatd);