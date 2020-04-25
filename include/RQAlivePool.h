#pragma once

/*
	���ݻ�Ծ��,�����ṩ��Ծ������
	�ö���Ϊ��̬��Ծ�أ����߱���̨�̶߳����ݵļ���
	�������û����û�ȡ��Ծ�ӿ�ʱ�����ݽ�����Чʱ�޼��

	Ҫ���û��ڲ���Ҫʹ��ע����ʱ�Ծ������ע��
*/

struct RQActPoolElement;		//��Ծ��Ԫ��
class RQSpinLock;
class RQStack;					
class RQAlivePool {
public:
	//���û�Ծ�ش�С
	RQAlivePool(unsigned int poolSize);
	~RQAlivePool();
	/*
		���������һ�������������ȡ�ò����ĳؾ��
		���룺
			hPool������_param����Ӧ�ĳؾ����
					��hPoolΪ0�����������_param��������_paramƥ��ľ��ֵ
					��hPool��0�����hPool��ָ���_param�Ļ�Ծʱ��
			_param�������Ծ�ε����ݳص�����
			timeOut�����ݵ�ʧЧʱ��
		�����
			return��������_param�򷵻سؾ��ֵ
					���������򷵻�_param��Ӧ�ĳؾ��
	*/

	int AddParam(int hPool, void* param, long timeOutSec);
	//�ӳ��л�ȡһ����ǰ�ڵ�_param,����_param�ӻ�Ծ�����Ƴ�
	void* GetAlive();
	//����ָ���ؾ������Դ
	void Remove(int hPool);

protected:
	void _ActParam(int hPool, void* param, long timeOut);

private:
	unsigned int _poolSize;				//��Ծ�ش�С����
	unsigned int _checkPos;				//��ǰ�Ĳ���λ��
	RQActPoolElement* _pool;			//��Ծ��
	RQSpinLock* _lock;					//������
	RQStack* _stackSaveIndex;			//���ݴ���±�洢ջ
	RQStack* _stackAlivePool;			//��Ծ�ش洢ջ
};

