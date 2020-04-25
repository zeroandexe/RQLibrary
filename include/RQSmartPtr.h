#pragma once
#include "node/RQSmartNode.h"
#include "RecycleBin.h"

/*
	该对象为数据节点管理器提供对数据的访问接口和
	节点的逻辑操作
*/

template<typename T>
class RQSmartPtr
{
public:
	RQSmartPtr(void* ptr = 0,
		RQSmartNode::ReleseFun_Type releaseFun = 0);
	RQSmartPtr(const RQSmartPtr& ptr);
	~RQSmartPtr();


	void operator=(void* ptr);
	void operator=(const RQSmartPtr& obj);
	void SetReleaseFun(RQSmartNode::ReleseFun_Type releaseFun);

	T* operator->();
	operator T* ();

private:
	void _SetPtr(void* ptr);
	void _SetPtr(void* ptr,
		RQSmartNode::ReleseFun_Type releaseFun);

	void _SetNode(RQSmartNode* node);

private:
	RQSmartNode* _node = 0;
};

template<typename T>
inline RQSmartPtr<T>::RQSmartPtr(void * ptr, RQSmartNode::ReleseFun_Type releaseFun)
{
	_SetPtr(ptr, releaseFun);
}

template<typename T>
inline RQSmartPtr<T>::RQSmartPtr(const RQSmartPtr & obj)
{
	_node = obj._node;
	_node->AddRef();
}

template<typename T>
inline RQSmartPtr<T>::~RQSmartPtr()
{
	_node->SubRef();
}

template<typename T>
inline void RQSmartPtr<T>::operator=(void * ptr)
{
	_SetPtr(ptr);
}

template<typename T>
inline void RQSmartPtr<T>::SetReleaseFun(RQSmartNode::ReleseFun_Type releaseFun)
{
	_node->SetFun(releaseFun);
}

template<typename T>
inline void RQSmartPtr<T>::operator=(const RQSmartPtr<T> & obj)
{
	_SetNode(obj._node);
}

template<typename T>
inline T * RQSmartPtr<T>::operator->()
{
	return (T*)_node->GetPtr();
}

template<typename T>
inline RQSmartPtr<T>::operator T*()
{
	return (T*)_node->GetPtr();
}

template<typename T>
inline void RQSmartPtr<T>::_SetPtr(void * ptr)
{
	///---将当前的减少当前的引用，并维持一个node的存在
	_node = _node && _node->SubRef() ? _node : RecycleBin::GerFreeRQSmartNode();
	_node->AddRef();
	_node->SetPtr(ptr);
}

template<typename T>
inline void RQSmartPtr<T>::_SetPtr(void * ptr, RQSmartNode::ReleseFun_Type releaseFun)
{
	_SetPtr(ptr);
	_node->SetFun(releaseFun);
}

template<typename T>
inline void RQSmartPtr<T>::_SetNode(RQSmartNode* node)
{
	if (node)
	{
		if (_node != node)
		{
			node->AddRef();
			_node->SubRef();
			_node = node;
		}
	}
}
