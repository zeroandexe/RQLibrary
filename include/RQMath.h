#pragma once
#include "math.h"
//计算一个整数的位数
//integer：将被计算的数
//负数不会计算‘负号’的长度
unsigned int RQIntegerLength(unsigned long long integer);
unsigned int RQIntegerLength(long long integer);