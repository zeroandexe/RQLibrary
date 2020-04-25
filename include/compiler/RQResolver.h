#pragma once
/*
	�ö���Ϊ�����������ɴʷ������﷨��������϶��ɣ�
	��Ҫ�����ṩ�ַ������뵽��ִ�д����ת��
*/

class RQSentence;
class RQScriptFile;
class RQGrammar;

class RQResoler {
public:
	/*
		����������ַ���������н��������������ע���С���ָ�������﷨�������½���ʧ��
		path		[in]	�ű��ļ�·��
		scriptFile	[in]	����������Ϣ����
		return		[out]	���ν����Ƿ�ɹ�
	*/
	static bool AnalyzeFile(const char* path, RQScriptFile* scriptFile);

	/*
		����ͬ�ϣ����ǽ�����һ�����
		codeStr		[in]		�������������
		scriptFile	[in-out]	�ṩ����������еĴ洢
		return		[out]		���˽����Ƿ�ɹ�
	*/
	static bool AnalyzeSentence(const char* codeStr, RQScriptFile* scriptFile);
};