#pragma once
/*
	该对象为线程池，用于并行执行外部回调
*/
class RQFunNode;						//函数节点
class RQSignalQueue;					//信号队列
class RQThread;
class RQDBStack;

class RQThreadPool
{
public:
	/*
		创建线程池对象，并启动线程池中线程
	*/
	static void CreatePool(unsigned int size);
	/*
		添加一个外部执行的函数节点到线程池用于线程执行。
		输入：
			node：期望被执行的函数节点
			isFirst：是否最优先
	*/
	static void Push(RQFunNode* node,bool isFirst = false);		

	/*
		向池中添加一个执行函数
		callFun		[in]	将被执行的函数地址
		param		[in]	函数执行时所使用的参数
		isFirst		[in]	是否作为最优先执行
	*/
	static void Push(void(*callFun)(void*), void* param, bool isFirst = false);

	//停止执行线程,并决定是否进行等待
	//若决定等待，那么被掉函数处于死循环，那么该线程交死等
	static void Stop(bool needWait=false);
	//获取当前线程池中待执行的任务节点数量
	static int GetNodeCount();

private:
	//杀死线程池内所有线程
	static void Kill();
protected:
	///static void _RunThreadPool(RQThreadPool* pool);			//启动线程池
	static void _RunThreadPool();			//启动线程池
	static void _ThreadFunModel(void* queue);	//线程执行模板
	static void _Wait();						//等待线程结束
private:
	static unsigned int _size;					//线程池中线程数量
	static RQThread* _array;					//线程存放数组
	static RQSignalQueue* _queue;				//函数节点存放地点
	static bool _continueFlag;					//线程继续运行标志
};

