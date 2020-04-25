#include "tool/RQMemory.h"
#include <memory>
#include "define/RQdefine.h"
#include "RQStrTool.h"
#include "RQPtrVal.h"
#include "string.h"
#include "math.h"

RQMemory::RQMemory(unsigned int size)
{
	_Create(size);
}

RQMemory::~RQMemory()
{
	delete _addr;
}

void * RQMemory::GetAddr()
{
	return _addr;
}


void RQMemory::ResetLength(unsigned int length)
{
	_length = length>_size?_size-1:length;
}

void RQMemory::Resize(unsigned int size,bool holdData, bool isMust)
{
	int isOk = (size> (_size-1))	///若输入大小小于现有大小不分配
		+ isMust;					///若要不要求必须按照输入分配则由大小比较为主
	if (isOk)	///检查是否需要进行分配
	{
		unsigned oldDataLen = _length;
		void* oldAddr = _Create(size);	///根据传入大小传创建新空间

		if (holdData)					///检查是否需要保留旧数据
		{
			Assign(oldAddr, oldDataLen);		///将旧值数据复制于新空间
			_length = oldDataLen;
		}
		delete oldAddr;
	}
}

void RQMemory::Create(void * param)
{
	RQPtrVal ptrVal(param);
	_Create(ptrVal._int);
}

void RQMemory::Clear()
{
	_length = 0;
	*(long long*)_addr = 0;				/// 清理记录数据
}

void RQMemory::Assign(void * addr, unsigned int length)
{
	if (addr != _addr)				///防止传入无效内存数据
	{
		Resize(length);		///根据输入长度扩展内存空间
		*(long long*)_addr = 0;			///将前8字节清空
		_Write(_addr, addr, length);	///从数据头拷贝数据
		_length = length;
		((char*)_addr)[_length] = 0;
	}

}

void RQMemory::Append(void * addr, unsigned int length)
{
	Resize(_length + length, true);						///扩展空间并保留旧数据
	_Write((char*)_addr + _length, addr, length);		///将新数据追加到
	_length += length;
}

void RQMemory::Swap(RQMemory * other)
{
	RQPtrVal ptrVal;
	M_SWAP(_addr, other->_addr, ptrVal._ptr);
	M_SWAP(_size, other->_size, ptrVal._uint);
	M_SWAP(_length, other->_length, ptrVal._uint);
}

unsigned int RQMemory::GetSize()
{
	return _size -1;
}

unsigned int RQMemory::GetLength()
{
	return _length;
}

void* RQMemory::_Create(unsigned int size)
{
	void* oldAddr = _addr;
	_size = size >= sizeof(long long)+1 ? size+1 : sizeof(long long)+1;
	_addr = (void*) new char[_size];
	_length = 0;
	*(long long*)_addr = 0;				///将低数据位置空
	return oldAddr;
}

void RQMemory::_Write(void * destAddr, void * sourAddr, unsigned int dataLen)
{
	if (M_MAX_RQCOPY < dataLen)		///参考最高效率进行拷贝
	{
		memcpy(destAddr, sourAddr, dataLen);
	}
	else {
		RQCopy(destAddr, sourAddr, dataLen);
	}
}


