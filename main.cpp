#include "main.h"


//================================
//================================
#define MUXER_IP 		"224.1.2.3"
#define MUXER_PORT		9010
//================================
//================================
extern char 	bind_ip[16];
extern thread_params_t	gkThreadParams;
//================================





int main(int argc, char* argv[])
{		

	printInfo();

	memset(bind_ip, 0, sizeof(bind_ip));
	getLocalIp(bind_ip);
	printf("localIp is: %s\n", bind_ip);

	loadPublicCfg();

	memcpy(gkThreadParams.pszControlIP, bind_ip, sizeof(bind_ip));
	printf("control ip: %s\n", gkThreadParams.pszControlIP);
	

	CreateGerneralThread(web_udp_thread);	
	CreateGerneralThread(web_process_thread);	


	while(1)
	{
		XPE_file_thread(&gkThreadParams);	
		Sleep(10);
	}

	return 0;
}



