// CMMB_File.cpp: implementation of the CMMB_File class.
//
//////////////////////////////////////////////////////////////////////

#include "cmmbFile.h"



#define TIME_SEND	20000    //20


#define MUXER_IP	"234.1.2.3"
#define MUXER_PORT	9001

const byte commonKey[17] = {"abcdefghijklmnop"};
const byte commonIv[17]  = {"1234567890123456"};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//====================================================


static PRS_PARAM_t 			rsHandle = NULL;
//static U8*					fec_block = NULL;
//static U8					fec_buf[32768];
//static S32					fec_length = 0;

//static thread_params_t*		gpThreadParams = NULL;
//static SOCKET				gsockUdp;
//static SOCKADDR_IN			gaddrOut;
static UINT					m_netRate=0;
static Udp					m_sUdp;
extern thread_params_t	gkThreadParams;


/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
BYTE  FillBuffer[188];

extern char 	bind_ip[16];



/////////////////////////////////////////////////////////////////////

static unsigned short crctab[1<<B] = {// as calculated by initcrctab()
	0x0000,  0x1021,  0x2042,  0x3063,  0x4084,  0x50a5,  0x60c6,  0x70e7,
	0x8108,  0x9129,  0xa14a,  0xb16b,  0xc18c,  0xd1ad,  0xe1ce,  0xf1ef,
	0x1231,  0x0210,  0x3273,  0x2252,  0x52b5,  0x4294,  0x72f7,  0x62d6,
	0x9339,  0x8318,  0xb37b,  0xa35a,  0xd3bd,  0xc39c,  0xf3ff,  0xe3de,
	0x2462,  0x3443,  0x0420,  0x1401,  0x64e6,  0x74c7,  0x44a4,  0x5485,
	0xa56a,  0xb54b,  0x8528,  0x9509,  0xe5ee,  0xf5cf,  0xc5ac,  0xd58d,
	0x3653,  0x2672,  0x1611,  0x0630,  0x76d7,  0x66f6,  0x5695,  0x46b4,
	0xb75b,  0xa77a,  0x9719,  0x8738,  0xf7df,  0xe7fe,  0xd79d,  0xc7bc,
	0x48c4,  0x58e5,  0x6886,  0x78a7,  0x0840,  0x1861,  0x2802,  0x3823,
	0xc9cc,  0xd9ed,  0xe98e,  0xf9af,  0x8948,  0x9969,  0xa90a,  0xb92b,
	0x5af5,  0x4ad4,  0x7ab7,  0x6a96,  0x1a71,  0x0a50,  0x3a33,  0x2a12,
	0xdbfd,  0xcbdc,  0xfbbf,  0xeb9e,  0x9b79,  0x8b58,  0xbb3b,  0xab1a,
	0x6ca6,  0x7c87,  0x4ce4,  0x5cc5,  0x2c22,  0x3c03,  0x0c60,  0x1c41,
	0xedae,  0xfd8f,  0xcdec,  0xddcd,  0xad2a,  0xbd0b,  0x8d68,  0x9d49,
	0x7e97,  0x6eb6,  0x5ed5,  0x4ef4,  0x3e13,  0x2e32,  0x1e51,  0x0e70,
	0xff9f,  0xefbe,  0xdfdd,  0xcffc,  0xbf1b,  0xaf3a,  0x9f59,  0x8f78,
	0x9188,  0x81a9,  0xb1ca,  0xa1eb,  0xd10c,  0xc12d,  0xf14e,  0xe16f,
	0x1080,  0x00a1,  0x30c2,  0x20e3,  0x5004,  0x4025,  0x7046,  0x6067,
	0x83b9,  0x9398,  0xa3fb,  0xb3da,  0xc33d,  0xd31c,  0xe37f,  0xf35e,
	0x02b1,  0x1290,  0x22f3,  0x32d2,  0x4235,  0x5214,  0x6277,  0x7256,
	0xb5ea,  0xa5cb,  0x95a8,  0x8589,  0xf56e,  0xe54f,  0xd52c,  0xc50d,
	0x34e2,  0x24c3,  0x14a0,  0x0481,  0x7466,  0x6447,  0x5424,  0x4405,
	0xa7db,  0xb7fa,  0x8799,  0x97b8,  0xe75f,  0xf77e,  0xc71d,  0xd73c,
	0x26d3,  0x36f2,  0x0691,  0x16b0,  0x6657,  0x7676,  0x4615,  0x5634,
	0xd94c,  0xc96d,  0xf90e,  0xe92f,  0x99c8,  0x89e9,  0xb98a,  0xa9ab,
	0x5844,  0x4865,  0x7806,  0x6827,  0x18c0,  0x08e1,  0x3882,  0x28a3,
	0xcb7d,  0xdb5c,  0xeb3f,  0xfb1e,  0x8bf9,  0x9bd8,  0xabbb,  0xbb9a,
	0x4a75,  0x5a54,  0x6a37,  0x7a16,  0x0af1,  0x1ad0,  0x2ab3,  0x3a92,
	0xfd2e,  0xed0f,  0xdd6c,  0xcd4d,  0xbdaa,  0xad8b,  0x9de8,  0x8dc9,
	0x7c26,  0x6c07,  0x5c64,  0x4c45,  0x3ca2,  0x2c83,  0x1ce0,  0x0cc1,
	0xef1f,  0xff3e,  0xcf5d,  0xdf7c,  0xaf9b,  0xbfba,  0x8fd9,  0x9ff8,
	0x6e17,  0x7e36,  0x4e55,  0x5e74,  0x2e93,  0x3eb2,  0x0ed1,  0x1ef0
};



//===========================================================
static WORD Get_16bitCRC(WORD icrc, BYTE* icp, UINT icnt)
{
	unsigned short crc = icrc;
	unsigned char *cp = icp;
	unsigned int cnt = icnt;
	
	while(cnt--)
		crc = (crc<<B) ^ crctab[(crc>>(W-B)) ^ *cp++];
	
	return (crc);
}


// Create XDB file
void CMMB_DTMB_MUX::CreateXDB(BYTE* pSrc, BYTE* pDst, WORD streamNo, UINT frameSeq, BYTE dataLen)
{
	if((!pSrc)||(!pDst))return;

	WORD crc = 0;	

	pDst[0]=0xff;
	pDst[1]=dataLen+6;   //fileNum + frameSeqNo + dataLen   179
	pDst[2]=0x00;
	pDst[3]=0x00;
	pDst[4]=0x10;
	pDst[5]=(streamNo>>8)&0xff;
	pDst[6]=(streamNo)&0xff;
	pDst[7]=(frameSeq>>24)&0xff;
	pDst[8]=(frameSeq>>16)&0xff;
	pDst[9]=(frameSeq>>8)&0xff;
	pDst[10]=(frameSeq)&0xff;
	memcpy(pDst+11,pSrc, dataLen);
	crc = Get_16bitCRC(0, pDst+4, dataLen+7);
	pDst[2] = (crc>>8)&0xff;
	pDst[3] = (crc)&0xff;
}



void  CMMB_DTMB_MUX::SetEncryptFlag(int flag)
{
	encryptFlag = flag;
}




CMMB_DTMB_MUX::CMMB_DTMB_MUX()
{
	m_srcFileName = "";
	dstFileName   = "";
	tsFileName    = "";
	simpleFileName= "";
	xpeFileName   = "";
	msgFileName   = "";	

	encryptFlag   = 1;

	m_sendType = CMMB_MODE;

	//================================
	string cmd=CMD_FOLDER;
	cmd += BS_SRCFILE;
	system(cmd.c_str());
	cmd = CMD_FOLDER;
	cmd += CS_DSTFILE;
	system(cmd.c_str());
	//================================
}


// Create TS frame
void CMMB_DTMB_MUX::CreateTS(BYTE* pSrc, BYTE* pDst, BYTE count)
{	
	WORD pid=0;
	//m_editPid.GetWindowTextA(pidStr);
	
	pid=(WORD)atoi("8000");

	pDst[0]=0x47;
	pDst[1]=0x40 | ((pid>>8)&0x1F);
	pDst[2]=pid & 0xff;
	pDst[3]=0x10 | (count%16);	

	memcpy(pDst+4, pSrc, XDB_FRAM_LEN);
}


void CMMB_DTMB_MUX::CreateRS(BYTE* pSrc, BYTE* pDst, UINT count)
{
	if(!rsHandle || !pDst || !pSrc)return;
	
	memset(pDst, 0, RS_MSG_LEN);
	memcpy(pDst, pSrc, count);
	encode_rs_char(rsHandle, pDst, pDst + RS_K); 	
}


void CMMB_DTMB_MUX::CreateXPE(BYTE* pSrc, BYTE* xpe_buf, UINT srcLen, thread_params_t* pThreadParams, SERVICE_PARAM_t* pServiceParam, U8 packet_ID)
{
	BYTE*	payload_buf = NULL;
	U32		payload_length = 0;
	U32     xpe_length = 0;
	U32		crc_32 = 0;
	U32		xpe_single_packet_addin = 0;

	payload_buf = pSrc;
	payload_length = srcLen;

	xpe_single_packet_addin =  5 + 4;   //xpe header + CRC 
	xpe_length = payload_length + xpe_single_packet_addin;	

	xpe_buf[0] = 0x00;
	xpe_buf[0] |= 0x80;										//开始标志置1
	xpe_buf[0] |= 0x40;										//结束标志置1
	//净荷类型
	if (pServiceParam->FEC_style != FEC_STYLE_NULL)
	{
		//xpe_buf[0] |= 0x10;									//进行过前向纠错的业务数据
	}
	xpe_buf[0] |= (payload_length & 0xf00) >> 8;			//长度位12bits

	xpe_buf[1] = (payload_length & 0x0ff);

	xpe_buf[2] = 0x00;
	//if (pServiceParam->bCRC32Indic)
	{
		xpe_buf[2] |= 0x80;								//CRC指示1bits
	}
	xpe_buf[2] |= (packet_ID & 0x7f);					//数据包标识:7bits

	xpe_buf[3] = 0x00;
	xpe_buf[3] |= ((0x01 & 0x01) << 7);	//业务模式指示:1bit
	xpe_buf[3] |= 0x7f;											//reserved : 7bits

	xpe_buf[4] = CalculateCheckSum(xpe_buf, 4);				//校验和:8bits

	memcpy(xpe_buf + 5, payload_buf, payload_length);
	//FEC纠错能力测试
	//				if (gkThreadParams.add_error_count > 0)
	//				{
	//					memset(xpe_buf + 5, 0x00, gkThreadParams.add_error_count);
	//				}

	if (pServiceParam->bCRC32Indic)
	{
		crc_32 = Encode_CRC_32(payload_buf, payload_length);
		xpe_buf[xpe_length - 4] = (crc_32 & 0xff000000) >> 24;
		xpe_buf[xpe_length - 3] = (crc_32 & 0x00ff0000) >> 16;
		xpe_buf[xpe_length - 2] = (crc_32 & 0x0000ff00) >> 8;
		xpe_buf[xpe_length - 1] = (crc_32 & 0x000000ff) >> 0;
	}
}


void CMMB_DTMB_MUX::CreateUdpMsg(BYTE *pSrc, BYTE *pDst, UINT srclen, SERVICE_PARAM_t* pServiceParam)
{
	if(!pSrc || !pDst)return;

	memset(pDst, 0, MSG_PKG_LEN);

	pDst[0] = 0x09;	//版本号	
	pDst[1] = 0xf1; //保留位
	pDst[2] = 0xa0;	//xpe 包
	pDst[3] = (pServiceParam->udp_count & 0xff00) >> 8;//UDP包序号高两位，递增1
    pDst[4] = (pServiceParam->udp_count & 0x00ff) ;//UDP包序号低两位
    pDst[5] = (pServiceParam->service_ID & 0xff00) >> 8;//存放service_id的高两位
    pDst[6] = (U8)(pServiceParam->service_ID & 0x00ff); //存放service_id的低两位
	pDst[7] = ((0x0f)<<4);
	pDst[7] |=(pServiceParam->msg_count & 0xf00) >> 8;	//数据序号高位
   	pDst[8] = (pServiceParam->msg_count & 0x0ff);	//数据序号低位

	memcpy(pDst+9, pSrc, srclen);
}


void CMMB_DTMB_MUX::DataRate(int rate)
{	
	int speed=0;
	UINT addBytes=0;
	
	speed = rate;

	printf("speed is %d Mbit/s\n", speed);

	if(5 == speed)
	{
		addBytes = 15*1024;
	}
	else if(6 == speed)
	{
		addBytes = 18*1024;
	}
	else if(7 == speed)
	{
		addBytes = 40*1024;
	}
	else if(8 == speed)
	{
		addBytes = 50*1024;
	}
	else if(9 == speed)
	{
		addBytes = 60*1024;
	}
	else if(10 == speed)
	{
		addBytes = 70*1024;
	}
	else if(11 == speed)
	{
		addBytes = 100*1024;
	}
	else if(12 == speed)
	{
		addBytes = 105*1024;
	}
	else if(13 == speed)
	{
		addBytes = 110*1024;
	}
	else if(14 == speed)
	{
		addBytes = 130*1024;
	}
	else if(15 == speed)
	{
		addBytes = 150*1024;
	}
	else if(16 == speed)
	{
		addBytes = 220*1024;
	}
	else if(17 == speed)
	{
		addBytes = 230*1024;
	}
	else if(18 == speed)
	{
		addBytes = 285*1024;
	}
	else if(19 == speed)
	{
		addBytes = 340*1024;
	}
	else if(20 == speed)
	{
		addBytes = 350*1024;
	}
	else
	{
		addBytes=0;
	}

	///("speed is %d", speed);
	
	m_netRate = 1000000/((speed*1024*1024/8+addBytes)/1316);	

}



void CMMB_DTMB_MUX::NetInit(thread_params_t* pthrParam)
{
	thread_params_t* pThreadParams = pthrParam;

	if((pThreadParams->SERVICE_PARAMS[0].alloc_bps > 0) &&
	   (pThreadParams->SERVICE_PARAMS[0].alloc_bps <= 10)
	  )
	{
	  	DataRate(pThreadParams->SERVICE_PARAMS[0].alloc_bps);		
	}
	else
	{
		DataRate(1);
	}


	//gsockUdp=socket(AF_INET,SOCK_DGRAM,0);		
	//setsockopt(gsockClient, SOL_SOCKET, )
	//int nBufLength = 1024*1024*32;
	//setsockopt(gsockUdp, SOL_SOCKET, SO_SNDBUF, (char*)&nBufLength, sizeof(nBufLength));

	//unsigned   long   param=1;   
	//if(SOCKET_ERROR==ioctlsocket(gsockUdp,FIONBIO,&param))
	//{
		//printf("Can't set block!\n");
		//return;
	//}
		
    //gaddrOut.sin_family=AF_INET;	
    //gaddrOut.sin_addr.s_un.s_addr=inet_addr(pThreadParams->pszMuxIP);//复用器ip—mux_ip从输出配置页面输入
    //gaddrOut.sin_port = htons(pThreadParams->mux_port);	

	//=================================================
	BOOL boolFlag = m_sUdp.Open(pThreadParams->pszControlIP, 5000);
	if(!boolFlag)
	{
		printf("Can't bind the ip\n");
		return;
	}

	boolFlag = m_sUdp.Connect(pThreadParams->pszMuxIP, pThreadParams->mux_port, pThreadParams->pszControlIP);

	if(!boolFlag)
	{
		printf("Can't connect destination ip\n");
	}
	
	
}


/*
UINT WriteNetData(PBYTE pBuffer, int writeSize)
{
	UINT length = sendto(gsockUdp,(const char*)pBuffer,writeSize,0,(SOCKADDR*)&gaddrOut,sizeof(SOCKADDR_IN));
	return length;
}

*/

WORD CMMB_DTMB_MUX::GetFileStreamNo(void)
{
	WORD streamNo = 0;	
	srand(time(NULL));
	streamNo = rand()%65536;

	return streamNo;
}

// File control frame
void CMMB_DTMB_MUX::CreateControlFrame(BYTE* pDst, BYTE counter, WORD fileStreamNo, UINT sumFrame, UINT fileLen, const char* fileName)
{
	if(!fileName)return;

	//CString pidStr, groupIdStr, userIdStr;

	WORD pid=0;
	WORD groupId=0;
	WORD userId=0;
	WORD crc=0;
	BYTE index=0;
	BYTE len=0;
	len=(BYTE)strlen(fileName);
	//m_editPid.GetWindowTextA(pidStr);
	//m_editGroupId.GetWindowTextA(groupIdStr);
	//m_editUserId.GetWindowTextA(userIdStr);
	pid=(WORD)atoi(DVBPID);
	groupId=(WORD)atoi(DVBGROUPID);
	userId=(WORD)atoi(DVBUSERID);

	//Ts header
	pDst[index++]=0x47;
	pDst[index++]=0x40 | ((pid>>8)&0x1F);
	pDst[index++]=pid & 0xff;
	pDst[index++]=0x10 | (counter%16);

	//xdb header
	pDst[index++]=0xff; //sync
	pDst[index++]=0x00; //len   index=5
	pDst[index++]=0x00; //crc high index=6
	pDst[index++]=0x00; //crc low index=7
	pDst[index++]=0x10; //type
	pDst[index++]=(fileStreamNo>>8)&0xff;
	pDst[index++]=(fileStreamNo)&0xff;	//file stream no
	pDst[index++]=0;
	pDst[index++]=0;
	pDst[index++]=0;
	pDst[index++]=0;	//fixed to 0
	//group ID
	pDst[index++]=(groupId>>8)&0xff;
	pDst[index++]=(groupId)&0xff;   //group ID
	//user ID
	pDst[index++]=(userId>>8)&0xff;
	pDst[index++]=(userId)&0xff;    //user ID
	//sum frame number
	pDst[index++]=(sumFrame>>24)&0xff;
	pDst[index++]=(sumFrame>>16)&0xff;
	pDst[index++]=(sumFrame>>8)&0xff;
	pDst[index++]=(sumFrame)&0xff;
	//file length
	pDst[index++]=(fileLen>>24)&0xff;
	pDst[index++]=(fileLen>>16)&0xff;
	pDst[index++]=(fileLen>>8)&0xff;
	pDst[index++]=(fileLen)&0xff;
	//fileName length
	pDst[index++]=len;  //filename length
	//TRACE("fileName is: %s, lenght is: %d", fileName, len);
	//file type
	pDst[index++]=0x56; //fixed
	pDst[index++]=0x41; //fixed
	//file name
	memcpy(pDst+index,fileName, len);
	index += len;
	//md5 value
	memcpy(pDst+index, m_checkSum, 16);
	index += 16;
	//crc check
	crc = Get_16bitCRC(0, pDst+8, index-8);
	pDst[6] = (crc>>8)&0xff;
	pDst[7] = (crc)&0xff;	
	//frame len;
	pDst[5] = index-9;
}

/*
void SleepExt(unsigned long us)
{
	LARGE_INTEGER PT_litmp;
	LONGLONG      PT_QPart1,PT_QPart2;
	double        PT_dfMinus,PT_dfFreq,PT_dfTim;
	
	QueryPerformanceFrequency(&PT_litmp);
	PT_dfFreq=(double)PT_litmp.QuadPart; //获得系统的主频

	QueryPerformanceCounter(&PT_litmp);
	PT_QPart1=PT_litmp.QuadPart;        //获得起始的值

	do{
		QueryPerformanceCounter(&PT_litmp);
		PT_QPart2=PT_litmp.QuadPart;    //获得结束计数
		PT_dfMinus=(double)(PT_QPart2-PT_QPart1);
		PT_dfTim=PT_dfMinus/PT_dfFreq; //计算出程序运行的时间，单位s
	
	}while(PT_dfTim*1000000<us);

}
//*/


unsigned char CMMB_DTMB_MUX::GetContinueCounter(BYTE* pSrc, unsigned int len, BYTE off, unsigned int flen)
{
	UINT ii=0;
	INT  jj=0;
	BYTE *ptr=NULL;
	BYTE Counter[10]={0};

	memset(Counter, 255, 10);

	for(ii=0; ii<len;)
	{

		if(CMMB_MODE == m_sendType)
		{
			if(0==ii)
				ptr = pSrc + ii + 3 + off;
			else
				ptr = pSrc + ii + 3 + 5;
		}
		else if(DTMB_MODE == m_sendType)
		{
			ptr = pSrc + ii + 3;
		}

		//==========================
		
		Counter[jj] = ptr[0]&0x0f;

		//==========================
		

		if(CMMB_MODE == m_sendType)
		{
			if(0==ii)
				ii += flen + 9;
			else
				ii += flen;
		}
		else if(DTMB_MODE == m_sendType)
		{
			ii=ii + TS_FRAM_LEN;
		}		
		
		//TRACE("Continue counter value: %d\n", Counter[jj]);
		jj++;
	}

	for(jj=9;jj>0;jj--)
	{
		if(Counter[jj] < 255)break;		
	}

	return Counter[jj];	
}




///*
UINT CMMB_DTMB_MUX::SendFile(const char *fname)
{
	//======================================================
	fstream xsendFile;
	const char* fileName;

	UINT retLen=0;	
	UINT restBytes=0;
	UINT proBytes=0;
	UINT stepBytes=0;
	//UINT packCou=0;
	//UINT sendBytes=0;
	BOOL hasDataFlag=FALSE;
	UINT fileLen=0;
	UINT sendCou=0;

	UINT wrtNum=0;
	UINT sendPacketCou=0;

	//=====================================================
	UINT frameLen=0;
	UINT sendUnitLen=0;
	BYTE continueCou=0;
	BYTE tsOff=0;
	//CString strTmp;
	BOOL headerFlag = TRUE;
	fileName = fname;
	//fileName += extname;

	printf("%s()->send file name is %s\n", __func__, fileName);		

	xsendFile.open(fileName, ios::in | ios::binary);

	if(!xsendFile.is_open())
	{
		printf("%s()->Can't open the send file : %s.\n", __func__, fileName);
		return 0;
	}

	//=====================================================
	BYTE *fileBuf=NULL, *tmpBuf=NULL, *proBuf=NULL, *fillBuf=NULL;


	if(CMMB_MODE == m_sendType)
	{
		sendUnitLen=MSG_SEND_UNIT;
		frameLen=MSG_PKG_LEN;       //ts: TS_FRAME_LEN    msg: MSG_PKG_LEN    xpe:XPE_PKG_LEN
	}
	else if(DTMB_MODE == m_sendType)
	{
		sendUnitLen=TS_SEND_UNIT;
		frameLen=TS_SEND_UNIT;      //ts: TS_FRAME_LEN    msg: MSG_PKG_LEN    xpe:XPE_PKG_LEN
	}

	//=====================================================
	printf("%s()->send_type: %d, sendUnitLen: %d, frameLen: %d\n", __func__, m_sendType, sendUnitLen, frameLen);
	//=====================================================
	
	fileBuf = new BYTE[TS_SEND_PACKAGES*sendUnitLen];
	tmpBuf  = new BYTE[sendUnitLen];
	proBuf  = new BYTE[TS_SEND_BUF*sendUnitLen];
	fillBuf = new BYTE[frameLen];
	

	/*
	BYTE fileBuf[TS_SEND_PACKAGES*MSG_SEND_UNIT];
	BYTE tmpBuf[MSG_SEND_UNIT];
	BYTE proBuf[TS_SEND_BUF*MSG_SEND_UNIT];
	BYTE fillBuf[MSG_PKG_LEN];
	//*/

	//==========================================

	printf("fileBuf=%p, tmpBuf=%p, proBuf=%p, fillBuf=%p\n", fileBuf, tmpBuf, proBuf, fillBuf);

	//==========================================

	xsendFile.seekg(0, ios::end);
	fileLen = xsendFile.tellg();
	xsendFile.seekg(0, ios::beg);
	//printf("[%s] length is: %d\n", fileName, fileLen);	
	//fileLen = sendFile.GetLength();
	//UINT coux=0;
	//=================
	//UINT frame=0;			
	//======================================================

	//do
	//{			
		do
		{
			memset(fileBuf, 0, TS_SEND_PACKAGES*sendUnitLen);  //TS_SEND_PACKAGES*TS_SEND_UNIT

			//retLen = sendFile.Read(fileBuf, TS_SEND_PACKAGES*sendUnitLen);

			xsendFile.read((char*)fileBuf, TS_SEND_PACKAGES*sendUnitLen);

			retLen = xsendFile.gcount();

			//printf("Get %d bytes form file [%s]\n", retLen, fileName);
			//printf("File End check: %d\n", sendFile.eof());

			if(!retLen)break;

			//===============================================
			if(headerFlag)
			{
				headerFlag = FALSE;
				memset(fillBuf, 0, frameLen);       //MSG_PKG_LEN
				memcpy(fillBuf, fileBuf, frameLen); //MSG_PKG_LEN
			}
			//===============================================

			stepBytes=0;
			
			memset(proBuf, 0, TS_SEND_BUF*sendUnitLen);		//TS_SEND_UNIT

			if(hasDataFlag)
			{					
				memcpy(proBuf, tmpBuf, restBytes);
				memcpy(proBuf + restBytes, fileBuf, retLen);
				proBytes = restBytes + retLen;
			}
			else 
			{									
				memcpy(proBuf, fileBuf, retLen);
				proBytes = retLen;	
			}

			hasDataFlag=FALSE;
			restBytes=0;
			
			do
			{
				if(proBytes >= sendUnitLen)  //TS_SEND_UNIT
				{			
					//process the per package
					//===============================================================
					//===============================================================				

					//send to do
					wrtNum = m_sUdp.Write(proBuf + stepBytes, sendUnitLen);	// TS_SEND_UNIT
					//wrtNum = WriteNetData(proBuf + stepBytes, sendUnitLen);
			
					//frame = GetFrameNo(proBuf + stepBytes, TS_SEND_UNIT);
					//strTmp.Format("frame: %d\n", frame);					
					//TRACE(strTmp);

					if(CMMB_MODE == m_sendType)
					{
						usleep(TIME_SEND);
					}
					else if(DTMB_MODE == m_sendType)
					{
						usleep(m_netRate);
					}					
							
					//===============================================================
					//===============================================================
					proBytes  -= sendUnitLen;    //TS_SEND_UNIT
					stepBytes += sendUnitLen;	 //TS_SEND_UNIT

					if(!proBytes)stepBytes=0;

					//======================	
					sendPacketCou++;
					if(sendPacketCou%100 == 0)
					{						
						printf("Sending packet %d\n", sendPacketCou);						
					}
					//======================
					
				}
				else
				{
					if(proBytes > 0)
					{
						memset(tmpBuf, 0x00, sendUnitLen);      //TS_SEND_UNIT
						memcpy(tmpBuf, proBuf + stepBytes, proBytes);
						hasDataFlag=TRUE;
						restBytes = proBytes;
					}
					break;
				}
			}while(proBytes>0);
		}while(retLen>0);	
		

		//========================================================
		if(restBytes > 0)
		{
			//printf("%s()->restBytes=%d\n", __func__, restBytes);
			//process the last package
			//send the last package
			memset(tmpBuf, 0xff, sendUnitLen);    //TS_SEND_UNIT
			memcpy(tmpBuf, proBuf+stepBytes, restBytes);

			//===================================================================

			if(MSG_PKG_LEN == frameLen)
				tsOff=14;
			else if(XPE_PKG_LEN == frameLen)
				tsOff=5;
			else
				tsOff=0;


			//printf("The last package is %d Bytes\n", restBytes);			
			//====================================================================
			///*
			if(restBytes < sendUnitLen)          //TS_SEND_UNIT
			{
				continueCou = GetContinueCounter(tmpBuf, restBytes, tsOff, XPE_PKG_LEN);

				while(restBytes < sendUnitLen)   //TS_SEND_UNIT
				{
					continueCou++;
					if(continueCou > 15)continueCou=0;						
						
					fillBuf[3+tsOff] = (fillBuf[3+tsOff]&0xf0)| continueCou;				

					if(CMMB_MODE == m_sendType)
					{						
						memcpy(tmpBuf + restBytes, fillBuf+9, XPE_PKG_LEN);  //TS_FRAME_LEN	
						restBytes += XPE_PKG_LEN;  //TS_FRAM_LEN
					}
					else if(DTMB_MODE == m_sendType)
					{						
						memcpy(tmpBuf + restBytes, fillBuf, TS_FRAME_LEN);  //TS_FRAME_LEN	
						restBytes += TS_FRAM_LEN;
					}				
				}				
			}

			//*/
			//====================================================================
			//send to do
			m_sUdp.Write(tmpBuf, sendUnitLen);		//TS_SEND_UNIT
			//WriteNetData(tmpBuf, sendUnitLen);
			sendCou += sendUnitLen;				//TS_SEND_UNIT			
			//====================================================================
		}
		restBytes = 0;
	//}while(!m_bStop);
		sendPacketCou++;
		printf("File [%s] has been send completed! times: %d\n", fname, sendPacketCou);
		//sendFile.seekg(0, ios::beg);
	//}while(0);

	//========================================================
	//release the buf
	xsendFile.close();
	
	delete []fileBuf;
	delete []fillBuf;
	delete []tmpBuf;
	delete []proBuf;
	fileBuf=fillBuf=tmpBuf=proBuf=NULL;

	
	return 0;
}




#if 0

UINT SendCmmbFile(LPCSTR fname)
{
	//======================================================
	CFile sendFile;
	CString fileName;

	UINT retLen=0;	
	UINT restBytes=0;
	UINT proBytes=0;
	UINT stepBytes=0;
	UINT packCou=0;
	UINT sendBytes=0;
	BOOL hasDataFlag=FALSE;
	UINT fileLen=0;
	UINT sendCou=0;

	UINT wrtNum=0;

	//=====================================================
	fileName = fname;
	//fileName += extname;

	printf("file name is %s\n", fileName);
	
	sendFile.Open(fileName,0,0);	

	//=====================================================
	BYTE *fileBuf=NULL, *tmpBuf=NULL, *proBuf=NULL, *fillBuf=NULL;

	fileBuf = new BYTE[TS_SEND_PACKAGES*TS_SEND_UNIT];
	tmpBuf  = new BYTE[TS_SEND_UNIT];
	proBuf  = new BYTE[TS_SEND_BUF*TS_SEND_UNIT];
	
	fileLen = sendFile.GetLength();

	UINT coux=0;
	//=================

	UINT frame=0;
	UINT frameLen=0;
	UINT sendUnitLen=0;
	BYTE continueCou=0;
	BYTE tsOff=0;
	CString strTmp;
	BOOL headerFlag = TRUE;

	frameLen=MSG_PKG_LEN;  //ts: TS_FRAME_LEN    msg: MSG_PKG_LEN    xpe:XPE_PKG_LEN
	sendUnitLen=TS_SEND_UNIT;

	fillBuf = new BYTE[frameLen];
	//======================================================

	//do
	//{			
		do
		{
			memset(fileBuf, 0, TS_SEND_PACKAGES*sendUnitLen);  //TS_SEND_PACKAGES*TS_SEND_UNIT
			retLen = sendFile.Read(fileBuf, TS_SEND_PACKAGES*sendUnitLen);
			if(!retLen)break;

			//===============================================
			if(headerFlag)
			{
				headerFlag = FALSE;
				memset(fillBuf, 0, frameLen);       //MSG_PKG_LEN
				memcpy(fillBuf, fileBuf, frameLen); //MSG_PKG_LEN
			}
			//===============================================

			stepBytes=0;
			
			memset(proBuf, 0, TS_SEND_BUF*sendUnitLen);		//TS_SEND_UNIT

			if(hasDataFlag)
			{					
				memcpy(proBuf, tmpBuf, restBytes);
				memcpy(proBuf + restBytes, fileBuf, retLen);
				proBytes = restBytes + retLen;
			}
			else 
			{									
				memcpy(proBuf, fileBuf, retLen);
				proBytes = retLen;	
			}

			hasDataFlag=FALSE;
			restBytes=0;
			
			do
			{
				if(proBytes >= sendUnitLen)  //TS_SEND_UNIT
				{			
					//process the per package
					//===============================================================
					//===============================================================				

					//send to do
					wrtNum = m_sUdp.Write(proBuf + stepBytes, sendUnitLen);	// TS_SEND_UNIT
					//wrtNum = WriteNetData(proBuf + stepBytes, sendUnitLen);
			
					//frame = GetFrameNo(proBuf + stepBytes, TS_SEND_UNIT);
					//strTmp.Format("frame: %d\n", frame);					
					//TRACE(strTmp);

					//SleepExt(m_netRate);
					Sleep(40);
							
					//===============================================================
					//===============================================================
					proBytes  -= sendUnitLen;    //TS_SEND_UNIT
					stepBytes += sendUnitLen;	 //TS_SEND_UNIT

					if(!proBytes)stepBytes=0;
				}
				else
				{
					if(proBytes > 0)
					{
						memset(tmpBuf, 0x00, sendUnitLen);      //TS_SEND_UNIT
						memcpy(tmpBuf, proBuf + stepBytes, proBytes);
						hasDataFlag=TRUE;
						restBytes = proBytes;
					}
					break;
				}
			}while(proBytes>0);
		}while(retLen>0);

		//========================================================
		if(restBytes > 0)
		{
			//process the last package
			//send the last package
			memset(tmpBuf, 0xff, sendUnitLen);    //TS_SEND_UNIT
			memcpy(tmpBuf, proBuf+stepBytes, restBytes);

			//===================================================================

			if(MSG_PKG_LEN == frameLen)
				tsOff=14;
			else if(XPE_PKG_LEN == frameLen)
				tsOff=5;
			else
				tsOff=0;
			
			//====================================================================
			if(restBytes < sendUnitLen)          //TS_SEND_UNIT
			{
				continueCou = GetContinueCounter(tmpBuf, restBytes, tsOff, frameLen);

				while(restBytes < sendUnitLen)  //TS_SEND_UNIT
				{
					continueCou++;
					if(continueCou > 15)continueCou=0;						
						
					fillBuf[3+tsOff] = (fillBuf[3+tsOff]&0xf0)| continueCou; 
					memcpy(tmpBuf + restBytes, fillBuf, frameLen);  //TS_FRAME_LEN
					
					restBytes += frameLen;  //TS_FRAM_LEN

					//TRACE("Fill continue count: %d", continueCou);
				}				
			}
			//====================================================================
			//send to do
			m_sUdp.Write(tmpBuf, sendUnitLen);		//TS_SEND_UNIT
			//WriteNetData(tmpBuf, sendUnitLen);
			sendCou += sendUnitLen;				//TS_SEND_UNIT

			
			//m_progressBar.SetPos(sendCou*100/fileLen
			//frame = GetFrameNo(tmpBuf, TS_SEND_UNIT);
			//strTmp.Format("last frame: %d", frame);
			//TRACE(strTmp);
			//AfxMessageBox(strTmp);
			//====================================================================
		}
		restBytes = 0;
		//sendFile.SeekToBegin();
		//m_sendCou++;
		//strTmp.Format("%d", m_sendCou);
		//m_editCount.SetWindowTextA(strTmp);					

	//}while(!m_bStop);

	printf("File [%s] has been send completed!\n", fname);

	//========================================================
	//release the buf
	delete []fileBuf;
	delete []tmpBuf;
	delete []proBuf;

	sendFile.Close();
	return 0;
}

#endif

//*/



///=======================end the file========================
//============================================================

/*
void CTransferFileToolDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	

	CString strValue;
	int speed=0;
	UINT addBytes=0;
	
	UpdateData();
	speed = m_sliderNetband.GetPos();


	if(5 == speed)
	{
		addBytes = 15*1024;
	}
	else if(6 == speed)
	{
		addBytes = 18*1024;
	}
	else if(7 == speed)
	{
		addBytes = 40*1024;
	}
	else if(8 == speed)
	{
		addBytes = 50*1024;
	}
	else if(9 == speed)
	{
		addBytes = 60*1024;
	}
	else if(10 == speed)
	{
		addBytes = 70*1024;
	}
	else if(11 == speed)
	{
		addBytes = 100*1024;
	}
	else if(12 == speed)
	{
		addBytes = 105*1024;
	}
	else if(13 == speed)
	{
		addBytes = 110*1024;
	}
	else if(14 == speed)
	{
		addBytes = 130*1024;
	}
	else if(15 == speed)
	{
		addBytes = 150*1024;
	}
	else if(16 == speed)
	{
		addBytes = 220*1024;
	}
	else if(17 == speed)
	{
		addBytes = 230*1024;
	}
	else if(18 == speed)
	{
		addBytes = 285*1024;
	}
	else if(19 == speed)
	{
		addBytes = 340*1024;
	}
	else if(20 == speed)
	{
		addBytes = 350*1024;
	}
	else
	{
		addBytes=0;
	}

	TRACE("speed is %d", speed);
	
	m_netRate = 1000000/((speed*1024*1024/8+addBytes)/1316);
	 
	strValue.Format("%d", speed);
	m_editNetband.SetWindowTextA(strValue);
	UpdateData(FALSE);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
//*/


/*
bool IsLegalIp(const char* ip)
{
	BYTE ii=0, cou=0, pos;
	CStringArray ipArray;

	StringSplit(ip, ipArray, '.');

	if(((atoi(ipArray[0])>0) && (atoi(ipArray[0])<=255)) &&
		((atoi(ipArray[1])>=0) && (atoi(ipArray[1])<=255)) &&
		((atoi(ipArray[2])>=0) && (atoi(ipArray[2])<=255)) &&
		((atoi(ipArray[3])>=0) && (atoi(ipArray[3])<=255))		
	  )return true;


	return false;
}
//*/





unsigned int CMMB_DTMB_MUX::GetFrameNo(BYTE* pSrc, unsigned int len)
{
	UINT ii=0;
	INT jj=0;
	BYTE *ptr=NULL;
	UINT frame[10]={0};	

	for(ii=0; ii<len;)
	{
		ptr = pSrc + ii + 11;
		frame[jj] = (((UINT)ptr[0])<<24)|(((UINT)ptr[1])<<16)|(((UINT)ptr[2])<<8)|(((UINT)ptr[3]));
		ii=ii + TS_FRAM_LEN;
		//TRACE("frame no: %d\n", frame[jj]);
		jj++;
	}

	for(jj=9;jj>0;jj--)
	{
		if(frame[jj] > 0)break;		
	}

	return frame[jj];
}



string CMMB_DTMB_MUX::GetSndFile(void)
{
	if(CMMB_MODE == m_sendType)
	{
		return msgFileName;
	}
	else if(DTMB_MODE == m_sendType)
	{
		return tsFileName;
	}

	return "";
}



BOOL CMMB_DTMB_MUX::CreateSndFile(void)
{
	if(m_srcFileName.empty())return FALSE;
	
	int ii=0;	
	ii = m_srcFileName.rfind('/');
	UINT fnLen = m_srcFileName.length();
	//
	printf("%s-> m_srcFileName: %s\n",__func__, m_srcFileName.c_str());
	
	if(ii>=0)
	{		
		simpleFileName = m_srcFileName.substr(ii+1, fnLen-ii-1);
	}
	printf("simple file name: %s\n", simpleFileName.c_str());
	//TRACE("ii= %d, %s", ii, simpleFileName);
	string sndpath = CMD_FOLDER; 
	string sndpathx = "";
	
	sndpath  += CS_DSTFILE;
	sndpath  += "/";
	sndpathx += CS_DSTFILE;
	sndpathx += "/";
	sndpath  += simpleFileName;
	sndpathx += simpleFileName;

	m_tmpPath = sndpathx;
	system(sndpath.c_str());
	printf("create send folder: [%s]\n", sndpath.c_str());
	//======================================================
	if(encryptFlag)simpleFileName += ".aes";
	if(gkThreadParams.fec_type)simpleFileName += ".fec.0";

	cout<<"simpleFileName:"<<simpleFileName<<endl;
	//======================================================	
	//create the xdb file
	dstFileName  = sndpathx.c_str();
	dstFileName += "/";
	dstFileName += simpleFileName.c_str();		
	dstFileName += ".xdb";	
	//==============================
	tsFileName  = sndpathx.c_str();
	tsFileName += "/";
	tsFileName += simpleFileName.c_str();	
	tsFileName += ".ts";
	//==============================
	xpeFileName  = sndpathx.c_str();
	xpeFileName += "/";
	xpeFileName += simpleFileName.c_str();	
	xpeFileName += ".xpe";
	//==============================
	msgFileName  = sndpathx.c_str();
	msgFileName += "/";
	msgFileName += simpleFileName.c_str();	
	msgFileName += ".msg";  //msg_cap
	//==============================
	
	printf("dstfile: [%s]\ntsfile: [%s]\nxpefile: [%s]\nmsgfile: [%s]\n", dstFileName.c_str(), tsFileName.c_str(),
																		  xpeFileName.c_str(), msgFileName.c_str());

	return TRUE;
	
}

//===========================================================

BOOL CMMB_DTMB_MUX::SetFormatFile(const char *fname)
{
	m_srcFileName = fname;
	CreateSndFile();
	EncryptFile();
	return TRUE;
}


void CMMB_DTMB_MUX::SetSendMode(int send_type)
{
	m_sendType = send_type;
}




int CMMB_DTMB_MUX::FileCheckSum(void)
{  
   U8 buffer[1024];   
   U8 ii=0;
   U32 size = 0;
   U32 read_size = 0;

   if(m_encryptFile.empty())return -1;
   FILE *fp;
   U32 sum = 0;
   fp = fopen(m_encryptFile.c_str(), "r");
   fseek(fp,0,SEEK_END);
   size = ftell(fp); 
   fseek(fp,0,SEEK_SET);	

   Md5 md5;
   InitMd5(&md5);
   memset(m_checkSum, 0, sizeof(m_checkSum));
 
   do
   {	  
	  read_size = fread(buffer,1, sizeof(buffer), fp);
	  sum += read_size;	
   	  Md5Update(&md5, buffer, read_size);
   }while(sum != size); 

   Md5Final(&md5, m_checkSum);
   for(ii=0; ii<16;ii++)
     printf("%02x", m_checkSum[ii]);
     printf("\n\n");	

	return 0;
}




int CMMB_DTMB_MUX::EncryptFile(void)
{
	int ii=0;
	int  ret=0, ret2=0;
	UINT fnLen = 0;
	UINT flLen = 0;
	string fxName;
	string dstName;
	fstream srcFilex;
	fstream dstFilex;
	char srcTmp[16];
    char desTmp[16]; 
	BYTE pad = 0;
	Aes enc;
	
	ii = m_srcFileName.rfind('/');
    fnLen = m_srcFileName.length();	
	
	if(ii>=0)
	{		
		fxName = m_srcFileName.substr(ii+1, fnLen-ii-1);
	}
	else
	{
		return -1;
	}
	
	dstName  = m_tmpPath;
	dstName += "/";
	dstName += fxName;
	dstName += ".aes";
	m_encryptFile = dstName;
//	cout<<"1367   m_encryptFile:"<<m_encryptFile<<endl;///figure/ftproot/sndtmp/0.tar/0.tar.aes
	//==========================================================
	ret2 = AesSetKey(&enc, commonKey, AES_BLOCK_SIZE, commonIv, AES_ENCRYPTION);
	//==========================================================	
	srcFilex.open(m_srcFileName.c_str(), ios::in | ios::binary);
	srcFilex.seekg(0, ios::end);
	flLen = srcFilex.tellg();
	srcFilex.seekg(0, ios::beg);
	pad = 16 - (flLen%16);  
	
	dstFilex.open(dstName.c_str(), ios::out | ios::binary | ios::trunc);	
	//==========================================================
	do
	{
		memset(srcTmp, pad, sizeof(srcTmp));
	  	memset(desTmp, pad, sizeof(desTmp));
		
		srcFilex.read(srcTmp, sizeof(srcTmp));
		ret = srcFilex.gcount();
		//printf("Get %d bytes form file [%s]\n", ret, m_srcFileName.c_str());
		//printf("File End check: %d\n", srcFile.eof());

		if(!ret)break;
		AesCbcEncrypt(&enc, (byte*)desTmp, (byte*)srcTmp, sizeof(srcTmp));
		dstFilex.write(desTmp, sizeof(desTmp));
	}while(ret);
	//===========================================================

	srcFilex.close();
	dstFilex.close();
	sleep(1);
	FecEncode();
	FileCheckSum();	

	return 0;
}

int CMMB_DTMB_MUX::FecEncode()
{
	int    fec_type  = gkThreadParams.fec_type;
	int    fec_width = gkThreadParams.fec_width;
	int    fec_rate  = gkThreadParams.fec_rate;
//	int    fec_num   = gkThreadParams.fec_num;

	if(fec_type == 0)
	{
		cout<<"Without FecEncode !"<<endl;
		return 1;
	}
	
	char cmdline[1024] = {0};
	string src_file;
	string fec_file;
 
	if(encryptFlag)   
		src_file = m_encryptFile;   //sndtmp/xx.tar.aes
	else	
		src_file = m_srcFileName; //bsfile/xx.tar
	
	fec_file = src_file + ".fec.0";
		
	cout<<"m_encryptFile is: "<<m_encryptFile<<endl; 
	cout<<"src_file is: "<<src_file<<endl;  
	cout<<"fec_file name is: "<<fec_file<<endl;
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "sarifec -S -i:%s -o:%s -x:%d -K:%d -X:%d -T:173", 
		src_file.c_str(), fec_file.c_str(), fec_type, fec_width, fec_rate);
	system(cmdline);

	cout<<cmdline<<endl;
	return 0;
}

void CMMB_DTMB_MUX::FormatChange(UINT idx)
{		
	UINT m_srcFileLen=0;	
	UINT retLen=0;
	UINT restBytes=0;
	UINT proBytes=0;
	UINT stepBytes=0;
	UINT ii=0;
	UINT fileFrameNo=0;
	UINT outBufPos=0;
	BYTE tsCounter=0;
	UINT sumFramex=0;
	UINT tsoutBufPos=0;
	UINT xpeoutBufPos=0;
	UINT msgoutBufPos=0;	//msg_cap
	UINT xpeIndex=0;
	BYTE smallFlag=0;
	string packFileName;
	WORD fileStreamNo=0;
	BOOL hasDataFlag=FALSE;
	//BOOL boolFlag=FALSE;
	BYTE *fileBuf=NULL, *xdbBuf=NULL, *proBuf=NULL, *tmpBuf=NULL, *outBuf=NULL, *tsoutBuf=NULL, *tsBuf=NULL, *fcfBuf=NULL;
	//msg_cap
	BYTE *xpeoutBuf=NULL, *msgoutBuf=NULL, *xpetmpBuf=NULL;
	BYTE pkt_pid = 0;
	//==============================
	//msg_cap
	BYTE  msgBuf[MSG_PKG_LEN];
	BYTE  rsBuffer[RS_MSG_LEN];
	BYTE  xpeBuf[XPE_PKG_LEN];

	//==============================

	if(encryptFlag)
		packFileName = m_encryptFile;
	else
		packFileName = m_srcFileName;
	if(packFileName.empty())return;
	
	if(gkThreadParams.fec_type)packFileName += ".fec.0";
	
	cout<<" in FormatChange-> packFileName"<<packFileName<<endl;
	//==============================
	rsHandle = (PRS_PARAM_t)init_rs_char(RS_SYMSIZE, 0x11d, RS_FCR, RS_PRIM, RS_T2,RS_PAD);

	if(rsHandle == NULL)
	{
		printf("Init RS  failed!\n");
		return;
	}
	//==============================	

	srcFile.open(packFileName.c_str(), ios::in | ios::binary);

	//==============================
	srcFile.seekg(0, ios::end);
	m_srcFileLen = srcFile.tellg();
	srcFile.seekg(0, ios::beg);
	//printf("[%s] length is: %d\n", m_srcFileName.c_str(), m_srcFileLen);
	//m_srcFileLen = srcFile.GetLength();
	//sum file frames
	sumFramex = m_srcFileLen/173;
	if(m_srcFileLen%173)sumFramex++;
	

	dstFile.open(dstFileName.c_str(), ios::out | ios::binary | ios::trunc);
	tsFile.open(tsFileName.c_str(), ios::out | ios::binary | ios::trunc);
	xpeFile.open(xpeFileName.c_str(), ios::out | ios::binary | ios::trunc);
	msgFile.open(msgFileName.c_str(), ios::out | ios::binary | ios::trunc);
	
	//===================================================
	fileBuf 	= new BYTE[READ_BUF_LEN*XDB_DATA_LEN];
	xdbBuf 		= new BYTE[XDB_FRAM_LEN];
	tmpBuf 		= new BYTE[XDB_DATA_LEN];
	tsBuf  		= new BYTE[TS_FRAM_LEN];
	proBuf 		= new BYTE[PROS_BUF_LEN*XDB_DATA_LEN];
	outBuf 		= new BYTE[READ_BUF_LEN*XDB_FRAM_LEN];
	tsoutBuf 	= new BYTE[READ_BUF_LEN*TS_FRAM_LEN*2];
	fcfBuf 		= new BYTE[TS_FRAM_LEN];
	xpeoutBuf 	= new BYTE[READ_BUF_LEN*XPE_PKG_LEN*2];
	msgoutBuf	= new BYTE[READ_BUF_LEN*MSG_PKG_LEN*2];    //msg_cap
	xpetmpBuf	= new BYTE[XPE_PKG_NUM*XPE_PKG_LEN*10];
	//===================================================
	memset(xpetmpBuf, 0, XPE_PKG_NUM*XPE_PKG_LEN*10);

	fileStreamNo = GetFileStreamNo();
	//fileStreamNo = 0x01;	
	
	if(fileBuf && tmpBuf && proBuf && outBuf)
	{
		do
		{			
			memset(fileBuf, 0, READ_BUF_LEN*XDB_DATA_LEN);

			//retLen = srcFile.Read(fileBuf, READ_BUF_LEN*XDB_DATA_LEN);

			srcFile.read((char*)fileBuf, READ_BUF_LEN*XDB_DATA_LEN);

			retLen = srcFile.gcount();

			//printf("Get %d bytes form file [%s]\n", retLen, packFileName.c_str());

			//printf("File End check: %d\n", srcFile.eof());

			//========================================
			//retLen = 15;			
			//for(ii=0; ii<15; ii++)fileBuf[ii]=ii+1;
			//========================================

			if(!retLen)break;

			stepBytes=0;
			outBufPos=0;
			tsoutBufPos=0;
			xpeoutBufPos=0;
			msgoutBufPos=0;   //msg_cap
			//xpeIndex = 0;
			
			memset(proBuf, 0, PROS_BUF_LEN*XDB_DATA_LEN);	
			memset(outBuf, 0, XDB_FRAM_LEN*READ_BUF_LEN);
			memset(tsoutBuf, 0, TS_FRAM_LEN*READ_BUF_LEN*2);
			memset(xpeoutBuf, 0, READ_BUF_LEN*XPE_PKG_LEN*2);
			memset(msgoutBuf, 0, READ_BUF_LEN*MSG_PKG_LEN*2);   //msg_cap
			//memset(xpetmpBuf, 0, XPE_PKG_NUM*XPE_PKG_LEN*10);

			if(hasDataFlag)
			{					
				memcpy(proBuf, tmpBuf, restBytes);
				memcpy(proBuf + restBytes, fileBuf, retLen);
				proBytes = restBytes + retLen;
			}
			else 
			{									
				memcpy(proBuf, fileBuf, retLen);
				proBytes = retLen;	
			}

			hasDataFlag=FALSE;
			restBytes=0;

			do
			{
				if(proBytes >= XDB_DATA_LEN)
				{			
					//process the per package
					//===========================					
					fileFrameNo++;
					outBufPos++;
					tsoutBufPos++;
					xpeoutBufPos++;
					//if(!msgoutBufPos)msgoutBufPos++;
					xpeIndex++;
					
					memset(xdbBuf, 0xff, XDB_FRAM_LEN);
					memset(tsBuf, 0xff, TS_FRAM_LEN);
					memset(xpeBuf, 0, XPE_PKG_LEN);		

					//Sleep(1);

					//=====================================================================================================
					//=====================================================================================================
					//wirte the file control frame					
					if(0x01 == fileFrameNo)
					{				
					//	for(ii=0;ii<idx+1;ii++)//========zxy
						for(ii=0;ii<10;ii++)
						{							
							memset(fcfBuf, 0, TS_FRAM_LEN);
							CreateControlFrame(fcfBuf, tsCounter, fileStreamNo, sumFramex , m_srcFileLen, simpleFileName.c_str());
							tsCounter++;
							memcpy(tsoutBuf + (tsoutBufPos-1)*TS_FRAM_LEN, fcfBuf, TS_FRAM_LEN);
							tsoutBufPos++;
							//=================================
							CreateRS(fcfBuf, rsBuffer, TS_FRAM_LEN);
							CreateXPE(rsBuffer, xpeBuf, RS_MSG_LEN, &gkThreadParams, &(gkThreadParams.SERVICE_PARAMS[idx]), pkt_pid);
							memcpy(xpeoutBuf + (xpeoutBufPos-1)*XPE_PKG_LEN, xpeBuf, XPE_PKG_LEN);
							
							pkt_pid++;
							if(!(pkt_pid%128))pkt_pid=0;

							//===================================
							//xpeIndex=xpeoutBufPos%6;
							//if(!xpeIndex)xpeIndex=1;

							//if(xpeIndex)
							{
								memcpy(xpetmpBuf+(xpeIndex-1)*XPE_PKG_LEN, xpeBuf, XPE_PKG_LEN);

								if(XPE_PKG_NUM == xpeIndex)
								{
									xpeIndex = 0;
									CreateUdpMsg(xpetmpBuf, msgBuf, XPE_PKG_LEN*XPE_PKG_NUM, &(gkThreadParams.SERVICE_PARAMS[idx]));  //msg_cap
									//memcpy(msgoutBuf + (msgoutBufPos-1)*MSG_PKG_LEN, msgBuf, MSG_PKG_LEN); //msg_cap
									memcpy(msgoutBuf + msgoutBufPos*MSG_PKG_LEN, msgBuf, MSG_PKG_LEN); //msg_cap

									msgoutBufPos++;  //msg_cap	

									memset(xpetmpBuf, 0, XPE_PKG_NUM*XPE_PKG_LEN*10);									
								}
							}								
					
							//===================================	
							xpeoutBufPos++;
							xpeIndex++;
							//CreateUdpMsg(xpeBuf, msgBuf, XPE_PKG_LEN, &gpThreadParams->SERVICE_PARAMS[idx]);  //msg_cap
							//memcpy(msgoutBuf + (msgoutBufPos-1)*MSG_PKG_LEN, msgBuf, MSG_PKG_LEN); //msg_cap
							//msgoutBufPos++;  //msg_cap
							//===================================							
							memset(xpeBuf, 0, XPE_PKG_LEN);
							//=================================
						}
						printf("Control frame is 1.\n");
						memset(FillBuffer, 0, TS_FRAM_LEN);
						memcpy(FillBuffer, fcfBuf, TS_FRAM_LEN);
					}
					else
					{
					#if 1
						if(!(fileFrameNo%3000))
						{
							memset(fcfBuf, 0, TS_FRAM_LEN);
							CreateControlFrame(fcfBuf, tsCounter, fileStreamNo, sumFramex , m_srcFileLen, simpleFileName.c_str());
							tsCounter++;
							memcpy(tsoutBuf + (tsoutBufPos-1)*TS_FRAM_LEN, fcfBuf, TS_FRAM_LEN);
							tsoutBufPos++;
							//=================================
							CreateRS(fcfBuf, rsBuffer, TS_FRAM_LEN);
							CreateXPE(rsBuffer, xpeBuf, RS_MSG_LEN, &gkThreadParams, &(gkThreadParams.SERVICE_PARAMS[idx]), pkt_pid);
							memcpy(xpeoutBuf + (xpeoutBufPos-1)*XPE_PKG_LEN, xpeBuf, XPE_PKG_LEN);
							
							pkt_pid++;
							if(!(pkt_pid%128))pkt_pid=0;

							//CreateUdpMsg(xpeBuf, msgBuf, XPE_PKG_LEN, &gpThreadParams->SERVICE_PARAMS[idx]);  //msg_cap
							//memcpy(msgoutBuf + (msgoutBufPos-1)*MSG_PKG_LEN, msgBuf, MSG_PKG_LEN); //msg_cap
							//msgoutBufPos++;  //msg_cap
							//===================================
							//xpeIndex=xpeoutBufPos%6;
							//if(!xpeIndex)xpeIndex=1;

							//if(xpeIndex)
							{
								memcpy(xpetmpBuf+(xpeIndex-1)*XPE_PKG_LEN, xpeBuf, XPE_PKG_LEN);

								if(XPE_PKG_NUM == xpeIndex)
								{
									xpeIndex=0;
									CreateUdpMsg(xpetmpBuf, msgBuf, XPE_PKG_LEN*XPE_PKG_NUM, &(gkThreadParams.SERVICE_PARAMS[idx]));  //msg_cap
									//memcpy(msgoutBuf + (msgoutBufPos-1)*MSG_PKG_LEN, msgBuf, MSG_PKG_LEN); //msg_cap
									memcpy(msgoutBuf + msgoutBufPos*MSG_PKG_LEN, msgBuf, MSG_PKG_LEN); //msg_cap

									msgoutBufPos++;  //msg_cap	

									memset(xpetmpBuf, 0, XPE_PKG_NUM*XPE_PKG_LEN*10);									
								}
							}					
							//===================================	
							xpeoutBufPos++;		
							xpeIndex++;
							memset(xpeBuf, 0, XPE_PKG_LEN);
							//=================================
						}
						#endif
					}
					 
					//=========================================================================================================
					//=========================================================================================================
					//write the xdb frame
					CreateXDB(proBuf + stepBytes, xdbBuf, fileStreamNo, fileFrameNo, XDB_DATA_LEN);
					CreateTS(xdbBuf, tsBuf, tsCounter);
					CreateRS(tsBuf, rsBuffer, TS_FRAM_LEN);
					CreateXPE(rsBuffer, xpeBuf, RS_MSG_LEN, &gkThreadParams, &(gkThreadParams.SERVICE_PARAMS[idx]), pkt_pid);
					//CreateUdpMsg(xpeBuf, msgBuf, XPE_PKG_LEN, &gpThreadParams->SERVICE_PARAMS[idx]);  //msg_cap
					//===================================
					//xpeIndex=xpeoutBufPos%6;
					//if(!xpeIndex)xpeIndex=1;

					//printf("Process 1. msgoutBufPos is %d\n", msgoutBufPos);

					//if(xpeIndex)
					{
						memcpy(xpetmpBuf+(xpeIndex-1)*XPE_PKG_LEN, xpeBuf, XPE_PKG_LEN);

						if(XPE_PKG_NUM == xpeIndex)
						{
							xpeIndex=0;
							CreateUdpMsg(xpetmpBuf, msgBuf, XPE_PKG_LEN*XPE_PKG_NUM, &(gkThreadParams.SERVICE_PARAMS[idx]));  //msg_cap
							memcpy(msgoutBuf + msgoutBufPos*MSG_PKG_LEN, msgBuf, MSG_PKG_LEN); //msg_cap
							msgoutBufPos++;  //msg_cap	

							memset(xpetmpBuf, 0, XPE_PKG_NUM*XPE_PKG_LEN*10);									
						}
					}					
					//===================================	
					tsCounter++;
					pkt_pid++;

					if(!(pkt_pid%128))pkt_pid=0;
					memcpy(outBuf + (outBufPos-1)*XDB_FRAM_LEN, xdbBuf, XDB_FRAM_LEN);
					memcpy(tsoutBuf + (tsoutBufPos-1)*TS_FRAM_LEN, tsBuf, TS_FRAM_LEN);
					memcpy(xpeoutBuf + (xpeoutBufPos-1)*XPE_PKG_LEN, xpeBuf, XPE_PKG_LEN);
					//memcpy(msgoutBuf + (msgoutBufPos-1)*MSG_PKG_LEN, msgBuf, MSG_PKG_LEN);

					//===========================
					proBytes -= XDB_DATA_LEN;
					stepBytes += XDB_DATA_LEN;

					if(!proBytes)stepBytes=0;
				}
				else
				{
					if(proBytes > 0)
					{
						memset(tmpBuf, 0xff, XDB_DATA_LEN);
						memcpy(tmpBuf,	proBuf + stepBytes, proBytes);
						hasDataFlag=TRUE;
						restBytes = proBytes;
					}
					break;
				}
			}while(proBytes>0);

			//once process
			if(fileFrameNo>0)
			{
				dstFile.write((char*)outBuf, outBufPos*XDB_FRAM_LEN);
				tsFile.write((char*)tsoutBuf, tsoutBufPos*TS_FRAM_LEN);
				xpeFile.write((char*)xpeoutBuf, xpeoutBufPos*XPE_PKG_LEN);
				//msg_cap
				//printf("Write file. msgoutBufPos is %d\n", msgoutBufPos);
				
				msgFile.write((char*)msgoutBuf, msgoutBufPos*MSG_PKG_LEN);
			}

		}while(retLen);
	}
	else
	{
	//	AfxMessageBox(__T("Can't get the enough memory!"));
	}	
	
	if(restBytes > 0)
	{
		//process the last package
		//====================================================================
		//printf("fileFrameNo: %d\n", fileFrameNo);
		
		if(0 == fileFrameNo)
		{
			//fileFrameNo++;
			//outBufPos++;
			//xpeIndex=xpeoutBufPos%6;
			tsoutBufPos++;			
			xpeoutBufPos++;
			msgoutBufPos++;
			xpeIndex++;

			smallFlag = 0xaa;

			memset(xpeBuf, 0, XPE_PKG_LEN);

		//	for(ii=0;ii<idx+1;ii++)//====zxy=====
			for(ii=0;ii<10;ii++) 
			{						
				memset(fcfBuf, 0, TS_FRAM_LEN);
				CreateControlFrame(fcfBuf, tsCounter, fileStreamNo, sumFramex , m_srcFileLen, simpleFileName.c_str());
				tsCounter++;
				memcpy(tsoutBuf + (tsoutBufPos-1)*TS_FRAM_LEN, fcfBuf, TS_FRAM_LEN);
				tsoutBufPos++;
				//=================================
				CreateRS(fcfBuf, rsBuffer, TS_FRAM_LEN);
				CreateXPE(rsBuffer, xpeBuf, RS_MSG_LEN, &gkThreadParams, &(gkThreadParams.SERVICE_PARAMS[idx]), pkt_pid);
				//CreateUdpMsg(xpeBuf, msgBuf, XPE_PKG_LEN, &gpThreadParams->SERVICE_PARAMS[idx]);  //msg_cap
				//===================================
				//xpeIndex=xpeoutBufPos%6;
				//if(!xpeIndex)xpeIndex=1;

				//printf("xpeIndex=%d, xpeoutBufPos=%d\n", xpeIndex, xpeoutBufPos);
				//if(xpeIndex)
				{
					memcpy(xpetmpBuf+(xpeIndex-1)*XPE_PKG_LEN, xpeBuf, XPE_PKG_LEN);
					//memcpy(xpetmpBuf+xpeIndex*XPE_PKG_LEN, xpeBuf, XPE_PKG_LEN);

					if(XPE_PKG_NUM == xpeIndex)
					{
						xpeIndex=0;
						//printf("Create msg frame! xpeIndex=%d\n", xpeIndex);
						CreateUdpMsg(xpetmpBuf, msgBuf, XPE_PKG_LEN*XPE_PKG_NUM, &(gkThreadParams.SERVICE_PARAMS[idx]));  //msg_cap
						memcpy(msgoutBuf + (msgoutBufPos-1)*MSG_PKG_LEN, msgBuf, MSG_PKG_LEN); //msg_cap
						msgoutBufPos++;  //msg_cap	

						memset(xpetmpBuf, 0, XPE_PKG_NUM*XPE_PKG_LEN*10);									
					}
				}					
				//===================================
				memcpy(xpeoutBuf + (xpeoutBufPos-1)*XPE_PKG_LEN, xpeBuf, XPE_PKG_LEN);
				xpeoutBufPos++;
				xpeIndex++;
				pkt_pid++;
				if(!(pkt_pid%128))pkt_pid=0;
				//memcpy(msgoutBuf + (msgoutBufPos-1)*MSG_PKG_LEN, msgBuf, MSG_PKG_LEN);  //msg_cap
				//msgoutBufPos++;  //msg_cap
				//=================================
			}
			//memset(FillBuffer, 0, TS_FRAM_LEN);
			//memcpy(FillBuffer, fcfBuf, TS_FRAM_LEN);
			printf("Control Frame is 0.\n");


			if(!fileFrameNo)
			{
				//dstFile.Write(outBuf, outBufPos*XDB_FRAM_LEN);
				tsFile.write((char*)tsoutBuf, (tsoutBufPos-1)*TS_FRAM_LEN);
				xpeFile.write((char*)xpeoutBuf, (xpeoutBufPos-1)*XPE_PKG_LEN);
				msgFile.write((char*)msgoutBuf, (msgoutBufPos-1)*MSG_PKG_LEN);  //msg_cap
			}

		}
		memset(xpeBuf, 0, XPE_PKG_LEN);
		//====================================================================
		fileFrameNo++;		
		memset(xdbBuf, 0xff, XDB_FRAM_LEN);
		memset(tsBuf, 0xff, TS_FRAM_LEN);
		memset(rsBuffer, 0, RS_MSG_LEN);
		CreateXDB(proBuf + stepBytes, xdbBuf,fileStreamNo, fileFrameNo, restBytes);
		CreateTS(xdbBuf, tsBuf, tsCounter);
		CreateRS(tsBuf, rsBuffer, TS_FRAM_LEN);
		CreateXPE(rsBuffer, xpeBuf, RS_MSG_LEN, &gkThreadParams, &(gkThreadParams.SERVICE_PARAMS[idx]), pkt_pid);		
		//CreateUdpMsg(xpeBuf, msgBuf, XPE_PKG_LEN, &gpThreadParams->SERVICE_PARAMS[idx]);  //msg_cap
		//===================================
		//xpeIndex=xpeoutBufPos%6;
		//if(!xpeIndex)xpeIndex=1;

		printf("Process 2.\n");


		//if(xpeIndex)
		{
			//memcpy(xpetmpBuf+(xpeIndex-1)*XPE_PKG_LEN, xpeBuf, XPE_PKG_LEN);


			if(smallFlag)
			{
				if(!xpeIndex)xpeIndex=1;
				memcpy(xpetmpBuf+(xpeIndex-1)*XPE_PKG_LEN, xpeBuf, XPE_PKG_LEN);
			}
			else
			{
				memcpy(xpetmpBuf+(xpeIndex)*XPE_PKG_LEN, xpeBuf, XPE_PKG_LEN);	
			}



			//=================================================
			if(xpeIndex < XPE_PKG_NUM)
			{
				xpeIndex++;
				memset(xpetmpBuf+xpeIndex*XPE_PKG_LEN, 0xff, XPE_PKG_LEN*(XPE_PKG_NUM-xpeIndex));
				printf("Last frame is  %d\n", xpeIndex);
			}

			xpeIndex=XPE_PKG_NUM;  //send last package;

			//=================================================

			if(XPE_PKG_NUM == xpeIndex)
			{
				CreateUdpMsg(xpetmpBuf, msgBuf, XPE_PKG_LEN*XPE_PKG_NUM, &(gkThreadParams.SERVICE_PARAMS[idx]));  //msg_cap
				//memcpy(msgoutBuf + (msgoutBufPos-1)*MSG_PKG_LEN, msgBuf, MSG_PKG_LEN); //msg_cap
				//msgoutBufPos++;  //msg_cap	

				memset(xpetmpBuf, 0, XPE_PKG_NUM*XPE_PKG_LEN*10);									
			}
		}					
		//===================================	

		
		dstFile.write((char*)xdbBuf, XDB_FRAM_LEN);
		tsFile.write((char*)tsBuf, TS_FRAM_LEN);
		xpeFile.write((char*)xpeBuf, XPE_PKG_LEN);
		msgFile.write((char*)msgBuf, MSG_PKG_LEN);	//msg_cap
		//====================================================================
	}

	srcFile.close();
	dstFile.close();
	tsFile.close();
	xpeFile.close();
	msgFile.close();

	//end
	delete []fileBuf;
	delete []tmpBuf;
	delete []xdbBuf;
	delete []tsBuf;
	delete []proBuf;
	delete []outBuf;
	delete []tsoutBuf;
	delete []fcfBuf;
	delete []xpeoutBuf;
	delete []xpetmpBuf;
	fileBuf=NULL;
	tmpBuf=NULL;
	xdbBuf=NULL;
	proBuf=NULL;
	outBuf=NULL;
	tsoutBuf=NULL;
	xpeoutBuf=NULL;
	xpetmpBuf=NULL;

}





#if 0

int XPE_buf_thread(LPVOID lpParam)
{
	thread_params_t* pThreadParams = (thread_params_t*)lpParam;
	char			 prompt_txt[] = "[File]->";
	
	SERVICE_PARAM_t*	pServiceParam = NULL;

	int nIndx = 0;
	int iix = 0;
	CString fnStr;

	printf("%s running(bind to address %s).....\n", prompt_txt, pThreadParams->pszInputIP);


	gpThreadParams = pThreadParams;
	assert(pThreadParams->recv_stop == 0);	

	pServiceParam = gpThreadParams->SERVICE_PARAMS;

//---------------------------------------mao--------------------------------------------------------------------------------------//
	NetInit(pThreadParams);

	//=================================
	//test
	//FormatChange(pServiceParam[0].pszFileName, 0);

	//=================================

	
	U32 sendIndx = 0;
	U32 sendLen=0;
	U8  *ptrHead = NULL;
	U8  *ptrRun = NULL;
	U8  *ptr = NULL;

	U8   endFlag= 0;
	U8   switchFlag=0;
//---------------------------------------------------------------------------------------------------------------------------------------------------//
	while(1)
	{	
		//printf("File receive is receive!\n");

		//==================================
		//==================================	
		/*
		ptrHead = peerData + sendIndx;
		sendLen = 0;

		for(ptrRun=ptrHead; ;ptrRun++)
		{
			ptr=ptrRun+3;
			if((ptr[0]==0x09)&&(ptr[1]==0xf1)&&(ptr[2]==0xa0))
			{
				break;
			}			
			sendLen++;
			sendIndx++;

			if(sendIndx+3 >= sizeof(peerData))
			{
					endFlag=0xff;
					break;
			}			
		}


		printf("%x %x %x %x %x, %d\n", ptrHead[0], ptrHead[1], ptrHead[2], ptrHead[3], ptrHead[4], sendLen+3);
        sendIndx += 3;

	    
		m_sUdp.Write(ptrHead, sendLen+3);	// TS_SEND_UNIT

		if(sendIndx >= sizeof(peerData))sendIndx=0;
		//*/

		//==================================
		//==================================	

		if(switchFlag==0)
		{
			switchFlag = 1;
			//m_sUdp.Write(peerData1, sizeof(peerData1));
			m_sUdp.Write(peerData3, sizeof(peerData3));
			printf("Send packet 1 len: %d\n", sizeof(peerData3));
		}
		else
		{
			switchFlag = 0;
			m_sUdp.Write(peerData4, sizeof(peerData4));
			printf("Send packet 2\n");
		}

		

		OS_Sleep(2000);
	}
	
//---------------------------------------------------------------------------------------------------------------------------------------------------//
	//pThreadParams->recv_stop = 1;
//	fclose(file_debug);

	printf("%s stop\n", prompt_txt);

//---------------------------------------mao-------------------------------------------------------//
	closesocket(gsockUdp);
	
//-------------------------------------------------------------------------------------------------------------------//

	return 0;
}

#endif

