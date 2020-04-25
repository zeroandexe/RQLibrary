#include "RQHandlePool.h"
#include <memory>
#include "RQPair.h"
#include "define/RQdefine.h"
#include "RQPtrVal.h"
#include "tool/RQMemory.h"
#include "RecycleBin.h"
#include <string.h>

RQHandlePool::RQHandlePool(unsigned int size)
{
	Create((void*)size);
}

void RQHandlePool::Create(void * param)
{
	_freeHandleMemory = 0;
	_lock = new RQSpinLock;

	Init(param);
}

void * RQHandlePool::Init(void * param)
{
	RQPtrVal ptrVal(param);
	_size = ptrVal._uint > 0 ? ptrVal._uint : 512;			///调整句柄值存储数量，默认为10
	if (!_freeHandleMemory)
	{
		_freeHandleMemory = RecycleBin::GetFreeRQMemory();
		_sourMemory = RecycleBin::GetFreeRQMemory();
		_handleStatusMemory = RecycleBin::GetFreeRQMemory();
	}
	Reset();
	return nullptr;
}

void RQHandlePool::Clear()
{
	RecycleBin::RecycleNode(_freeHandleMemory);
	RecycleBin::RecycleNode(_sourMemory);
	RecycleBin::RecycleNode(_handleStatusMemory);

	_freeHandleMemory = 0;	
}

void RQHandlePool::Resize(unsigned int size,bool holdData, bool isMust )
{
	if (_size > size || isMust)					///检查当前需求重置池的大小
	{
		int oldSize = _size;
		///---保留旧空间数据，待回收
		unsigned int* oldFreeHandleArray = _freeHandleArray;
		void** oldSourArray = _sourArray;
		unsigned int* oldHandleStatusArray = _handleStatusArray;
		int oldFreeCount = _freeCount;

		_size = size;
		_freeHandleArray = 0;
		_sourArray = 0;
		_handleStatusArray = 0;

		Reset();												///根据新size创建并重置池（含初始化）
		if (oldFreeCount != oldSize 
			&& holdData 
			&& !isMust)					///检查当前池是否有数据需要保存，若为强制，则不保存数据
		{
			///---恢复空闲句柄记录数据
			memcpy(_freeHandleArray, oldFreeHandleArray, sizeof(int)*oldFreeCount);		///恢复旧空闲句柄存储数组所保存的数据部分
			for (int i = oldFreeCount; i < oldSize; i++)				///将本次分配所超量的下标存入新空闲句柄数组
			{
				_freeHandleArray[i] = _freeHandleArray[_size - i + oldFreeCount];
			}
			_freeCount = _size - oldSize + oldFreeCount;
			///---恢复资源存储数组
			memcpy(_sourArray, _sourArray, sizeof(void*)*oldSize);
			///---恢复句柄状状态处理数组
			memcpy(_handleStatusArray, oldHandleStatusArray, sizeof(int)*oldSize);
		}

		///删除不使用的空间
		delete oldFreeHandleArray;
		delete oldSourArray;
		delete oldHandleStatusArray;
	}
}


unsigned int RQHandlePool::SaveSour(void * sour)
{
	unsigned int handle = 0;
	RQAutoLock lock(_lock);
	if (_freeCount > 0)				///检查当前是否还有可分配的句柄值
	{
		handle = _freeHandleArray[--_freeCount];		///获取空闲句柄
		_handleStatusArray[handle] = M_HANDLE_WORK;		///指定该句柄处于工作状态
		_sourArray[handle] = sour;						///保存期望的资源地址
	}
	return handle;
}

void* RQHandlePool::RecyclHandle(unsigned int handle)
{
	int isOk = (handle > 0)*(handle <= _size);		///若handle不在范围内则isOk = 0，若handle在范围内则isOk = 1;
	void* sour = 0;
	if (isOk)
	{
		RQAutoLock lock(_lock);
		if (M_HANDLE_FREE !=_handleStatusArray[handle]) ///检测当前句柄是否未被回收
		{
			sour = _sourArray[handle];					///返回将被回收的句柄
			_handleStatusArray[handle] = M_HANDLE_FREE;	///将当前句柄状体设置为空闲状态
			_freeHandleArray[_freeCount++] = handle;	///将句柄存入待分配句柄数组
			_sourArray[handle] = 0;
		}
	}
	return sour;
}

void * RQHandlePool::GetSour(unsigned int handle)
{
	int isOk = (handle > 0)*(handle <= _size);
	return _sourArray[handle*isOk];				///资源0位始终为2空
}

int RQHandlePool::GetSaveCount()
{
	return _size-_freeCount;
}

int RQHandlePool::GetFreeCount()
{
	return _freeCount;
}

int RQHandlePool::GetStatus(unsigned int handle)
{
	int isOk = (handle > 0)*(handle <= _size);		///若句柄有效isOk=1，若无效则为0
	return _handleStatusArray[handle*isOk];			///同理，有效时handle=handle,无效时handle=0
}

void RQHandlePool::Reset()
{
	_Create();
	_Init();
}

void RQHandlePool::ChangeSour(unsigned int handle, void * sour)
{
	int isOk = (handle > 0)*(handle <= _size);		///若句柄有效isOk=1，若无效则为0
	if (isOk)
	{
		_sourArray[handle] = sour;					///更换句柄所对应的资源地址
	}
}

unsigned int RQHandlePool::GetUsedHandle(unsigned int startHandle)
{
	long retHandle = 0;
	startHandle = startHandle > 0 ? startHandle : 1;		///调整输入句柄的值，要求作为起点的句柄值>=1.
	while (startHandle < _size)
	{
		if (M_HANDLE_FREE != _handleStatusArray[startHandle])
		{
			retHandle = startHandle;
			break;
		}
		startHandle++;
	}
	return retHandle;
}

void RQHandlePool::_Create()
{
	if (_freeHandleMemory)
	{
		_freeHandleMemory->Resize(sizeof(int*)*_size);
		_sourMemory->Resize(sizeof(int*)*(_size + 1));
		_handleStatusMemory->Resize(sizeof(int*)*(_size + 1));

		_freeHandleArray = (unsigned int*)_freeHandleMemory->GetAddr();
		_sourArray = (void**)_sourMemory->GetAddr();
		_handleStatusArray = (unsigned int*) _handleStatusMemory->GetAddr();
	}
}

void RQHandlePool::_Init()
{
	///---初始化空闲句柄存储数组
	for (int i = 1; i < _size; i++)
	{
		_freeHandleArray[i] = i;
	}
	_freeCount = _size;
	///---置空资源存储数组
	memset(_sourArray, 0, _size);				///指控
	///---初始化句柄状态几率数组
	memset(_handleStatusArray, 0, _size);
	_handleStatusArray[0] = M_HANDLE_INVALID;
}
