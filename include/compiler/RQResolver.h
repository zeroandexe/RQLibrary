#pragma once
/*
	该对象为解析器对象，由词法器和语法器构建组合而成，
	主要对外提供字符串代码到可执行代码的转换
*/

class RQSentence;
class RQScriptFile;
class RQGrammar;

class RQResoler {
public:
	/*
		分析闯入的字符串代码进行解析，若传入的是注释行、空指针或错误语法都将导致解析失败
		path		[in]	脚本文件路径
		scriptFile	[in]	保存代码的信息对象
		return		[out]	本次解析是否成功
	*/
	static bool AnalyzeFile(const char* path, RQScriptFile* scriptFile);

	/*
		作用同上，但是仅解析一行语句
		codeStr		[in]		被解析的语句行
		scriptFile	[in-out]	提供被解析语句行的存储
		return		[out]		本此解析是否成功
	*/
	static bool AnalyzeSentence(const char* codeStr, RQScriptFile* scriptFile);
};