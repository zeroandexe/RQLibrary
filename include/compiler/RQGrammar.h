#pragma once
//#include <map>
#include <string>
#include "RQVar.h"
#include "define/RQdefine.h"
#include "define/RQInterDefine.h"
#include "RQMap.h"
/*
	�����Ǵʷ����������﷨����������������紫���һ����䣨ÿ����䶼�Ǹ�������䣩��������ִ��
	ע�⣺�Խ����ڵ�ֵ�������ؽ�

	//�������﷨������Ҫ���ʷ����������﷨����������

*/
class RQDBStack;		//����ջ���û����ձ����ڵ�
class RQStack;			//���ղ������ʽ���ṩ������ջ�Ͳ���ջ
class RQWord;				//Ԫ�ʴ洢�ռ�

class RQASTNode;		//�﷨���ڵ�
class RQSymbolInfo;
class RQScriptFile;		//�ű�����
class RQSentence;
class RQExpression;
class RQEnviVarTable;

class RQGrammar
{
public:
	RQGrammar(RQScriptFile* scriptObj, void* junkTsId);
	~RQGrammar();

	typedef RQVar*(RQGrammar::* Type_SymbolFun)(RQASTNode*, RQSentence* sentence);
	//���ݴ����Ԫ�������������ӱ��ʽ
	RQSentence* AnalyszWords(RQWord** wordArray);

	static void InitObj();

private:
	//����Ԫ���飬�����ظ���Ԫ�������������﷨��,���﷨������ʧ�ܣ��򷵻ؿ�ָ��
	RQASTNode* _CreateASTTree(RQWord** wordArray);

	//���ݴ�����﷨�����б��ʽ��������ȡ�ӱ��ʽ����
	void _AnalyszASTTree(RQASTNode* root, RQSentence* sentenceArray);

	/*pRoot���﷨�����ڵ㣨�������ָ����Ϊ��������ȡ�﷨���ڲ��������﷨�����ڵ㣩
		startNode����δ�����﷨���ڵ��е����(�ʼʱΪ�գ���������Ϊ��)
		son����ǰ
		*/
	RQASTNode * _InsertNewNodeToTree(RQASTNode* pRoot, RQASTNode* startNode, RQASTNode* newNode);

	//��ȡ���п��õı���
	RQASTNode* _GetFreeRQASTNode();
	RQExpression* _GetFreeExpression();
	RQVar* _GetFreeVar();
	RQSentence* _GetFreeSentence();

	/*
		word�����������Ԫ��
		return��������Ԫ�ʴ��ڣ����Ƿ���Ԫ�ʣ���ǰ���ű�Ҳ֧�ָ÷��ţ��򷵻طɿ�
				�������ؿ�
	*/
	RQASTNode* _wordToAstNode(RQWord* word);

	/*
		��wordת��Ϊ��Ӧ�ı�������
		��Ԫ�ʵ������޷�ת��Ϊ�ű��������򷵻ؿ�ָ��
	*/
	RQVar* _wordToVar(RQWord* word);

	static void _InitSymbolInfoMap();			//�����������Ϣ��
	static void _InitSymbolFunArray();
private:
	///std::map<std::string, RQVar*>* _localMap;			//�ֲ��������ñ���ÿ���ű�����һ��(��ÿ��������ֻ��һ��)
	RQMap<std::string, RQVar*>* _localMap;
	///static std::map<std::string, RQSymbolInfo>* _symbolMap;	//��������ű����ظ��ַ��ŵ���Ϣ	
	static RQMap<std::string, RQSymbolInfo>* _symbolMap;
	static Type_SymbolFun* _symbolFunArray;			//�����������������

	void* _scriptTsId;
	void* _junkTsId;
	RQVar* _exp;

	///RQStack* _funParamStack;		//�����ڴ����������ʱ�Բ����ļ�¼		
	RQVar* _paramStack[MAX_PARAM_PER_FUN+1];				//����ջ
	int _paramCount;							//��������
private:
	//���µȺŷ������㺯����ԭ�����ڵȺŵķ���ֵΪ�����
	RQVar * SymbolFun_Equal(RQASTNode* symbolNode, RQSentence* sentence);	// =

	//������������������ڴ�����ֵ�������Ҳ����������
	RQVar* SymbolFun_Com(RQASTNode* symbolNode, RQSentence* sentence);

	//&&����������ӱ��ʽ����ʱ����Ҫ�Ա��ʽ����if-true-and�Ŀ���
	RQVar* SymbolFun_And(RQASTNode* symbolNode, RQSentence* sentence);

	//&&����������ı���=�����
	RQVar* SymbolFun_AndToEqual(RQASTNode* symbolNode, RQSentence* sentence);

	// ||����������ӱ��ʽʱ����Ҫ�Ա��ʽ����if-true-or�Ŀ���
	RQVar* SymbolFun_Or(RQASTNode* symbolNode, RQSentence* sentence);

	// ||����������ı���=�����
	RQVar* SymbolFun_OrToEqual(RQASTNode* symbolNode, RQSentence* sentence);

	//���¶�������������ڶ����������Ϊ��������ҪΪ������������ѹջ
	RQVar* SymbolFun_Comma(RQASTNode* symbolNode, RQSentence* sentence);		

	//�������������Ҫ�ԼĴ����������з��أ�ͬʱҲ��Ҫ����������
	RQVar* SymbolFun_Fun(RQASTNode * symbolNode, RQSentence* sentence);

	RQVar* SymbolFun_Null(RQASTNode* symbolNode, RQSentence* sentence);

	//����ĿǰҪ��if-while-elseЯ���������ŵģ�����������Ҫ��ֵ��ֵ������ȥ
	RQVar* SymbolFun_LeftBrace(RQASTNode* symbolNode, RQSentence* sentence);

	//�һ�����ĿǰҪ����뵥��һ�У����Դ˺�������Ҫ����κδ���
	RQVar* SymbolFun_RightBrace(RQASTNode* symbolNode, RQSentence* sentence);

	//С���㴦��
	RQVar* SymbolFun_Point(RQASTNode* symbolNode, RQSentence* sentence);

	//@ȫ�ֱ�������
	RQVar* SymbolFun_AT(RQASTNode* symbolNode, RQSentence* sentence);

	//��Ч����ʹ��
	RQVar* SymbolFun_Invalid(RQASTNode* symbolNode, RQSentence* sentence);
private:
	//����������
	RQVar* _CreateFloat(RQVar* left, RQVar* right);
};