#include "Udp.h"
#include "ver.h"
#include "type.h"
#include "cmmbXpe.h"
#include "configSet.h"
#include "cmmbFile.h"
#include "bsDataProc.h"
#include "cmdDataParse.h"

Udp  g_udp;
BOOL openFlag = FALSE;
char 	bind_ip[16]={0};
thread_params_t	gkThreadParams;
FECCONFIG fecConfig;


CMMB_DTMB_MUX dtmbCmmbMux;
WebDataProc	  webData;
CmdDataParse  dataParse;


void Sleep(unsigned int sec)
{
	sleep(sec);
}



void getLocalIp(char *ipbuf)
{
	FILE *fd;
	char buf[10];
	bzero(buf, sizeof(buf));	
	fd = popen("ifconfig | grep 255.255 | cut -d\":\" -f2 | cut -d\" \" -f1", "r");
	//bzero(buf, sizeof(buf));
	fread(ipbuf, 1, 16, fd);
	pclose(fd);
}



void loadPublicCfg(void)
{
	FILE *in=NULL;

	int			i;
	int			port_index;
	char		pszServiceName[16];
	char		pszPortName[16];

	memset(&gkThreadParams, 0x00, sizeof(thread_params_t));
	//===============zxy
	memset(&fecConfig, 0, sizeof(FECCONFIG));
	in = fopen(CFG_FILE, "r");
	if (NULL == in)
	{		
		printf("No Config file\n");
		
		return ;
	}
	else
	{
		fclose(in);
		printf("open %s is ok!!!\n", CFG_FILE);
	}

	//read_profile_string("OutputCfg", "MuxerIp", gkThreadParams.pszMuxIP, 16, MUXER_IP, CFG_FILE);	
	//GetPrivateProfileString("OutputCfg","MuxerIp",MUXER_IP, gkThreadParams.pszMuxIP, 16, CFG_FILE);
	//gkThreadParams.mux_port = GetPrivateProfileInt("OutputCfg", "MuxerPort", MUXER_PORT, CFG_FILE);
	

	gkThreadParams.service_count = GetPrivateProfileInt("ServiceCfg", "ServiceNum", 0, CFG_FILE);


	for (i = 0; i < gkThreadParams.service_count; i++)
	{
		sprintf(pszServiceName, "SERVICE%d_CFG", i);
		
		gkThreadParams.SERVICE_PARAMS[i].service_ID = GetPrivateProfileInt(pszServiceName, "ServiceId", 0xffff, CFG_FILE);
		gkThreadParams.SERVICE_PARAMS[i].upper_style = GetPrivateProfileInt(pszServiceName, "PacketMethod", 0, CFG_FILE);

		gkThreadParams.SERVICE_PARAMS[i].port_count = GetPrivateProfileInt(pszServiceName, "PortNum", 0, CFG_FILE);
		for (port_index = 0; port_index < gkThreadParams.SERVICE_PARAMS[i].port_count; port_index++)
		{
			sprintf(pszPortName, "InputPort%d", port_index);
			gkThreadParams.SERVICE_PARAMS[i].input_port[port_index] = GetPrivateProfileInt(pszServiceName, pszPortName, 0xffff, CFG_FILE);
		}

		gkThreadParams.SERVICE_PARAMS[i].bCRC32Indic = GetPrivateProfileInt(pszServiceName, "CRC", 1, CFG_FILE);
		gkThreadParams.SERVICE_PARAMS[i].FEC_style = GetPrivateProfileInt(pszServiceName, "FEC", 1, CFG_FILE);
		gkThreadParams.SERVICE_PARAMS[i].alloc_bps = GetPrivateProfileInt(pszServiceName, "BANDWIDTH", 1, CFG_FILE);

		GetPrivateProfileString(pszServiceName, "GroupIp", "", gkThreadParams.SERVICE_PARAMS[i].pszMulticastAddr, 16, CFG_FILE);

		GetPrivateProfileString(pszServiceName, "PackDecrib", "", gkThreadParams.SERVICE_PARAMS[i].pszUpperStyle, 32, CFG_FILE);
		GetPrivateProfileString(pszServiceName, "FileName", "",   gkThreadParams.SERVICE_PARAMS[i].pszFileName, 1024, CFG_FILE);
		GetPrivateProfileString(pszServiceName, "CRCDecrib", "", gkThreadParams.SERVICE_PARAMS[i].pszCRCStyle, 32, CFG_FILE);
		GetPrivateProfileString(pszServiceName, "FECDecrib", "", gkThreadParams.SERVICE_PARAMS[i].pszFECStyle, 32, CFG_FILE);
	
		GetPrivateProfileString(pszServiceName, "ComProtoc", "UDP", gkThreadParams.SERVICE_PARAMS[i].pszProtocol, 16, CFG_FILE);

		
//		pThreadParams->SERVICE_PARAMS[i].bRmvIter = GetPrivateProfileInt(pszServiceName, "去加扰标识", 0, iniFullPath);
		gkThreadParams.SERVICE_PARAMS[i].bRmvIter = GetPrivateProfileInt(pszServiceName, "NoRtpHeader", 0, CFG_FILE);
	}

	/////////////////////////add by wangying///////////////////////////////////////
	//载入跟数据封装相关参数
	gkThreadParams.xpe_size = GetPrivateProfileInt("ServiceDataEncap", "LLDP_SIZE", 1024, CFG_FILE);
	gkThreadParams.udp_size = GetPrivateProfileInt("ServiceDataEncap", "UDP_SIZE", 1024, CFG_FILE);

	//-----------------for sarifec   zxy--------1219------------
	gkThreadParams.fec_file_send_count = GetPrivateProfileInt("ServiceCfg", "FecFileSendCount", 1, CFG_FILE);
	fecConfig.fec_type= GetPrivateProfileInt("ServiceCfg", "FecType", 5, CFG_FILE);
	fecConfig.fec_width= GetPrivateProfileInt("ServiceCfg", "FecWidth", 64, CFG_FILE);
	fecConfig.fec_rate= GetPrivateProfileInt("ServiceCfg", "FecRate", 200, CFG_FILE);
	fecConfig.fec_num= GetPrivateProfileInt("ServiceCfg", "FecNum", 1, CFG_FILE);

//	fecConfig.fec_los= GetPrivateProfileInt("ServiceCfg", "FecLos", 0, CFG_FILE);
	//-----------------for sarifec   --------------------

	//跟调试相关参数
	gkThreadParams.enable_recv_debug = GetPrivateProfileInt("DebugCfg", "RecvDebug", 0, CFG_FILE);
	gkThreadParams.enable_rtp_debug  = GetPrivateProfileInt("DebugCfg", "RtpDebug", 0, CFG_FILE);
	gkThreadParams.enable_pack_debug = GetPrivateProfileInt("DebugCfg", "PacketDebug", 0, CFG_FILE);
	gkThreadParams.enable_send_debug = GetPrivateProfileInt("DebugCfg", "SendDebug", 1, CFG_FILE);
	gkThreadParams.enable_raw_socket = GetPrivateProfileInt("DebugCfg", "RawSocket", 1, CFG_FILE);

	GetPrivateProfileString("InputCfg", "Address", "127.0.0.1", gkThreadParams.pszInputIP, 16, CFG_FILE);
	memcpy(gkThreadParams.pszControlIP, gkThreadParams.pszInputIP, sizeof(gkThreadParams.pszControlIP));

	//载入跟复用配置相关的参数
	gkThreadParams.mux_port = GetPrivateProfileInt("OutputCfg", "MuxerPort", 9010, CFG_FILE);
	gkThreadParams.mux_port_same = GetPrivateProfileInt("OutputCfg", "SameInputPort", 0, CFG_FILE);

	GetPrivateProfileString("OutputCfg", "IP", "127.0.0.1", gkThreadParams.pszOutputIP, 16, CFG_FILE);
	GetPrivateProfileString("OutputCfg", "MuxerIp", "127.0.0.1", gkThreadParams.pszMuxIP, 16, CFG_FILE);
	GetPrivateProfileString("DebugCfg", "LogFile", "", gkThreadParams.pszLogFile, 256, CFG_FILE);


	//=======================================================================
	printf("muxer ip: %s:%d\n", gkThreadParams.pszMuxIP, gkThreadParams.mux_port);
	//=======================================================================

	//=========zxy=======1220========
	printf("#####   send file times:%d   ####\n", gkThreadParams.fec_file_send_count);
	printf("fec_type:%d, fec_rate:%d, fec_width:%d, fec_num:%d\n",fecConfig.fec_type, fecConfig.fec_rate,fecConfig.fec_width,fecConfig.fec_num);
	
}


//====================================
void *XPE_file_thread(void *arg)
{
	thread_params_t* pThreadParams = (thread_params_t*)arg;
	char			 prompt_txt[] = "[File]->";
	//list<PSEND_FILECMD_PARAM> parseLink;
	SERVICE_PARAM_t*	pServiceParam = NULL;
	//list<PSEND_FILECMD_PARAM>::iterator parseIt;
	PSEND_FILE_MANAGE  pSendFileManage = NULL;
//	char cmdLine[1024] = {0};//zxy--delete the file that have been sent
	
	int iix = 0;	
	int sendCou = 0;
	string fnStr;

	//int cmdCount = 0;

	iix=iix;

	//printf("%s running(bind to address %s).....\n", prompt_txt, pThreadParams->pszInputIP);


	//gpThreadParams = pThreadParams;
	//assert(pThreadParams->recv_stop == 0);	

	pServiceParam = pThreadParams->SERVICE_PARAMS;
	
//---------------------------------------mao--------------------------------------------------------------------------------------//
	dtmbCmmbMux.NetInit(pThreadParams);//creat socket 224.12.34.56:9010

	if(0 == pServiceParam[0].service_ID)
	{
		dtmbCmmbMux.SetSendMode(DTMB_MODE);
	}
	else
	{
		dtmbCmmbMux.SetSendMode(CMMB_MODE);	
	}
		
	printf("%s(): server num is %d, send mode is %d\n", __func__, pThreadParams->service_count, dtmbCmmbMux.GetSendMode());
		
	//=====================================
	//test
	//FormatChange(pServiceParam[0].pszFileName, 0);
	//=====================================


	int fileCount=0;

//---------------------------------------------------------------------------------------------------------------------------------------------------//
	while(1)
	{
		fileCount = dataParse.GetFileCount();
		//========================================================
		//printf("fileCount is %d\n", fileCount);

		if(fileCount > 0)
		{				
			printf("%s()->size1 is %d, %d\n", __func__, dataParse.GetFileCount(), dataParse.GetFileCount());

			while(!(dataParse.FileEmpty()))
			{			
				//pFileCmdParam = parseLink.front();	
				pSendFileManage = dataParse.GetFileUnit();	
				//dataParse.DispOneData(pFileCmdParam);

				//printf("%s()->send file name is: %s\n", __func__, pSendFileManage->fileName);
				//=================================
				///*
				memset(pServiceParam[0].pszFileName, 0, sizeof(pServiceParam[0].pszFileName));
				memcpy(pServiceParam[0].pszFileName, pSendFileManage->fileName, strlen(pSendFileManage->fileName));

				if(FILE_STYLE == pServiceParam[0].upper_style)
				{
					dtmbCmmbMux.SetEncryptFlag(1);//send file.tar.aes
				}
				else
				{
					dtmbCmmbMux.SetEncryptFlag(0);//send file.tar
				}


				//if(FILE_STYLE == pServiceParam[0].upper_style)
				{
					printf("%s()->send filename is : %s\n", __func__, pServiceParam[0].pszFileName);
					fnStr = pServiceParam[0].pszFileName;					

					dtmbCmmbMux.SetFormatFile(pServiceParam[0].pszFileName);

					//=========================================
					//=========================================
					//=========================================
					//test				
					//continue;
					//=========================================
					//=========================================
					//=========================================

					if(!fnStr.empty())
					{			
						if(access(fnStr.c_str(),0)==-1)
						{
							printf("src file not exist!\n");
							//continue;
						}
						else
						{
							//TODO					
							
							if(access(dtmbCmmbMux.GetSndFile().c_str(), 0)==-1)
							{
								printf("dst file not exist!\n");

								printf("Generating the dest file...\n");
								//===========================
								//format dst file
								dtmbCmmbMux.FormatChange(0);							
								//continue;
							}

							//ToDo
							//================================
							//Send file
					//		while(sendCou < 3)
						//----------zhanxy-------------------
							while(sendCou < gkThreadParams.fec_file_send_count)
							{
								dtmbCmmbMux.SendFile(dtmbCmmbMux.GetSndFile().c_str());
								//==================================================

								sendCou++;

								printf("+++++++++++++++++++++++++++++++++++++++++++++++++++\n");
								printf("+        Send count is %d\n", sendCou);
								printf("+++++++++++++++++++++++++++++++++++++++++++++++++++\n");

								sleep(3);
							}
						//	dtmbCmmbMux.SendFile("/work/app/null_package.tar.ts");
							sleep(5);
							sendCou = 0;
							

							//==================================================
												
						}
					}
					else
					{
						printf("%s()->file name is empty!\n", __func__);
						continue;
					}
				}

				//*/

				//=================================
				dataParse.PopFileUnit();
				delete pSendFileManage;
				pSendFileManage=NULL;

				printf("%s()->size2 is %d, %d\n", __func__, dataParse.GetFileCount(), dataParse.GetFileCount());
				sleep(1);
			//	sleep(30);//---zxy--add
			}
			
		}
		
		sleep(2);

		//========================================================		
		
		//sleep(300000);
	}
	
//---------------------------------------------------------------------------------------------------------------------------------------------------//
	//pThreadParams->recv_stop = 1;
//	fclose(file_debug);

	printf("%s stop\n", prompt_txt);

//---------------------------------------mao-------------------------------------------------------//
	//closesocket(gsockUdp);
	
//-------------------------------------------------------------------------------------------------------------------//

	return 0;
}

//====================================
//====================================

void *web_udp_thread(void *arg)
{	
	BYTE rcvBuf[65535];
	//int 	rcvLen=0;
	int  nRecvSize = 0;
	//int rcvSock;	
	//string str;

	printf("%s()->start\n", __func__);

	//CreateSocket(&rcvSock, 0, , SELF_PORT, 1, 0);
	g_udp.CreateSocket(0,(char*)SELF_IP,SELF_PORT,1,0);

	while(1)
	{
		memset(rcvBuf, 0, sizeof(rcvBuf));		

		nRecvSize = g_udp.Readx(rcvBuf, sizeof(rcvBuf));
		webData.putDataIn((const char*)rcvBuf, nRecvSize);		
	}

	return NULL;
}


void *web_process_thread(void *arg)
{
	string fileCmd="";
	int readPtr=0;
	int writePtr=0;
	int cmdCount=0;
	int ii = 0;
//	int cnt = 2;//code file number

	int cnt = fecConfig.fec_num;
	PSEND_FILECMD_PARAM pFileCmdParam = NULL;
	
	while(1)
	{
		readPtr = webData.getReadPtr();
		writePtr = webData.getWritePtr();

		//printf("readPtr: %d, writePtr: %d\n", readPtr, writePtr);
		
		if(RW_NEQ == webData.getRWStatus())
		{
			fileCmd = webData.getDataOut();
						
			printf("[%s]: %s\n", __func__, fileCmd.c_str());

			dataParse.cmdParser(fileCmd);			
		}

		//==========================================

		cmdCount = dataParse.GetCmdCount();
		//========================================================
		//printf("%s()->cmdCount is %d\n",__func__, cmdCount);

		if(cmdCount > 0)
		{				
			printf("%s()->size1 is %d, %d\n", __func__, dataParse.GetCmdCount(), dataParse.GetCmdCount());

			while(!(dataParse.DataEmpty()))
			{
			
				//pFileCmdParam = parseLink.front();	
				pFileCmdParam = dataParse.GetDataUnit();	
				printf("---------------------------\n");
				dataParse.DispOneData(pFileCmdParam);//
				printf("---------------------------\n");
				for(ii=0; ii<cnt; ii++) //zxy  four feccode file
				{

					//=================================				
					dataParse.GenerateSendFile(pFileCmdParam,ii,fecConfig);
					//=================================
				}
				dataParse.PopDataUnit();
				delete pFileCmdParam;
				pFileCmdParam=NULL;

				printf("%s()->size2 is %d, %d\n", __func__, dataParse.GetCmdCount(), dataParse.GetCmdCount());
				sleep(1);
			}
			
		}

		//==========================================

		sleep(3);
	}

	return NULL;
}

