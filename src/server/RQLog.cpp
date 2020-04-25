#include "RQLog.h"
#include "RQTime.h"


RQLog::RQLog(const char *  prefix, unsigned int logSize, const char * root)
{
	_bufSize = logSize;
	_buff = new char[logSize];
	OpenLog(prefix, root);
}

RQLog::~RQLog()
{
	_logFile.close();
	delete _buff;
}

bool RQLog::OpenLog(const char * prefix, const char * root )
{
	if (_logFile.is_open())
	{
		_logFile.close();
	}
	RQTime time;

	///g构建文件名称
	snprintf(_buff, _bufSize,"%s/%s%s.log", root,prefix, time.GetDateStr("%04d-%02d-%02d"));
	///采用追加和输出的方式打开/创建日志文件
    _logFile.open(_buff, std::ios_base::out | std::ios_base::app);
	return _logFile.is_open();
}

void RQLog::Resize(unsigned int logSize)
{
	if (logSize > _bufSize)
	{
		delete _buff;
		_buff = new char[logSize];
		_bufSize = logSize;
	}
}

std::ofstream & RQLog::Log()
{
	_logFile.flush();
	RQTime time;

	snprintf(_buff, _bufSize,"[%s]: ", time.GetDTStr());
	_logFile << _buff;
	return _logFile;
}

std::ofstream & RQLog::CLog()
{
	_logFile.flush();
	return _logFile;
}

void RQLog::Log(const char * msg, const char endCh)
{
	RQTime time;
	snprintf(_buff, _bufSize,"[%s]: %s%c", time.GetDTStr(),msg,endCh);

	_logFile << _buff;
	_logFile.flush();
}
