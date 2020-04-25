#include "RQJsonHead.h"
#include "RecycleBin.h"



std::map<std::string, RQVar*> RQJsonKeyMap::_map;

void RQJsonKeyMap::InitObj()
{
	static bool needInit = true;
	if (needInit)
	{
		RQVar* var;
		///---设置：true
		var = RecycleBin::GetFreeRQVar();
		_map["true"] = var;
		var->SetInt(1);

		///---设置：false
		var = RecycleBin::GetFreeRQVar();
		_map["false"] = var;
		var->SetInt(0);
		///---采用同样设置：null
		_map["null"] = var;
	}
}

RQVar * RQJsonKeyMap::SearchKeyWord(const char * keyWord,int length)
{

	RQVar* retVar = 0;
	if (keyWord
		&& length > 0 )
	{
		std::string key(keyWord, keyWord + length);
		auto iter = _map.find(key);
		if (iter != _map.end())
		{
			retVar = iter->second;
		}
	}

	return retVar;
}
