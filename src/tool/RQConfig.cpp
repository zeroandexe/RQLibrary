#include "RQConfig.h"
#include "RecycleBin.h"
#include "RQCompiler.h"
#include "compiler/RQScriptTable.h"
#include "compiler/RQScriptSour.h"
#include "compiler/RQScriptFile.h"

//
//int RQLoadConfig(const char * filePath, std::map<std::string,RQVar*>* map, const int bufSize, char ignoerlLabel )
//{
//	std::ifstream file(filePath);
//	if (!file.is_open())
//	{
//		return -1;
//	}
//
//	RQMemory* readMemory = RecycleBin::GetFreeRQMemory();
//	RQMemory* nameMemory = RecycleBin::GetFreeRQMemory();
//	RQMemory* valMemory = RecycleBin::GetFreeRQMemory();
//	readMemory->Resize(bufSize);
//	nameMemory->Resize(bufSize);
//	valMemory->Resize(bufSize);
//
//	char* readBuf = (char*)readMemory->GetAddr();		//�����ļ��ַ�������
//	char* nameBuf = (char*)nameMemory->GetAddr();		//��������Э������������
//	char* valBuf = (char*)valMemory->GetAddr();			//����ֵ������
//
//	file.getline(readBuf, bufSize);
//	while (!file.eof()|| readBuf[0])
//	{
//		nameBuf[0] = valBuf[0] = 0;
// 		sscanf(readBuf, "%s = %s", nameBuf, valBuf);
//
//		int isOk = nameBuf[0]					//����Ƿ�����������
//			* (ignoerlLabel - nameBuf[0]);		//������������Ƿ���ע����
//
//		if (isOk)								//����Э���Ƿ���ж�Ӧ�����нű�
//		{
//			RQVar* val = new RQVar;
//			val->SetStr(valBuf);				//����ǰֵ����ű�����
//			(*map)[nameBuf] = val;				//���ű���Э����Ϊ���ű�����Ϊֵ����
//		}
//		readBuf[0] = 0;
//		file.getline(readBuf, bufSize);
//	}
//	RecycleBin::RecycleNode(readMemory);
//	RecycleBin::RecycleNode(nameMemory);
//	RecycleBin::RecycleNode(valMemory);
//
//	return map->size();
//}

RQConfig::RQConfig()
{
	_configScript = 0;
	_configMap = 0;
}

RQConfig::~RQConfig()
{
	_configMap = 0;
	RecycleBin::RecycleNode((RQNode*)_configScript);
}

int RQConfig::LoadConfig(const char * configPath)
{
	RQCompiler compiler;
	int retInt = compiler.RunScript(configPath);		///���벢�������ýű�
 
	if (!retInt)			///��������ļ������Ƿ���Ч
	{
		RQScriptSour* configScriptSour = RQScriptTable::GetScriptFile(configPath);	///��ȡ���к�����ýű���Դ
		_configScript = configScriptSour->_scriptFile;				///�ӽű�������Դ��ȡ���ű�����
		configScriptSour->_scriptFile = 0;							///���ű���Դ�ÿգ���ʾ���ýű������Ѵӽű����н���
		RQScriptTable::ReleaseScriptFile(configScriptSour);			///�ͷŽű���Դ

		_configMap = _configScript->GetLocalMap();					///��ȡ�����úõ����ñ�
	}
	return retInt;
}

RQVar * RQConfig::operator[](const char * key)
{
	RQVar* retVar = 0;
	if (_configMap)
	{
		retVar = (*_configMap)[key];							///�����ñ��л�ȡ�����Ľű�����
	}
	return retVar;
}
