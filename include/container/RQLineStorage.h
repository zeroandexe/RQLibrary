#pragma once
#include "RQIterator.h"
/*
	���������ݴ洢�ṹ�ĸ��࣬���ڶ�����Ĵ洢����ͳһ��
	��Ч�ڵ㣺����Ϊͷβ�ı�ǽڵ���������ڵ�
*/
class RQNode;				//�����ڵ�����
class RQLock;

class RQLineStorage
{
public:
	RQLineStorage();
	virtual ~RQLineStorage();

	//������β�˲���
	virtual void push_back(RQNode*);
	virtual RQNode* pop_back() ;

	//��������ǰ�˲���
	virtual void push_front(RQNode*);
	virtual RQNode* pop_front() ;
	
	//�������
	virtual void Clear();

	//��ȡ��ǰ��ЧԪ�ظ���
	virtual unsigned int GetCount() ;

	//�Ӵ洢�������Ƴ�Ŀ��node�����ǲ������node���ڴ�ռ�
	virtual void Remove(RQNode*);
	//virtual void remove(RQIterator*) = 0;

	//��������������Ч�ڵ��Ƴ������ǲ�����սڵ���ڴ�ռ�
	virtual void removeAll();

	//��ȡ�ڵ��е�������
	virtual RQLock* getLock();

	virtual RQIterator begin() = 0;
	virtual RQIterator end() = 0;

	//����������ǰ��ǰ/��ƫ��һ���ڵ㵥λ
	virtual void AddIteator(RQIterator* iterator) = 0;
	virtual void SubIterator(RQIterator* iterator) = 0;

	//������������ָ��Ľڵ㣬��ͬʱ����������ǰƫ��
	virtual void Erase(RQIterator* iterator) = 0;
protected:
	RQIteratorProxy* _proxy;
};