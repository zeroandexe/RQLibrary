#include "compiler/RQGrammar.h"
#include "compiler/RQWord.h"
#include "RQDBStack.h"
#include "define/RQInterDefine.h"
#include "define/RQASTDefine.h"
#include "compiler/RQASTNode.h"
#include "RQStrTool.h"
#include "compiler/RQSymbolInfo.h"
#include "RQStack.h"
#include "RecycleBin.h"
#include "RQCompiler.h"
#include "compiler/RQScriptFile.h"
#include "compiler/RQSentence.h"
#include "compiler/RQExpression.h"
#include "RQSpinLock.h"
#include "RQVar.h"
#include "compiler/RQEnviVarTable.h"
#include "RQMath.h"

typedef RQGrammar::Type_SymbolFun Type_SymbolFun;
Type_SymbolFun* RQGrammar::_symbolFunArray;

RQMap<std::string, RQSymbolInfo>* RQGrammar::_symbolMap = 0;

RQGrammar::RQGrammar(RQScriptFile* scriptObj, void* junkTsId)
{
	_localMap = scriptObj->GetLocalMap();		//获取脚本对象独有的局部变量表
	_exp = scriptObj->GetExp();					//获取解释器寄存器变量
	_scriptTsId = scriptObj->GetTsId();			//获取脚本对象中存储有效对象的暂存器
	_junkTsId = junkTsId;						//获取临时对象暂存器

	_paramStack[MAX_PARAM_PER_FUN] = 0;
	_paramCount = 0;
}

RQGrammar::~RQGrammar()
{
}

//脚本变量获取
RQVar * RQGrammar::_GetFreeVar()
{
	RQVar* retVar = RecycleBin::GetFreeRQVar();
	RecycleBin::TemStorage(_scriptTsId, retVar);	///由于变量对象和脚本对象的生成期一样，所以应当存入脚本的暂存器
	return retVar;
}

//语法树节点获取
RQASTNode * RQGrammar::_GetFreeRQASTNode()
{
	RQASTNode* node = RecycleBin::GetFreeRQASTNode();		
	RecycleBin::TemStorage(_junkTsId,node);		///由于语法树节点的生存期仅在语法树的建立，所以应当存放再临时对象存储器
	return node;
}

RQExpression * RQGrammar::_GetFreeExpression()
{
	RQExpression* expression = RecycleBin::GetFreeRQExpression();
	RecycleBin::TemStorage(_scriptTsId, expression);		///由于语法树节点的生存期仅在语法树的建立，所以应当存放再临时对象存储器
	return expression;
}

//语句对象获取
RQSentence * RQGrammar::_GetFreeSentence()
{
	RQSentence* retSentence = RecycleBin::GetFreeRQSentence();
	RecycleBin::TemStorage(_scriptTsId, (RQNode*)retSentence);	///由于语句对象和脚本对象的生成期一样，所以应当存入脚本的暂存器
	return retSentence;
}


//符号表创建
void RQGrammar::_InitSymbolInfoMap()
{

	if (_symbolMap)
	{
		return;
	}
#define M_INIT_SYMBOL(symbol_str,token,priority)	\
{													\
	RQSymbolInfo& info = (*_symbolMap)[std::string(symbol_str)];			\
	info._token = token;							\
	info._priority = priority;						\
}
	///_symbolMap = new std::map<std::string, RQSymbolInfo>();
	_symbolMap = new RQMap<std::string, RQSymbolInfo>();

	//逗号运算符
	M_INIT_SYMBOL(",", M_SYMBOL_TOKEN_COMMA, M_SYMBOL_PRIORITY_COMMA);

	//赋值运算
	M_INIT_SYMBOL("=", M_SYMBOL_TOKEN_EQUAL, M_SYMBOL_PRIORITY_EQUAL);
	M_INIT_SYMBOL("+=", M_SYMBOL_TOKEN_ADD_EQUAL, M_SYMBOL_PRIORITY_ADD_EQUAL);
	M_INIT_SYMBOL("-=", M_SYMBOL_TOKEN_SUB_EQUAL, M_SYMBOL_PRIORITY_SUB_EQUAL);
	M_INIT_SYMBOL("*=", M_SYMBOL_TOKEN_MULIT_EQUAL, M_SYMBOL_PRIORITY_MULIT_EQUAL);
	M_INIT_SYMBOL("/=", M_SYMBOL_TOKEN_DIV_EQUAL, M_SYMBOL_PRIORITY_DIV_EQUAL);
	M_INIT_SYMBOL("%=", M_SYMBOL_TOKEN_MOLD_EQUAL, M_SYMBOL_PRIORITY_MOLD_EQUAL);

	//四则运算
	M_INIT_SYMBOL("+", M_SYMBOL_TOKEN_ADD, M_SYMBOL_PRIORITY_ADD);
	M_INIT_SYMBOL("-", M_SYMBOL_TOKEN_SUB, M_SYMBOL_PRIORITY_SUB);
	M_INIT_SYMBOL("*", M_SYMBOL_TOKEN_MULIT, M_SYMBOL_PRIORITY_MULIT);
	M_INIT_SYMBOL("/", M_SYMBOL_TOKEN_DIV, M_SYMBOL_PRIORITY_DVI);

	//取模运算
	M_INIT_SYMBOL("%", M_SYMBOL_TOKEN_MOLD, M_SYMBOL_PRIORITY_MOLD);

	//比较运算
	M_INIT_SYMBOL("<", M_SYMBOL_TOKEN_LESS, M_SYMBOL_PRIORITY_LESS);
	M_INIT_SYMBOL(">", M_SYMBOL_TOKEN_MORE, M_SYMBOL_PRIORITY_MORE);
	M_INIT_SYMBOL("==", M_SYMBOL_TOKEN_EQUAL_EQUAL, M_SYMBOL_PRIORITY_EQUAL_EQUAL);
	M_INIT_SYMBOL(">=", M_SYMBOL_TOKEN_MORE_EQUAL, M_SYMBOL_PRIORITY_MORE_EQUAL);
	M_INIT_SYMBOL("<=", M_SYMBOL_TOKEN_LESS_EQUAL, M_SYMBOL_PRIORITY_LESS_EQUAL);
	M_INIT_SYMBOL("!=", M_SYMBOL_TOKEN_NON_EQUAL, M_SYMBOL_PRIORITY_NON_EQUAL);


	//添加逻辑运算符
	M_INIT_SYMBOL("||", M_SYMBOL_TOKEN_OR, M_SYMBOL_PRIORITY_OR);
	M_INIT_SYMBOL("&&", M_SYMBOL_TOKEN_AND, M_SYMBOL_PRIORITY_AND);
	M_INIT_SYMBOL("!", M_SYMBOL_TOKEN_NON, M_SYMBOL_PRIORITY_NON);

	//添加小括号
	M_INIT_SYMBOL("(", M_SYMBOL_TOKEN_SMALL_LEFT_PARENTHESES, M_SYMBOL_PRIORITY_SMALL_LEFT_PARENTHESES);
	M_INIT_SYMBOL(")", M_SYMBOL_TOKEN_SMALL_RIGHT_PARENTHESES, M_SYMBOL_PRIORITY_SMALL_RIGHT_PARENTHESES);


	//添加大括号
	M_INIT_SYMBOL("{", M_SYMBOL_TOKEN_BIG_LEFT_PARENTHESES, M_SYMBOL_PRIORITY_BIG_LEFT_PARENTHESES);
	M_INIT_SYMBOL("}", M_SYMBOL_TOKEN_BIG_RIGHT_PARENTHESES, M_SYMBOL_PRIORITY_BIG_RIGHT_PARENTHESES);

	//添加.
	M_INIT_SYMBOL(".", M_SYMBOL_TOKEN_POINT, M_SYMBOL_PRIORITY_POINT);

	//添加@
	M_INIT_SYMBOL("@", M_SYMBOL_TOKEN_AT, M_SYMBOL_PRIORITY_AT);

}

//元词转脚本变量
RQVar * RQGrammar::_wordToVar(RQWord * word)
{
	RQVar* val = M_NULL(RQVar*);
	RQVar* enviVar;
	switch (word->_type)
	{
	case M_WORD_TYPE_NAME:
		///按照变量名在环境变量名中查找，若未找到则在局部变量名中查找，
		///若仍未找到，那么执行失败
		///enviVar = _enviTable->GetVar(word->_buff);
		enviVar = RQEnviVarTable::FindVar(word->_buff);
		val = (*_localMap)[word->_buff];
		if (!val)									///检测在所有符号表中是否不存在该变量
		{
			val = _GetFreeVar();
			val->SetName(word->_buff);				///在变量对象中设置脚本中变量名称
			(*_localMap)[word->_buff] = val;	///非字面量都将存入局部变量表，方便以后使用
		}
		if (enviVar&&val!=enviVar)
		{
			val->Assign(enviVar);
		}

		break;
	case M_WORD_TYPE_VAL_INT:						///元词为数字则将先将字符中的数字提转换为整数，然后将其再所要一个变量对象，并将数字存储再变量中
		double  num;
		num = RQStrToDouble(word->_buff,word->_len);			///由于元词是数字字面量，所以转换成数字变量
		val = _GetFreeVar();						///由于数字变量不是命名变量，所以申请一个新空间
		val->SetDouble(num);
		break;
	case M_WORD_TYPE_VAL_STR:						
		val = _GetFreeVar();
		val->SetStr(word->_buff, word->_len);
		break;
	}

	return val;
}

//元词转语法树节点
RQASTNode * RQGrammar::_wordToAstNode(RQWord * word)
{
	RQSymbolInfo& info = (*_symbolMap)[std::string(word->_buff)];

	RQASTNode* retNode = 0;
	if (M_SYMBOL_TOKEN_NULL != info._token)			///检查是否是支持的符号
	{
		retNode = _GetFreeRQASTNode();				///获取一个标签节点，在以下将使用该节点的左子节点作为根节点
		retNode->_priority = info._priority;		///将符号特性赋予语法节点
		retNode->_token = info._token;
	}
	
	return retNode;
}

//根据元词创建语法树
RQASTNode * RQGrammar::_CreateASTTree(RQWord ** wordArray)
{
	bool createIsOk = true;
	RQASTNode* root = _GetFreeRQASTNode();
	RQASTNode* startNode = M_NULL(RQASTNode*);
	RQASTNode* node = 0;
	unsigned int detal_priority = 0;	///当前优先级增量，用于控制树生成时对运算符优先性进行控制
	RQASTNode* lastNode = 0;
	for (int i = 0; wordArray[i]; i++)
	{
		//根据元词构建一个抽象语法树节点	
		lastNode = node;
		node = M_NULL(RQASTNode*);
		if (M_WORD_TYPE_SYMBOL == wordArray[i]->_type)
		{
			//元词是运算符，并且为非小括号形式可直接转换为语法树节点
			node = _wordToAstNode(wordArray[i]);

			//检测当前符号是否是‘(’或‘)’，两者不进入语法树，‘（’=1；‘）’=-1，其他为0，两则不可能同时存在
			int is = !(M_SYMBOL_TOKEN_SMALL_LEFT_PARENTHESES - node->_token) * 1
				+ !(M_SYMBOL_TOKEN_SMALL_RIGHT_PARENTHESES - node->_token)*-1;
			if (is)
			{
				detal_priority += M_SYMBOL_PRIORITY_SMALL_PARENTHESES * is;			//根据特征值对当前优先级增量进行计算
				if (M_SYMBOL_TOKEN_SMALL_RIGHT_PARENTHESES == node->_token)			//右括号不入
				{
					continue;
				}
				//continue;		//跳过该符号，不进行入树操作
			}

		}
		else {	//非符号元词具有变量特性，应先转换为变量再进行节点转换
			RQVar* var = _wordToVar(wordArray[i]);				//获取当前元词的字面量

			if (var)											//防止不合法变量存在
			{
				//构建变量语法节点
				node = _GetFreeRQASTNode();
				node->_token = M_SYMBOL_TOKEN_VAR;
				node->_priority = M_SYMBOL_PRIORITY_VAR;
				if (M_VAR_TYPE_FUN == var->GetType())
				{
					node->_token = M_SYMBOL_TOKEN_FUN;
					node->_priority = M_SYMBOL_PRIORITY_FUN;
				}
				node->_value = var;
			}
		}
		//检测是否生成了语法树节点，生成了则将该节点加入到语法树中，
		//若未生成则认为元词组有问题，回收语法树并返回空根节点表示失败
		if (node)
		{
			node->_priority += detal_priority;					//根据当前的括号情况，调整当前运算符节点的优先级
			startNode = _InsertNewNodeToTree(root, startNode, node);			//进入新节点到语法树中，并返回下次语法起点
			if (!startNode)				//检测节点加入语法树时是否失败，
			{
				goto ErrReturn;			//失败跳转失败处理
			}
		}
		else {						//节点有效则加入到语法树中
		ErrReturn:
			root = (RQASTNode*)0;				//置空根节点标识，语法树生成失败
			break;
		}
	}

	if (!createIsOk)
	{
		root = M_NULL(RQASTNode*);
	}

	return root;
}

//将传入的节点添加入语法树
RQASTNode * RQGrammar::_InsertNewNodeToTree(RQASTNode * root, RQASTNode * startNode, RQASTNode * newNode)
{
	static long* ifFunAddr = (long*)RQEnviVarTable::GetVar("if")->GetFun();
	static long* whileFunAddr = (long*)RQEnviVarTable::GetVar("while")->GetFun();
	static long* elseFunAddr = (long*)RQEnviVarTable::GetVar("else")->GetFun();

	//更换父节点的子节点
#define M_CHANGE_SON(parent,oldSon,newSon)		\
{												\
	if(parent->_left == oldSon)					\
	{											\
		parent->_left = newSon;					\
	}											\
	else {										\
		parent->_right = newSon;				\
	}											\
	newSon->_parent = parent;					\
}

	/*
	parent：父节点
	branches：父节点左分支或右分支
	son：子节点
	*/
#define M_BIND_PARENT_SON(parent,branches,son)		\
{													\
	branches = son;									\
	son->_parent = parent;							\
}
	///之前的设备的设计是左小括号不入
	if (newNode->_token == M_SYMBOL_TOKEN_SMALL_LEFT_PARENTHESES)		
	{
		if (startNode)
		{ 
			int isNo = (startNode->_token - M_SYMBOL_TOKEN_FUN)
				*(startNode->_parent->_token - M_SYMBOL_TOKEN_POINT);
			if (!isNo)
			{
				if (startNode->_parent->_token == M_SYMBOL_TOKEN_POINT)
				{
					startNode = startNode->_parent;
				}
				startNode->_priority += M_SYMBOL_PRIORITY_SMALL_PARENTHESES;
				newNode->_priority -= M_SYMBOL_PRIORITY_SMALL_PARENTHESES;		///将小括号的优先级还原到当前级数值
				M_CHANGE_SON(startNode->_parent, startNode, newNode);
				M_BIND_PARENT_SON(newNode, newNode->_left, startNode);
				startNode = newNode;
			}
		}
		return startNode;
	}

	if (!startNode)				//起点为空，表示整个树都未建立
	{
		M_BIND_PARENT_SON(root, root->_left, newNode);				//将子节点绑定在父节点的左分支上

		long* newNodeFunAddr = 0;
		RQVar* nodeValue = newNode->_value;
		if (nodeValue && nodeValue->GetType() == M_VAR_TYPE_FUN)
		{
			newNodeFunAddr = (long*)newNode->_value->GetFun();
		}

		///由于目前涉及到的关键符号都是语句中的第一个词源，所以在此对第一个节点进行检测计算，得出这个语法树的特征量（只会满足其中一个）
		root->_priority = !(newNodeFunAddr - ifFunAddr)*M_KEY_LABEL_IF	///计算该语法第一个字符节点是否是if,若是则标记为if语法行
			+ !(newNodeFunAddr - whileFunAddr)*M_KEY_LABEL_WHILE						///同理 while
			+ !(newNodeFunAddr - elseFunAddr)*M_KEY_LABEL_ELSE							/// 同理 else
			+ !(M_SYMBOL_TOKEN_BIG_LEFT_PARENTHESES - newNode->_token)*M_KEY_LABEL_BIG_LEFT_PARENTHESES			//或则是否是左括号
			+ !(M_SYMBOL_TOKEN_BIG_RIGHT_PARENTHESES - newNode->_token)*M_KEY_LABEL_BIG_RIGHT_PARENTHESES;		//或者是否是右括号

		if (!root->_priority)		///检测该语法是否已被标记，若未被标记则无效关键标识，标识这是一个非特殊含义语法
		{
			root->_priority = M_KEY_LABEL_INVALID;
		}

		return newNode;
	}

	RQASTNode* newStartNode = M_NULL(RQASTNode*);			//声明定义用于返回新起点的标记节点


															//检测起点是变量节点，新节点是符号节点：后续需要继续处理，
	int is = !(M_SYMBOL_TOKEN_VAR - startNode->_token)		//计算起点是否是变量节点
		*(M_SYMBOL_TOKEN_VAR - newNode->_token);			//计算新节点不是变量节点（运算符节点）

	if (is)
	{
		//由于父节点可能储存在符号节点

		//将新节点替换旧节点与父节点的关联
		M_CHANGE_SON(startNode->_parent, startNode, newNode);
		//将旧节点作为新节点的左子节点
		M_BIND_PARENT_SON(newNode, newNode->_left, startNode);
		//将新节点作为新起点
		newStartNode = newNode;

		//更新起点用于后续符号节点的继续比较
		startNode = newNode->_parent;
	}

	is = !(M_SYMBOL_TOKEN_FUN - startNode->_token)			//检测起点是否是函数节点
		*(!(M_SYMBOL_TOKEN_VAR - newNode->_token)
			+ !(M_SYMBOL_TOKEN_NON - newNode->_token));			//检测新节点是否是!或者变量，其他符号不予加入

	if (is)
	{
		int magnPriority = newNode->_priority / M_SYMBOL_PRIORITY_SMALL_PARENTHESES;			//计算当前优先级倍率
																								//startNode->_priority = M_SYMBOL_TOKEN_FUN + magnPriority * M_SYMBOL_PRIORITY_SMALL_PARENTHESES;		//计算当前函数节点的最新优先级
																								//由于是刚转换成的函数节点所以，不存在子节点，将新节点与函数节点进行连接
		M_BIND_PARENT_SON(startNode, startNode->_right, newNode);
		newStartNode = newNode;
	}

	//检测起点是符号节点，新节点是变量节点
	is = (M_SYMBOL_TOKEN_VAR - startNode->_token)			//计算起点不是变量（运算符节点）
		* !(M_SYMBOL_TOKEN_VAR - newNode->_token);				//计算新节点是否是变量
	if (is)
	{
		//检测起点的左孩子是否存在，若不存在则将新节点挂载到左子节点，若存在则将子节点挂载到右子节点。
		//RQASTNode** child = startNode->_left ? &(startNode->_right) : &(startNode->_left);
		//M_BIND_PARENT_SON(startNode, *child, newNode);
		//操作直接将新节点连接上起点的右分支
		M_BIND_PARENT_SON(startNode, startNode->_right, newNode);
		//将新节点作为新起点
		newStartNode = newNode;
	}

	//检测起点是符号节点，新节点也是符号节点
	bool isContinue = true;
	while (isContinue)
	{
		isContinue = false;
		is = (M_SYMBOL_TOKEN_VAR - startNode->_token)			//检测起点是否不是变量节点（运算符节点）
			*(M_SYMBOL_TOKEN_VAR - newNode->_token)				//检测新节点是否不是变量节点（运算符节点）
			*(M_SYMBOL_TOKEN_NULL - startNode->_token);			//检测起点是否不是根节点
		if (!is)	//检测是否不满足运算符比较条件
		{
			break;
		}

		//比较符号优先级，新节点优先级>起点优先级，则进行右存放
		int isOk = ((newNode->_priority - startNode->_priority) > 0)		//检测新节点优先级是否高于旧节点优先级
			+ (!(newNode->_priority - startNode->_priority)					//计算起点节点和新节点优先级是否相同
				*(!(newNode->_priority - M_SYMBOL_PRIORITY_EQUAL) + !(M_SYMBOL_TOKEN_COMMA - newNode->_token)));
		//检测新节点优先级是否和旧节点优先级一样，若一样新节点是否是等号级优先级，
		//或者新旧起点都是逗号运算符若是则不予排序
		if (isOk)
		{
			M_BIND_PARENT_SON(startNode, startNode->_right, newNode);
			newStartNode = newNode;
		}
		else {			//若新节点优先级<起点优先级，则进行回溯比较
						//先将新节点与起点父节点绑定

						//将新节点的左子节点存入起点的右子节点
						//startNode->_right = newNode->_left;
			M_BIND_PARENT_SON(startNode, startNode->_right, newNode->_left);

			M_CHANGE_SON(startNode->_parent, startNode, newNode);

			//将新节点作为起点的父节点,起点为新节点的左子节点
			M_BIND_PARENT_SON(newNode, newNode->_left, startNode);

			//将新节点的父节点作为起点，继续回溯
			startNode = newNode->_parent;
			isContinue = true;
		}
	}
	return newStartNode;
}

//分析语法树获取子表达式
void RQGrammar::_AnalyszASTTree(RQASTNode * root, RQSentence * sentence)
{
	//选择遍历起点
	RQASTNode* currentNode = root->_left;

	int is;					//用于在布尔条件中最为判断条件的依据

	//开始遍历并分析语法树
	bool isContinue = true;
	while (isContinue) {

		//步骤1：获取左子树中第一个叶子节点
		while (true)
		{
			///查找当前左子节点支路没有左子节点的节点
			while (currentNode->_left) currentNode = currentNode->_left;
			///扫描当前节点的右支路
			if (currentNode->_right)
			{
				///当前节点具有右子节点，所以不是叶节点，将右子节点设置为当前节点，并再次进行查找
				currentNode = currentNode->_right;	
			}
			else {
				break;					///当前节点没有右子节点，所以完成叶节点的查找
			}
		}
		//跳出循环，表示当前节点是叶节点


		///声明当前叶节点的兄弟节点和父节点
		while (true)
		{
			///获取当前叶节点的父节点
			RQASTNode* parent = currentNode->_parent;			
			///获取当前叶节点的兄弟节点
			RQASTNode* brother = parent->_left == currentNode ? parent->_right : parent->_left;		

			///检查当前节点是否时函数节点，保护无参函数的调用
			///is = !(M_SYMBOL_TOKEN_FUN - currentNode->_token);
			//检查是否是小括号调用
			is = !(M_SYMBOL_TOKEN_SMALL_LEFT_PARENTHESES - currentNode->_token);

			if (is)
			{
				parent = currentNode;
			}
			///if (brother && M_SYMBOL_TOKEN_FUN == brother->_token)
			if (brother && M_SYMBOL_TOKEN_SMALL_LEFT_PARENTHESES == brother->_token)
			{
				parent = brother;
			}

			//步骤2：进行父节点特征检测检测（假设这是一颗正确的树）
			///父节点是根节点则，则表示语法树已得出结果（语法树中除根外只有一个语法节点,对于该节点若是函数节点则需要继续执行）
			if (M_SYMBOL_TOKEN_NULL == parent->_token)
			{
				isContinue = false;						//树已处理完毕，无需进行循环处理
				break;
			}

			/// ||和&&运算符是二元表达式运算符，和二元操作数运算符不一样，他们可以在具有右子树的时候进行运算。
			is = !(M_SYMBOL_TOKEN_AND - parent->_token)				///检测父节点是否是||运算符
				+ !(M_SYMBOL_TOKEN_OR - parent->_token);			///检测父节点是否是&&运算符

			RQVar* result=0;
			if (!is)
			{
				//父节点是非根节点，则检查同胞右子节点是否有子节点
				//有则再次进入循环进行查找
				//同胞右子节点不存在，同胞右子节点是叶节点，都进入运算符运算状态
				if (brother && (brother->_left || brother->_right))
				{
					currentNode = brother->_left ? brother->_left : brother->_right;				//将兄弟节点具有的子节点作为下次处理的起点，继续处理
					break;		//退出循环，兄弟节点还存在子节点
				}

				//此处为检测后的执行阶段，此时当前父节点可能为叶节点（函数），也可能为常规节点（运算符）
				result = (this->*_symbolFunArray[parent->_token])(parent, sentence);
			}
			else {
				///本次运算的运算符是and和or,将其进行变体组装子表达式
				result = (this->*_symbolFunArray[parent->_token])(parent,sentence);
				parent = parent->_left;
			}

			if (result)			///检查是否存在需替换的值，即将当前节点变体为变量节点
			{
				parent->_token = M_SYMBOL_TOKEN_VAR;
				parent->_value = result;
				parent->_left = M_NULL(RQASTNode*);
				parent->_right = M_NULL(RQASTNode*);
			}

			currentNode = parent;
		}
	}
}

//分析传入的元词组，获取子表达式
RQSentence* RQGrammar::AnalyszWords(RQWord** wordArray)
{
	RQSentence* sentence = 0;				///初始化0下标元素，表示目前未获取到语句端
	RQASTNode* root = _CreateASTTree(wordArray);		///创建语法树
	if (root)							///检查语法数创建是否成功
	{
		sentence = _GetFreeSentence();
		sentence->_cscLabel = root->_priority;			///将语法树解析出来的语句特征交给语句对象
		_AnalyszASTTree(root, sentence);			///根据语法树获取子表达式
		sentence->SetResult(root->_left->_value);					///设置语句的返回值，用于if-while-else等程序流的控制
	}
	return sentence;
}

void RQGrammar::InitObj()
{
	static bool needInit = true;
	if (needInit)
	{
		_InitSymbolFunArray();
		_InitSymbolInfoMap();
		needInit = false;
	}
}

typedef RQVar* (RQGrammar::*Type_SymbolFun)(RQASTNode*, RQSentence* sentence);
void RQGrammar::_InitSymbolFunArray()
{
	if (_symbolFunArray)
	{
		return;
	}

	_symbolFunArray = new Type_SymbolFun[M_SYMBOL_TOKEN_TOTAL];

	//添加空节点的函数处理
	_symbolFunArray[M_SYMBOL_TOKEN_NULL] = &RQGrammar::SymbolFun_Null;

	//添加赋值操作
	//=
	_symbolFunArray[M_SYMBOL_TOKEN_EQUAL] = &RQGrammar::SymbolFun_Equal;		//添加=处理函数
	_symbolFunArray[M_SYMBOL_TOKEN_ADD_EQUAL] = &RQGrammar::SymbolFun_Equal;	//+=
	_symbolFunArray[M_SYMBOL_TOKEN_SUB_EQUAL] = &RQGrammar::SymbolFun_Equal;	//-=
	_symbolFunArray[M_SYMBOL_TOKEN_MULIT_EQUAL] = &RQGrammar::SymbolFun_Equal;	//*=
	_symbolFunArray[M_SYMBOL_TOKEN_DIV_EQUAL] = &RQGrammar::SymbolFun_Equal;	///=
	_symbolFunArray[M_SYMBOL_TOKEN_MOLD_EQUAL] = &RQGrammar::SymbolFun_Equal;

	//添加四则运算
	_symbolFunArray[M_SYMBOL_TOKEN_ADD] = &RQGrammar::SymbolFun_Com;			//+
	_symbolFunArray[M_SYMBOL_TOKEN_SUB] = &RQGrammar::SymbolFun_Com;			//-
	_symbolFunArray[M_SYMBOL_TOKEN_MULIT] = &RQGrammar::SymbolFun_Com;			//*
	_symbolFunArray[M_SYMBOL_TOKEN_DIV] = &RQGrammar::SymbolFun_Com;			// /
	_symbolFunArray[M_SYMBOL_TOKEN_MOLD] = &RQGrammar::SymbolFun_Com,			//%


																			//添加比较处理
	_symbolFunArray[M_SYMBOL_TOKEN_LESS] = &RQGrammar::SymbolFun_Com;			//<
	_symbolFunArray[M_SYMBOL_TOKEN_MORE] = &RQGrammar::SymbolFun_Com;			//>
	_symbolFunArray[M_SYMBOL_TOKEN_LESS_EQUAL] = &RQGrammar::SymbolFun_Com;		//<=
	_symbolFunArray[M_SYMBOL_TOKEN_MORE_EQUAL] = &RQGrammar::SymbolFun_Com;		//<=
	_symbolFunArray[M_SYMBOL_TOKEN_EQUAL_EQUAL] = &RQGrammar::SymbolFun_Com;	//==

																			//添加逻辑运算
	_symbolFunArray[M_SYMBOL_TOKEN_AND] = &RQGrammar::SymbolFun_And;		//添加 &&
	_symbolFunArray[M_SYMBOL_TOKEN_OR] = &RQGrammar::SymbolFun_Or;			//添加 ||
	_symbolFunArray[M_SYMBOL_TOKEN_NON] = &RQGrammar::SymbolFun_Com;		//添加 ！
	_symbolFunArray[M_SYMBOL_TOKEN_NON_EQUAL] = &RQGrammar::SymbolFun_Com;
																			//添加逗号运算符
	_symbolFunArray[M_SYMBOL_TOKEN_COMMA] = &RQGrammar::SymbolFun_Comma;	//添加 ,

																			//添加函数调用
	_symbolFunArray[M_SYMBOL_TOKEN_FUN] = &RQGrammar::SymbolFun_Invalid;		//添加函数调用(由于添加了'('所以函数不能直接调用)
	_symbolFunArray[M_SYMBOL_TOKEN_SMALL_LEFT_PARENTHESES] = &RQGrammar::SymbolFun_Fun;		//添加函数调用

																			//添加大括号
	_symbolFunArray[M_SYMBOL_TOKEN_BIG_LEFT_PARENTHESES] = &RQGrammar::SymbolFun_LeftBrace;		// {
	_symbolFunArray[M_SYMBOL_TOKEN_BIG_RIGHT_PARENTHESES] = &RQGrammar::SymbolFun_Invalid;	// }

	_symbolFunArray[M_SYMBOL_TOKEN_AND_TO_EQUAL] = &RQGrammar::SymbolFun_AndToEqual;	//添加and变体处理
	_symbolFunArray[M_SYMBOL_TOKEN_OR_TO_EQUAL] = &RQGrammar::SymbolFun_OrToEqual;		//添加or变体处理

	_symbolFunArray[M_SYMBOL_TOKEN_POINT] = &RQGrammar::SymbolFun_Point;	//添加. 处理

	_symbolFunArray[M_SYMBOL_TOKEN_AT] = &RQGrammar::SymbolFun_AT;			//添加@ 处理
}


//该函数供and和or插入if表达式
void InsertIfExpression(RQSentence* sentence, RQVar* param, void* scriptTsId,RQVar* exp)
{
	static RQVar* ifVar = RQCompiler::GetEnviVar("if");		//由于后续表达式都将使用到if变量，所以设置为静态
	RQExpression* expression = RecycleBin::GetFreeRQExpression();
	RecycleBin::TemStorage(scriptTsId, expression);
	expression->_cscLabel = M_KEY_LABEL_IF;				//表明该表达式是if表达式
	expression->_braceType = M_BRACE_TYPE_LEFT;			//目前操作设定if-while行具有左括号
	expression->_funAddr = ifVar;						//设置if
	expression->_result = exp;
	expression->_optToken = M_SYMBOL_TOKEN_FUN;		//设置表示当前为函数操作
	expression->_varArray[M_VARARRAY_INDEX_EXP] = exp;
	expression->_varArray[M_VARARRAY_INDEX_PARAM_START] = param;	//将and的左叶的值作为if的函数地址
	expression->_varArray[M_VARARRAY_INDEX_PARAM_START+1] = 0;	//将and的左叶的值作为if的函数地址
	sentence->AddExpression(expression);
}

//在语句对象中插入一个右括号表达式
void InsertBrackRightExpression(RQSentence* sentence, void* scriptTsId)
{
	RQExpression* expression = RecycleBin::GetFreeRQExpression();
	RecycleBin::TemStorage(scriptTsId, expression);
	expression->_braceType = M_BRACE_TYPE_RIGHT;
	expression->_optToken = M_SYMBOL_TOKEN_BIG_RIGHT_PARENTHESES;
	sentence->AddExpression(expression);
}

void InsertComExpression(RQSentence* sentence, RQVar* left, RQVar* right,RQVar* result, int optToken,
							void* scriptTsId,RQVar* funAddr = 0)
{
	RQExpression* expression = RecycleBin::GetFreeRQExpression();
	RecycleBin::TemStorage(scriptTsId, expression);
	expression->_optToken = optToken;			///设置当前操作的特征值
	expression->_left = left;					///设置左操作数
	expression->_right = right;					///设置右操作数
	expression->_result = result;				///设置操作结果返回值
	expression->_funAddr = funAddr;				///设置操作函数
	sentence->AddExpression(expression);		///将表达式存入语句中

}

///*------------------运算符处理函数----------------------*/
//赋值操作
//符号=：将右子节点的数据赋值给左子节点，并将符号节点转换为变量节点
//检测右值是否是寄存器变量，若是寄存器变量则将赋值修改为交换，以便于减少数据拷贝次数
RQVar* RQGrammar::SymbolFun_Equal(RQASTNode* symbolNode, RQSentence* sentence)
{
	RQVar* right = symbolNode->_right->_value;
	int symbolToken = symbolNode->_token;
	if (right == _exp)
	{
		//right = _exp;
		symbolToken = M_SYMBOL_TOKEN_OTHER_SWAP;
	}
	InsertComExpression(sentence,
		symbolNode->_left->_value,
		right,
		symbolNode->_left->_value,
		symbolToken,
		_scriptTsId);

	return symbolNode->_left->_value;
}

//常规运算符表达式解析
RQVar * RQGrammar::SymbolFun_Com(RQASTNode * symbolNode, RQSentence* sentence)
{
	RQVar* result = _GetFreeVar();
	RQVar* left = 0;

	if (symbolNode->_left)				///防止一元参数导致崩溃
	{
		left = symbolNode->_left->_value;
	}

	InsertComExpression(sentence,
		left,
		symbolNode->_right->_value,
		result,
		symbolNode->_token,
		_scriptTsId);

	return result;
}


//逻辑运算处理
//&& ： 在处理这类运算符前需要先向赋予if表达式，然后将节点变体转换为等号表达式
RQVar* RQGrammar::SymbolFun_And(RQASTNode* symbolNode, RQSentence* sentence)
{
	RQVar* left = _GetFreeVar();
	///为了不改变原有值，所以先左赋值操作，将左参数节点拷贝到新值上
	InsertComExpression(sentence, left, symbolNode->_left->_value, left, M_SYMBOL_TOKEN_EQUAL, _scriptTsId);

	///在语句中插入if表达式
	InsertIfExpression(sentence, left, _scriptTsId,_exp);

	///由于左参数计算值已被替换，所以转换为新值存储计算
	symbolNode->_left->_value = left;

	///将当前and节点变体为=节点
	symbolNode->_token = M_SYMBOL_TOKEN_AND_TO_EQUAL;
	///返回空指针表示本次操作不用进行节点替换
	return 0;
}

RQVar * RQGrammar::SymbolFun_AndToEqual(RQASTNode * symbolNode, RQSentence * sentence)
{
	RQVar* left = symbolNode->_left->_value;
	RQVar* right = symbolNode->_right->_value;
	///为了统一之前的转换操作，现在让左参数获取右参数的值
	InsertComExpression(sentence, left, right, left, M_SYMBOL_TOKEN_EQUAL, _scriptTsId);

	///由于and的作用域结束，所以进行附上右花括号
	InsertBrackRightExpression(sentence, _scriptTsId);

	///将当前值转换为对应的布尔值进行处理执行：left = !left
	InsertComExpression(sentence, left, left, left, M_SYMBOL_TOKEN_NON, _scriptTsId);

	///再次执行取非操作，让值变回正常值
	InsertComExpression(sentence, left, left, left, M_SYMBOL_TOKEN_NON, _scriptTsId);

	return left;
}


//||：与and存在区别，先插当前结果的取反表达式，然后将结果作为if表达式值存入，然后将节点变体为复制表达式
RQVar* RQGrammar::SymbolFun_Or(RQASTNode* symbolNode, RQSentence* sentence)
{
	RQVar* oldleft = symbolNode->_left->_value;
	RQVar* newLeft = _GetFreeVar();
	RQVar* result = _GetFreeVar();

	///先将左值进行取非操作，用于查看是否需要后续代码的执行
	InsertComExpression(sentence, newLeft, oldleft, newLeft, M_SYMBOL_TOKEN_NON, _scriptTsId);

	///加入if处理
	InsertIfExpression(sentence, newLeft, _scriptTsId,_exp);

	///由于表达式使用了新的数据变量，所以左子节点的数据变量应当被替换
	symbolNode->_left->_value = newLeft;
	symbolNode->_token = M_SYMBOL_TOKEN_OR_TO_EQUAL;

	return 0;
}

RQVar * RQGrammar::SymbolFun_OrToEqual(RQASTNode * symbolNode, RQSentence * sentence)
{
	RQVar* left = symbolNode->_left->_value;
	RQVar* right = symbolNode->_right->_value;
	
	///为了统一在进入if表达式之前的数据形式，现在将右参数也转换为同等形式
	InsertComExpression(sentence, left, right, left, M_SYMBOL_TOKEN_NON, _scriptTsId);

	///由于or的作用域结束，所以进行附上右花括号
	InsertBrackRightExpression(sentence, _scriptTsId);

	///将结果值反转为正确值
	InsertComExpression(sentence, left, left, left, M_SYMBOL_TOKEN_NON, _scriptTsId);

	return left;
}

//,
RQVar* RQGrammar::SymbolFun_Comma(RQASTNode* symbolNode, RQSentence* sentence)
{
	///由于逗号运算符的右子节点将向上升节，若以仅存入左子节点 
	///_funParamStack->push(symbolNode->_left->_value);		
	_paramStack[MAX_PARAM_PER_FUN - ++_paramCount] = symbolNode->_left->_value;
	return symbolNode->_right->_value;
}
 
//放置根节点的函数调用
RQVar* RQGrammar::SymbolFun_Null(RQASTNode* symbolNode, RQSentence* sentence)
{
	return 0;
}
  
RQVar * RQGrammar::SymbolFun_LeftBrace(RQASTNode * symbolNode, RQSentence * sentence)
{
	return symbolNode->_left->_value;
}

RQVar * RQGrammar::SymbolFun_RightBrace(RQASTNode * symbolNode, RQSentence * sentence)
{
	//InsertBrackRightExpression(sentence, _scriptTsId);
	//return _exp;
	return 0;
}

///创建浮点数
RQVar* RQGrammar::_CreateFloat(RQVar* left,RQVar* right)
{
	RQVar* result = _GetFreeVar();
	double val = left->GetDouble();				///获取浮点数整数部分
	long long  decimal = right->GetLLong();		///获取浮点数小数部分

	int precision = RQIntegerLength(decimal);	///获取小数位数值长度
	val = val + decimal / pow(10, precision);	///计算得到最终值
	result->SetDouble(val);
	return result;
}

RQVar * RQGrammar::SymbolFun_Point(RQASTNode * symbolNode, RQSentence * sentence)
{
	RQVar* leftVar = symbolNode->_left->_value;
	RQVar* rightVar = symbolNode->_right->_value;
	char* rightName = rightVar->GetName();
	RQVar* result = 0;

	if (rightName&&rightName[0])				///检查右参数是否具有名称若具有则该该表达式为成员获取
	{
		result = leftVar->Member(rightName);	///获取leftVar的成员变量
	}
	else {
		result = _CreateFloat(leftVar, rightVar);
	}
	return  result;
 }

/*
	@的作为将全局变量表中的变量引入到局部变量表中，用以避免过多的内存拷贝
*/
RQVar* RQGrammar::SymbolFun_AT(RQASTNode* symbolNode, RQSentence* sentence)
{
	char* enviVarName = symbolNode->_right->_value->GetName();
	RQVar* enviVar = RQEnviVarTable::FindVar(enviVarName);

	if (!enviVar)											///检查该变量是否还未出现在环境变量表中
	{
		enviVar = symbolNode->_right->_value;
		RecycleBin::RemoveFromTs(_junkTsId, enviVar);		///从暂存器中移除该节点，防止脚本对象回收后环境变量表遗失了变量
		RQEnviVarTable::SetVar(enviVarName, enviVar);		///若从未出现过则将变量注册入环境变量
	}
	else {
		enviVar = RQEnviVarTable::GetVar(enviVarName);		///若出现过那么获取环境变量表中的变量，来替换接下来对该名称变量的使用
		(*_localMap)[enviVarName] = enviVar;				///替换已有的脚本变量（这不是泄漏）
	}
	
	return enviVar;											///返回环境变量执行操作
}

/*
由于逗号运算符会保留右子节点作为自身值并向上传递的原因，
*/
RQVar* RQGrammar::SymbolFun_Fun(RQASTNode * symbolNode, RQSentence* sentence)
{

	int paramIndex = M_VARARRAY_INDEX_PARAM_START-1;
	RQExpression* expression = _GetFreeExpression();
	RQVar** varArray = expression->_varArray;

	///设置函数的默认参数
	///varArray[M_VARARRAY_INDEX_LOCALMAP] = (RQVar*)_localMap;					///设置局部变量表
	///varArray[M_VARARRAY_INDEX_ENVIMAP] = (RQVar*)RQCompiler::GetEnviMap();	///设置环境变量表
	varArray[M_VARARRAY_INDEX_EXP] = _exp;										///设置寄存器变量
	varArray[M_VARARRAY_INDEX_PARAM_START] = 0;									///防止空参数调用
	RQCopy(varArray + M_VARARRAY_INDEX_PARAM_START, _paramStack + (MAX_PARAM_PER_FUN - _paramCount), _paramCount * sizeof(void*));

	if (symbolNode->_right)		///检查当前函数是否是带参函数
	{
		int paramIndex = M_VARARRAY_INDEX_PARAM_START + _paramCount;	///是带参函数，填写最后一个参数
		varArray[paramIndex++] = symbolNode->_right->_value;			///将参数数组设置结尾符
		varArray[paramIndex] = 0;								
	}

	expression->_optToken = M_SYMBOL_TOKEN_FUN;
	expression->_funAddr = symbolNode->_left->_value;
	expression->_result = _exp;

	sentence->AddExpression(expression);
	_paramCount = 0;								///将参数个数统计置空，用于下次从新计数
	RQVar* result = _GetFreeVar();
	
	///在函数返回时添加一个新表达式，将寄存器变量的数据拷贝给临时变量，用于提供函数嵌套调用
	InsertComExpression(sentence,
		result,
		_exp,
		result,
		M_SYMBOL_TOKEN_EQUAL,
		_scriptTsId);

	return result;
}


RQVar* RQGrammar::SymbolFun_Invalid(RQASTNode* symbolNode, RQSentence* sentence)
{
	return 0;
}