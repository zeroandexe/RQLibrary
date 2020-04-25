#pragma once
#include "node/RQNode.h"
#include <string>
/*
	�ýڵ��Ƕ�����ĳ�����Ϊ��������ڵ�Ļ���
*/
class RQLoadNode;
class RQDivNode;
class RQTaskNode : public RQNode
{
public:
	friend class RQTaskManager;
	friend class RQTaskContainer;

	virtual void Create(void* param = 0);
	virtual void Clear();

	//����Ϊ�豸�ڵ�ָ��
	virtual void Execute(RQDivNode* param) = 0;	//��ΪC������ִ�����õĶ���������
	virtual void Destroy(RQDivNode* param) = 0;	//��ΪC������ִ������������Ϊ���ú���Ӧ��������д��

	void SetOpterId(int opterId);
	int GetOpterId();

	void SetOpterType(int opterType);
	int GetOpterType();

	void SetTaskType(int taskType);
	int GetTaskType();

	void SetTimeOutMs(int timeOutMs);
	int GetTimeOutMs();

	void SetExeScriptName(const char* scriptName);
	void SetExeScriptName(const std::string& scriptName);
	const std::string& GetExeScriptName();

	void SetDestroyScriptName(const char* scriptName);
	void SetDestroyScriptName(const std::string& scriptName);
	const std::string& GetDestroyScriptName();

	bool IsRegister();

private:
	int _opterId;							//ָ���ض�ִ����id
	int _opterType;							//��ָ��ִ���ߣ��������ض�ĳ��ִ����
	///int _taskId;							//����id�����ʶ��
	int _taskType;							//����������͵�����
	unsigned long _timeOutMs;				//����ʧЧ�ĳ�ʱʱ��
	std::string _exescriptName;				//��Ϊ�ű�ִ�У���Ϊִ�нű�������
	std::string _destroyScriptName;			//��Ϊ�ű�ִ�У���Ϊִ����������Ľű�����
	/*
		������ע���Ǳ�����������ע����ô�ñ�ǩ��������Ϊtrue,
		��ʱ�������������������ڵ���Ϣ���޸Ķ���ʧ��
	*/
	bool _isRegister;			
	RQLoadNode* _loadNode;					//���ظ�����ڵ�Ĺ��ؽڵ�
};