#pragma once
#include <map>
#include "RQVar.h"
#include "RQMap.h"
//class RQVar;
///*
//	加载配置文件并进行配置
//	filePath	[in]	配置文件地址
//	map			[out]	配置键值对数据
//	bufSize		[in]	配置过程中用于装载文本行的缓冲区大小,默认为512
//	ignoerlLabel	[in]	配置文件中用于做注释行的第一个字节，默认为'#'
//	文件配置形式为：
//		name = val
//*/
//int RQLoadConfig(const char * filePath, std::map<std::string, RQVar*>* map, const int bufSize = 512, char ignoerlLabel = '#');

class RQScriptFile;
class RQVar;

class RQConfig
{
public:
	RQConfig();
	~RQConfig();
	/*
		传入配置文件路径，加载配置文件
		configPath	[in]	配置文件路径
		return		[out]	文件加载是否成功（0表示成功）
	*/
	int LoadConfig(const char* configPath);

	/*
		根据传入的关键字获取对应的动态变量
		key		[in]	配置文件中的左值key
		return	[out]	若存在则返回有效动态变量地址，否则返回空
	*/
	RQVar* operator[](const char* key);
private:
	RQScriptFile* _configScript;			//配置脚本对象
	RQMap<std::string, RQVar*>* _configMap;
};