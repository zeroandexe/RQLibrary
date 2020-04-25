#include "compiler/RQScriptFile.h"
#include "compiler/RQSentence.h"
#include "RecycleBin.h"
#include "compiler/RQLine.h"

RQScriptFile::RQScriptFile()
{
	Create();
}


RQScriptFile::~RQScriptFile()
{
	Clear();	
	///RecycleBin::RecycleNode(_codeSet);
	RecycleBin::RecycleNode((RQNode*)_exp);
	RecycleBin::RecycleTemStorage(_tsId);
	RecycleBin::RecycleTS(_tsId);
	RecycleBin::RecycleNode(_codeSet);
	delete _localmap;

}

void RQScriptFile::Create(void * param)
{
	_tsId = RecycleBin::CreateTS();					///每一个脚本对象都应拥有自己的暂存器
	_exp = RecycleBin::GetFreeRQVar();				///作为一个脚本执行后最终的返回值保存处
	_localmap = new RQMap<std::string,RQVar*>();	///用于容纳脚本中被命名的脚本变量
	_codeSet = RecycleBin::GetFreeRQCodeSet();		///一个脚本的代码，已被抽象为一个集合

	Clear();
}

void RQScriptFile::Clear()
{
	RecycleBin::RecycleTemStorage(_tsId);			///清除脚本对象中创建的脚本变量以及语句行和表达式等
	_codeSet->Clear();								///将代码集的存储和读取位置调整到第一行
	_ClearLocalMap();
}


void RQScriptFile::SetIndexInMap(RQScriptFile ** indexPtr)
{
	_indexInMap = indexPtr;
}


RQScriptFile ** RQScriptFile::GetIndexInMap()
{
	return _indexInMap;
}

void RQScriptFile::Reset()
{
	_codeSet->Clear();								///将代码集的存储和读取位置调整到第一行
}


void RQScriptFile::SetTime(unsigned long highTime, unsigned long lowTime)
{
	_time.first = highTime;
	_time.second = lowTime;
}

std::pair<unsigned long, unsigned long> RQScriptFile::GetTime()
{
	return _time;
}

RQVar * RQScriptFile::GetExp()
{
	return _exp;
}

void* RQScriptFile::GetTsId()
{
	return _tsId;
}

RQMap<std::string, RQVar*>* RQScriptFile::GetLocalMap()
{
	return _localmap;
}

void RQScriptFile::AddSentence(RQSentence * sentence)
{
	_codeSet->AddLine(sentence);
}

void RQScriptFile::ResetHeadLine()
{
	_codeSet->ResetHeadLine();
}

RQCodeSet * RQScriptFile::GetCodeSet()
{
	return _codeSet;
}

void RQScriptFile::_ClearLocalMap()
{
	///_localmap->clear();
	_localmap->Clear();
}
