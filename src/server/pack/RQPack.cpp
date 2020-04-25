#include "RQPack.h"
#include "RQPtrVal.h"
RQPack::RQPack(unsigned int size)
{
	Create((void*)size);
	RQNode::Reinit();
}

void RQPack::Create(void * param)
{
	RQPtrVal ptrVal(param);
	if (!ptrVal._ptr)
	{
		ptrVal._int = 1024 * 8;
	}

	RQMemory::Create(ptrVal._ptr);			///按照内存空间对象的方式创建内容
	Clear();
}

void RQPack::Clear()
{
	RQMemory::Clear();
	long long* ptr = (long long *)GetDataPtr();
	*ptr = 0;
	RQPackHead::SetZero(GetPackHead());
}

RQPackHead * RQPack::GetPackHead()
{
	return (RQPackHead*)_addr;
}

void * RQPack::GetDataPtr()
{
	return (RQPackHead*)(((char*)_addr)+sizeof(RQPackHead));
}

unsigned int RQPack::GetSize()
{
	return _size;
}

unsigned int RQPack::GetDataLen()
{
	return _length;
}

unsigned int RQPack::GetPackLen()
{
	return _length+sizeof(RQPackHead);
}

void RQPack::WriteData(void * data, unsigned int dataLen)
{
	if (data)
	{
		RQMemory::_Write((char*)_addr + sizeof(RQPackHead), data, dataLen);
	}
	_length = dataLen;
	((char*)_addr)[sizeof(RQPackHead) + _length] = 0;
}

void RQPack::AppendData(void * data, unsigned int dataLen)
{
	if (data)
	{
		RQMemory::_Write((char*)_addr + sizeof(RQPackHead) + _length, data, dataLen);
	}		
	_length += dataLen;
}

void RQPack::Swap(RQPack * pack)
{
	RQMemory::Swap((RQMemory*)pack);			///交换与外界的内存存储对象
}

void RQPack::SetPackLen(unsigned int packLen)
{
	///packLen -= sizeof(RQPackHead);				///将数据包长度调整为数据长度
	SetDateLen(packLen - sizeof(RQPackHead));
	///_length = packLen > (_size-sizeof(RQPackHead)) ? 0: packLen;	///检测数据长度是否为有效长度，若不是归0，若是存储
}

void RQPack::SetDateLen(unsigned int dataLen)
{
	_length = dataLen > (_size - sizeof(RQPackHead)) ? 0 : dataLen;
}


void RQPack::Resize(unsigned int size, bool holdData, bool isMust)
{
	RQMemory::Resize(size + sizeof(RQPackHead), holdData, isMust);
}


//unsigned int RQPack::GetOptProtocol()
//{
//	return ((RQPackHead*)_addr)->_optProtocol;
//}
//
//unsigned int RQPack::GetOptDecorate()
//{
//	return ((RQPackHead*)_addr)->_optDecorate;
//}
