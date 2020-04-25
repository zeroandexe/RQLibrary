#pragma once
//#include <string>
///#include "RQLineStorage.h"
/*
	这是一个接口类，提供节点特性供数据对象继承

	接口含义：
	Create	：创建接口仅用于对象成员的创建，是内存分配的保障
	Init	：用于使对象恢复到可重新使用的状态（额，这个函数好像多余了）
	Clear	：用于清理当前已使用的数据，但不回收所创建的内存空间，使对象重置到可使用状态，是对象可被重复使用的保障

	所以要求在Create中需要调用Clear进行初始化

	考虑废弃Init的使用，原因在于Create和Clear结合起来就完成了整个对象的构建，所以这里也就也意味着Init不再有用
*/
///class RQList;
class RQLineStorage;
class RQNode {
public:
	RQNode();
	RQLineStorage* _saveContainer;	//存储该容器的list表容器
	RQNode*	_next;					//下一个节点
	RQNode*	_prev;					//获取上一个节点

	virtual void Create(void* param = nullptr);		//数据节点创建
	virtual void* Init(void* param = nullptr);		//提供对节点对象的子对象进行初始化操作
	virtual void Clear();							//提供对节点数据的清除操作
	virtual void Destroy();
	virtual void Reinit() final;					//重置节点相关的所有数据
	virtual ~RQNode() {}
	void* _param;				//用于节点存储额外/意外数据使用
};