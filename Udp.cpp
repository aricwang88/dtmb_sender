
#include "Udp.h"
#include "type.h"

Udp::Udp(UINT mtu) :Socket(mtu)
{
	m_isConnect = FALSE;
}

Udp::~Udp()
{
}

BOOL Udp::Open(char *bindIp, int bindPort, char *target_ip)
{
	if (m_isOpen)
		return FALSE;

	m_isOpen = FALSE;
	m_isConnect = FALSE;

	//int error = 0;
	int i_val = 0;
	
	if (m_Socket)close(m_Socket);
	m_Socket= socket(AF_INET, SOCK_DGRAM, 0);;
	if ( m_Socket == -1 )
	{		
		printf("%s()->Create socket error\n", __func__);
		return FALSE;
	}

	i_val = 1;	
	ioctl(m_Socket, FIONBIO, &i_val);

	i_val = (int)(1024 * 1024 * 5);	
	setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF,(const char*)&i_val, sizeof(int));
	setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF,(const char*)&i_val, sizeof(int));
	i_val = 1;
	setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, &i_val, sizeof(i_val));
	i_val = 5;
	setsockopt(m_Socket, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&i_val, sizeof(i_val));	
	
	// 绑定套接字
	memset(&m_BindAddr, 0, sizeof(m_BindAddr));

	m_BindAddr.sin_family = AF_INET;   
	m_BindAddr.sin_port = htons(bindPort);
	m_BindAddr.sin_addr.s_addr = inet_addr(bindIp);
	
	//if ( IN_MULTICAST(ntohl(m_BindAddr.sin_addr.s_addr))  || m_BindAddr.sin_addr.s_addr == INADDR_BROADCAST )
	//	m_BindAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(m_Socket, (struct sockaddr *)&m_BindAddr, sizeof(m_BindAddr))==-1)
	{
		printf("%s()->Bind error\n", __func__);
		return FALSE;
	}


	//if ( ! SetMulticast(target_ip, bindIp))
	//	return FALSE;

	m_isOpen = TRUE;
	return TRUE;
}

BOOL Udp::Connect(char *connectIp, int connectPort, char *bindIp)
{
	if (!m_isOpen)
		return FALSE;

	int error = 0;
	int i_val = 0;

	memset(&m_ConnectAddr, 0, sizeof(m_ConnectAddr));

	if (IN_MULTICAST(ntohl(inet_addr(connectIp))))//判断是否为组播地址
	{
		i_val = 1;
		error =  setsockopt(m_Socket, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&i_val, sizeof(i_val) );
		
		SetMulticast(connectIp, bindIp);		
	}	

	m_ConnectAddr.sin_family = AF_INET;
	m_ConnectAddr.sin_port = htons(connectPort);//9010
	m_ConnectAddr.sin_addr.s_addr = inet_addr(connectIp);//224.12.34.56	

	//udp使用connect,定向发送数据，节省连接时间，可以直接使用tcp的send、recv
	connect(m_Socket, (SOCKADDR*)&m_ConnectAddr, sizeof(m_ConnectAddr)) ;
	
	m_isConnect = TRUE;
	return TRUE;
}

int Udp::Read(BYTE* pBuffer, UINT16 bufferSize, UINT nTimeOut)
{
	int iRead;

	if ( !m_isOpen )
		return -1;
	
	iRead = Socket::Read(pBuffer, bufferSize, nTimeOut);

	return iRead;
}

int Udp::Write(PBYTE pBuffer, UINT16 bufferSize, UINT nTimeOut)
{
	int iWrite;

	if ( !m_isOpen || !m_isConnect)
		return -1;

	iWrite = Socket::Write(pBuffer, bufferSize, nTimeOut);//tcp ->write

	return iWrite;
}


BOOL Udp::SetMulticast(char *group_ip, char *loc_ip)
{
	//int error = 0;
	//int i_val = 0;
	
	struct ip_mreq mreq;
	
	bzero(&mreq, sizeof(struct ip_mreq));
	inet_aton(group_ip, &mreq.imr_multiaddr);
	inet_aton(loc_ip, &mreq.imr_interface);
	//mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	if(setsockopt(m_Socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(struct ip_mreq)) == -1)
	{
		perror("Add into multi_group fail\n");
		return FALSE;
	}	
	return TRUE;
}


void Udp::CreateSocket(unsigned short ethNo,char* ip,int port,int SendOrRecv,int TcpOrUdp)
{
	struct sockaddr_in addr;
	unsigned int g_ttl=65;

	addr.sin_family = AF_INET;
	if(SendOrRecv == 0)
	{
		addr.sin_port = htons(0);
	}
	else
	{
		addr.sin_port = htons(port);
	}

	if(TcpOrUdp == 0)//udp
	{
		m_Socket = socket(AF_INET, SOCK_DGRAM, 0);//SOCK_DGRAM   ======UDP
	}
	else
	{
		m_Socket = socket(AF_INET, SOCK_STREAM, 0);//SOCK_STREAM  ======TCP
		unsigned long ul = 1;
		ioctl(m_Socket, FIONBIO, &ul);
	}
											
	if (m_Socket == -1)
	{
		printf("stream[%d]----Error in create listen socket %s\n",0,strerror(errno));
	}

	SetPortReuse(1);

	addr.sin_addr.s_addr= htonl(INADDR_ANY);


	/*
	switch(ethNo)
	{
		case 0: memcpy(&addr.sin_addr.s_addr,&IpInfo_eth0.uIP,sizeof(in));
				//addr.sin_addr.s_addr= htonl(IpInfo_eth0.uIP);
				break;
		case 1: memcpy(&addr.sin_addr.s_addr,&IpInfo_eth1.uIP,sizeof(in));
				break;
		case 2: memcpy(&addr.sin_addr.s_addr,&IpInfo_eth1.uIP,sizeof(in));
				break;
		case 3:memcpy(&addr.sin_addr.s_addr,&IpInfo_eth3.uIP,sizeof(in));
				break;				
		default: memcpy(&addr.sin_addr.s_addr,&IpInfo_eth3.uIP,sizeof(in));
				break;
	}
	//*/

	//join the group  recv
	if(SendOrRecv == 1)//接收socket
	{
		long lIP = inet_addr(ip);
		if((lIP & htonl(0xF0000000)) == htonl(0xE0000000))//for board
		{
			struct ip_mreq mreq;
			mreq.imr_multiaddr.s_addr = inet_addr(ip);
			addr.sin_addr.s_addr= inet_addr(ip);
			
			if(bind(m_Socket, (struct sockaddr *)&addr, sizeof(addr)) == -1)
			{
				printf("stream[%d]---error in bind the listen socket %s\n",0,strerror(errno));
			}	

			mreq.imr_interface.s_addr = htonl(INADDR_ANY);
			/*
			switch(ethNo)
			{
				case 0: memcpy(&mreq.imr_interface.s_addr,&IpInfo_eth0.uIP,sizeof(in));
						break;
				case 1: memcpy(&mreq.imr_interface.s_addr,&IpInfo_eth1.uIP,sizeof(in));
						break;
				case 2: memcpy(&mreq.imr_interface.s_addr,&IpInfo_eth2.uIP,sizeof(in));
						break;
				case 3: memcpy(&mreq.imr_interface.s_addr,&IpInfo_eth3.uIP,sizeof(in));
						break;				
				default: memcpy(&mreq.imr_interface.s_addr,&IpInfo_eth0.uIP,sizeof(in));
			}	

			//*/
			
	
			if(setsockopt(m_Socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(struct ip_mreq)) < 0)
			{
				if(strcmp("Address already in use",strerror(errno)) == 0)
				{
					printf("stream[%d]---%s joined --\n",0,ip);
				}
				else		
				{
					printf("steam[%d]----Failed to join group - %s\n",0,strerror(errno)); 
				}	
			}			
		}
		else
		{
			if(bind(m_Socket, (struct sockaddr *)&addr, sizeof(addr)) == -1)
			{
				printf("stream[%d]---error in bind the listen socket %s\n",0,strerror(errno));
			}
		}
		
	}
	else if(SendOrRecv == 0)//
	{
		if(bind(m_Socket, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		{
			printf("stream[%d]---error in bind the listen socket %s\n",0,strerror(errno));
		}

		if(TcpOrUdp == 0)//udp
		{
			int i_opt = 1;
		    setsockopt(m_Socket, SOL_SOCKET, SO_BROADCAST, (void*) &i_opt,sizeof( i_opt));
		}

	}
	int nSendBuf = 50 * 1024 * 1024;
	setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF,(const char*)&nSendBuf,sizeof(int));

	if(TcpOrUdp == 0)//udp
	{
		if(setsockopt(m_Socket, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&g_ttl, sizeof(g_ttl)) < 0)
		{
			printf("Set TTL failed. Error:%s\n",strerror(errno)); 
		}
	}

	
}


int Udp::SetPortReuse(int bReuse)
{
	int value = bReuse ? 1 : 0;
	if (setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)) < 0)
	{
		//log_debug("setsockopt:SO_REUSEADDR failed - %s\n", strerror(errno));
		return 0;
	}
	else
	{
		return 1;
	}
}


int Udp::Readx(BYTE* pBuffer, UINT bufferSize)
{
	int nLen=sizeof(struct sockaddr_in);
	return recvfrom(m_Socket, pBuffer, bufferSize, 0, (struct sockaddr*)&addrFrom, (socklen_t*)&nLen);
}
/*
int Udp::Writex(BYTE* pBuffer, UINT bufferSize)
{
	int nLen=sizeof(struct sockaddr_in);
	return sendto(m_Socket, pBuffer, bufferSize, 0, (struct sockaddr*)&addrFrom, (socklen_t*)&nLen);
}
*/
