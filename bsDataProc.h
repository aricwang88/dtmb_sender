#ifndef __BS_DATAPROC_H__
#define __BS_DATAPROC_H__


#include <stdio.h>
#include <string>


#define  MAX_DATA_NUM	100


using namespace std;


typedef enum RW_STAUS
{
	RW_EQ=0,
	RW_NEQ
}RWSTA;



class WebDataProc
{
public:
	WebDataProc();
	~WebDataProc(){};

private:

public:
	int putDataIn(const char *dat, int size);
	string getDataOut();
	int getWritePtr(){return writePtr;};
	int getReadPtr(){return readPtr;};
	int getRWStatus();


private:
	string dataList[MAX_DATA_NUM];
	int readPtr;
	int writePtr;
	

};


#endif

