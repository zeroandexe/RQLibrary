#pragma once

#include <string>
/*
	传入被检测的源字符和检测表达式，并根据检测结果获取子字符串
	输入：
	sourStr：被检测的源字符串
	patternStr：正则表达式
	checkPatternStr：在第一次匹配结果的基础上再次匹配截取字串的表达式
	strArray：作为结果返回的二维数组
	arrayCount：二维数组的二维元素个数
	startIndex：写入时的第一个元素下标
	输入出：
	strArray：获取目标子字符串
	return：子字符串个数，失败为0（或字串长度）

*/


/*
	循环匹配：
		获取所有与表达式匹配的字串，并放在数组的每个元素中。
	sourStr		[in]	被检索的字符串
	patternStr	[in]	用于截取子串的表达式
	strArray	[out]	用于装载截取结果的内存首地址
	arrayCount	[in]	内存中规划的元素个数
	elementSize	[in]	每个元素可用的字节数
	startIndex	[in]	数组中存储子串的起始元素下标
*/
int RQRegex_match_loop(const std::string& sourStr,
	const char* patternStr,
	char* strArray[], int arrayCount, unsigned int elementSize, int startIndex = 0);

int RQRegex_match_loop(const char* sourStr, unsigned int sourLen,
	const char * patternStr,
	char* strArray[],
	unsigned int arrayCount, unsigned int elementSize, int startIndex = 0);

/*
	循环匹配：
		匹配所有与表达式配匹配的字串，并将其拼接在统一字串中（返回字串长度）
	sourStr		[in]	被检索的字符串
	patternStr	[in]	用于截取子串的表达式
	outBuff		[out]	用于装载截取结果的内存首地址
	buffLen		[in]	结果存储内存能装下的字节数
*/
int RQSRegex_match_loop(const std::string& sourStr,
	const char* patternStr,
	char* outBuff, unsigned int buffLen);

int RQSRegex_match_loop(const char* sourStr, unsigned int sourLen,
	const char* patternStr,
	char* outBuff, unsigned int buffLen);


/*
	获取第一个与表达式匹配的字串
*/
bool RQRegex_match(const std::string& sourStr,
	const char* patternStr,
	char* strArray, unsigned int elementSize);

bool RQRegex_match(const char* sourStr,
	const char* patternStr,
	char* strArray, unsigned int elementSize);

/*
	循环匹配+结构校整
		获取匹配searchPattern的字符串，并根据checkPattern进行调整
*/
int RQARegex_match_loop(const std::string& sourStr,
	const char* searchPattern, const char* checkPattern,
	char* strArray[], unsigned int arrayCount, unsigned int elementSize, int startIndex = 0);

int RQARegex_match_loop(const char* sourStr, unsigned int sourLen,
	const char* searchPattern, const char* checkPattern,
	char* strArray[], unsigned int arrayCount,
	unsigned int elementSize, int startIndex = 0);



int RQASRegex_match_loop(const std::string& sourStr,
	const char* searchPattern, const char* checkPattern,
	char* outBuff, unsigned int buffLen);

int RQASRegex_match_loop(const char* sourStr,unsigned int sourLen,
	const char* searchPattern, const char* checkPattern,
	char* outBuff, unsigned int buffLen);

/*
	在sourStr中根据checkPattern进行匹配，并获取tiems次匹配结果(默认为第一次匹配的结果)
	注意：若请求的times超过了结果的个数将返回sourStr的总长
	sourStr			[in]	用于被检测的字符串内容
	checkPattern	[in]	目标子串的表达式
	times			[in]	期望获取第N个符合条件的子串
	return			[out]	返回目标子串在原字符串中的偏移量，若times超过有效结果个数将返回sourStr的总长
*/
int RQRegex_search(const std::string& sourStr,
	const char* checkPattern, int times = 1);

int RQRegex_search(const char* sourStr,
	const char* checkPattern, int times = 1);


/*
	检查字符串中的目标字符，将符合要求的子串进行替换
	sourStr			[in]	源字符串
	checkPattern	[in]	检测子串的表达式
	replaceStr		[in]	用于替换子串的新字符串
	outBuff			[in]	替换后结果保存的内存地址
*/
void RQRegex_replace(const std::string& sourStr,
	const char* checkPattern, const char* replaceStr,
	char* outBuff);

void RQRegex_replace(const char* sourStr,
	const char* checkPattern, const char* replaceStr,
	char* outBuff);