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
//	char* readBuf = (char*)readMemory->GetAddr();		//配置文件字符缓冲区
//	char* nameBuf = (char*)nameMemory->GetAddr();		//变量名（协议名）缓冲区
//	char* valBuf = (char*)valMemory->GetAddr();			//变量值缓冲区
//
//	file.getline(readBuf, bufSize);
//	while (!file.eof()|| readBuf[0])
//	{
//		nameBuf[0] = valBuf[0] = 0;
// 		sscanf(readBuf, "%s = %s", nameBuf, valBuf);
//
//		int isOk = nameBuf[0]					//检查是否有数据输入
//			* (ignoerlLabel - nameBuf[0]);		//检查输入数据是否不是注释行
//
//		if (isOk)								//检测该协议是否具有对应的运行脚本
//		{
//			RQVar* val = new RQVar;
//			val->SetStr(valBuf);				//将当前值存入脚本变量
//			(*map)[nameBuf] = val;				//将脚本以协议作为键脚本名作为值存入
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
	int retInt = compiler.RunScript(configPath);		///编译并运行配置脚本
 
	if (!retInt)			///检查配置文件运行是否有效
	{
		RQScriptSour* configScriptSour = RQScriptTable::GetScriptFile(configPath);	///获取运行后的配置脚本资源
		_configScript = configScriptSour->_scriptFile;				///从脚本对象资源中取出脚本对象
		configScriptSour->_scriptFile = 0;							///将脚本资源置空，表示配置脚本对象已从脚本表中劫掠
		RQScriptTable::ReleaseScriptFile(configScriptSour);			///释放脚本资源

		_configMap = _configScript->GetLocalMap();					///获取已配置好的配置表
	}
	return retInt;
}

RQVar * RQConfig::operator[](const char * key)
{
	RQVar* retVar = 0;
	if (_configMap)
	{
		retVar = (*_configMap)[key];							///从配置表中获取期望的脚本变量
	}
	return retVar;
}
