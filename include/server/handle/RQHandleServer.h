#pragma once

/*
	������Ϊ���������ṩ�������ĸ��࣬�ṩ���������ӿ�
*/
class RQHandle;
class RQHandleServer {
public:
	virtual void DestrayHandle(RQHandle* handle) = 0;	//���������յ�ͨ�ýӿ� 
};