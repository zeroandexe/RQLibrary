#pragma once
#include "node/RQNode.h"

/*
	�ö���Ϊ���ض��������ڲ���Ӻ��޸Ĺ��ڶ��������£�ʹ�����ܹ�ʹ��RQ����
*/

class RQLoadNode :
	public RQNode
{
public:
	RQLoadNode();
	virtual ~RQLoadNode();
	virtual void Create(void* param=0);
	virtual void Clear();

	void* _obj;							//���ؽڵ������ص�����
	RQLoadNode* _loadNode;				//��һ����������Ĺ��ؽڵ�
};