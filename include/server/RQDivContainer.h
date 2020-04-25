#pragma once
#include "RQMap.h"
#include "RQSpinLock.h"
class RQList;
/*
	�ö���Ϊ�豸�������Ĵ洢�������豸��Դ�ı���ͻ�ȡ�Ȳ�������Ϊ������������
*/
class RQDivNode;

class RQDivContainer 
{
private:
	struct Three {
		RQSpinLock	_lock;
		RQMap<int, RQDivNode*> _divIdMap;	//�豸idӳ���
		RQList*		_divFreeList;				//�豸�洢����
	};
private:
	friend class RQDivManager;
	/*
		���豸������������ʱ�������豸����
		�����豸Ϊ�ظ�id�豸���򷵻�false
	*/
	static bool Insert(RQDivNode* divNode);
	static void Remove(RQDivNode* divNode);
	static RQDivNode* GetDivByTypeId(int divType, int divId);
	static RQDivNode* GetDivByType(int divType);
	static void ChangeSock(RQDivNode* divNode, int socket);
	/*
		֪ͨ�洢�����豸ִ�����������׼����������豸����
	*/
	static void TaskOver(RQDivNode* divNode);
	/*
		֪ͨ�洢�������豸��Ҫִ������Ӧ���ӿ����豸�������뿪
	*/
	static void WillRunTask(RQDivNode* divNode);
private:
	static RQDivNode* _GetDivByTypeId(int divType, int divId);
	static RQDivNode* _GetDivById(Three& third, int divId);

	/*
		���豸�ڵ���뵽�豸���ͱ��У������豸id��ʹ���򽫷��ص�ǰʹ�ø�id���豸
	*/
	static RQDivNode* _InsertInTypeMap(RQDivNode*divNode);		
	static void _InsertInSockMap(RQDivNode* divNode);		//���豸�ڵ����յ�ǰsock�����豸����
	static void _RemoveFromTypeMap(RQDivNode* divNode);
	static void _RemoveFromSockMap(RQDivNode* divNode);
private:
	/*
	�����豸���������豸����Ϊ�������豸ʵ�ʴ洢���ݽṹΪֵ�ı�
*/
	static RQMap<int, Three> _typeDivMap;
	/*
		socket-�豸������socketΪ�����豸�ڵ�Ϊֵ������socket����ʹ�õı�
	*/
	static RQMap<int, RQDivNode*> _sockDivMap;
};