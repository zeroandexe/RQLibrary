#include "RQBase64.h"
#include <string.h>

const char* const  RQBase64::BASE64_STR = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

#define M_SOUR_COUNT	3	//源字节个数
#define M_DET_COUNT		4	//目标字节个数

int RQBase64::Encoded(char * det, const char * sour, unsigned int sourLen)
{
	return Coded(det, sour, sourLen, M_SOUR_COUNT, true);
}

int RQBase64::Decoded(char * det, const char * sour, unsigned int sourLen)
{
	return Coded(det, sour, sourLen, M_DET_COUNT, false);
}

RQBase64::RQBase64(const char * base64Str)
{
	_base64Str = base64Str;
	memset(_decodeLibrary, 0, 256);
	for (int i = 0; i < 64; i++)
	{
		_decodeLibrary[base64Str[i]] = i;
	}
	_decodeLibrary[base64Str[64]] = 0;
}

int NeedMSize(unsigned int len, unsigned int iUnitSize, unsigned int oUnitSize)
{
	int count = len / iUnitSize;
	count += len > count * iUnitSize ? 1 : 0;
	return count * oUnitSize + 1;
}

int RQBase64::EncodeMSize(unsigned int sourLen)
{
	return NeedMSize(sourLen, M_SOUR_COUNT, M_DET_COUNT);
}

int RQBase64::DecodeMSize(unsigned int base64StrLen)
{
	return NeedMSize(base64StrLen, M_DET_COUNT, M_SOUR_COUNT);
}

int RQBase64::Coded(char * det, const char * sour, unsigned int sourLen, unsigned int unitSize, bool isEncode)
{
	char* oldDet = det;
	if (sour && sourLen)
	{
		int count = sourLen / unitSize;									///计算需要多少次非补位编码
		det = _code_complete(det, sour, count,unitSize,isEncode);							///进行非补位编码
		count *= unitSize;												///计算非补位编码所消耗的字节数
		det = _code_incomplete(det, sour + count, sourLen - count,unitSize,isEncode);		///将剩余字节数按补位编码计算
	}
	det[0] = 0;
	return det - oldDet;
}

char* RQBase64::_code_complete(char * det, const char * sour, unsigned int times,unsigned int unitSize, bool isEncode)
{
	for (int i = 0; i < times; i++)
	{
		det = _code(det, sour + unitSize * i, 3,isEncode);
	}
	return det;
}

char* RQBase64::_code_incomplete(char * det, const char * sour, unsigned int strlen, unsigned int unitSize, bool isEncode)
{
	 char sourBuff[4] = { 0,0,0,0 };
	if (strlen)
	{
		strlen = strlen < unitSize ? strlen : 2;
		for (int i = 0; i < strlen; i++)
			sourBuff[i] = sour[i];
		det = _code(det, sourBuff, strlen,isEncode);
	}
	return det;
}

void _SetSour1(char* baseDet, const char* sour) { baseDet[0] = (sour[0] & 0xfc) >> 2; }
void _SetSour2(char* baseDet, const char* sour) { baseDet[1] = ((sour[0] & 0x03) << 4);}
void _SetSour3(char* baseDet, const char* sour) { baseDet[1] += ((sour[1] & 0xf0) >> 4); }
void _SetSour4(char* baseDet, const char* sour) { baseDet[2] = ((sour[1] & 0x0f) << 2); }
void _SetSour5(char* baseDet, const char* sour) { baseDet[2] += ((sour[2] & 0xc0) >> 6); }
void _SetSour6(char* baseDet, const char* sour) { baseDet[3] = sour[2] & 0x3f; }

void SetSour(char* baseDet, const char* sour, unsigned int sourLen,int invalidIndex)
{
	typedef void(*SetSour_FunType)(char*, const char*);
	static SetSour_FunType Fun[6] = { _SetSour1 ,_SetSour2 ,_SetSour3 ,_SetSour4 ,_SetSour5 ,_SetSour6 };

	int i = 0;
	for (int j = 0; i < sourLen; i++)
	{
		Fun[j++](baseDet, sour);
		Fun[j++](baseDet, sour);
	}
	for (int j=M_DET_COUNT-1; i<M_SOUR_COUNT;j--,i++)
	{
		baseDet[j] = invalidIndex;
	}
}

char * RQBase64::_code(char * det, const char * sour, unsigned int sourLen, bool isEncode)
{
	if (isEncode)
	{
		return _encode(det, sour, sourLen);
	}
	return _decode(det,sour,sourLen);
}

char* RQBase64::_encode(char * det, const char * sour,unsigned int sourLen)
{
	char baseDet[4] = { 0,0,0,0 };
	SetSour(baseDet, sour, sourLen,64);

	for (int i = 0; i < M_DET_COUNT; i++)
	{
		(det++)[0] = _base64Str[baseDet[i]];
	}
	return det;
}

char * RQBase64::_decode(char * det, const char * sour, unsigned int sourLen)
{
	char sourVal[M_DET_COUNT];
	for (int i = 0; i < M_DET_COUNT; i++)
		sourVal[i] = _decodeLibrary[sour[i]];

	det[0] = (sourVal[0] << 2) + ((sourVal[1] & 0x30) >> 4);
	det[1] = ((sourVal[1] & 0x0f) << 4) + ((sourVal[2] & 0x3c) >> 2);
	det[2] = ((sourVal[2] & 0x03) << 6) + sourVal[3];
	return det + 3;
}
