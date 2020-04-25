#include "server/pack/RQPackHead.h"
#include "RQSys.h"
#include "define/RQdefine.h"


void RQPackHead::Hton(RQPackHead * packHead)
{
	packHead->_optProtocol = RQHton(packHead->_optProtocol);
	packHead->_optDecorate = RQHton(packHead->_optDecorate);
	packHead->_serialNumber = RQHton(packHead->_serialNumber);
	packHead->_rspNumber = RQHton(packHead->_rspNumber);
	packHead->_dataLen = RQHton(packHead->_dataLen);
}

void RQPackHead::Ntoh(RQPackHead * packHead)
{
	packHead->_optProtocol = RQNtoh(packHead->_optProtocol);
	packHead->_optDecorate = RQNtoh(packHead->_optDecorate);
	packHead->_serialNumber = RQNtoh(packHead->_serialNumber);
	packHead->_rspNumber = RQNtoh(packHead->_rspNumber);
	packHead->_dataLen = RQNtoh(packHead->_dataLen);
}

bool RQPackHead::CheckSumIsoK(RQPackHead * packHead)
{
	return !_SumPackHead(packHead);
}

bool RQPackHead::CheckDataIsOk(RQPackHead * packHead)
{
	return packHead->_checkData == _EndData(packHead);
}

bool RQPackHead::CheckHeadIsOk(RQPackHead * packHead)
{
	return M_PACKHEAD_HEAD_CHAR == ((char*)packHead)[0];
}

void RQPackHead::AdjustHeadChar(RQPackHead * packHead)
{
	packHead->_headChar = M_PACKHEAD_HEAD_CHAR;
}

void RQPackHead::AdjustCheckSum(RQPackHead* packHead)
{
	packHead->_checkSum = 0;
	packHead->_checkSum = 0 - _SumPackHead(packHead);
}

void RQPackHead::AdjustCheckData(RQPackHead * packHead)
{

	packHead->_checkData = _EndData(packHead);
}

void RQPackHead::SetZero(RQPackHead * packHead)
{
	packHead->_headChar = 0;
	packHead->_optProtocol = 0;
	packHead->_optDecorate = 0;
	packHead->_serialNumber = 0;
	packHead->_rspNumber = 0;
	packHead->_checkSum = 0;
	packHead->_checkData = 0;
	packHead->_dataLen = 0;
}

unsigned char RQPackHead::_SumPackHead(RQPackHead * packHead)
{
	unsigned char sum = 0;
	unsigned char* ptr = (unsigned char*)packHead;

	for (int i = 0; i < sizeof(RQPackHead); i++)
	{
		sum += ptr[i];
	}
	return sum;
}

unsigned int RQPackHead::_EndData(RQPackHead* packHead)
{
	///unsigned int dataLen = RQNtoh(packHead->_dataLen);
	unsigned int* ptr = (unsigned int*)((char*)packHead + (packHead->_dataLen - 4));
	return *ptr;
}