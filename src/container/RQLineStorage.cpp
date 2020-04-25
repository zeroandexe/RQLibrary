#include "container/RQLineStorage.h"
#include "container/RQIteratorProxy.h"

typedef void(*ErrFun_Type)();
#define CALL_ERR()	((ErrFun_Type)0)()


RQLineStorage::RQLineStorage()
{
	_proxy = new RQIteratorProxy(this);
}


RQLineStorage::~RQLineStorage()
{
	delete _proxy;
}

void RQLineStorage::push_back(RQNode*) 
{  
	CALL_ERR();
}
 RQNode* RQLineStorage::pop_back() 
 { 
	 CALL_ERR();
	 return 0;
 }

 void RQLineStorage::push_front(RQNode*) 
 {
	 CALL_ERR();
 }
 RQNode* RQLineStorage::pop_front() 
 {
	 CALL_ERR(); 
	 return 0; 
 }


 void RQLineStorage::Clear() {
	 CALL_ERR();
 }

 unsigned int RQLineStorage::GetCount() 
 { 
	 CALL_ERR();
	 return 0;
 }

 void RQLineStorage::Remove(RQNode *)
 {
	 CALL_ERR();
 }

 void RQLineStorage::removeAll() 
 {
	 CALL_ERR();
 }

RQLock* RQLineStorage::getLock() 
{ 
	CALL_ERR();
	return 0;
}