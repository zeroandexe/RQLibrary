#pragma once
#include "RQJson.h"
#include "RQList.h"
#include "node/RQNullNode.h"
/*
	解析json中的数组
	用于解析文法中的：array_val
*/

class RQJsonArray : public RQJson
{
public:
	RQJsonArray();
	~RQJsonArray();

	virtual void Create(void* param = nullptr);						//数据节点创建
	virtual void Clear();												//提供对节点数据的清除操作
	virtual void* Init(void* param = 0);
	void push(RQJson* json);
	virtual RQJson& operator[](int index);
	virtual int Count();
	virtual operator bool();
	virtual void Refurbish();			//刷新数组的索引
private:
	RQList* _saveList;
	RQNullNode* _indexAry;			//遍历数组
};

