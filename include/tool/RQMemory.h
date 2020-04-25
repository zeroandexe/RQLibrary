#pragma once
#include "node/RQNode.h"
/*
	该类对象为内存对象，用于自动控制内存大小和数据长度记录。
*/

class RQMemory : public RQNode {
public:
	RQMemory(unsigned int size = sizeof(long));
	~RQMemory();

	/*
		创建内存对象，并初始化内存空间
	*/
	virtual void Create(void* param = (void*)sizeof(long));	
	/*
		清理数据长度记录
	*/
	virtual void Clear();
	/*
		获取_addr所指向地址空间
	*/
	void* GetAddr();
	/*
		重置数据长度
	*/
	void ResetLength(unsigned int length);
	/*
		重置内存空间大小
		holdData	[in]	是否保留原有数据，默认不保留
		isMust		[in]	是否决定按照输入大小重置空间
							false	：则(size+1)>_size?(size+1):_size
							true	：无论_size的大小都以size+1为准
	*/
	virtual void Resize(unsigned int size,bool holdData = false,bool isMust=false);


	/*
		将输入的addr中的值存入内存中，
		addr	[in]	指向目标数据内存的指针
		length	[in]	目标内存中数据的字节数
	*/
	void Assign(void* addr, unsigned int length);
	/*
		作用同上，但是用于对后续数据进行追加
	*/
	void Append(void* addr, unsigned int length);
	/*
		交换两者内部的成员数据
		other	[in]	另一个内存对象
	*/
	void Swap(RQMemory* other);

	/*
		返回所占用的内存字节数
	*/
	unsigned int GetSize();

	/*
		返回当前数据所使用的长度
	*/
	unsigned int GetLength();
protected:
	/*
		根据size大小创建新空间，并重置数据长度记录同时将旧空间返回
		size	[in]	新内存空间大小
		return	[out]	旧内存空间大小
	*/
	void* _Create(unsigned int size);

protected:
	/*
		从指定的内存位置写入数据
	*/
	static void _Write(void* destAddr, void* sourAddr, unsigned int dataLen);
protected:
	void* _addr;
	unsigned int _length;
	unsigned int _size;
};