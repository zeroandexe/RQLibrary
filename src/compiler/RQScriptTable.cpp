#include "compiler/RQScriptTable.h"
#include "compiler/RQScriptFile.h"
#include "RecycleBin.h"
#include "compiler/RQSentence.h"
#include "define/RQInterDefine.h"
#include "compiler/RQLexical.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include "compiler/RQGrammar.h"
#include "RQSys.h"
#include "RQPair.h"
#include "RQMutex.h"
#include "compiler/RQScriptSour.h"
#include "RQPtrVal.h"
#include "compiler/RQResolver.h"

//extern std::map<std::string, RQScriptSour>* _scriptMap;			//应用当前已创建的脚本变量表
RQMap<std::string, RQScriptSour> RQScriptTable::_scriptMap;

RQScriptSour * RQScriptTable::GetScriptFile(const char * path)
{
	RQScriptSour* sour = _GetScriptSour(path);		///获取脚本资源
	_UpdateScriptSour(sour, path);					///更新脚本资源
	if (!sour->_scriptFile)							///检测脚本是否无法加载
	{
		_ReleaseScriptSour(sour);					///该资源无法加载，进行释放
	}
	return sour;
}

void RQScriptTable::ReleaseScriptFile(RQScriptSour * sour)
{
	_ReleaseScriptSour(sour);
}

void RQScriptTable::RemoveScriptFile(const char * path)
{
	RQScriptSour* sour = &(_scriptMap)[std::string(path)];
	RemoveScriptFile(sour);
}

void RQScriptTable::RemoveScriptFile(RQScriptSour * sour)
{
	if (sour->_scriptFile)
	{
		RecycleBin::RecycleNode(sour->_scriptFile);			///回收节点对象
		sour->_scriptFile = 0;
	}
}

RQScriptFile * RQScriptTable::_LoadScriptFile(const char * path)
{
	RQScriptFile* newScript = 0;
	if (path)
	{
		newScript = RecycleBin::GetFreeRQScriptFile();		///索要一个存储可执行代码的脚本对象
		if (!RQResoler::AnalyzeFile(path,newScript))		///分析即将加载的脚本，并进行解析，然后查看是否解析失败
		{
			RecycleBin::RecycleNode(newScript);				///解析失败回收脚本对象
			newScript = 0;									///将脚本对象置空，对外表示解析失败，可能脚本有问题
		}
	}
	
	return newScript;			///返回解析结果
}

RQScriptSour * RQScriptTable::_GetScriptSour(const char * path)
{
	RQScriptSour* sour = &((_scriptMap)[path]);
	sour->_mutex.Lock();		///加上互斥锁，防止对脚本对象进行争抢

	return sour;
}

void RQScriptTable::_ReleaseScriptSour(RQScriptSour * sour)
{
	sour->_mutex.Unlock();
}

void RQScriptTable::_UpdateScriptSour(RQScriptSour * sour,const char* path)
{
	///从文件属性中获取当前文件的修改时间
	std::pair<unsigned long, unsigned long> pathTime = GetWritTimeByFilePath(path);
	///通过文件的修改时间进行计算，查看该文件是否存在
	int haveFile = (!!pathTime.first + !!pathTime.second);
	RQScriptFile* newScriptPtr = 0;
	RQScriptFile* oldScriptPtr = sour->_scriptFile;

	if (haveFile)		///为避免多余运算，若文件不存在则不予计算
	{
		newScriptPtr = oldScriptPtr;
		bool isNeedLoad = true;		///脚本重载标志
		if (sour->_scriptFile)		///检测脚本是否存在
		{
			///获取脚本文件
			std::pair<unsigned long, unsigned long> scriptTime = sour->_scriptFile->GetTime();

			isNeedLoad =
				!!(pathTime.first - scriptTime.first)		///检测时间高位是否相同
				+ !!(pathTime.second - scriptTime.second);	///检测时间低位是否相同，
		}

		if (isNeedLoad)		///检测当前脚本是否未进行加载
		{
			RecycleBin::RecycleNode(sour->_scriptFile);					///回收脚本资源
			newScriptPtr = _LoadScriptFile(path);						///加载新脚本
			if (newScriptPtr)
			{
				newScriptPtr->SetTime(pathTime.first, pathTime.second);	///更新脚本加载时间
			}
		}
	}
	///调整脚本对象指针，若该文件不存在则将其值为0，若存在则值不变
	sour->_scriptFile = newScriptPtr;
}
