#include "compiler/RQCSC.h"
#include "compiler/RQLine.h"
#include "define/RQASTDefine.h"
#include "define/RQInterDefine.h"
#include "RQVar.h"

typedef RQCSC::Type_KeyLabelFun Type_KeyLabelFun;
Type_KeyLabelFun* RQCSC::_keyLabelFunArray;	//�ؼ��ֺ�������

RQCSC::RQCSC()
{
	int* start = new int[MAX_RUNSTATE_DEPTH + 1];
	_runStateArray = start + 1;			//�����ڼ��״̬ʱ������������һ��״̬��Ϊ�˱���ʹ�ù�����ʹ��if����������Ϊ����һ��Ԫ�ص������
	Reset();
}


RQCSC::~RQCSC()
{
	int * end = _runStateArray - 1;
	delete[]end;
}

unsigned int RQCSC::KeyLabelFun_Invalid(bool result)
{
	return M_SCRIPT_OPT_GET_LINE;
}

unsigned int RQCSC::Control(unsigned int cscLabel, bool result)
{
	return (this->*_keyLabelFunArray[cscLabel])(result);
}

//����if�������Ľű�����
//true��ִ��if֮�µĴ���飬����ȡ��һ�д���
//false������if��ǰ����飬ִ�к������루else���������룩
//����if�ǳɹ�����ʧ�ܣ�����Ҫ��¼�ж�״̬
unsigned int RQCSC::KeyLabelFun_If(bool result)
{
	int is = !(M_RUN_STATE_IF_FALSE_READY - _runStateArray[_runStateCount - 1]) //���ǰһ��״̬�Ƿ���ifʧ��״̬�����Ҹ�ifû��else���봦���
		+ !(M_RUN_STATE_IF_END - _runStateArray[_runStateCount - 1]);		//���֮ǰ��if�Ƿ��ѱ���ȫ����

	if (is)		//���ݼ���ֵ��⵱ǰ״̬�Ƿ���Ч
	{
		_runStateCount--;
	}

	int writeIndex = _runStateCount++;						//��¼д���±�
	_runStateArray[writeIndex] = M_RUN_STATE_IF_TRUE;				//Ĭ��if�����ɹ�
	unsigned int reply_opt = M_SCRIPT_OPT_GET_LINE;

	if (!result)			//���if�����Ƿ�ʧ�ܣ�
	{
		reply_opt = M_SCRIPT_OPT_JUMP_DOWN;	//����ifʧ�ܣ�������if���ڵĴ����
		_runStateArray[writeIndex] = M_RUN_STATE_IF_FALSE_READY;	//����if״̬����Ϊif_false����else���������ж�
	}
	return reply_opt;
}

/*
����while�Ĵ�����Ҫ���while���жϽ����
true��while�ɹ��������������whileѭ��״̬����ȡ��һ�д������ִ��
false��whileʧ�ܣ�while�Ĵ���鲻��ִ�У���������whileѭ��״̬��while״̬����¼
*/
unsigned int RQCSC::KeyLabelFun_While(bool result)
{
	int is = !(M_RUN_STATE_IF_FALSE_READY - _runStateArray[_runStateCount - 1]) //���ǰһ��״̬�Ƿ���ifʧ��״̬�����Ҹ�ifû��else���봦���
		+ !(M_RUN_STATE_IF_END - _runStateArray[_runStateCount - 1]);		//���֮ǰ��if�Ƿ��ѱ���ȫ����

	if (is)		//���ݼ���ֵ��⵱ǰ״̬�Ƿ���Ч
	{
		_runStateCount--;
	}

	unsigned int reply_opt = M_SCRIPT_OPT_JUMP_DOWN;				//Ĭ��whileʧ�ܣ�
	if (result)				//���while�ж������Ƿ�ɹ�
	{
		reply_opt = M_SCRIPT_OPT_GET_LINE;			//�����жϳɹ����ȡ��һ��������ִ��
		_runStateArray[_runStateCount++] = M_RUN_STATE_WHILE;		//��ǵ�ǰ�ѽ���whileѭ��״̬
	}
	return reply_opt;
}

/*
else���봦����鵱ǰ״̬
if_end������else�Ĵ���鲢��ȡ����if״̬
if_false����������ִ��
*/
unsigned int RQCSC::KeyLabelFun_Else(bool result)
{
	unsigned int reply_opt = M_SCRIPT_OPT_GET_LINE;
	switch (_runStateArray[_runStateCount - 1])
	{
	case M_RUN_STATE_IF_FALSE_READY:					//���if���ж��Ƿ���ʧ��,
		_runStateArray[_runStateCount - 1] = M_RUN_STATE_IF_FALSE;
		reply_opt = M_SCRIPT_OPT_GET_LINE;
		break;
	case M_RUN_STATE_IF_END:
		reply_opt = M_SCRIPT_OPT_JUMP_DOWN;			//��������else�����
		_runStateCount--;								//����if״̬
		break;
	}
	return reply_opt;
}


/*
�����Ŵ�����Ҫ���ֵ�ǰ�������ڵ����
����while���������ϻ�����while�ж������
����if_true����if_false�������޸�if_true״̬Ϊif_end��������ȡ��һ��ִ��
*/
unsigned int RQCSC::KeyLabelFun_BigRightParentheses(bool result)
{
	int is = !(M_RUN_STATE_IF_FALSE_READY - _runStateArray[_runStateCount - 1]) //���ǰһ��״̬�Ƿ���ifʧ��״̬�����Ҹ�ifû��else���봦���
		+ !(M_RUN_STATE_IF_END - _runStateArray[_runStateCount - 1]);		//���֮ǰ��if�Ƿ��ѱ���ȫ����

	if (is)		//���ݼ���ֵ��⵱ǰ״̬�Ƿ���Ч
	{
		_runStateCount--;
	}
	unsigned int reply_opt = M_SCRIPT_OPT_GET_LINE;

	int lastStateIndex = _runStateCount - 1;				//����״̬Ԫ���±�
	switch (_runStateArray[lastStateIndex])
	{
		//whileѭ������
	case M_RUN_STATE_WHILE:
		reply_opt = M_SCRIPT_OPT_JUMP_UP;				//����while�������ж�
		_runStateCount--;								//����whileѭ��״̬
		break;

		//if����鴦��
	case M_RUN_STATE_IF_FALSE:
	case M_RUN_STATE_IF_TRUE:
		_runStateArray[lastStateIndex] = M_RUN_STATE_IF_END;			//���if��������
		break;
	}
	return reply_opt;
}


void RQCSC::InitObj()
{
	if (_keyLabelFunArray)
	{
		return;
	}

	_keyLabelFunArray = new Type_KeyLabelFun[M_KEY_LABEL_TOTAL];
	_keyLabelFunArray[M_KEY_LABEL_INVALID] = &RQCSC::KeyLabelFun_Invalid;
	_keyLabelFunArray[M_KEY_LABEL_IF] = &RQCSC::KeyLabelFun_If;
	_keyLabelFunArray[M_KEY_LABEL_WHILE] = &RQCSC::KeyLabelFun_While;
	_keyLabelFunArray[M_KEY_LABEL_ELSE] = &RQCSC::KeyLabelFun_Else;
	_keyLabelFunArray[M_KEY_LABEL_BIG_RIGHT_PARENTHESES] = &RQCSC::KeyLabelFun_BigRightParentheses;
}

void RQCSC::Reset()
{
	_runStateCount = 0;
	_runStateArray[-1] = M_RUN_STATE_NATURAL;				//���Ԫ�ص�����״̬Ϊ����
	_runStateArray[0] = M_RUN_STATE_NATURAL;
}