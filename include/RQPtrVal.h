#pragma once

/*
	该类主要用于在GCC环境下指针和数值的类型转换
*/
union RQPtrVal
{
	RQPtrVal() { _llong = 0; }
	RQPtrVal(void* param) { _ptr = param; }
	RQPtrVal(char param) { _char = param; }
	RQPtrVal(short param) { _short = param; }
	RQPtrVal(int param) { _int = param; }
	RQPtrVal(long param) { _long = param; }
	RQPtrVal(long long param) { _llong = param; }
	RQPtrVal(float param) { _float = param; }
	RQPtrVal(double param) { _double = param; }
	RQPtrVal(bool param) { _bool = param; }

	char _buff[sizeof(long)];
	void* _ptr;
	long long _llong;
	long _long;
	int _int;
	short _short;
	char _char;
	bool _bool;
	
	unsigned long long _ullong;
	unsigned long _ulong;
	unsigned int _uint;
	unsigned short _ushort;
	unsigned char _ubuff[sizeof(long)];
	unsigned char _uchar;

	float _float;
	double _double;
};