#pragma once
#include "define/RQInterDefine.h"
#include "node/RQNode.h"
/*
	���ʽڵ㣬���ڱ����﷨��������Ԫ��
*/

class RQWord :public RQNode
{
public:
	RQWord();
	~RQWord();
	virtual void Create(void* param = 0);
	virtual void Clear();

	unsigned int _len;								//���ʳ���
	unsigned int _type;								//�������ͣ����ơ����š��������֡��ַ�����
	char _buff[MAX_STR_LEN_PER_WORD];				//ÿ�����ʵ�����
};