#include "node/RQSmartNode.h"
#include "RecycleBin.h"
#include "RQThreadPool.h"

RQSmartNode::RQSmartNode()
{
	Create();
}

RQSmartNode::~RQSmartNode()
{
}

void RQSmartNode::Create(void * param)
{
	_lock = new RQSpinLock();
	_ClearData();
}

void RQSmartNode::Clear()
{
	_ClearData();
}

void RQSmartNode::AddRef()
{
	_count++;
}

bool RQSmartNode::SubRef()
{
	bool isAlive = true;							//默认该节点还不会应为引用-1而回收
	RQAutoLock lock(_lock);
	if (!_count										//当前是否应当释放
		|| !--_count)								//引用减少后是否需要释放
	{
		if (_releaseFun)
		{
			RQThreadPool::Push(_releaseFun, _ptr);	//直接将销毁执行抛入线程池
		}
		RecycleBin::RecycleNode(this);
		isAlive = false;
	}
	return isAlive;
}

void* RQSmartNode::GetPtr()
{
	RQAutoLock lock(_lock);
	return _ptr;
}

void RQSmartNode::SetPtr(void* ptr)
{
	RQAutoLock lock(_lock);
	_ptr = ptr;
}

void RQSmartNode::SetFun(ReleseFun_Type releaseFun)
{
	_releaseFun = releaseFun;
}

void RQSmartNode::_ClearData()
{
	_ptr = 0;
	_count = 0;
	_releaseFun = 0;
}
