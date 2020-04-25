#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "RQSys.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include "RQPtrVal.h"
#include "RQStrTool.h"

#include <dlfcn.h>			///加载动态库


std::pair<unsigned long, unsigned long> GetWritTimeByFilePath(const char * path)
{
	std::pair<unsigned long, unsigned long> retTime;
		struct stat buf;
	if (!stat(path, &buf))
	{
		retTime.first = buf.st_mtime;
		retTime.second = buf.st_mtime;
	}
	return retTime;
}


//睡眠，以秒为单位
void RQSleepSec(int sec)
{
	sleep(sec);
}

void RQSleepMs(int msec)
{
	usleep(msec * 1000);
}

short RQHton(short data)
{
	return htons(data);
}
unsigned short RQHton(unsigned short data)
{
	return RQHton((short)data);
}
unsigned int RQHton(unsigned int data)
{
	return RQHton((int)data);
}
int	RQHton(int data)
{
	return htonl(data);
}
long RQHton(long data)
{
	if (sizeof(long) - 4)
	{
		return RQHton((long long)data);
	}
	else {
		return htonl(data);
	}
}
unsigned long RQHton(unsigned long data)
{
	return RQHton((long)data);
}
long long RQHton(long long data)
{
	int oldLow = data & 0xffffffff;
	int low = ntohl(oldLow);
	if (oldLow != low)				///查看该机器是否是小端
	{
		int hight = ntohl(data >> 32);
		data = (unsigned int)low;
		data <<= 32;
		data |= (unsigned int)hight;
	}
	return data;
}
unsigned long long RQHton(unsigned long long data)
{
	return RQHton((long long) data);
}
float	RQHton(float data)
{
	int ret = htonl(*(int*)&data);
	return *(float*)&ret;
}
double	RQHton(double data)
{
	long long ret = RQHton(*(long long*)&data);
	return *(double*)&ret;
}
void*	RQHton(void* data)
{
	RQPtrVal ptr(data);
	ptr._long = RQHton(ptr._long);
	return ptr._ptr;
}

/*
	将网络数据转换为本地数据
*/
short RQNtoh(short data)
{
	return ntohs(data);
}
unsigned short RQNtoh(unsigned short data)
{
	return RQNtoh((short)data);
}
int	RQNtoh(int data)
{
	return ntohl(data);
}
unsigned int RQNtoh(unsigned int data)
{
	return RQNtoh((int)data);
}
long RQNtoh(long data)
{
	if (sizeof(long) - 4)
	{
		return RQNtoh((long long )data);
	}
	else {
		return ntohl(data);
	}
}
unsigned long RQNtoh(unsigned long data)
{
	return RQNtoh((long) data);
}
long long RQNtoh(long long data)
{
	int oldLow = data & 0xffffffff;
	int low = ntohl(oldLow);
	if (oldLow != low)				///查看该机器是否是小端
	{
		int hight = ntohl(data >> 32);
		data = (unsigned long)low;
		data <<= 32;
		data |= (unsigned int)hight;
	}
	return data;
}
unsigned long long RQNtoh(unsigned long long data)
{
	return RQNtoh((long long)data);
}
float	RQNtoh(float data)
{
	int ret = ntohl(*(int*)&data);
	return *(float*)&ret;
}
double RQNtoh(double data)
{
	long long ret = RQNtoh(*(long long *)&data);
	return *(double*)&ret;
}
void*	RQNtoh(void* data)
{
	RQPtrVal ptrVal(data);
	ptrVal._long = RQNtoh(ptrVal._long);
	return ptrVal._ptr;
}

int Inet_addr(const char * ipStr)
{
	return ipStr ? inet_addr(ipStr) : 0;
}

const char * Inet_addr(int ipVal, char * buff, int bufLen)
{
	const char* ipStr = inet_ntoa(*(in_addr*)&ipVal);
	int writeBytes = RQCopy(buff, ipStr, RQStrlen(ipStr));
	buff[writeBytes] = 0;
	return buff;
}

int GetCpuCoreNum()
{
	return get_nprocs_conf();
}

void LoadNowTime(tm* nowTm)
{
	time_t tmt = time(nullptr);
	tm* t = localtime(&tmt);
	if (t)
	{
		RQCopy(nowTm, t, sizeof(tm));
	}
}

void LoadNowTimeVal(unsigned long long & sec, unsigned long long & usec)
{
	timeval tmval;
	sec = 0;
	usec = 0;
	int retErr = gettimeofday(&tmval, NULL);
	if (!retErr)
	{
		sec = tmval.tv_sec;
		usec = tmval.tv_usec;
	}
}

void LoadNowTimeSec(unsigned long long & sec)
{
	unsigned long long usec;
	LoadNowTimeVal(sec, usec);
}

void LoadNowTimeMS(unsigned long long & msec)
{
	unsigned long long usec;
	LoadNowTimeVal(msec, usec);
	msec = msec * 1000 + usec / 1000;
}


int GetSockStatus(int sock)
{
	int optval;
	socklen_t optlen = sizeof(int);
	getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&optval, &optlen);
	return optval;
}

void * LoadDll(const char * dllPath, int mode)
{
	mode = mode ? mode : RTLD_NOW;
	return dlopen(dllPath, mode);
}

void UnloadDll(void * dll)
{
	if (dll)
	{
		dlclose(dll);
	}
}

void * LoadFunFromDll(void * dll, const char* funName)
{
	void* funAddr = 0;
	if (dll
		&& funName)
	{
		funAddr = dlsym(dll, funName);
	}
	return funAddr;
}

unsigned long long CpuRdtsc()
{
	unsigned int lo, hi;
	__asm__ __volatile__
	(
		"rdtsc":"=a"(lo), "=d"(hi)
	);
	return (unsigned long long)hi << 32 | lo;
}