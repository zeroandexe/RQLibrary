#pragma once
#include "node/RQNode.h"
/*
	�ö���Ϊ���������������ṩRQ�������ı�������
*/

class RQIteratorProxy;
class RQNode;

class RQIterator {
public:
	/*
		�޹����ĵ������������κε�����
	*/
	RQIterator();
	/*
		ָ���������Ĺ�������ͬʱָ���õ�������λ��
	*/
	RQIterator(RQIteratorProxy* proxy,RQNode* node);
	/*
		���õ������й����򣬴Ӵ�����ע���õ�����
	*/
	~RQIterator();

	RQIterator(const RQIterator& other);
	//RQIterator(RQIteratorProxy&& proxy);


	virtual void* Init(void* param = 0);
	virtual void Clear();

	void operator =(const RQIterator& other);
	void operator = (RQIterator&& other);
	bool operator == (const RQIterator& other);
	bool operator == (RQIterator&& other);
	bool operator != (const RQIterator& other);
	bool operator != (RQIterator&& other);

	bool isNull();

	/*
		������λ�Ƶĳ�����������
		������ɢ�͵��������ɽڵ����ڲ�������ָ�����
		���������Ե���������ڴ��ַ����ָ�볤��ƫ��
	*/
	void operator ++();
	void operator --();
	virtual RQNode*& operator *();

	/*
		������������֤�����������Ƿ����ڶ�Ӧ�Ĺ�������
	*/
	bool IsWon(RQIteratorProxy* proxy);
protected:
	RQNode* _node;				//����RQ����Ϊ�ڵ�Ϊ���ģ����Խ�������������Ϊ�ڵ����
	RQIteratorProxy* _proxy;			//�õ����������ڵ���������
};