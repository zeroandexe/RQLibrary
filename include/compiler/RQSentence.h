#pragma once
#include "node/RQNode.h"
#include "define/RQInterDefine.h"
#include "RQCodeSet.h"
#include "RQLine.h"

/*
	�ö����ǽű��е���������Ϊ������ӱ��ʽ�ļ���
	ע�⣺
		�����е�����Ԫ������������������������ﲢû�л��յ�Ȩ����
		����Ļ��ս����ǽ����ϵ��е�������һ�У���ʾ���Ͽ����ٴν��и��Ǻͷ���
*/

class RQWord;			//Ԫ�ʶ���
class RQVar;			//
class RQCodeSet;		//���뼯��
class RQExpression;	//�ӱ��ʽ

class RQSentence : public RQLine
{
public:
	RQSentence();
	virtual ~RQSentence();

	virtual void Create(void* param = 0);
	virtual void Clear();

	//��ȡ�������ı�ǩ
	int GetLabel();

	//���ñ������ı�ǩ
	void SetLabel(int label);

	//�������������һ���������ʽ
	void AddExpression(RQExpression* expression);

	//���û�ȡ�е���һ�����ʽ
	void ResetHeadLine();

	//��ȡ���б��ʽ�Ľ��
	RQVar* GetResult();
	void SetResult(RQVar* result);

	//��ȡ�������еı��ʽ���뼯��
	RQCodeSet* GetCodeSet();

private:
	RQCodeSet* _codeSet;
	RQVar* _result;			//������������ļ�����
};