#pragma once
#include <map>
#include "RQVar.h"
/*
	Json解析中的特定关键字表
*/

class RQJsonKeyMap
{
public:

	static void InitObj();
	/*
		传入具有C结尾的关键字，并根据关键字从表中获取关键字所对应的动态变量
		return	[out]	若表中有该关键字，则返回关键字所对应的动态变量副本
					若否则返回空
	*/
	static RQVar* SearchKeyWord(const char* keyWord, int length);

private:
	static std::map<std::string, RQVar*> _map;
};

