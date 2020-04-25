#pragma once
//#include <map>
#include <string>
#include "RQVar.h"
#include "define/RQdefine.h"
#include "define/RQInterDefine.h"
#include "RQMap.h"
/*
	该类是词法分析器和语法分析器，分析从外界传入的一行语句（每行语句都是个完整语句），并进行执行
	注意：以仅存在的值不允许重建

	//待补充语法树后需要将词法分析器和语法分析器分离

*/
class RQDBStack;		//回收栈，用户回收变量节点
class RQStack;			//仿照波兰表达式，提供操作符栈和参数栈
class RQWord;				//元词存储空间

class RQASTNode;		//语法树节点
class RQSymbolInfo;
class RQScriptFile;		//脚本对象
class RQSentence;
class RQExpression;
class RQEnviVarTable;

class RQGrammar
{
public:
	RQGrammar(RQScriptFile* scriptObj, void* junkTsId);
	~RQGrammar();

	typedef RQVar*(RQGrammar::* Type_SymbolFun)(RQASTNode*, RQSentence* sentence);
	//根据传入的元词组分析代码的子表达式
	RQSentence* AnalyszWords(RQWord** wordArray);

	static void InitObj();

private:
	//传入元词组，并返回根据元词组所创建的语法树,若语法树创建失败，则返回空指针
	RQASTNode* _CreateASTTree(RQWord** wordArray);

	//根据传入的语法树进行表达式分析，获取子表达式数组
	void _AnalyszASTTree(RQASTNode* root, RQSentence* sentenceArray);

	/*pRoot：语法树根节点（传入二阶指针作为篮子来获取语法树内部决定的语法树根节点）
		startNode：这次处理的语法树节点中的起点(最开始时为空，后续不得为空)
		son：当前
		*/
	RQASTNode * _InsertNewNodeToTree(RQASTNode* pRoot, RQASTNode* startNode, RQASTNode* newNode);

	//获取空闲可用的变量
	RQASTNode* _GetFreeRQASTNode();
	RQExpression* _GetFreeExpression();
	RQVar* _GetFreeVar();
	RQSentence* _GetFreeSentence();

	/*
		word：运算符符号元词
		return：当传入元词存在，并是符号元词，当前符号表也支持该符号，则返回飞空
				其他返回空
	*/
	RQASTNode* _wordToAstNode(RQWord* word);

	/*
		将word转换为对应的变量对象
		若元词的特征无法转换为脚本变量，则返回空指针
	*/
	RQVar* _wordToVar(RQWord* word);

	static void _InitSymbolInfoMap();			//创建运算符信息表
	static void _InitSymbolFunArray();
private:
	///std::map<std::string, RQVar*>* _localMap;			//局部变量表，该表是每个脚本富含一个(即每个解释器只有一个)
	RQMap<std::string, RQVar*>* _localMap;
	///static std::map<std::string, RQSymbolInfo>* _symbolMap;	//运算符符号表，承载各种符号的信息	
	static RQMap<std::string, RQSymbolInfo>* _symbolMap;
	static Type_SymbolFun* _symbolFunArray;			//声明运算符函数类型

	void* _scriptTsId;
	void* _junkTsId;
	RQVar* _exp;

	///RQStack* _funParamStack;		//用于在处理逗号运算符时对参数的记录		
	RQVar* _paramStack[MAX_PARAM_PER_FUN+1];				//函数栈
	int _paramCount;							//参数个数
private:
	//留下等号符号运算函数，原因在于等号的返回值为左参数
	RQVar * SymbolFun_Equal(RQASTNode* symbolNode, RQSentence* sentence);	// =

	//常规运算符函数，用于处理返回值不是左右参数的运算符
	RQVar* SymbolFun_Com(RQASTNode* symbolNode, RQSentence* sentence);

	//&&运算符在做子表达式分析时，需要对表达式进行if-true-and的控制
	RQVar* SymbolFun_And(RQASTNode* symbolNode, RQSentence* sentence);

	//&&运算符运算后的变体=运算符
	RQVar* SymbolFun_AndToEqual(RQASTNode* symbolNode, RQSentence* sentence);

	// ||运算符在做子表达式时，需要对表达式进行if-true-or的控制
	RQVar* SymbolFun_Or(RQASTNode* symbolNode, RQSentence* sentence);

	// ||运算符运算后的变体=运算符
	RQVar* SymbolFun_OrToEqual(RQASTNode* symbolNode, RQSentence* sentence);

	//留下逗号运算符，由于逗号运算符较为特殊它需要为函数参数进行压栈
	RQVar* SymbolFun_Comma(RQASTNode* symbolNode, RQSentence* sentence);		

	//函数运算符，需要对寄存器变量进行返回，同时也需要整理函数参数
	RQVar* SymbolFun_Fun(RQASTNode * symbolNode, RQSentence* sentence);

	RQVar* SymbolFun_Null(RQASTNode* symbolNode, RQSentence* sentence);

	//由于目前要求if-while-else携带对左花括号的，所以左花括号要将值左值负载上去
	RQVar* SymbolFun_LeftBrace(RQASTNode* symbolNode, RQSentence* sentence);

	//右花括号目前要求必须单独一行，所以此函数不需要添加任何处理
	RQVar* SymbolFun_RightBrace(RQASTNode* symbolNode, RQSentence* sentence);

	//小数点处理
	RQVar* SymbolFun_Point(RQASTNode* symbolNode, RQSentence* sentence);

	//@全局变量引用
	RQVar* SymbolFun_AT(RQASTNode* symbolNode, RQSentence* sentence);

	//无效函数使用
	RQVar* SymbolFun_Invalid(RQASTNode* symbolNode, RQSentence* sentence);
private:
	//创建浮点数
	RQVar* _CreateFloat(RQVar* left, RQVar* right);
};