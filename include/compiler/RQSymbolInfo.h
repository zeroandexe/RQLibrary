#pragma once

/*
	运算符号信息表
*/

class RQSymbolInfo {
public:
	RQSymbolInfo();

	int _token;						//符号特征值
	int _priority;					//符号优先级
};