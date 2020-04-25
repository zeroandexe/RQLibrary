#pragma once
#include "RQRoutaNode.h"
#include "RQPair.h"
#include "RQSpinLock.h"

class RQList;
class RQLoadNode;
/*
	该对象为网络节点/路由节点
	该对象析构时将会自动回收表内所有元素
*/

class RQRoutaTable
{
public:
	RQRoutaTable();
	~RQRoutaTable();

	/*
		根据传入参数查找目标路由节点，若节点不存在，则返回空
		ip		[in]	用于检索目标节点的Ip值(大端)
		port	[in]	用于检索目标节点的端口（大端）
		return	[out]	期望获取的路由节点
	*/
	RQRoutaNode* GetBy(int ip, short port);
	/*
		作用同上
		type	[in]	指定获取type类型的节点
		return	[out]	期望获取的路由节点
	*/
	RQRoutaNode* GetBy(char type);

	/*
		采用输入的网络地址作为路由节点依据，
		若该节点存在则不创建该节点，直接将该节点返回
		若节点不存在则创建新节点，但该节点并不存入路由表中，也不保存人路由映射表中
	*/
	RQRoutaNode* CreateBy(int ip,short port);

	/*
		保存从该类的CreateBy中创建的节点，非本类创建节点
	*/
	void SaveInTable(RQRoutaNode* routaNode);

	/*
		移除并回收路由节点
		routaNode	[in]	被移除节点
	*/
	void Remove(int ip, short port);
	void Remove(char type);
	void Remove(RQRoutaNode* routaNode);

	/*
		获取下一个路由节点，若容器的没有可便利节点则返回空
		currentNode		[in]	作为选择遍历节点的锚点
								为0时，返回第一个可遍历节点
								非空时，则返回当前节点的下一个可遍历节点
		return			[out]	若有可遍历节点则返回节点指针，否则为空
	*/
	RQRoutaNode* GetNext(RQRoutaNode* currentNode);

	/*
		查询传入节点是否是本表的节点
	*/
	bool IsAttach(RQRoutaNode* node);


	//显示，打印当前的路由表
	void Show();

protected:
	/*
		根据传入类型从类型表中移除
	*/
	void _RemoveInTypeTable(RQRoutaNode* node);
	void _ClearInTypeTable();
	/*
		根据传入地址从地址表中移除
	*/
	void _RemoveInAddrTable(int ip, short port);
	void _ClearInAddrTable();
	/*
		从存储表中移除
	*/
	void _RemoveInList(RQRoutaNode* node);
	void _ClearInList();

	/*
		将节点保存到存储容器中
	*/
	RQLoadNode* _SaveInList(RQRoutaNode* node);

	void _Show(RQRoutaNode* node);
private:
	///RQMap<unsigned char, RQLoadNode*>* _table_type;			//类型查询表
	RQMap<unsigned char, std::pair<RQList*,RQSpinLock>>* _table_type;			//类型查询表
	RQMap<int,RQMap <short, RQLoadNode*>>* _table_addr;		//地址查询表
	RQList* _saveList;										//存储链表
};