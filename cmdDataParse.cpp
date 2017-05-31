
#include "cmdDataParse.h"

CmdDataParse::CmdDataParse()
{
	//======================
	//todo
	//======================
	
}


list<PSEND_FILECMD_PARAM> CmdDataParse::GetCmdList(void)
{
	return m_cmdList;
}


int  CmdDataParse::GetCmdCount(void)
{
	return m_cmdList.size();
}


int  CmdDataParse::DataEmpty(void)
{
	return m_cmdList.empty();
}


int CmdDataParse::cmdParser(string cmd)
{
	if(cmd.empty())return 0;

	
	int    sendLevel;
	int    groupId;
	
//	int    fec_type;
//	int    fec_width;
//	int    fec_rate;
//	int    fec_num;
	string cmdStr, cmdTypeStr, modeStr, filePathStr, fileNameStr;
	string sendTimeStr, startTimeStr, endTimeStr, realFileNameStr;;

	PSEND_FILECMD_PARAM pSendFileCmdParam;
	
	//=========================
	//check the valid
		
	if (reader.parse(cmd.c_str(), root))  
    {  
       cmdStr      		= root["cmd"].asString();  
       cmdTypeStr  		= root["type"].asString();
	   modeStr     		= root["mode"].asString();
	   filePathStr 		= root["path"].asString();
	   fileNameStr 		= root["name"].asString();
	   realFileNameStr	= root["realname"].asString();
	   sendLevel		= atoi(root["level"].asString().c_str());
	   groupId			= atoi(root["group"].asString().c_str());
	   startTimeStr 	= root["starttime"].asString();
	   endTimeStr		= root["endtime"].asString();
	   sendTimeStr		= root["time"].asString();

//	   fec_type		= atoi(root["fec_type"].asString().c_str());
//	   fec_width		= atoi(root["fec_width"].asString().c_str());
//	   fec_rate		= atoi(root["fec_rate"].asString().c_str());
//	   fec_num	   = atoi(root["fec_num"].asString().c_str());


	   pSendFileCmdParam = new SEND_FILECMD_PARAM[1];
	   memset(pSendFileCmdParam, 0, sizeof(SEND_FILECMD_PARAM));
	   
	   pSendFileCmdParam->sendLevel = sendLevel;
	   pSendFileCmdParam->groupId   = groupId;
	   cmdStr.copy(pSendFileCmdParam->cmd, cmdStr.size());
	   cmdTypeStr.copy(pSendFileCmdParam->cmdType, cmdTypeStr.size());
	   modeStr.copy(pSendFileCmdParam->mode, modeStr.size());
	   filePathStr.copy(pSendFileCmdParam->filePath, filePathStr.size());
	   fileNameStr.copy(pSendFileCmdParam->fileName, fileNameStr.size());
	   realFileNameStr.copy(pSendFileCmdParam->realFileName, realFileNameStr.size());
	   startTimeStr.copy(pSendFileCmdParam->startTime, startTimeStr.size());
	   endTimeStr.copy(pSendFileCmdParam->endTime, endTimeStr.size());
	   sendTimeStr.copy(pSendFileCmdParam->sendTime, sendTimeStr.size());	   

//	   pSendFileCmdParam->fec_type = fec_type;
//	   pSendFileCmdParam->fec_rate = fec_rate;
//	   pSendFileCmdParam->fec_width = fec_width;
//	   pSendFileCmdParam->fec_num = fec_num;


	   m_cmdList.push_back(pSendFileCmdParam);

	  	   /*
	   printf("cmd: %s\ncmdType: %s\nmode: %s\nfilePath: %s\nfileName: %s\nrealName: %s\nlevel: %d\ngroupId: %d\nstartTime: %s\nendTime: %s\nsendTime: %s\n", pSendFileCmdParam->cmd, pSendFileCmdParam->cmdType, 
	   	pSendFileCmdParam->mode, pSendFileCmdParam->filePath, pSendFileCmdParam->fileName,
	   	pSendFileCmdParam->realFileName,pSendFileCmdParam->sendLevel,pSendFileCmdParam->groupId, 
	   	pSendFileCmdParam->startTime, pSendFileCmdParam->endTime, pSendFileCmdParam->sendTime);
	   //*/

		
	   //printf("cmdStr=%s, typeStr=%s\n", cmdStr.c_str(), typeStr.c_str());
		 
    }  
	else
	{
		printf("%s()->cmd parse error.\n", __func__);
	}

	return 0;
}


void CmdDataParse::DispOneData(PSEND_FILECMD_PARAM pSendFileCmdParam)
{
	if(pSendFileCmdParam)
	{		 
		 printf("cmdx: %s\ncmdType: %s\nmode: %s\nfilePath: %s\nfileName: %s\nrealName: %s\nlevel: %d\ngroupId: %d\nstartTime: %s\nendTime: %s\nsendTime: %s\n", pSendFileCmdParam->cmd, pSendFileCmdParam->cmdType, 
		   	pSendFileCmdParam->mode, pSendFileCmdParam->filePath, pSendFileCmdParam->fileName,
		   	pSendFileCmdParam->realFileName,pSendFileCmdParam->sendLevel,pSendFileCmdParam->groupId, 
		   	pSendFileCmdParam->startTime, pSendFileCmdParam->endTime, pSendFileCmdParam->sendTime);
	}
}


PSEND_FILECMD_PARAM CmdDataParse::GetDataUnit(void)
{
	PSEND_FILECMD_PARAM retPtr;
	retPtr = m_cmdList.front();
	return retPtr;
}


void CmdDataParse::PopDataUnit(void)
{
	m_cmdList.pop_front();
}


void CmdDataParse::GenerateSendFile(PSEND_FILECMD_PARAM cmdParam, int cnt, FECCONFIG fecConfig)
{
	if(!cmdParam)return;

// string strx="";
	char strx[1024] = {0};

//-----------------------//
	//string str = "";
	char str[256] = {0};

	char filename[256] = {0};
	char cmdLine[1024] = {0};
//	char buf[8] = {0};
	int  elen = 1000;
	int  ret = 0;
	elen = elen*cnt;// 0   1000  2000  3000

	int    fec_type = 0;
	int    fec_width=0;
	int    fec_rate=0;
	int    fec_num=0;
	
	fec_type = fecConfig.fec_type;
	fec_width = fecConfig.fec_width;
	fec_rate = fecConfig.fec_rate;
	fec_num = fecConfig.fec_num;

//======================================
	PSEND_FILE_MANAGE pSendFileManage = NULL;
	pSendFileManage = new SEND_FILE_MANAGE[1];
	memset(pSendFileManage, 0, sizeof(SEND_FILE_MANAGE));

	pSendFileManage->groupId = cmdParam->groupId;
	pSendFileManage->sendLevel = cmdParam->sendLevel;
	memcpy(pSendFileManage->sendMode, cmdParam->mode, sizeof(pSendFileManage->sendMode));	
//=======================================

	if(fec_type == 4 || fec_type == 5)//fec code
	{
		memcpy(strx,cmdParam->filePath,sizeof(cmdParam->filePath));

		strcat(strx,cmdParam->realFileName);
		memcpy(filename,cmdParam->fileName,sizeof(cmdParam->fileName));

		sprintf(str,"%s.fec.%d", strx, cnt);//xx.tar.fec.0  xx.tar.fec.1   xx.tar.fec.2   xx.tar.fec.3
	//	str = strcat(strx, buf);

		memset(cmdLine, 0, sizeof(cmdLine));
		sprintf(cmdLine,"sarifec -S -i:%s -o:%s -x:%d -X:%d -K:%d -E:%d -T:173",strx, str, fec_type, fec_rate, fec_width, elen);
				
		if((ret = system(cmdLine)) < 0)   system(cmdLine);
		
		printf("in %s->(),cmdLine :%s\n",__func__, cmdLine);
		
		strcat(cmdParam->fileName,".fec.0");//change cmdParam->fileName to cmdParam->fileName.fec.0
		strcat(cmdParam->realFileName,".fec.0");//change cmdParam->fileName to cmdParam->fileName.fec.0
		static int once_print = 0;
		if(!once_print)
		{
			printf("### cmdParam->fileName:%s ,cmdParam->realFileName:%s\n",cmdParam->fileName,cmdParam->realFileName);
			once_print = 1;
		}

		memcpy(pSendFileManage->fileName, str, sizeof(str));//zxy
	}
	else if(fec_type == 0)//no fec,origin file
	{
		string strx="";
		strx = cmdParam->filePath;
		strx += cmdParam->realFileName;
		strx.copy(pSendFileManage->fileName, strx.size());
	}
	else
		printf("illegal fec_type !!!\n");

	m_fileList.push_back(pSendFileManage);

}


int  CmdDataParse::GetFileCount(void)
{
	return m_fileList.size();
}


int CmdDataParse::FileEmpty(void)
{	
	return m_fileList.empty();
}


PSEND_FILE_MANAGE CmdDataParse::GetFileUnit(void)
{
	PSEND_FILE_MANAGE retPtr;
	retPtr = m_fileList.front();
	return retPtr;
}


void CmdDataParse::PopFileUnit(void)
{
	m_fileList.pop_front();
}





