#pragma once
#include "RQMutex.h"
/*
	�ö���Ϊ�ű����ר�ô洢������Ϊ�ṩ���������й��ܵĽű�����洢����
*/
class RQScriptFile;

class RQScriptSour
{
public:
	RQMutex _mutex;
	RQScriptFile* _scriptFile;
};