#pragma once
#include <map>
/*
	该类是一个词法分析器，功能是将外部传入的字符串分解为元词集，并对外返回
*/

class RQElementManager;
class RQWord;

class RQSentence;
class RQLexical
{
public:
	static void InitObj();			//初始化字码表
	/*
		解析传入的C代码，输入词源组
		globalBuff：C语言代码字符串
		wordArray：根据语句对象识别到的词源组
		tsId：暂存器，用于词源的分配和回收
		wordCount	[out]	本次解析获取到的元词数量
		return：本句语句的特性（左右括号）
	*/
	static unsigned int Analysz(const char* buff,RQWord** wordArray,void* junkTsId);

};