#include "compiler/RQWord.h"
#include "define/RQInterDefine.h"

RQWord::RQWord()
{
	
}


RQWord::~RQWord()
{
}

void RQWord::Create(void* param)
{
	Clear();
}

void RQWord::Clear()
{
	_len = 0;
	_type = M_WORD_TYPE_END;				//当前正在解析终止符（\r，\n，//或其他非法字符）E_INVALID;
	_buff[0] = 0;
}