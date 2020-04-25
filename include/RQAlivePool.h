#pragma once

/*
	数据活跃池,用于提供活跃的数据
	该对象为静态活跃池，不具备后台线程对数据的监视
	即，在用户调用获取活跃接口时对数据进行有效时限检测

	要求用户在不需要使用注册句柄时对句柄进行注销
*/

struct RQActPoolElement;		//活跃池元素
class RQSpinLock;
class RQStack;					
class RQAlivePool {
public:
	//设置活跃池大小
	RQAlivePool(unsigned int poolSize);
	~RQAlivePool();
	/*
		往池中添加一个计算参数并获取该参数的池句柄
		输入：
			hPool：传入_param所对应的池句柄，
					若hPool为0则向池中新增_param，并返回_param匹配的句柄值
					若hPool！0则更新hPool所指向的_param的活跃时间
			_param：放入活跃次的数据池的数据
			timeOut：数据的失效时间
		输出：
			return：若新增_param则返回池句柄值
					若非新增则返回_param对应的池句柄
	*/

	int AddParam(int hPool, void* param, long timeOutSec);
	//从池中获取一个当前在的_param,并将_param从活跃池中移除
	void* GetAlive();
	//回收指定池句柄的资源
	void Remove(int hPool);

protected:
	void _ActParam(int hPool, void* param, long timeOut);

private:
	unsigned int _poolSize;				//活跃池大小限制
	unsigned int _checkPos;				//当前的查找位置
	RQActPoolElement* _pool;			//活跃池
	RQSpinLock* _lock;					//自旋锁
	RQStack* _stackSaveIndex;			//数据存放下标存储栈
	RQStack* _stackAlivePool;			//活跃池存储栈
};

