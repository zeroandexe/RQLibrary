#include "compiler/RQActuator.h"
#include "compiler/RQGrammar.h"
#include "compiler/RQActuator.h"
#include "compiler/RQSentence.h"
#include "compiler/RQScriptFile.h"
#include "compiler/RQCSC.h"
#include "compiler/RQExpressionCompute.h"
#include "compiler/RQExpression.h"
#include "compiler/RQLine.h"
#include "compiler/RQCodeSet.h"
#include "RQVar.h"
#include "define/RQInterDefine.h"

RQActuator::RQActuator()
{
	_sentenceCSC = new RQCSC;
	_expressionCSC = new RQCSC;
	_expressionCompute = new RQExpressionCompute;
}

RQActuator::~RQActuator()
{
	delete _sentenceCSC;
	delete _expressionCSC;
	delete _expressionCompute;
}

void RQActuator::RunScriptFile(RQScriptFile* scriptFile)
{
	///ȡ���ű��еĴ��������󣬲������������������������������ִ�к������д���
	scriptFile->ResetHeadLine();
	_sentenceCSC->Reset();
	_RunCodeSet(scriptFile->GetCodeSet(), _sentenceCSC, &RQActuator::_RunSentence);
	
}

unsigned int RQActuator::_RunSentence(RQLine * param)
{
	RQSentence* sentence = (RQSentence*) param;
	///ȡ���������е��ӱ��ʽ���뼯����������ʽ�����������������ʽִ�к������д���
	sentence->ResetHeadLine();
	_RunCodeSet(sentence->GetCodeSet(),_expressionCSC, &RQActuator::_RunExpresion);
	///������󷵻ر��ε�ִ�н�������������Ŀ���
	
	RQVar* resultVar = sentence->GetResult();
	bool result = false;
	//if (resultVar && resultVar->GetAddr())
	if(resultVar)
	{
		result = resultVar->GetBool();
	}

	return _sentenceCSC->Control(sentence->_cscLabel, result);
}

unsigned int RQActuator::_RunExpresion(RQLine * param)
{
	_expressionCSC->Reset();
	RQExpression* expression = (RQExpression*)param;
	///�����ʽ���м��㣬�õ�������
	_expressionCompute->Compute(expression);
	///���ؼ����������������Ŀ���
	RQVar* resultVar = expression->_result;
	bool result = false;
	//if (expression->_result && expression->_result->GetAddr())
	if(resultVar)
	{
		result = resultVar->GetBool();
	}
	return  _expressionCSC->Control(expression->_cscLabel,result);
}

void RQActuator::_RunCodeSet(RQCodeSet * codeSet, RQCSC * csc, unsigned int(RQActuator::* Run)(RQLine *))
{
	RQLine* line = codeSet->GetNextLine();
	unsigned long reply_opt;
	while (line)
	{
		reply_opt = (this->*Run)(line);
		switch (reply_opt)
		{
		case M_SCRIPT_OPT_JUMP_DOWN:
			codeSet->JumpDownCodeBlock();
			break;
		case M_SCRIPT_OPT_JUMP_UP:
			codeSet->JumpUpCodeBlock();
			break;
		default:
			reply_opt = M_SCRIPT_OPT_GET_LINE;
			break;
		}
		line = codeSet->GetNextLine();
	}
}