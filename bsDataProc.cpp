
#include "bsDataProc.h"


WebDataProc::WebDataProc(void)
{
	readPtr  = 0;
	writePtr = 0;
}



int WebDataProc::putDataIn(const char *dat, int size)
{
	if(((writePtr+1)%MAX_DATA_NUM) == readPtr)
	{
		printf("%s()->data is full!\n", __func__);
	}
	else
	{
		dataList[writePtr].append(dat, size);
		writePtr++;	
		if(writePtr >= MAX_DATA_NUM)writePtr=0;		
	}
	
	return 0;
}


string WebDataProc::getDataOut(void)
{
	string dat="";	

	if(readPtr == writePtr)
	{		
		printf("%s()-> no data.\n", __func__);	
	}
	else
	{
		dat = dataList[readPtr];
		readPtr++;
		if(readPtr >= MAX_DATA_NUM)readPtr=0;	
	}

	return dat;
}



int WebDataProc::getRWStatus()
{
	if(readPtr != writePtr)
	{
			
		return RW_NEQ;
	}
	else
	{
		return RW_EQ;	
	}
}
