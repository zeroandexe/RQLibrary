#pragma once
#include "tool/RQMemory.h"
#include "server/pack/RQPackHead.h"
#include "define/RQdefine.h"

class RQPack : public RQMemory
{
public:
	RQPack(unsigned int size = 0);
	virtual void Create(void* param = 0);
	virtual void Clear();

	/*
		获取数据包中的数据包头
	*/
	RQPackHead* GetPackHead();
	/*
		获取数据包中数据段的起始地址
	*/
	void* GetDataPtr();
	/*
		获取数据包中数据段（不关心包头）的内存空间大小
	*/
	unsigned int GetSize();
	/*
		获取数据包中数据段的数据长度
	*/
	unsigned int GetDataLen();
	/*
		获取数据包的总长度（包括包头）
	*/
	unsigned int GetPackLen();

	/*
		以新建数据包的方式将数据写入数据段
		并重新进行长度计数
	*/
	void WriteData(void* data, unsigned int dataLen);

	/*
		以追加的方式将传入的数据写入数据段中
		并累计追加的数据长度
	*/
	void AppendData(void* data, unsigned int dataLen);
	/*
		交换内存空间，以及长度记录
	*/
	void Swap(RQPack* pack);
	/*
		设置数据包长度(含包头),
		（若设置长度大于当前存储空间长度，那么意味所传入的数据长度为错误数据，当前数据长度归0）
	*/
	void SetPackLen(unsigned int packLen);
	/*
		设置数据段长度（不包含包头）
	*/
	void SetDateLen(unsigned int dataLen);
	/*
		重置数据包长度
	*/
	virtual void Resize(unsigned int size, bool holdData = false, bool isMust = false);

public:
	////获取数据包主协议
	//unsigned int GetOptProtocol();
	////获取数据包辅助协议
	//unsigned int GetOptDecorate();

};