#pragma once
#include <fstream>
#include <iostream>
//该类是一个日志类,用于系统日志记录

class RQLog
{
public:
	RQLog(const char* prefix= "", unsigned int logSize = 1024, const char* root = ".");
	~RQLog();

	bool OpenLog( const char*  prefix = "", const char* root = "." );

	/*
		重置日志缓冲区的大小
	*/
	void Resize(unsigned int logSize);

	//日志定向输出
	template<typename T>
	std::ofstream& operator << (T& param);

	//开始记录函数入口(非线程安全)
	std::ofstream& Log();

	//继续上一行进行记录(非线程安全)
	std::ofstream& CLog();
	
	//同时记录传入的字符串和时间标签，用户多线程并行日志记录
	//msg组装好的记录信息
	void Log(const char* msg,const char endCh = '\n');

private:
	bool _needShow;
	std::ofstream _logFile;			///日志文件输出流
	int _bufSize;					///缓冲区大小
	char* _buff;					///缓冲区
};

template<typename T>
inline std::ofstream & RQLog::operator<<(T& param)
{
	_logFile << param;
	_logFile.flush();
	return _logFile;
}
