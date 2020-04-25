#include "server/socket/RQSockOpt.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "RQSys.h"

static socklen_t socklen = sizeof(sockaddr);

int RQSockOpt::SocketT4()
{
	return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

int RQSockOpt::Close(int sock)
{
	return close(sock);
}

int RQSockOpt::Accept(int listenSock, sockaddr_in * clientAddr)
{
	return accept(listenSock, (sockaddr*)clientAddr, &socklen);
}

int RQSockOpt::Listen4(int listenSock, const char * bindAddr, short bindPort, int listenSize)
{
	if (bindAddr)
	{
		return Listen4(listenSock, Inet_addr(bindAddr), bindPort, listenSize);
	}
	return Listen4(listenSock, (int)INADDR_ANY, bindPort, listenSize);
}

int RQSockOpt::Listen4(int listenSock, int bindAddr, short bindPort, int listenSize)
{
	sockaddr_in addr;
	_SetAddr4(addr, AF_INET, bindAddr, bindPort);				///设置目标监听参数

	int retInt = bind(listenSock, (sockaddr*)&addr, socklen);
	if (!retInt)
	{
		retInt = listen(listenSock, listenSize);
	}

	return retInt;
}

int RQSockOpt::Connect(int sock, const char * ip, short port)
{
	return Connect(sock, Inet_addr(ip), RQHton(port));
}

int RQSockOpt::Connect(int sock, int ip, short port)
{
	sockaddr_in addr;
	_SetAddr4(addr, AF_INET, ip, port);
	return connect(sock, (sockaddr*)&addr, socklen);
}

int RQSockOpt::Send(int sock, void * bufPtr, int dataLen)
{
	return send(sock, bufPtr,dataLen,0);
}

int RQSockOpt::Recv(int sock, void* bufPtr, int bufSize)
{
	return recv(sock,bufPtr,bufSize,0);
}

int RQSockOpt::ToAsy(int sock)
{
	return fcntl(sock, F_SETFL, O_NONBLOCK);
}

int RQSockOpt::ToReusePort(int sock)
{
	unsigned value = 1;
	return setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
}

int RQSockOpt::SetRecvBuffSize(int sock,int size)
{
	return setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &size, socklen);
}

int RQSockOpt::SetSendBuffSize(int sock, int size)
{
	return setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &size, socklen);
}

void RQSockOpt::_SetAddr4(sockaddr_in & addr, int family, int ip, short port)
{
	(*(unsigned long long*)&addr.sin_zero) = 0;
	addr.sin_family = family;
	addr.sin_port = port;
	addr.sin_addr.s_addr = ip;
}

void RQSockOpt::_SetAddr4(sockaddr_in & addr, int family, const char * ip, short port)
{
	_SetAddr4(addr, family, Inet_addr(ip), RQHton(port));
}
