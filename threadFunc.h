#ifndef __THREAD_FUNC_H__
#define __THREAD_FUNC_H__


#include "ver.h"


void Sleep(unsigned int sec);
void getLocalIp(char *ipbuf);
void loadPublicCfg(void);
void *XPE_file_thread(void *arg);
void *web_udp_thread(void *arg);
void *web_process_thread(void *arg);



#endif
