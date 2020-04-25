#pragma once
#include "node/RQNode.h"

/*
	本类是一个抽象语法树的统一节点
	符号和变量都有特征值。变量的特征值都一样，但是符号会根据符号内容的不同而特征值不同。
	只有符号有优先级，变量没有优先级。
*/


class RQVar;					//脚本变量

class RQASTNode :
	public RQNode
{
public:
	RQASTNode();
	virtual ~RQASTNode();

	virtual void Create(void* param = 0);
	virtual void Clear();

	RQASTNode * _left;				//左子节点
	RQASTNode * _right;				//右子节点
	RQASTNode * _parent;			//父节点
	RQVar* _value;					//当前节点所具备的值
	int _token;			//节点符号特征值
	int _oldToken;		//该节点的旧特征值，供and和or进行变体使用
	int _priority;		//节点优先级
};