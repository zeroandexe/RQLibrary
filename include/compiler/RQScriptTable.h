#pragma once
//#include <map>
#include <string>
#include "RQMap.h"

/*
	该对象是脚本对象的存储类和检索类，提供了脚本对象的键值对应存储。
	即在对象中，采用文件路径名作为键，脚本文件对象指针作为值进行检索

	同时也提供在脚本被替换或者更新后对旧脚本的资源回收以及对新脚本的重编译
*/
class RQScriptFile;
class RQScriptSour;

class RQScriptTable
{
public:
	//根据文件路径获取在表中的脚本文件对象，
	//若文件对象不存在，则从传入的路径中进行加载
	//若文件已被更新，则加载最新的脚本文件，并对文件进行解析和存储
	static RQScriptSour* GetScriptFile(const char* path);

	//释放获取到的脚本对象
	static void ReleaseScriptFile(RQScriptSour* scriptFile);

	//回收传入的脚本对象，并根据脚本对象中的索引位，将脚本对象从表中移除
	static void RemoveScriptFile(RQScriptSour* sour);
private:
	//根据传入的新脚本路径，加载脚本对象
	static RQScriptFile * _LoadScriptFile(const char* path);
	//根据传入的脚本路径，获取对应的存储资源（若对象不存在则返回空）
	static RQScriptSour * _GetScriptSour(const char* path);
	//释放脚本资源
	static void _ReleaseScriptSour(RQScriptSour* sour);
	//更新当前的脚本资源
	static void _UpdateScriptSour(RQScriptSour* sour,const char* path);

private:
	static RQMap<std::string, RQScriptSour> _scriptMap;
private:	//暂时不提供的函数

	//根据传入的脚本路径在表中检索，并回收该脚本对象
	static void RemoveScriptFile(const char* path);
};