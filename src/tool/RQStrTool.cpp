#include "RQStrTool.h"
#include "RQMath.h"
#include "define/RQdefine.h"
#include "math.h"

static void _RQCopyChar(char* dest, char* sour, int length)
{
	for (int i = 0; i < length; i++)
	{
		dest[i] = sour[i];
	}

}

static void _RQCopyBlock(long* dest, long* sour, int blockSize)
{

	for (int i = 0; i < blockSize; i++)
	{
		dest[i] = sour[i];
	}
}

static void _RQCopy(void* dest, const void* sour, int length)
{
	static int blockSize = sizeof(long);		///计算单位拷贝块大小
	int blockCount = length / blockSize;		///计算本次拷贝块块数量
	int lessChar = length - blockCount * blockSize;		///计算剩余数量

	_RQCopyBlock((long*)dest, (long*)sour, blockCount);	///按照计算的块数量进行拷贝
	///计算按照块大小拷贝后的偏移量
	dest = (((char*)dest) + blockCount * blockSize);	
	sour = (((char*)sour) + blockCount * blockSize);
	///拷贝剩余字符串
	_RQCopyChar((char*)dest, (char*)sour, lessChar);
}

//宽窄字符的拷贝函数
//当det和sour以及len其中一个为0的时候拷贝，都不会执行
//return：真是拷贝的字节个数
//template<typename CharType>
//unsigned int RQCopy(CharType* det, const CharType* sour,unsigned int len)
//{
//	int copyCount = 0;					//记录拷贝的字节个数
//	int isOk = (!!det)*(!!sour)*len;	//检测是否存在空指针
//
//	if (isOk)				
//	{
//		do {
//			*det++ = *sour;				//按照字节从sour拷贝数据到det
//			copyCount++;
//		} while ((*sour++) && --len);
//	}
//	return copyCount-1+!(len)*1;
//}

//宽字符拷贝
//unsigned int RQCopy(wchar_t* det, const wchar_t* sour,unsigned int len)
//{
//	return RQCopy<wchar_t>(det, sour,len);
//}

//窄字符拷贝
unsigned int RQCopy(void* det, const void* sour,unsigned int len)
{
	if (-1 == len)
	{
		len = RQStrlen((char*)sour);
	}
	_RQCopy(det, sour, len);
	///((char*) det)[len] = 0;
	return len;
}

unsigned int RQCopyStr(char * det, const char * sour, unsigned int len)
{
	len = RQCopy(det, sour, len);
	det[len] = 0;
	return len;
}


//计算字符串的长度
template<typename CharType>
static unsigned int mStrlen(const CharType* str)		//字符串长度计算的模板
{
	unsigned int count = 0;						//长度记载变量
	if (str)									//检测
	{
		while (*str++)count++;
	}
	return count;
}

//宽字符长度计算
unsigned int RQStrlen(const wchar_t* str)
{
	return mStrlen<wchar_t>(str);
}

//窄字符拷贝
unsigned int RQStrlen(const char* str)
{
	return mStrlen<char>(str);
}

template <typename DT,typename ST>
unsigned int _RQCharToChar(DT* det, const ST* sour,ST endChar)
{
	DT* beginDet = det;
	const ST * iter = sour;
	if (det && sour)
	{
		while ((*iter) != endChar)
		{
			*det++ = (DT)*iter++;
		}
		*det = 0;
	}
	return det - beginDet;
}

//窄字符转换为宽字符
unsigned int RQCharToWchar(wchar_t* det, const char* sour)
{
	return _RQCharToChar<wchar_t, char>(det, sour, '0');
}

unsigned int RQWcharToChar(char * det, const wchar_t * sour)
{
	return _RQCharToChar<char, wchar_t>(det, sour, L'\0');
}

//将integer转换为字符并存入buffer
//integer：即将被转换的整数，可以为正或为负数
//buffer：用于存储转换后的数据结果
//size：转换缓冲区的大小(要求size为整数长度+1)
//int：实际转换的字节数（如果buffer为空，integer的位数超过了buff的size则返回-1，并且不对buffer进行修改）
//IntegerType：只有unsigned long long 和long long两种类型
template<typename IntegerType>
int RQIntegerToString(IntegerType integer, char* buffer, unsigned int bufferLen)
{
	unsigned int length = RQIntegerLength(integer);				//计算获取数字长度
	int offset = 0;												//记录偏移位，用于越过符号位
	if (integer < 0)
	{
		offset=1;												//由于产生负数符号位，产生计算偏移
		integer *= -1;											//将负数转换为正数
	}

	if (length + 1 + offset < bufferLen)						//加1的目标在于保证具备C字符结尾标识的内存空间,offset是为了保证能够完全存储数字
	{
		for (unsigned int i = 0; i < length; i++)				//从数字最低位（十进制）开始存储
		{
			buffer[length - 1 - i + offset] = integer%10 + '0';
			integer /= 10;
		}
		if (offset > 0)
		{
			buffer[0] = '-';							//加上负号
		}

		buffer[length + offset] = 0;					//打上结尾符
	}
	return length + offset;								//返回拷贝的字节数
}

//将无符号整形转换为字符串写入目标内存
int RQIntegerToString(unsigned long long integer, char * buffer, unsigned int buffLen)
{
	return RQIntegerToString<unsigned long long>(integer, buffer, buffLen);
}

int RQIntegerToString(long long integer, char * buffer, unsigned int buffLen)
{
	return RQIntegerToString<long long>(integer, buffer, buffLen);
}

//
//int RQIntegerToString(unsigned int integer, char * buffer, unsigned int buffLen)
//{
//	return RQIntegerToString((unsigned long long)integer,buffer,buffLen);
//}
//
//int RQIntegerToString(int integer, char * buffer, unsigned int buffLen)
//{
//	return RQIntegerToString((long long )integer, buffer, buffLen);
//}

long long RQStrToInt10(const char * str,unsigned int len,int* offset)
{
	int oldLen = len;		///保存就字符串长度值，准备用于计算字符偏移量
	if (NULL == str)
	{
		return -1;
	}

	//默认为正数
	int label = 1;
	if ('-' == *str)		//检测是否是负数
	{
		label = -1;			//是负数转换为负数符号
		len--;
		str++;
	}

	long long sum = 0;			//定义最后的值
	
	while (*str&&len--)
	{
		if (*str < 0x30 || *str>0x39)		///若遇见非数字字符串则退出
		{
			break;
		}
		sum = sum * 10 + ((*str++) & 0x0f);
	}

	///计算本次计算进行的偏移量
	if (offset)
	{
		*offset = oldLen - len;
	}

	//将最后的结果返回
	return sum*label;
}

int RQFloatToStr(double data, long long precision, char* buffer, unsigned int buffLen)
{
	//计算整数部分
	long long integer = (long long)data;						///获取整数部分
	int offset = RQIntegerToString(integer, buffer, buffLen);	///将整数转换为浮点数，并获取转换后的字符长度
	buffer[offset++] = '.';										///整数部分已经写完，写入小数点进行隔离
	data -= integer;

	//转换小数0部分
	for (int i = 10; !(long long)(data*i); i *= 10)
	{
		buffer[offset++] = '0';
	}

	//转换小数非0部分
	integer = (long long )(data * precision);		///移除浮点数整数位，提升最高小数位
	integer *= integer < 0 ? -1 : 1;							///调整小数位符号为，取消负号
	while (!(integer % 10)&&integer)
		integer /= 10;	///去除小数部分的0
	offset += RQIntegerToString(integer, buffer + offset, buffLen - offset);	///将小数部分拷贝到，字符串小数位置
	return offset;
}

int RQDoubleToStr(double data, char * buffer, unsigned int buffLen,long long precision)
{
	return RQFloatToStr(data, precision, buffer, buffLen);
}

//乘阶7位
int RQFloatToStr(float data, char * buffer, unsigned int buffLen, long long precision)
{
	return RQFloatToStr(data, precision, buffer, buffLen);
}

double RQStrToDouble(const char * str, unsigned int len)
{
	double result = 0;
	if (str&&len)			///防止空传入
	{
		int isOk = M_IS_NUM(str[0])+M_IS_NUM(str[len > 1 ? 1: 0]);	///检查是否字符串中是否存有数字
		if (isOk)
		{
			int offset = 0;
			result = (double)RQStrToInt10(str, len, &offset);
			double flag = result >= 0 ? 1 : -1;
			double decimal = (double)RQStrToInt10(str + offset, len - offset, &offset);
			result += flag*decimal / pow(10, offset);
		}
	}
	return result;
}

//提供对字符的比较操作
template<typename CharType>
int RQStrComp(const CharType * strLeft, const CharType * strRight, unsigned int compNum)
{
	int ret = !!strRight - !!strLeft;				//计算当存在左右量值存在空指针的情况的大小比较
	int haveNull = !(strRight)+!(strLeft);			//检测是否存在空指针

	if (!haveNull)
	{
		do {
			ret = (unsigned char)*strRight - (unsigned char)*strLeft;						//获取字节数据的差
		} while (!ret && (*(strLeft++) | *(strRight++)) && --compNum);	//检测是否已得到不等于的比较结果，检测当前是否已达到字符串结尾
	}
	return ret;
}
int RQStrComp(const char * strLeft, const char * strRight, unsigned int compNum)
{
	return RQStrComp<char>(strLeft, strRight, compNum);
}

int RQWStrComp(const wchar_t * strLeft, const wchar_t * strRight, unsigned int compNum)
{
	return RQStrComp<wchar_t>(strLeft, strRight,compNum);
}

int RQSearchStr(const char * sour, const char * model)
{
	int sourLen = RQStrlen(sour);						//获取原字符长度
	int modelLen = RQStrlen(model);						//获取模板字符长度
	int detal = sourLen - modelLen;
	int ret = -1;						

	if (detal >=0)								//是否需要对sour进行扫描
	{
		for (int i = 0; i < detal; i++)
		{
			if (RQStrComp(sour + i, model,modelLen)==0)			//检测字符串是否相等
			{
				ret = i;
				break;
			}
		}
	}
	return ret;
}


//str为将转换的字符集
//len表示字符串长度
//[min,max]是可以转换的字符集连续闭区间
//add表示符合要求的区间转换量
int mCharAToB(char* str, unsigned int len,char min, char max, char add)
{
	int turnCount = 0;						//记录转换个数
	for (unsigned int i = 0; i < len;i++)
	{
		if(str[i] >= min
			&& str[i] <= max)
		{
			str[i] += add;
			turnCount++;
		}
	}
	return turnCount;
}

int RQCharUpToLow(char * str, unsigned int len)
{
	return mCharAToB(str, len, 'A', 'Z', 'a' - 'A');
}

int RQCharLowToUp(char * str, unsigned int len)
{
	return mCharAToB(str, len, 'a', 'z', 'A' - 'a');
}

double RQOriginMove(long long integer)
{
	int lable = integer > 0 ? 1 : -1;	///获取输入数的符号位
	integer *= lable;					///将输入值调整为正数

	double result = 0;
	while (integer)
	{
		result = (result + integer % 10) / 10;	///将数值向右偏移
		integer /= 10;					///向右偏移
	}

	return result * lable;
}

long long RQOleftMove(double floatd)
{
	int label = floatd > 0 ? 1 : -1;		///获取输入数的符号位
	floatd *= label;						///将输入值调整为正数
	long long result = 0;

	while (floatd)
	{
		int val = ((long long )(floatd * 10))%10;			///第一位小数数值
		result = result * 10 + val;		///将结果值将结果值右移
		floatd = floatd * 10 - val;		///将小数左移
	}

	return result * label;
}
