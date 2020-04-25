#pragma once
/*
	该对象为数据包包头对象
	（成员若为暗注释，则表是当前元素仅保留，但不使用）
*/

#pragma pack(push,1)
class RQPackHead
{
public:
	unsigned char _headChar;	//包头标识字节
	unsigned int _optProtocol;	//本次数据包要求的处理操作
	unsigned int _optDecorate;	//本次操作请求的辅助描述
	unsigned int _serialNumber;	//本次数据包的序列号，回应数据包应和接收到的数据包相同
	unsigned int _rspNumber;	///本次数据报的回应序列号，该值为0的数据包则不要求回应
	unsigned char _checkSum;	//本次数据的效验和，在大端模式下效验
	unsigned int _checkData;	//本次数据包在组包完成后对包尾的4字节效验，要求和网络字节序下的包尾最后4字节数据相同
	unsigned int _dataLen;		//整个数据包的字节长度（包括包头）
public:
	static void Hton(RQPackHead* packHead);			//将包头数据从本机字节序转换为网络字节序
	static void Ntoh(RQPackHead* packHead);			//将包头数据从网络字节序转换为本机字节序
	static bool CheckSumIsoK(RQPackHead* packHead);		//效验包头的字节序情况，忽略字节序均可效验
	static bool CheckDataIsOk(RQPackHead* packHead);	//效验数据包尾数据完整情况，在网络字节序模式下
	static bool CheckHeadIsOk(RQPackHead* packHead);	//验证包头效验字节是否匹配，用于确认当前网络数据是否从正确的包数据开始

	static void AdjustHeadChar(RQPackHead* packHead);		//填写表头效验字节
	/*
		要求在除checkSum外所有成员都已正确设置的情况下调用
	*/
	static void AdjustCheckSum(RQPackHead* packHead);		//自动计算包头的效验和
	//--以下调用在大小端转换后使用
	/*
		为保证数据的一致性，要求验证数据为打断模式，即要求数据段的数据都数据在为网络字节序
		（数据包头未转换）
	*/
	static void AdjustCheckData(RQPackHead* packHead);		//自动计算效验尾数据
	/*
		将数据包头置空
	*/
	static void SetZero(RQPackHead* packHead);
private:
	static unsigned char _SumPackHead(RQPackHead* packHead);		//计算数据包包头的字节和
	static unsigned int _EndData(RQPackHead* packHead);				//用于尾部效验的数据提取
};

#pragma pack(pop)