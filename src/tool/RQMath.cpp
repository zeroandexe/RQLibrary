#include "RQMath.h"

//����������λ��
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

//���㸺����λ��
unsigned int RQIntegerLength(long long integer)
{
	if (integer < 0)
	{
		integer *= -1;				//������ת��Ϊ������Ϊ��������׼��
	}

	return RQIntegerLength((unsigned long long)integer);
}
