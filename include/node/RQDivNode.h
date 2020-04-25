#pragma once 
#include "node/RQNode.h"
/*
	�ö���Ϊ�豸�Ļ�����������ṩ�豸������ʹ�õ�Լ������,����ͨ���̳�ʹ��
*/
class RQLoadNode;

class RQDivNode : public RQNode
{
public:
	friend class RQDivManager;
	friend class RQDivContainer;

	virtual void Create(void* param = 0);
	virtual void Clear();  

	virtual void Destory(void* param = 0) = 0;		//�豸���ٺ����������豸δ��Ծ��ʱ�豸���������豸�ĳ�ʱ����

	void SetDivId(int divId);
	int GetDivId();

	void SetDivType(int divType);
	int GetDivType();

	void SetSock(int sock);
	int GetSock();

	void SetTimeOutMs(unsigned long timeOutMs);
	unsigned long GetTimeOutMs();


	int GetDivStatus(int status);
	bool IsRegister();

private:
	int _divId;					//�豸id
	int _divType;				//�豸����
	int _divStatus;				//�豸״̬
	int _sock;					//�豸socket
	bool _isRegister;			//�Ƿ���ע�����豸������
	RQLoadNode* _loadNode;		//����ڵ�Ĺ��ؽڵ�
	unsigned long _timeOutMs;	//��������/��Ծ��ʱʱ��
};