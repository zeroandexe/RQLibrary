#include "RQSys.h"
#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib,"ws2_32.lib")

std::pair<unsigned long, unsigned long> GetWritTimeByFilePath(const char * path)
{

	WIN32_FIND_DATAA fileTime;
	HANDLE hFind = FindFirstFileA(path, &fileTime);
	std::pair<unsigned long, unsigned long> time(0, 0);
	if (-1 != (int)hFind)
	{
		time.first = fileTime.ftLastWriteTime.dwHighDateTime;
		time.second = fileTime.ftLastWriteTime.dwLowDateTime;
		FindClose(hFind);
	}
	return time;
}

void RQSleepSec(int sec)
{
	Sleep(sec * 1000);
}

void RQSleepMs(int msec)
{
	Sleep(msec);
}

/*
	-------------------------------网络数据转换
*/
//本地转网络
short RQHton(short data)
{
	return (short) htons((unsigned short)data);
}

int RQHton(int data)
{
	return (int)htonf(*(float*)&data);
}

long RQHton(long data)
{
	return (long ) htonl((unsigned long) data);
}

long long RQHton(long long data)
{
	return (long long) htonll( (unsigned long long )data);
}

float RQHton(float data)
{
	unsigned int result = htonf(data);
	return *(float*)&result;
}

double RQHton(double data)
{
	unsigned long long result = htonll(*(unsigned long long *)&data);
	return *(double*)&result;
}

void * RQHton(void * data)
{
	unsigned long result = htonl(*(unsigned long*)&data);
	return *(void**)&result;
}

//网络转本地
short RQNtoh(short data)
{
	return (short) ntohs((unsigned short)data);
}

int RQNtoh(int data)
{
	float result = ntohf((unsigned int)data);
	return *(int*)&result;
}

long RQNtoh(long data)
{
	return (long) ntohl((unsigned long)data);
}

long long RQNtoh(long long data)
{
	return (long long)ntohll((unsigned long long)data);
}

float RQNtoh(float data)
{
	return ntohf(*(unsigned int*)&data);
}

double RQNtoh(double data)
{
	unsigned long long result = ntohll(*(unsigned long long *)&data);
	return *(double *)&result;
}

void * RQNtoh(void * data)
{
	unsigned long result = ntohl(*(unsigned long*)&data);
	return *(void**)&result;
}
