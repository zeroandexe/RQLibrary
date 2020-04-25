#pragma once
//#include <map>
#include <string>
#include "RQMap.h"

/*
	�ö����ǽű�����Ĵ洢��ͼ����࣬�ṩ�˽ű�����ļ�ֵ��Ӧ�洢��
	���ڶ����У������ļ�·������Ϊ�����ű��ļ�����ָ����Ϊֵ���м���

	ͬʱҲ�ṩ�ڽű����滻���߸��º�Ծɽű�����Դ�����Լ����½ű����ر���
*/
class RQScriptFile;
class RQScriptSour;

class RQScriptTable
{
public:
	//�����ļ�·����ȡ�ڱ��еĽű��ļ�����
	//���ļ����󲻴��ڣ���Ӵ����·���н��м���
	//���ļ��ѱ����£���������µĽű��ļ��������ļ����н����ʹ洢
	static RQScriptSour* GetScriptFile(const char* path);

	//�ͷŻ�ȡ���Ľű�����
	static void ReleaseScriptFile(RQScriptSour* scriptFile);

	//���մ���Ľű����󣬲����ݽű������е�����λ�����ű�����ӱ����Ƴ�
	static void RemoveScriptFile(RQScriptSour* sour);
private:
	//���ݴ�����½ű�·�������ؽű�����
	static RQScriptFile * _LoadScriptFile(const char* path);
	//���ݴ���Ľű�·������ȡ��Ӧ�Ĵ洢��Դ�������󲻴����򷵻ؿգ�
	static RQScriptSour * _GetScriptSour(const char* path);
	//�ͷŽű���Դ
	static void _ReleaseScriptSour(RQScriptSour* sour);
	//���µ�ǰ�Ľű���Դ
	static void _UpdateScriptSour(RQScriptSour* sour,const char* path);

private:
	static RQMap<std::string, RQScriptSour> _scriptMap;
private:	//��ʱ���ṩ�ĺ���

	//���ݴ���Ľű�·���ڱ��м����������ոýű�����
	static void RemoveScriptFile(const char* path);
};