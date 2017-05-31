
#include <string>
#include <vector>

#include <netinet/in.h>

#define UINT16  unsigned short
#define UINT 	unsigned int
#define ULONG 	unsigned long
#define TRUE	1
#define FALSE   0
#define BOOL	unsigned char
#define VOID 	void
#define INT		int
#define BYTE	unsigned char
#define PVOID	void*
#define	PBYTE	BYTE*
#define DWORD	unsigned long
#define INT64	long long
#define UINT32	unsigned int
#define FALSE       0
#define TRUE        1


#define SOCKADDR_IN struct sockaddr_in


//#include "type.h"


// select mode 
static const int SELECT_MODE_READY = 0x001;
static const int SELECT_MODE_WRITE = 0x002;

// select return codes 
static const int SELECT_STATE_READY =  0;
static const int SELECT_STATE_ERROR =  1;
static const int SELECT_STATE_ABORTED = 2;
static const int SELECT_STATE_TIMEOUT = 3;




class Socket
{
public:
	Socket(UINT mtu = 1500);
	virtual ~Socket();

	virtual void Close();

	virtual int Write(BYTE* pBuffer, int writeSize, UINT nTimeOut = 500000);  // 0.5sec
	virtual int Read(BYTE* pBuffer, int readSize, UINT nTimeOut = 500000); // 0.5sec

	virtual struct sockaddr_in GetBindAddr();
	virtual struct sockaddr_in GetConnectAddr();

	virtual	UINT GetMTU();

	//static BOOL GetLocalIPList(vector<string>& vIPList);
	//static BOOL GetAdapterSpeed(vector<int>& vList);

protected:
	void	ReportError();
	int Select(int iMode, int nTimeoutUsec);

	BOOL	m_isOpen;

	int		m_Socket;
	struct sockaddr_in m_BindAddr;
	struct sockaddr_in m_ConnectAddr;

	UINT	m_Mtu;
};
