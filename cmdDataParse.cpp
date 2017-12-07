
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


void CmdDataParse::GenerateSendFile(PSEND_FILECMD_PARAM cmdParam, FECCONFIG Config)
{
	if(!cmdParam)return;

	string strx="";
	strx = cmdParam->filePath;
	strx += cmdParam->realFileName;

//-------auto_send_100_file-----------------------//
	char str_send_100[1024] = {0};

	int    fec_type = 0;
	int    fec_width = 0;
	int    fec_rate = 0;
	int    fec_num = 0;
	
	fec_type = Config.fec_type;
	fec_width = Config.fec_width;
	fec_rate = Config.fec_rate;
	fec_num = Config.fec_num;

	static  int count =	0;
//-------auto_send_100_file-----------------------//
	

//================initial ==========
	PSEND_FILE_MANAGE pSendFileManage = NULL;

	pSendFileManage = new SEND_FILE_MANAGE[1];
	memset(pSendFileManage, 0, sizeof(SEND_FILE_MANAGE));

	pSendFileManage->groupId = cmdParam->groupId;
	pSendFileManage->sendLevel = cmdParam->sendLevel;
	memcpy(pSendFileManage->sendMode, cmdParam->mode, sizeof(pSendFileManage->sendMode));	

//==================================
	if(fec_num == 0)
	{
		sprintf(str_send_100, "/figure/ftproot/bsfile/%d.tar", count);
		count++;

		memcpy(pSendFileManage->fileName, str_send_100, sizeof(str_send_100));
		
		if(count == fec_num) count = 0;//从0-100，第二次发送的时候仍然从0-100
	}
	else
	{
		strx.copy(pSendFileManage->fileName, strx.size());
	}

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





