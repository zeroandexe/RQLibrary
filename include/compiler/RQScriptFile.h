#pragma once
#include "define/RQInterDefine.h"
#include "node/RQNode.h"
#include "tool/lock/RQLock.h"
//#include <map>
#include "RQMap.h"
/*
	该对象是脚本文件对象，用于按顺序装载脚本中解析后的元词组。
	并且在装载时对各行元词组进行跳转连接，用于代码流程控制
	通常代码都是加载完毕后才继续运行，所以下方的行操作下标只使用一个变量。
	第一行对应行下标为1.
*/

class RQSentence;				//脚本语句对象
class RQCodeSet;
class RQVar;

class RQScriptFile : public RQNode
{
public:
	RQScriptFile();
	~RQScriptFile();

	virtual void Create(void* param = 0);
	virtual void Clear();

	
	//设置在脚本存储表中，该脚本对象所在的索引位置
	void SetIndexInMap(RQScriptFile** indexPtr);
	
	//获取该对象在脚本存储表中的位置
	RQScriptFile** GetIndexInMap();
	
	/*
		重置脚本对象，但不回收局部变量表中的变量
	*/
	void Reset();

	//根据时间的高位和低位更新时间
	void SetTime(unsigned long highTime, unsigned long lowTime);

	//获取该对象的最新时间
	std::pair<unsigned long, unsigned long> GetTime();

	//本次脚本运行的返回值
	RQVar* GetExp();

	//获取脚本对象内部的暂存器
	void* GetTsId();

	//获取脚本对象的局部变量表
	///std::map<std::string, RQVar*>* GetLocalMap();
	RQMap<std::string, RQVar*>* GetLocalMap();

	//向脚本对象中添加一行语句
	void AddSentence(RQSentence* sentence);

	//重置获取行到第一条表达式
	void ResetHeadLine();

	//获取脚本对象内部的行代码集合
	RQCodeSet* GetCodeSet();
protected:
	void _ClearLocalMap();				//清理本对象的局部变量表
private:
	RQScriptFile**	_indexInMap;		//保存脚本对象在脚本变量表中的存储节点位
	std::pair<unsigned long, unsigned long>	_time;	//文件加载时文件的修改时间
	///std::map<std::string, RQVar*>*	_localmap;	//脚本对象的局部变量表
	RQMap<std::string, RQVar*>* _localmap;
	void*	_tsId;
	RQVar*	_exp;				//脚本对象独有的寄存器变量，通常作为脚本运行结束对外界的返回值
	RQCodeSet*	_codeSet;		//脚本中的语句集合
};