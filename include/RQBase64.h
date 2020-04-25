#pragma once
class RQBase64
{
public:
	//标准base64字符串
	static	const char* const  BASE64_STR;
public:
	RQBase64(const char* base64Str = BASE64_STR);


	/*
		将传入的元数据按照标准base64子码表进行编码，
		并输入编码长度，同时将会为结果字符串添加\0结尾符
		det			[out]	base64字符串存储空间
		sour		[in]	将用于base64编码的源数据
		sourLen		[in]	源数据长度
		base64Str	[in]	用于本次编码的base64字符串，默认提供标准base64编码字符串（提供长度要求为64个字符）
		return		[out]	返回base64实际字符串长度
	*/
	int Encoded(char* det, const char* sour, unsigned int sourLen);
	int Decoded(char* det, const char* sour, unsigned int sourLen);
	/*
		计算源码进行base64编码后需要的内存空间大小
		sourlen		[in]	源码字节长度
		return		[out]	编码后需要的字节长度（包括结尾符）
	*/
	static int EncodeMSize(unsigned int sourLen);
	/*
		计算将base64字符串解码后需要的内存空间大小
		base64StrLen	[in]	已编码后的base64字符串长度
		return			[out]	解码后需要的字节数（包括结尾符）
	*/
	static int DecodeMSize(unsigned int base64StrLen);
private:
	int Coded(char* det, const char* sour, unsigned int sourLen,unsigned int unitSize,bool isEncode);
private:
	//能够完整执行base64编码的数据部分
	char* _code_complete(char* det, const char* sour, unsigned int times, unsigned int unitSize, bool isEncode);
	//源数据字节数不足，补充字节进行编码
	char* _code_incomplete(char* det, const char* sour, unsigned int strlen, unsigned int unitSize, bool isEncode);

	char* _code(char* det, const char* sour, unsigned int sourLen, bool isEncode);
	//将传入的数据进行编码并存入目标
	char* _encode(char* det, const char* sour, unsigned int sourLen);
	char* _decode(char* det, const char* sour, unsigned int sourLen);
private:
	const char* _base64Str;
	unsigned char _decodeLibrary[256];
};

