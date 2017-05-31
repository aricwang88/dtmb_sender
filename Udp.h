
#ifndef __UDP_H__
#define __UDP_H__


#define UINT16  unsigned short
#define UINT 	unsigned int
#define ULONG 	unsigned long
#define TRUE	1
#define FALSE   0
#define BOOL	unsigned char
#define VOID 	void
#define INT		int
#define BYTE	unsigned char

#include "Socket.h"




class Udp: public Socket
{
public:

	Udp(UINT mtu = 1500);
	virtual ~Udp();

	virtual BOOL Open(char *bindIp, int bindPort = 0, char *target_ip = NULL);

	virtual BOOL Connect(char *connectIp, int connectPort, char *bindIp);

	virtual int  Read(BYTE* pBuffer, UINT16 bufferSize, UINT nTimeOut = 500000);

	virtual int  Write(BYTE* pBuffer, UINT16 bufferSize, UINT nTimeOut = 500000);

	void CreateSocket(unsigned short ethNo,char* ip,int port,int SendOrRecv,int TcpOrUdp);
	
	int SetPortReuse(int bReuse);

	int Readx(BYTE* pBuffer, UINT bufferSize);
	
	int Writex(BYTE* pBuffer, UINT bufferSize);
	

protected:

	BOOL SetMulticast(char *group_ip, char *loc_ip);

	BOOL	m_isConnect;

	struct sockaddr_in addrFrom;
};

#endif
