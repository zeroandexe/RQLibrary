#pragma once
#include <map>
#include "tool/lock/RQLock.h"
#include "RQMutex.h"
/*
	由于STL中的map并不是多线程安全，所以这里需要做的就是对map进行一层封装。
	对于map的操作，通常插入和删除会消耗掉3ms的时间（假设目标机器频率为3G），
	所这里对锁的使用进行公开，具体使用什么锁由外部决定，但默认使用互斥锁
*/

template<typename Kt,typename Vt>

class RQMap
{
public:
	/*typedef std::conditional_t<std::is_same_v<Kt, Vt>,
		typename std::_Tree_comp_alloc<std::_Tmap_traits<Kt, Vt, std::less<Kt>, std::allocator<std::pair<const Kt, Vt>>, false>>::const_iterator,
		typename std::_Tree_comp_alloc<std::_Tmap_traits<Kt, Vt, std::less<Kt>, std::allocator<std::pair<const Kt, Vt>>, false>>::iterator> InterType;
*/
	using InterType = typename std::map<Kt, Vt>::iterator;

	RQMap(RQLock* lock=0) 
	{
		_lock = lock;
	}
	~RQMap()
	{
		if (_lock)
			delete _lock;
	}

	Vt& operator [](const Kt& key)
	{
		RQAutoLock lock(_lock);
		return _map[key];
	}
	Vt& operator[](Kt&& key)
	{
		RQAutoLock lock(_lock);
		return _map[key];
	}

	void Remove(const Kt& key)
	{
		RQAutoLock lock(_lock);
		_map.erase(key);
	}
	void Remove(Kt&& key)
	{
		RQAutoLock lock(_lock);
		_map.erase(key);
	}
	void Remove(InterType iter)
	{
		if (iter != _map.end())
		{
			RQAutoLock lock(_lock);
			_map.erase(iter);
		}
	}

	void Clear()
	{
		RQAutoLock lock(_lock);
		_map.clear();
	}

	InterType Find(const Kt& key)
	{
		RQAutoLock lock(_lock);
		return _map.find(key);
	}

	InterType Find(Kt&& key)
	{
		RQAutoLock lock(_lock);
		return _map.find(key);
	}
	
	InterType end()
	{
		return _map.end();
	}

	InterType begin()
	{
		return _map.begin();
	}

private:
	std::map<Kt, Vt> _map;
	RQLock* _lock;
};