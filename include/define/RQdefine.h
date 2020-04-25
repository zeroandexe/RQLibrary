#pragma once

/*
	本文是RQ容器的基础声明，属于容器共用
*/

#define M_DEBUG

//表示参数是输入型参数
#define M_Input
//表示参数是输出型参数
#define M_Output

//标识该类是一个接口类
#define M_Interface

#ifndef M_NULL
#define M_NULL(type) ((type)0)
#endif

#define M_CHECK_EQUAL(tar,ch)		(!((tar)-(ch)))				//当tar==ch，返回1，其他返回0
#define M_CHECK_NON_EQUAL(tar,ch)	(!!((tar)-(ch)))			//当tar==ch，返回0，其他返回1
#define M_CHECK_MORE(tar,ch)		(!((tar)<=(ch)))			//当tar>ch，返回1，其他返回0
#define M_CHECK_LESS(tar,ch)		(!((tar)>=(ch)))			//当tar<ch，返回0，其他返回1
#define M_CHECK_MORE_EQUAL(tar,ch)	(!((tar)<(ch)))				//当tar>=ch，返回1，其他返回0
#define M_CHECK_LESS_EQUAL(tar,ch)	(!((tar)>(ch)))				//当tar<=ch，返回1，其他返回0

//获取两者中最小的一个
#define M_MIN(left,right)	(((left)<(right))?(left):(right))
//获取两则中最大的一个
#define M_MAX(left,right)	(((left)>(right))?(left):(right))

//检测是否是数字字符
#define M_IS_NUM(ch)		((ch-0x3a)*(ch-0x2f)<0)

//定义系统内句柄状态
#define M_HANDLE_INVALID	-1
#define M_HANDLE_FREE		0
#define M_HANDLE_WORK		1

#define M_MAX_RQCOPY		(512*sizeof(long)/2)					//定义能超过memcpy的RQCopy字符阈值

//利用中间变量tmep交换left和right的数值
#define M_SWAP(left,right,temp)		\
{									\
	temp = left;					\
	left = right;					\
	right = temp;					\
}

//将指针转换为整数（由于gcc认为类型强转不安全，故采用取地址获取）
#define M_PTR_TO_VAL(ptr)			(*(long*)&ptr)


///配置参数
#define M_REGULAR_STIME_MS		100				//定时池睡眠间隔时间
#define M_SESSION_TOTAL			1024			//程序连接最大上限
#define M_SESSION_POOL_SIZE		512				//句柄池大小
#define M_PACKHEAD_HEAD_CHAR	((char)0xa5)	//数据包包头验证字符


enum {
	M_DIV_STATUS_INVALID,					//设备无效
	M_DIV_STATUS_FREE,						//设备当前空闲
	M_DIV_STATUS_WORK						//设备工作中
};

enum {
	M_COMCA_PROTOCOL = 1,			//通讯模块协议
	M_ROUTA_PROTOCOL,				//路由协议
	M_END_PROTOCOL = 10000,			//此数据后可由用户自定义协议
};

//---路由详细协议
enum M_ROUTA_PROT {
	INSERT = 1,						//路由信息新增
	REMOVE,							//路由信息移除
	ERROR,							//存在异常路由信息
	COPY,							//路由表复制请求
	TOTAL,							//协议上限
};


///---数据包主协议
enum M_COMCA_PROT {
	M_COMCA_PROT_ERR = 1,			//数据报存在问题
	M_COMCA_PROT_RESPOND,			//当前数据报是回应数据报
};


///---通讯错误码
enum M_COMCA_ERR {
	OK,					//没有错误
	UNKNOW,				//未知错误
	CHECK_SUM,			//效验和错误
	CHECK_HEAD,			//效验头错误
	CHECK_DATA,			//数据效验错误
	CHECK_SQN,			//序列号错误

	DISCONNECT,			//网络断开
	HEAD_SHORT,			//数据包头数据太短
	PACK_SHORT,			//数据包太短
	NOT_DATA,			//并未接收到任何数据
};