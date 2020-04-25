#pragma once
#include "define/RQInterDefine.h"
#include "node/RQNode.h"
/*
	单词节点，用于保存语法分析出的元词
*/

class RQWord :public RQNode
{
public:
	RQWord();
	~RQWord();
	virtual void Create(void* param = 0);
	virtual void Clear();

	unsigned int _len;								//单词长度
	unsigned int _type;								//单词类型，名称、符号、还是数字、字符串等
	char _buff[MAX_STR_LEN_PER_WORD];				//每个单词的容量
};