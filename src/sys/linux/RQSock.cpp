#include "RQSock.h"
#include "RecycleBin.h"
#include "tool/RQMemory.h"
#include "RQSys.h"
#include "server/socket/RQSockOpt.h"
#include <netinet/in.h>

RQSock::RQSock()
{
	_sock = -1;
}

RQSock::~RQSock()
{
}

int RQSock::SetSock(int newSock)
{
	int oldSock = _sock;
	_sock = newSock;
	return _sock;
}

int RQSock::socket()
{
	return	_sock = RQSockOpt::SocketT4();;
}

void RQSock::Close()
{
	if (-1 != _sock
		&& IsConnecting())
	{
		RQSockOpt::Close(_sock);
		_sock = -1;
	}
}

RQSock::operator int()
{
	return _sock;
}

int RQSock::SetRecvBuffSize(int size)
{
	return RQSockOpt::SetRecvBuffSize(_sock, size);
}

int RQSock::SetSendBuffSize(int size)
{
	return RQSockOpt::SetSendBuffSize(_sock, size);
}

int RQSock::Listen(const char * ip, short port, int listenSize)
{
	return RQSockOpt::Listen4(_sock,ip,port,listenSize);
}

int RQSock::Listen(int ip, short port, int listenSize)
{
	return RQSockOpt::Listen4(_sock, ip, port, listenSize);
}

int RQSock::Accept(sockaddr_in * clientAddr)
{
	return RQSockOpt::Accept(_sock,clientAddr);
}

int RQSock::Accept(SockAddr_In & clientAddr)
{
	sockaddr_in addr;
	int retInt = Accept(&addr);
	clientAddr = addr;
	return retInt;
}

int RQSock::Connect(const char * ip, short port)
{
	return RQSockOpt::Connect(_sock,ip,port);
}

int RQSock::Connect(int ip, short port)
{
	return RQSockOpt::Connect(_sock, ip, port);
}

int RQSock::Send(char * buff, unsigned int dataLen)
{
	return RQSockOpt::Send(_sock,buff,dataLen);
}

int RQSock::Recv(char * buff, unsigned int buffSize)
{
	return RQSockOpt::Recv(_sock,buff,buffSize);
}

int RQSock::ToAsy()
{
	return RQSockOpt::ToAsy(_sock);
}

int RQSock::ToReusePort()
{
	unsigned value = 1;
	return RQSockOpt::ToReusePort(_sock);
}

bool RQSock::IsConnecting()
{
	return !GetSockStatus(_sock);
}

bool RQSock::IsOk()
{
	return _sock>0;
}

void SockAddr_In::operator=(const sockaddr_in & addr)
{
	_addr = addr.sin_addr.s_addr;
	_family = addr.sin_family;
	_port = addr.sin_port;
}
