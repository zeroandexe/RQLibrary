#pragma once

/*
	code stream control����������������������CSC�����ڴ���if-else-while�ȴ����ĳ���ִ��ʱ��֧��ת����
*/

class RQLine;
class RQVar;

class RQCSC
{
public:
	RQCSC();
	typedef unsigned int(RQCSC::* Type_KeyLabelFun)(bool);			//�����ؼ���ʶ����������
	virtual ~RQCSC();

	//�����������ǩ�͵�ǰ����ִ�н���������������Ե���תָ��
	unsigned int Control(unsigned int cscLabel,bool result);

	//��ʼ����������������������ģ��
	static void InitObj();

	//���ó����������������ڲ�����
	void Reset();

private:
	int* _runStateArray;					//�ű�if��whileǶ������״̬�ļ�¼��ֵ
	int _runStateCount;						//��״̬��¼�����е�ǰ״̬�ĸ���
	static Type_KeyLabelFun* _keyLabelFunArray;	//�ؼ��ֺ�������

private:
	unsigned int KeyLabelFun_Invalid(bool result);
	unsigned int KeyLabelFun_If(bool result);
	unsigned int KeyLabelFun_While(bool result);
	unsigned int KeyLabelFun_Else(bool result);
	unsigned int KeyLabelFun_BigRightParentheses(bool result);

};