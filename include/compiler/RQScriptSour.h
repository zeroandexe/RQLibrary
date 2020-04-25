#pragma once
#include "RQMutex.h"
/*
	该对象为脚本表的专用存储对象，作为提供编译器并行功能的脚本对象存储部分
*/
class RQScriptFile;

class RQScriptSour
{
public:
	RQMutex _mutex;
	RQScriptFile* _scriptFile;
};