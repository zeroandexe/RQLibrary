#pragma once
#include "RQMap.h"
#include "RQSpinLock.h"
class RQTaskNode;
class RQList;

/*
	�ö���Ϊ���������������洢ģ�飬��������Ĵ洢��Ϊ��������

	�洢�������룬Ҫ��������ѽ������
*/

class RQTaskContainer
{
private:
	friend class RQTaskManager;

	typedef RQMap<int, RQTaskNode*> IdTaskMap_Type;					//idӳ���

	class Three {
	public:
		IdTaskMap_Type _idTaskMap;		//�豸id-����ڵ�ӳ���
		RQList* _taskList;				//�����͵��豸�洢��
		RQSpinLock _lock;
	};

	typedef Three IdTaskSave_Type;									//������
	typedef RQMap<int, IdTaskSave_Type> DivTaskMap_Type;			//�豸��
	typedef RQMap<int, std::pair<RQSpinLock, RQList*>>	TypeTaskMap_Type;	//�������ͱ�
private:
	static void Insert(RQTaskNode* taskNode);			//���������洢��
	static void Remove(RQTaskNode* taskNode);			//������ӹ��������Ƴ�
	static RQTaskNode* GetTaskByTaskType(int taskType);	//���ݴ�����������ͻ�ȡ���񣨸����񽫱��Ƴ���
	static RQTaskNode* GetTaskByDivTypeId(int divType, int divId);	//���ݴ�����豸���ͺ�id��ȡƥ������һ����ȡ�����񽫱��Ƴ���
	static RQTaskNode* GetTaskByDivType(int divType);			//���ݴ�����豸���ͣ���ȡƥ�����͵�����
private:
	static void _InsertDivTaskMap(RQTaskNode* taskNode);		//����������豸��
	static void _InsertTaskTypeMap(RQTaskNode* taskNode);		//����������������ͱ�
	static void _RemoveDivTaskMap(RQTaskNode* taskNode);		//��������豸�����Ƴ�
	static void _RemoveTaskTypeMap(RQTaskNode* taskNode);		//���豸���������ͱ����Ƴ�
private:
	static DivTaskMap_Type _divTaskMap;							//�豸��ϢΪ�����������
	static TypeTaskMap_Type _typeTaskMap;						//��������Ϊ�����������
};