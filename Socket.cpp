
#include "Socket.h"
#include "type.h"



Socket::Socket(UINT mtu)
:m_Mtu(mtu)
{
	//WSADATA wsaData;
	//WORD wVersionRequested = MAKEWORD( 2, 2 );
	//WSAStartup(wVersionRequested, &wsaData);

	m_Socket = NULL;
	memset(&m_BindAddr, 0, sizeof(m_BindAddr));
	memset(&m_ConnectAddr, 0, sizeof(m_ConnectAddr));

	m_isOpen = FALSE;
}

Socket::~Socket()
{
	Close();
}

void Socket::Close()
{
	if (m_Socket)close(m_Socket);
	m_Socket = -1;
	m_isOpen = FALSE;
}

int Socket::Read(BYTE* pBuffer, int readSize, UINT nTimeOut)
{
	int selectState;
	int recvSize;

	if (!pBuffer || !m_isOpen)
		return -1;

	selectState = Select(SELECT_MODE_READY, nTimeOut);
	if (SELECT_STATE_TIMEOUT == selectState)
		return 0;

	if (SELECT_STATE_READY == selectState)
	{
		recvSize = recv(m_Socket, (char*)pBuffer, readSize, 0);
		if (recvSize <= 0)
			return -1;
		return recvSize;
	}
	return -1;
}

int Socket::Write(PBYTE pBuffer, int writeSize, UINT nTimeOut)
{
	int selectState = 0;
	int sendSize = 0;

	if (!pBuffer || !m_isOpen)
		return -1;

	selectState = Select(SELECT_MODE_WRITE, nTimeOut);
	if (selectState == SELECT_STATE_TIMEOUT)
		return 0;

	if (selectState == SELECT_STATE_READY)
	{
		sendSize = send(m_Socket, (char*)pBuffer, writeSize, 0);
		if (sendSize <= 0)
			return -1;
		return sendSize;
	}	

	return -1;
}

SOCKADDR_IN Socket::GetBindAddr()
{
	return m_BindAddr;
}

SOCKADDR_IN Socket::GetConnectAddr()
{
	return m_ConnectAddr;
}

UINT Socket::GetMTU()
{
	return m_Mtu;
}


int Socket::Select( int iMode, int nTimeoutUsec )
{
	fd_set fdset;
	fd_set *readSet, *writeSet;
	timeval selectTimeout;
	int ret;

	selectTimeout.tv_sec  = 0;
	selectTimeout.tv_usec = nTimeoutUsec*1000;

	FD_ZERO (&fdset);
	FD_SET  (m_Socket, &fdset);

	readSet = (iMode & SELECT_MODE_READY) ? &fdset : NULL;
	writeSet = (iMode & SELECT_MODE_WRITE) ? &fdset : NULL;

	ret = select ( (int)m_Socket + 1, readSet, writeSet, NULL, &selectTimeout);

	
	if(-1 == ret)
	{
		printf("select error\n");
		return SELECT_STATE_ERROR;
	}
	else if(0 == ret)
	{
		printf("select timeout\n");
		return SELECT_STATE_TIMEOUT;	
	}
	else
	{
		return SELECT_STATE_READY;
	}	

	return SELECT_STATE_TIMEOUT;
}




