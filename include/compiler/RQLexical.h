#pragma once
#include <map>
/*
	������һ���ʷ��������������ǽ��ⲿ������ַ����ֽ�ΪԪ�ʼ��������ⷵ��
*/

class RQElementManager;
class RQWord;

class RQSentence;
class RQLexical
{
public:
	static void InitObj();			//��ʼ�������
	/*
		���������C���룬�����Դ��
		globalBuff��C���Դ����ַ���
		wordArray������������ʶ�𵽵Ĵ�Դ��
		tsId���ݴ��������ڴ�Դ�ķ���ͻ���
		wordCount	[out]	���ν�����ȡ����Ԫ������
		return�������������ԣ��������ţ�
	*/
	static unsigned int Analysz(const char* buff,RQWord** wordArray,void* junkTsId);

};