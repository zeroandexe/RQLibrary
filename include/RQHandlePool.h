#pragma once
#include "define/RQdefine.h"
#include "node/RQNode.h"
#include "RQSpinLock.h"
/*
	该对象用于提供句柄值与对象值的转换

	暂时不提供对已分配的句柄进行遍历
*/
class RQPair;
class RQMemory;

class RQHandlePool  : public RQNode
{
public:
	RQHandlePool(unsigned int size = 0);
	

	virtual void Create(void* param = 0);
	virtual void* Init(void* param = 0);
	virtual void Clear();

	/*
		扩展池大小，若holdData为true则保存旧数据
		size		[in]	池将被扩展的池大小
		holdData	[in]	是否保留旧池中的数据（默认为false，即不保留任何旧池中的数据，一旦重置大小将清空池中的数据
		isMust		[in]	池是否必须被扩展（默认为false，则表是若池的大小大于要求大小则不进行任何处理）
	*/
	void Resize(unsigned int size ,bool holdData = false, bool isMust = false);

	/*
		功能：存入资源并获取对应的句柄值，若句柄耗尽则不予存储（sour不进行任何操作）
		reutnr：返回资源所对应的句柄值，存入成功return>0，失败return=0
	*/
	unsigned int SaveSour(void* sour);

	/*
		回收句柄，同时回收句柄资源所占用的空间。
		并返回句柄所对应的资源地址。
		无效句柄将返回空
	*/
	void* RecyclHandle(unsigned int handle);

	/*
		获取句柄所对应的资源，句柄无效返回空
	*/
	void* GetSour(unsigned int handle);

	/*
		获取当前已经存入的资源数量
	*/
	int GetSaveCount();

	/*
		获取当前还空闲的句柄数量
	*/
	int GetFreeCount();

	/*
		获取当前句柄的状态,若句柄值无效将返回M_HANDLE_INVALID错误值
	*/
	int GetStatus(unsigned int handle);

	/*
		重置句柄池设置
	*/
	void Reset();

	/*
		更换句柄所对应的资源地址
	*/
	void ChangeSour(unsigned int handle,void *sour);
	/*
		从给定的句柄开始查找，若存在被使用的句柄则返回对应的句柄值
		startHandle		[in]	开始检测的句柄下标值
		return			[out]	若有句柄存在则返回该句柄值，若没有句柄则返回0
	*/
	unsigned int GetUsedHandle(unsigned int startHandle = 0);

private:
	void _Create();					///为成员对象创建存储空间
	void _Init();					///初始化

private:
	unsigned int _size;						///当前可容纳的资源个数
	///创建内存对象
	RQMemory* _freeHandleMemory;	///空闲句柄存储内存对象
	RQMemory* _sourMemory;			///资源存储内存对象
	RQMemory* _handleStatusMemory;	///句柄状态存储内存对象

	unsigned int* _freeHandleArray;			///空闲句柄值数组
	void** _sourArray;				///资源数组，成员first表示当资源是否存在（1存在，0不在），second表示当前资源地址
	unsigned int* _handleStatusArray;			///句柄状态数组，用于查询句柄当前所处状态
	unsigned int _freeCount;					///当前可用句柄数量
	RQSpinLock* _lock;				///定义自旋锁，用于保护资源
};