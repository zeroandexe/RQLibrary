#pragma once
#include <utility>
#include <time.h>


/*
	由于有些操作需要系统的部分功能进行处理化
*/
//void RQInit_sys();

//根据传入的文件路径，获取本文件的写时间
//若文件不存在则返回(0，0)
std::pair<unsigned long, unsigned long> GetWritTimeByFilePath(const char* path);

//睡眠，以秒为单位
void RQSleepSec(int sec);
//睡眠，以毫秒为单位
void RQSleepMs(int msec);

/*
	将本地数据转换为网络数据
*/
short	RQHton(short data);
unsigned short RQHton(unsigned short data);

int		RQHton(int data);
unsigned int RQHton(unsigned int data);

long	RQHton(long data);
unsigned long RQHton(unsigned long data);

long long RQHton(long long data);
unsigned long long RQHton(unsigned long long data);

float	RQHton(float data);
double	RQHton(double data);
void*	RQHton(void* data);

/*
	将网络数据转换为本地数据
*/
short	RQNtoh(short data);
unsigned short RQNtoh(unsigned short data);

int		RQNtoh(int data);
unsigned int RQNtoh(unsigned int data);

long	RQNtoh(long data);
unsigned long RQNtoh(unsigned long data);

long long RQNtoh(long long data);
unsigned long long RQNtoh(unsigned long long data);

float	RQNtoh(float data);
double RQNtoh(double data);
void*	RQNtoh(void* data);

int Inet_addr(const char* ipStr);
//---将数值ip值转换为字符串并保存（ip值，存储内存地址，内存长度）
const char* Inet_addr(int ipVal, char* buff, int bufLen);

//---获取cpu核心个数
int GetCpuCoreNum();

//---获取当前时间
void LoadNowTime(tm* nowTm);
void LoadNowTimeVal(unsigned long long& sec,
	unsigned long long &nsec);

void LoadNowTimeSec(unsigned long long&sec);
void LoadNowTimeMS(unsigned long long&msec);

//---检测sock是否有效
int GetSockStatus(int sock);

/*
	加载动态库
	dllPath	[in]	动态库路径
	mode	[in]	动态库加载模式
						RTLD_LAZY：待查找函数符号时才进行解析
						RTLD_NOW ：立即解析动态库的函数符号
*/
void* LoadDll(const char* dllPath, int mode);
//---卸载动态库
void UnloadDll(void* dll);
//---从动态库中加载出函数
void* LoadFunFromDll(void* dll,const char* funName);

//---获取cpu时间戳
unsigned long long CpuRdtsc();