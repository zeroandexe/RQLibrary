#include "server/routa/RQRoutaData.h"

RQRoutaData::RQRoutaData(int ip, short port, char type)
{
	Set(ip, port, type);
}

void RQRoutaData::Set(int ip, short port, char type)
{
	_addr.Set(ip, port);
	_type = type;
}

void RQRoutaData::Clear()
{
	Set(0, 0, 0);
}

void RQRoutaData_Addr::Set(int ip, short port)
{
	_ip = ip;
	_port = port;
}
