#pragma once
#include <time.h>

class RQTime {
public:
	RQTime();
	//---获取当前时间
	void Now();
	//---获取微秒级别数值
	unsigned long long GetUSecVal();
	//---获取毫秒级别数值
	unsigned long long GetMSecVal();
	//---获取秒数级别数值
	unsigned long long GetSecVal();
	 
	const char* GetDateStr(const char* form= "%04d/%02d/%02d");
	const char* GetTimeStr(const char* form = "%02d:%02d:%02d");
	const char* GetDTStr(const char* dataForm = "%04d/%02d/%02d",
		const char* timeForm = "%02d:%02d:%02d");

private:
	int _WriteDate(const char* form,int offset = 0);
	int _WriteTime(const char* form,int offest = 0);

private:
	unsigned long long _sec;		//秒数值
	unsigned long long _usec;		//微妙数值
	tm _nowTime;					//当前时间
	static const int _bufSize = 128;		//缓冲区
	char _buff[_bufSize];			//字符输出
};
