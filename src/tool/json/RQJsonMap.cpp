#include "RQJsonHead.h"


RQJsonMap::RQJsonMap()
{
	Create();
}

RQJsonMap::~RQJsonMap()
{
	Clear();
	delete _valMap;
}

void RQJsonMap::Create(void * param)
{
	RQJson::Create();
	_valMap = new std::map<std::string, RQJson*>();
}

void * RQJsonMap::Init(void * param)
{
	RQJson::Init();
	return nullptr;
}


void RQJsonMap::Clear()
{
	for (auto iter : *_valMap)
	{
		RecycleBin::RecycleNode(iter.second);
	}
	_valMap->clear();
	RQJson::Clear();
}

void RQJsonMap::SetVal(RQVar * key, RQJson * val)
{
	if (key
		&& val
		&& key->GetStr())
	{
		(*_valMap)[key->GetStr()] = val;
	}        
}

RQJson & RQJsonMap::operator[](const char * keyName)
{
	RQJson* json = _InvalidJson();
	if (keyName)
	{
		auto iter = _valMap->find(keyName);
		if (iter != _valMap->end())
		{
			json = iter->second;
		}
	}
	return *json;
}

int RQJsonMap::Count()
{
	int retInt = 0;
	if (_valMap)
	{
		retInt = _valMap->size();
	}
	return retInt;
}

RQJsonMap::operator bool()
{
	return _valMap;
}
