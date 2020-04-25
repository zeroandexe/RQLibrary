#include "RQTime.h"
#include "RQSys.h"
#include <stdio.h>

RQTime::RQTime()
{
	Now();
}

void RQTime::Now()
{
	LoadNowTimeVal(_sec, _usec);			///加载当前的秒数以及毫秒数
	LoadNowTime(&_nowTime);
}

unsigned long long RQTime::GetUSecVal()
{
	return _sec * 1000000LL + _usec;
}

unsigned long long RQTime::GetMSecVal()
{
	return _sec * 1000LL + _usec / 1000LL;
}

unsigned long long RQTime::GetSecVal()
{
	return _sec;
}

const char * RQTime::GetDateStr(const char* form)
{
	if (!_WriteDate(form))
	{
		_buff[0] = 0;
	}
	return _buff;
}

const char * RQTime::GetTimeStr(const char* form)
{
	if (!_WriteTime(form))
	{
		_buff[0] = 0;
	}
	return _buff;
}

const char * RQTime::GetDTStr(const char * dataForm, const char * timeForm)
{
	int writeBytes = _WriteDate(dataForm);
	bool isWriteOk = false;
	if (writeBytes)
	{
		if (_WriteTime(timeForm, writeBytes))
		{
			isWriteOk = true;
		}
	}

	if (!isWriteOk)
	{
		_buff[0] = 0;
	}

	return _buff;
}

int RQTime::_WriteDate(const char * form,int offset)
{
	return sprintf(_buff+offset, form,
		_nowTime.tm_year + 1900,
		_nowTime.tm_mon + 1,
		_nowTime.tm_mday);
}

int RQTime::_WriteTime(const char * form,int offset)
{
	_buff[offset++] = ' ';
	return sprintf(_buff+offset, form,
		_nowTime.tm_hour,
		_nowTime.tm_min,
		_nowTime.tm_sec);
}
