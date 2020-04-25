#pragma once
#include "RQJson.h"
#include <map>
#include "RQVar.h"
/*
	用于解析
	用于解析文法中的：json
*/

class RQJsonMap : public RQJson
{
public:
	RQJsonMap();
	~RQJsonMap();
	virtual void Create(void* param = 0);
	virtual void* Init(void* param = 0);
	virtual void Clear();
	void SetVal(RQVar* key,RQJson* val);

	virtual RQJson& operator[](const char* keyName);
	virtual int Count();
	virtual operator bool();
private:
	std::map<std::string, RQJson*>* _valMap;
};

