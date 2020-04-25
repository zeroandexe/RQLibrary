#pragma once
#include "define/RQdefine.h"
#include "RQList.h"
/*
	该类是先入后出的栈结构
*/

class RQLock;

class RQStack
{
public:
	RQStack(RQLineStorage* lineStorage);
	~RQStack();
	void push(RQNode* node);
	RQNode* pop();
	
	void Clear();

	RQLineStorage* GetStorage();

	static RQStack* Create_list(RQLock* lock=M_NULL(RQLock*));
	static void Destroy(RQStack* stack);

	unsigned int GetCount();
private:
	RQLineStorage * _lineStorage;
};