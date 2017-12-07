// CMMB_File.h: interface for the CMMB_File class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMMB_FILE_H__D37E7D37_234C_4132_8FAB_C74F5013A175__INCLUDED_)
#define AFX_CMMB_FILE_H__D37E7D37_234C_4132_8FAB_C74F5013A175__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <fstream>
#include <iostream>
//#include <io.h>
//#include "CMMB_platform.h"
#include "cmmbXpe.h"
//#include "CMMB_INet.h"
#include "CRC_32.h"
#include "Udp.h"
#include "reed_solom.h"
#include "cyassl/ctaocrypt/aes.h"
#include "cyassl/ctaocrypt/md5.h"


using namespace std;


//=============================================================
//=============================================================

#define XDB_DATA_LEN		173
#define XDB_FRAM_LEN		184
#define TS_FRAM_LEN			188
#define READ_BUF_LEN		24245
#define PROS_BUF_LEN		24246
#define TS_SEND_PACKAGES	1600
#define	TS_SEND_UNIT		1316	 // 188:7_TS=1316     273:5_MSG=1365        264:5_XPE=1320
#define MSG_SEND_UNIT		1329
#define TS_SEND_BUF			1601
#define PROGRESS_TIMER		0x01

#define DVBPID				"8000"
#define DVBGROUPID			"65535"
#define DVBUSERID			"65535"


#define BS_SRCFILE			"/figure/ftproot/bsfile"
#define CS_DSTFILE			"/figure/ftproot/sndtmp"
#define CMD_FOLDER			"mkdir -p -m 777 "


#define SELF_IP				"127.0.0.1"
#define SELF_PORT			10000

//============================================================
//RS codec

#define RS_SYMSIZE			8
#define RS_GFPOLY			0x11d
#define RS_FCR				1
#define RS_PRIM				1
#define RS_T2				48
#define RS_K				207
#define RS_N				(RS_K + RS_T2)
#define RS_PAD				((1<<RS_SYMSIZE) - 1 - RS_N)
#define RS_MSG_LEN			255
#define XPE_PKG_LEN			264
#define TS_FRAME_LEN		188
#define MSG_PKG_LEN			1329
#define XPE_PKG_NUM			5
//============================================================

// number of bits in CRC: don't change it.
#define W 16
// this the number of bits per char: don't change it.
#define B 8

enum encapStyle
{
	STREAM_STYLE = 0,
	FILE_STYLE
};


enum xsendMode
{
	DTMB_MODE = 0,
	CMMB_MODE
};

//=============================================================
//=============================================================

class CMMB_DTMB_MUX  
{
public:
	CMMB_DTMB_MUX();
	~CMMB_DTMB_MUX(){};

private:
	void CreateXDB(BYTE* pSrc, BYTE* pDst, WORD streamNo, UINT frameSeq, BYTE dataLen);
	void CreateTS(BYTE* pSrc, BYTE* pDst, BYTE count);
	void CreateRS(BYTE* pSrc, BYTE* pDst, UINT count);
	void CreateXPE(BYTE* pSrc, BYTE* xpe_buf, UINT srcLen, thread_params_t* pThreadParams, SERVICE_PARAM_t* pServiceParam, U8 packet_ID);
	void CreateUdpMsg(BYTE *pSrc, BYTE *pDst, UINT srclen, SERVICE_PARAM_t* pServiceParam);
	void DataRate(int rate);	
	WORD GetFileStreamNo(void);
	void CreateControlFrame(BYTE* pDst, BYTE counter, WORD fileStreamNo, UINT sumFrame, UINT fileLen, const char* fileName);
	unsigned char GetContinueCounter(BYTE* pSrc, unsigned int len, BYTE off, unsigned int flen);	
	unsigned int GetFrameNo(BYTE* pSrc, unsigned int len);
	
	
public:
	void NetInit(thread_params_t* pthrParam);
	UINT SendFile(const char *fname);
	void FormatChange(UINT idx);
	BOOL SetFormatFile(const char *fname);
	BOOL CreateSndFile(void);
	string GetSndFile(void);
	void SetSendMode(int send_type);
	int  GetSendMode(){return m_sendType;}
	int  GetEncryptFlag(){return encryptFlag;}
	int  EncryptFile(void);
	int  FileCheckSum(void);
	void  SetEncryptFlag(int flag);
	int  FecEncode();

private:
	int    m_sendType;	
	BYTE   m_checkSum[16];
	int    encryptFlag;
	string m_srcFileName;
	string dstFileName;
	string tsFileName;
	string simpleFileName;
	string xpeFileName;
	string msgFileName;
	string m_tmpPath;
	string m_encryptFile;

	fstream srcFile;
	fstream dstFile;
	fstream tsFile;
	fstream xpeFile;
	fstream msgFile;
	

};

#endif // !defined(AFX_CMMB_FILE_H__D37E7D37_234C_4132_8FAB_C74F5013A175__INCLUDED_)
