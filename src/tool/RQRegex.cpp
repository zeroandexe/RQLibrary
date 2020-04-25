#include "RQRegex.h"
#include <regex>
#include <string.h>
#include "RQStrTool.h"

int RQRegex_match_loop(const std::string& sourStr, const char * patternStr,
	char* strArray[], int arrayCount,
	unsigned int elementSize, int startIndex)
{
	return RQRegex_match_loop(sourStr.c_str(), sourStr.length(),
		patternStr, strArray, arrayCount, elementSize, startIndex);
}

int RQRegex_match_loop(const char* sourStr, unsigned int sourLen,
	const char * patternStr, char* strArray[], unsigned int arrayCount,
	unsigned int elementSize, int startIndex)
{
	const std::regex pattern(patternStr);					///根据传入的正则表达式，构建表达式对象

	unsigned int writeIndex = 0;
	for (std::cregex_iterator iter_start(sourStr, sourStr + sourLen, pattern), iter_end;		///遍历源字符串中所有匹配正则表达式的子字符串
		iter_start != iter_end && writeIndex < arrayCount; iter_start++)
	{
		if (iter_start->str().length() < elementSize)
		{
			void *ptr = (char*)strArray + (startIndex + writeIndex++)* elementSize;
			RQCopy(ptr, iter_start->str().c_str(), iter_start->str().length() + 1);		///将匹配的字符串拷贝到目标控件中
		}
	}
	return writeIndex;
}

int RQSRegex_match_loop(const char * sourStr, unsigned int sourLen, const char * patternStr, char * outBuff, unsigned int buffLen)
{
	int writeBytes = 0;
	const std::regex pattern(patternStr);					///根据传入的正则表达式，构建表达式对象

	for (std::cregex_iterator iter_start(sourStr, sourStr + sourLen, pattern), iter_end;		///遍历源字符串中所有匹配正则表达式的子字符串
		iter_start != iter_end
		&& buffLen > writeBytes
		; iter_start++)
	{
		int strLen = iter_start->str().length();
		if ( strLen < buffLen - writeBytes) 
		{
			RQCopy(outBuff + writeBytes, iter_start->str().c_str(), strLen + 1);				///将匹配的字符串拷贝到目标控件中
			writeBytes += strLen;
		}
	}
	return writeBytes;
}

int RQSRegex_match_loop(const std::string & sourStr, const char * patternStr, char * outBuff, unsigned int buffLen)
{
	return RQSRegex_match_loop(sourStr.c_str(),sourStr.length(),patternStr,outBuff,buffLen);
}

bool RQRegex_match(const std::string & sourStr, const char * patternStr,
	char* strArray, unsigned int elementSize)
{
	const std::regex pattern(patternStr);

	std::match_results<std::string::const_iterator> result;
	bool retBol = false;
	if (std::regex_match(sourStr, result, pattern))
	{
		if (result[1].str().length() < elementSize)
		{
			retBol = true;
			memcpy(strArray, result[1].str().c_str(), result[1].str().length() + 1);
		}
	}
	return retBol;
}

bool RQRegex_match(const char * sourStr, const char * patternStr,
	char * strArray, unsigned int elementSize)
{
	std::string content(sourStr);
	return RQRegex_match(content, patternStr, strArray, elementSize);
}

int RQARegex_match_loop(const std::string& sourStr,
	const char* searchPattern, const char* checkPattern,
	char* strArray[], unsigned int arrayCount,
	unsigned int elementSize, int startIndex)
{
	return RQARegex_match_loop(sourStr.c_str(), sourStr.length(),
		searchPattern, checkPattern,
		strArray, arrayCount, elementSize, startIndex);
}

int RQARegex_match_loop(const char * sourStr, unsigned int sourLen,
	const char * searchPattern, const char * checkPattern,
	char * strArray[], unsigned int arrayCount,
	unsigned int elementSize, int startIndex)
{
	std::string str;
	int strCount = RQRegex_match_loop(sourStr, sourLen, searchPattern, strArray, arrayCount, elementSize, startIndex);
	for (int j = 0; j < strCount; j++)
	{
		char* startChar = (char*)strArray + (startIndex + j)* elementSize;
		str = startChar;
		int isOk = RQRegex_match(str, checkPattern, startChar, elementSize);
		if (!isOk)
		{
			startChar[0] = 0;
		}
	}
	return strCount;
}

///---RQASregex
int RQASRegex_match_loop(const std::string & sourStr, const char * searchPattern,
	const char * checkPattern,
	char * outBuff, unsigned int buffLen)
{
	return RQASRegex_match_loop(sourStr.c_str(),sourStr.length(),searchPattern,checkPattern,outBuff,buffLen);
}

int RQASRegex_match_loop(const char * sourStr, unsigned int sourLen, const char * searchPattern, const char * checkPattern, char * outBuff, unsigned int buffLen)
{
	int writeBytes = 0;
	const std::regex pattern(searchPattern);					///根据传入的正则表达式，构建表达式对象

	for (std::cregex_iterator iter_start(sourStr, sourStr + sourLen, pattern), iter_end;		///遍历源字符串中所有匹配正则表达式的子字符串
		iter_start != iter_end
		&& buffLen > writeBytes
		; iter_start++)
	{
		if (RQRegex_match(iter_start->str().c_str(), checkPattern, outBuff+writeBytes, buffLen - writeBytes))	///利用新验证表达式对获取到的数据进行调整
		{
			writeBytes += RQStrlen(outBuff + writeBytes);			
		}
	}
	outBuff[writeBytes] = 0;
	return writeBytes;

}

///---RQAsregex
int RQRegex_search(const std::string & sourStr, const char * checkPattern, int times)
{
	std::regex pattern(checkPattern);
	std::smatch result;
	std::regex_search(sourStr, result, pattern);
	return result.position(times);
}

int RQRegex_search(const char * sourStr, const char * checkPattern, int times)
{
	std::string content = sourStr;
	return RQRegex_search(content, checkPattern, times);
}



void RQRegex_replace(const std::string & sourStr,
	const char * checkPattern,
	const char * replaceStr,
	char* outBuff)
{
	std::regex pattern(checkPattern);
	///*std::regex_replace(outBuff, sourStr.begin(), sourStr.end(), pattern, replaceStr) = 0;
	*std::regex_replace(outBuff, sourStr.begin(), sourStr.end(), pattern, std::string(replaceStr));
}

void RQRegex_replace(const char * sourStr, const char * checkPattern, const char * replaceStr, char * outBuff)
{
	std::string copySourStr(sourStr);
	RQRegex_replace(copySourStr, checkPattern, replaceStr, outBuff);
}
