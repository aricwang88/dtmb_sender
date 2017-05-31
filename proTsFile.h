//////////////////////////////////////////////////////////////////////


#include "VBufferT.h"
#include <stdio.h>

#define DWORD	unsigned long
#define INT		int
#define INT64 	long long
#define PVOID  	void*
#define BOOL  	unsigned char
#define BYTE	char



const int TS_PACKET_BUFFER_SIZE_MAX = 1024 * 1024 * 100;

const int TS_PACKET_SIZE_MAX = 204;
const int TS_PACKET_SIZE_MIN = 188;

const int TS_PACKET_188 = 188;
const int TS_PACKET_196 = 196;
const int TS_PACKET_204 = 204;

const int PCR_BASS_FREQUENCY = 90000;	
const int PCR_BASS = 90;				

typedef void (*GetTsPacketDataCB)(void* p_user_data);

class Source_TsFile
{
public:
	Source_TsFile();

	~Source_TsFile();

	BOOL	Open(char *ts_file_path);

	BOOL	Close();

	BOOL	SetGetTsPacketDataCB(GetTsPacketDataCB pCB, void* p_user_data, DWORD packet_get_size);
	
	BOOL	GetTsPacketData(BYTE*& p_data, DWORD& data_size);
	BOOL		m_bFinish;

private:
	static 		PVOID ReadTSThread(PVOID param);

	DWORD		ReadTS();

	DWORD 		GetTickCount();

	void		FillBuffer(const BYTE* pData, DWORD dataSize);
	
	BOOL		GetPcr(const BYTE* p_ts_packet, INT64* p_pcr, INT* p_pcr_pid);

	BOOL		GotoSync();
	
	BOOL		ReadBuffer();

	FILE		*m_hFile;

	GetTsPacketDataCB	m_pGetDataCB;
	void*		m_pCBUserData;
	DWORD		m_PacketGetSize;

	Buffer		m_BufferSync;
	Buffer		m_Buffer;
	int			m_nTsPacketSize;
	VBufferT	m_vTsPacket;
	Buffer		m_TsPacketData;
	

};
