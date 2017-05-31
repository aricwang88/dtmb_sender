
#ifndef __TYPE_H__
#define __TYPE_H__

#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h> 
#include <net/if.h> 
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>


//#include "libwebserve.h"



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
#define FILE_LEN	256
#define SOCKADDR_IN struct sockaddr_in
#define SOCKADDR	struct sockaddr
#define WORD	unsigned int



typedef void * (ThreadFunc)(void *);

static inline int CreateGerneralThreadWithArg(ThreadFunc threadFunc, void *arg)
{
	pthread_attr_t attr;
	struct sched_param param;
	pthread_t thread;
	int ret;
	
	sched_getparam(0, &param);
	param.sched_priority = sched_get_priority_max(SCHED_FIFO);
	
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	pthread_attr_setschedparam(&attr, &param);
	
	ret = pthread_create(&thread, &attr, threadFunc, arg);
	pthread_attr_destroy(&attr);
	return ret;
}

#define CreateGerneralThread(threadFunc)		CreateGerneralThreadWithArg(threadFunc, NULL)





#endif


