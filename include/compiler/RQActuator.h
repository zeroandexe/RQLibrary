#pragma once
/*
	�����Ǳ�������ִ������������Ԫַ�������ִ�д���
*/
class RQCSC;				//������������
class RQExpressionCompute;	//���ʽ������
class RQScriptFile;			//�ű��ļ�����
class RQSentence;
class RQCodeSet;			//���뼯��
class RQExpression;
class RQVar;
class RQLine;

class RQActuator
{
public:
	RQActuator();
	~RQActuator();

	//����ִ�нű�
	void RunScriptFile(RQScriptFile* scriptFile);	//ִ�д����������
private:

	unsigned int _RunSentence(RQLine* param);
	unsigned int _RunExpresion(RQLine* param);

	void _RunCodeSet(RQCodeSet* codeSet,RQCSC* csc,unsigned int(RQActuator::* Run)(RQLine*));
private:
	RQCSC * _sentenceCSC;							//�ű���������������
	RQCSC* _expressionCSC;							//���ʽ��������������
	RQExpressionCompute* _expressionCompute;		//���ʽ������
};