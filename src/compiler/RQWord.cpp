#include "compiler/RQWord.h"
#include "define/RQInterDefine.h"

RQWord::RQWord()
{
	
}


RQWord::~RQWord()
{
}

void RQWord::Create(void* param)
{
	Clear();
}

void RQWord::Clear()
{
	_len = 0;
	_type = M_WORD_TYPE_END;				//��ǰ���ڽ�����ֹ����\r��\n��//�������Ƿ��ַ���E_INVALID;
	_buff[0] = 0;
}