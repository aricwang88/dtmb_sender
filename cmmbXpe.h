#ifndef _CMMB_XPE_H_
#define _CMMB_XPE_H_



#include "IntTypes.h"
#include "type.h"
//#include "PacketFIFO.h"

#define LPVOID	void*


#define REALBROADCASTING		1
#define MAX_SERVICES			10

//#define WM_MAINTHREAD_EXIT		WM_USER + 1000


#define CONTROL_PORT  2323
#define TELNET_PORT  23

/*--------------------------------------------------------------------
	Thread parameters' table
--------------------------------------------------------------------*/
typedef enum
{
	DEBUG_OPEN		= 0,
	DEBUG_INFO,
	DEBUG_WARNING,
	DEBUG_ERROR,
	DEBUG_IDLE,
	DEBUG_SAVE,
	DEBUG_CLOSE
}DEBUGX;

typedef enum
{
	UPPER_STYLE_STREAM = 0,
	UPPER_STYLE_FILE = 1,
}UPPER_STYLE_STREAMX;

typedef enum
{
	FEC_STYLE_RS_255_207 = 0,
	FEC_STYLE_NULL = -1,
}FEC_STYLEX;

typedef struct
{
	int		nn;						//�ܳ���
	int		nroot;					//У�����ݳ���
	int		dd;						//ԭʼ���ݳ���

	int		param_length;			//��������
	int		rows;					//��֯������

} FEC_PARAM_RS_t;

typedef struct
{
	int			   msg_level;

	unsigned short wYear; 
    unsigned short wMonth; 
    unsigned short wDay; 
    unsigned short wHour; 
    unsigned short wMinute; 
    unsigned short wSecond; 
    unsigned short wMilliseconds; 

	char			pszMsg[256];

} XPE_MSG_t;

typedef struct
{
	U16				service_ID;

	S32				port_count;
	U16				input_port[16];

	U16				old_ip_packet_id;				//added by chendelin		20090902	����˿�����û�п��ǣ���Ҫ�ٲ���

	S32				detect_bps[16];
	S32				native_bps[16];

	S8				pszProtocol[16];
	S8				pszMulticastAddr[16];

	U8				upper_style;
	S8				pszUpperStyle[32];
	S8				pszFileName[1024];

	S32				alloc_bps;
	S32				FEC_style;
	S8				pszFECStyle[32];

	U8*				pxpe_input_buf;
	U8*				pxpe_output_buf;

	U8				packet_index;
	U16             udp_count;
	U16             msg_count;//add by wangying

	union
	{
		FEC_PARAM_RS_t		rs_param;
	} u;

	S32				running;
	S32				tcp_self_exit;

	//CPacketFIFO*	pXPE_FIFO;

	//HANDLE			hRecvThread;

	//HANDLE			hAccess;

	BOOL			bCRC32Indic;
	S8				pszCRCStyle[8];

	BOOL			bServStat;
	U32				nSendByteCount;
	U32				nRecvByteCount;
	BOOL			bRmvIter;

} SERVICE_PARAM_t;

typedef struct thread_params_s
{
	//�������߳���صĲ���
	S32				send_running;
	S32				send_stop;
	S32				send_self_exit;

	//�������߳���صĲ���
	S32				recv_running;
	S32				recv_stop;

	//��telnet�߳���صĲ���
	S32				telnet_running;
	S32				telnet_stop;
	S32				telnet_self_exit;

	//�������߳���صĲ���
	S32				control_running;
	S32				control_stop;
	S32				control_self_exit;

	//����������ص�����
	BOOL            mux_port_same;//wy
	U16				mux_port;
	S8				pszOutputIP[16];
	S8              pszMuxIP[16];//add by wangying 
	

	//��װ����������
	S8				pszHostName[64];

	//����ͨ����IP��ַ
	S8				pszControlIP[16];

	//��ҵ��ǰ����ص�����
	S8				pszInputIP[16];

	//�����ݷ�װ��ز���
	U32             xpe_size;
	U32             udp_size;//wy

	//��ҵ����صĲ���
	S32				service_count;
	SERVICE_PARAM_t	SERVICE_PARAMS[MAX_SERVICES];

	//��������صĲ���
	S32				enable_recv_debug;
	S32				enable_rtp_debug;
	S32				enable_pack_debug;
	S32				enable_send_debug;
	S32				enable_raw_socket;

	S8				pszLogFile[256];

	//CPacketFIFO*	pDEBUG_FIFO;
	BOOL			bMainThreadRunning;
	//HANDLE			hMainThreadAccess;
	BOOL			bMainThreadStat;
	//HANDLE			bClientIPGot;
	SOCKADDR_IN		ctrladdr;

	//-------------for  sarifec---zxy---------------
	S32             fec_file_send_count;
	S32             fec_type;
	S32             fec_width;
	S32             fec_rate;
	S32             fec_num;
	//-------------------------------------
} thread_params_t, *pthread_params_t;


typedef struct msg_params_s
{
	U16				service_ID;
	BOOL			bServStat;
	float			fSendSpeed;
	float			fRecvSpeed;
} msg_params_t, *pmsg_params_t;

typedef struct  msg_fdback_s
{
	BOOL			bMainThreadStat;
	U16				nServiceValidNum;
	msg_params_t	SERVICEPARAM[MAX_SERVICES];
} msg_fdback_t, *pmsg_fdback_t;

typedef struct msg_serv_s
{
	U16			nServ_ID;
	BOOL		bServStat;
}msg_serv_t, *pmsg_serv_t;

typedef struct msg_send_s
{
	msg_serv_t	ServParam[MAX_SERVICES];
	
} msg_recv_t, *pmsg_recv_t;



typedef struct
{
	U16				service_ID;
	S8				pszServiceName[64];
	
	S8				pszMulticastAddr[16];
	S32				port_count;
	U16				input_port[16];
	
	S32				native_bps[16];
	
	S8				pszProtocol[16];
	
	U8				upper_style;
	S8				pszUpperStyle[32];
	S8				pszFileName[1024];
	
	S32				alloc_bps;
	S32				FEC_style;
	S8				pszFECStyle[32];
	
	U8				packet_index;
	
	union
	{
		FEC_PARAM_RS_t		rs_param;
	} u;
	
	BOOL			bCRC32Indic;
	S8				pszCRCStyle[8];
	BOOL			bRmvIter;
	
} SERVICE_PARAM_CTRL_t;


typedef struct thread_params_ctrl_s
{
	//��ҵ����صĲ���
	S32				service_count;
	SERVICE_PARAM_CTRL_t	SERVICE_PARAMS[MAX_SERVICES];
	//�����ݷ�װ��ز���
	U32             xpe_size;
	U32             udp_size;       
	//---------kernel�������ʹ�õĲ���------------------
	
	//	S32				netcard_index;
	S8				pszInputIP[16];					//��װ������󶨵�IP��ַ
	S32				input_same_with_control;
	
	S8				pszOutputIP[16];				//��װ������󶨵�IP��ַ
	S32				output_same_with_control;
	
	U16				mux_port;						//�븴������ͨ�Ŷ˿�
	S8              pszMuxIP[16];
	BOOL            mux_port_same;
	
} thread_params_ctrl_t, *pthread_params_ctrl_t;


//U32 XPDC_IP_thread(LPVOID lpParam);
//U32 XPDC_UDP_thread(LPVOID lpParam);
//U32 XPDC_TCP_thread(LPVOID lpParam);
/*
S32 XPDC_Encode_XPE(thread_params_t* pThreadParams,SERVICE_PARAM_t* pServiceParam, U8* sp_buf, S32 sp_length, U8 packet_ID, u_short in_port, SOCKET sockClient);
S32 XPDC_Encode_XPE_FEC(thread_params_t* pThreadParams,SERVICE_PARAM_t* pServiceParam, U8* sp_buf, S32 sp_length, U8 packet_ID, u_short in_port, SOCKET sockClient);
S32 XPDC_Encode_MSG(thread_params_t* pThreadParams, SERVICE_PARAM_t* pServiceParam,U8* sp_buf, S32 sp_length, u_short in_port, SOCKET sockClient);
S32 XPDC_PushOutStack(SERVICE_PARAM_t* pServiceParam, U8* tcp_buf, S32* ptcp_length);
*/

//U32 CmmbXDC_send_tcp_client_thread(LPVOID lpParam);						//TCP����
//U32 CmmbXDC_send_tcp_server_thread(LPVOID lpParam);						//TCP����

void OnXpdcStart(thread_params_t* pThreadParams, int service_index);
void OnXpdcStop(thread_params_t* pThreadParams, int service_index);


/*
#ifdef __cplusplus
extern "C"{
#endif

//int XPE_send_thread(LPVOID lpParam);
//int XPE_receive_thread(LPVOID lpParam);
void *XPE_file_thread(LPVOID lpParam);
//int XPE_telnet_thread(LPVOID lpParam);
//int XPE_control_thread(LPVOID lpParam);
//int XPE_buf_thread(LPVOID lpParam);

#ifdef __cplusplus
}
#endif

//*/



int XPE_SaveIniFile(unsigned char* buf, int length);


#if 0
__inline void XPE_DEBUG(thread_params_t* pThreadParams, int level, char* pszMsg)
{
	XPE_MSG_t		msg;
	SYSTEMTIME		systime;

	if ((pThreadParams != NULL) && (pszMsg))
	{
		if (pThreadParams->pDEBUG_FIFO != NULL)
		{
			GetSystemTime(&systime);

			msg.msg_level = level;

			msg.wYear = systime.wYear;
			msg.wMonth = systime.wMonth;
			msg.wDay = systime.wDay;
			msg.wHour = systime.wHour;
			msg.wMinute = systime.wMinute;
			msg.wSecond = systime.wSecond;
			msg.wMilliseconds = systime.wMilliseconds;

			sprintf(msg.pszMsg, "<%04d-%02d-%02d %02d:%02d:%02d:%03d> ", msg.wYear, msg.wMonth, msg.wDay, msg.wHour, msg.wMinute, msg.wSecond, msg.wMilliseconds);

			switch (level)
			{		
			case DEBUG_INFO:
				strcat(msg.pszMsg, "[INFO] -->");
				break;
			case DEBUG_ERROR:
				strcat(msg.pszMsg, "[ERROR] -->");
				break;
			case DEBUG_WARNING:
				strcat(msg.pszMsg, "[WARNING] -->");
				break;
			default:
				strcat(msg.pszMsg, "[INFO] -->");
				break;
			}

			strcat(msg.pszMsg, pszMsg);

			pThreadParams->pDEBUG_FIFO->PushIntoPacket((unsigned char*)&msg, sizeof(XPE_MSG_t));
		}
	}
}

#endif

#endif
