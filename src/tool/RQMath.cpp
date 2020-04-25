#include "RQMath.h"

//计算正数的位数
unsigned int RQIntegerLength(unsigned long long integer)
{
	int length = 0;
	do
	{
		length++;
		integer /= 10;
	} while (integer);

	return length;
}

//计算负数的位数
unsigned int RQIntegerLength(long long integer)
{
	if (integer < 0)
	{
		integer *= -1;				//将负数转换为正数，为后续工作准备
	}

	return RQIntegerLength((unsigned long long)integer);
}
