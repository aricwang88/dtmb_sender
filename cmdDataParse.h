
#ifndef __CMD_DATAPARSE_H__
#define __CMD_DATAPARSE_H__



#include <stdio.h>
#include <string>
#include <list>
#include <string.h>
#include <stdlib.h>
#include "json/json.h"

//==========for BS interface=============
//#define fec_type 4
//#define fec_width 64
//#define  fec_rate 50
//#define fec_num  3
//=======================



using namespace std;


typedef struct SendFileCmd_tag
{
	char cmd[20];
	char cmdType[20];
	char mode[10];
	char filePath[1024];
	char fileName[256];
	char realFileName[256];
	int  sendLevel;
	int  groupId;
	char startTime[20];
	char endTime[20];
	char sendTime[20];
//	int    fec_type;
//	int    fec_width;
//	int    fec_rate;
//	int    fec_num;
}SEND_FILECMD_PARAM,*PSEND_FILECMD_PARAM;

//=========================for sarifec=====
//extern thread_params_t	gkThreadParams;

typedef struct FecConfig
{
	int    fec_type;
	int    fec_width;
	int    fec_rate;
	int    fec_num;	
//	int    fec_los;
}FECCONFIG;

typedef struct SendFileManage_tag
{
	char fileName[1024];
	int  sendCou;
	char startTime[20];
	char endTime[20];
	char sendMode[10];
	int  sendLevel;
	char fileType;
	int  groupId;
	int  userId;
}SEND_FILE_MANAGE,*PSEND_FILE_MANAGE;



class CmdDataParse
{

public:
	CmdDataParse();
	~CmdDataParse(){};


public:
	int  cmdParser(string cmd);
	int  GetCmdCount(void);
	list<PSEND_FILECMD_PARAM> GetCmdList(void);
	void DispOneData(PSEND_FILECMD_PARAM pSendFileCmdParam);
	
	PSEND_FILECMD_PARAM GetDataUnit(void);
	void PopDataUnit(void);
	int  DataEmpty(void);
	void GenerateSendFile(PSEND_FILECMD_PARAM cmdParam,int count, FECCONFIG fecConfig);
	int  GetFileCount(void);
	int FileEmpty(void);
	void PopFileUnit(void);
	PSEND_FILE_MANAGE GetFileUnit(void);  

private:	
	Json::Value  root;
	Json::Reader reader;
	list<PSEND_FILECMD_PARAM> m_cmdList;
	list<PSEND_FILE_MANAGE>	  m_fileList;
};


#endif



