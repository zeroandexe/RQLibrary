#include "compiler/RQResolver.h"
#include "compiler/RQLexical.h"
#include "compiler/RQGrammar.h"
#include "define/RQInterDefine.h"
#include "compiler/RQSentence.h"
#include "RecycleBin.h"
#include "compiler/RQScriptFile.h"
#include <fstream>

bool RQResoler::AnalyzeFile(const char * path, RQScriptFile * scriptFile)
{
	std::ifstream inFile(path);								///打开脚本文件输入流
	if (!inFile.is_open())
	{
		return false;
	}

	void* junkTsId = RecycleBin::CreateTS();				///创建一个废弃物暂存器，用于本次脚本解析后可立即回收的对象，用于空间节省
	RQGrammar grammar(scriptFile, junkTsId); 
	RQWord* wordArray[MAX_WORD_NUM_PER_LINE];			///声明定义元词数组，用于装载解析后元词

	char readBuff[MAX_STR_LEN_PER_LINE];					///声明创建接收缓冲区


	inFile.getline(readBuff, MAX_STR_LEN_PER_LINE);			///获取第一行代码
	bool retBol = true;
	while (!inFile.eof() || readBuff[0])					///防止空行退出，防止最后一行未解析
	{
		///先将获取到的代码指令，在词法器中转换并获取到词源组,并获取词源
		unsigned int braceType = RQLexical::Analysz(readBuff, wordArray, junkTsId);
		if (!wordArray[0])			///检测本次解析是否获取到有效的元词（用于注释行过滤）
		{
			inFile.getline(readBuff, MAX_STR_LEN_PER_LINE);			///获取下一行代码
			continue;				///解析下一行
		}
		RQSentence* sentence = grammar.AnalyszWords(wordArray);		///解析元词组，并获取解析后的语句对象

		if (!sentence)				///检测语法树创建是否成功，不成功表示语法存在问题，中止解析，回收所有资源
		{
			retBol = false;
			break;					///终止解析
		}

		sentence->_braceType = braceType;			///将词法器解析出花括号特征赋予语句对象
		scriptFile->AddSentence(sentence);			///将解析完成的语句对象存入脚本对象中

		inFile.getline(readBuff, MAX_STR_LEN_PER_LINE);			///获取下一行代码
	}
	RecycleBin::RecycleTemStorage(junkTsId);		///回收本次脚本解析产生的垃圾
	RecycleBin::RecycleTS(junkTsId);				///回收暂存器
	inFile.close();					///解析结束关闭文件流
	return retBol;											///返回代码解析情况
}

bool RQResoler::AnalyzeSentence(const char * codeStr, RQScriptFile * scriptFile)
{
	void* junkTsId = RecycleBin::CreateTS();			///创建一个废弃物暂存器，用于本次脚本解析后可立即回收的对象，用于空间节省
	RQGrammar grammar(scriptFile, junkTsId);			///创建语法分析器，分析代码语法错误
	RQWord* wordArray[MAX_WORD_NUM_PER_LINE];			///声明定义元词数组，用于装载解析后元词
	unsigned int braceType = RQLexical::Analysz(codeStr, wordArray, junkTsId);		///对输入代码进行词法解析
	bool retBool = false;

	if (wordArray[0])
	{
		RQSentence* sentence = grammar.AnalyszWords(wordArray);	///解析元词组，并获取解析后的语句对象
		if (sentence)									///检测语法是否成功解析
		{
			sentence->_braceType = braceType;			///将词法器解析出花括号特征赋予语句对象
			scriptFile->AddSentence(sentence);			///将成功解析的代码存入脚本对象中
			retBool = true;
		}
	}
	RecycleBin::RecycleTemStorage(junkTsId);		///回收本次脚本解析产生的垃圾
	RecycleBin::RecycleTS(junkTsId);				///回收暂存器
	return retBool;
}
