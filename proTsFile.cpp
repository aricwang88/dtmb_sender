//////////////////////////////////////////////////////////////////////


#include "proTsFile.h"
#include "type.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Source_TsFile::Source_TsFile()
{
	//m_hStopCapThreadEvent = NULL;
	//m_hCapThread = NULL;
	m_hFile = NULL;
	m_bFinish = FALSE;

	m_pGetDataCB = NULL;
	m_pCBUserData = NULL;
	m_PacketGetSize = 0;
}


Source_TsFile::~Source_TsFile()
{
	Close();
}

BOOL Source_TsFile::SetGetTsPacketDataCB(GetTsPacketDataCB pCB, void* p_user_data, DWORD packet_get_size )
{
	//if (m_hCapThread)
	//	return FALSE;

	m_pGetDataCB = pCB;
	m_pCBUserData = p_user_data;
	m_PacketGetSize = packet_get_size;

	return TRUE;
}


BOOL Source_TsFile::Open(char *ts_file_path)
{
	//if(m_hCapThread != NULL)
	//	return TRUE;

	if(!ts_file_path)return FALSE;

	if(m_pGetDataCB == NULL || m_PacketGetSize == 0)
		return FALSE;	

	m_hFile = fopen(ts_file_path, "r");
	if(!m_hFile)return FALSE;	

	m_TsPacketData.AllocateBuffer(m_PacketGetSize);
	m_Buffer.AllocateBuffer(1024*1024*2);
	m_BufferSync.AllocateBuffer(1024*1024*2+TS_PACKET_SIZE_MAX*3);
	memset(m_BufferSync.m_pBuffer, 0, m_BufferSync.m_nBufferSize);
	CreateGerneralThreadWithArg(ReadTSThread, this);	

	return TRUE;
}

BOOL Source_TsFile::Close()
{
	if(m_hFile)fclose(m_hFile);	
	m_hFile = NULL;

	m_Buffer.FreeBuffer();
	m_BufferSync.FreeBuffer();
	m_TsPacketData.FreeBuffer();

	return TRUE;
}

PVOID Source_TsFile::ReadTSThread(PVOID param)
{
	Source_TsFile* pThis = (Source_TsFile*)param;
	pThis->ReadTS();
	return NULL;
}

DWORD Source_TsFile::GetTickCount()
{
	struct timeval tv;
	ulong time_val;
	gettimeofday(&tv, NULL);
	// return ulong(tv.tv_sec*1000L+tv.tv_usec/1000L);
	time_val = tv.tv_sec*1000L + tv.tv_usec/1000L;
	return time_val;
}

DWORD Source_TsFile::ReadTS()
{
	DWORD	thread_waite;

	INT64	start_pcr = 0;
	int		start_pcr_pid = 0;
	INT64	current_pcr = 0;

	INT64	pcr = 0;
	int		pcr_pid = 0;

	DWORD	start_time = 0;
	DWORD	current_time = 0;

	Buffer*	p_ts_packet = NULL;
	
	while(TRUE)
	{		
		current_time = GetTickCount();
		if (current_time - start_time <  (current_pcr - start_pcr) / PCR_BASS)
		{
			usleep(10000);
			continue;
		}
		
		if (m_BufferSync.m_nDataSize < TS_PACKET_SIZE_MAX*3)
		{
			if ((ReadBuffer() == FALSE) && (!m_bFinish))
			{
				usleep(10000);
				continue;
			}
		}
		
	
		if (GotoSync() == FALSE)
		{
			usleep(10000);
			continue;
		}
		
		if (GetPcr(m_BufferSync.m_pData, &pcr, &pcr_pid) == TRUE)
		{			
			if (start_pcr == 0)
			{
				start_pcr = pcr;
				start_pcr_pid = pcr_pid;
				start_time = current_time;
			}
			
			if (pcr_pid == start_pcr_pid)
			{				
				if ( (pcr - current_pcr) / PCR_BASS > 100 || (pcr - current_pcr) < 0)
				{	
					start_pcr = pcr;
					start_time = current_time;
					//printf("Reset pcr and time.\n");
				}
				current_pcr = pcr;
			}
		}

		
		p_ts_packet = m_vTsPacket.GetEmptyBuffer();
		if (p_ts_packet == NULL)
		{
			if (m_vTsPacket.GetFullBufferSize() < TS_PACKET_BUFFER_SIZE_MAX)
			{
				p_ts_packet = new Buffer();
			}
			else
			{
				p_ts_packet = m_vTsPacket.GetFullBuffer();
				printf("TS packet is too more,droped!\n");
			}
		}
		p_ts_packet->FillData(m_BufferSync.m_pData, m_nTsPacketSize);
		m_vTsPacket.AddFullBuffer(p_ts_packet);

		m_BufferSync.m_pData += m_nTsPacketSize;
		m_BufferSync.m_nDataSize -= m_nTsPacketSize;
		
		if (m_pGetDataCB && m_vTsPacket.GetDataSize() >= m_PacketGetSize)
			m_pGetDataCB(m_pCBUserData);
	}

	return 0;
}

BOOL Source_TsFile::GetPcr(const BYTE* p_ts_packet, INT64* p_pcr, INT* p_pcr_pid)
{
	if (( p_ts_packet[0] == 0x47 ) &&
		( p_ts_packet[3]&0x20 ) && 
		( p_ts_packet[5]&0x10 ) &&
		( p_ts_packet[4] >= 7 ) )			
	{
		*p_pcr_pid = ((INT)p_ts_packet[1] & 0x1F) << 8 | p_ts_packet[2];

		*p_pcr =	( (INT64)p_ts_packet[6] << 25 ) |
					( (INT64)p_ts_packet[7] << 17 ) |
					( (INT64)p_ts_packet[8] << 9  ) |
					( (INT64)p_ts_packet[9] << 1  ) |
					( (INT64)p_ts_packet[10] >> 7 );
		return TRUE;
	}

	return FALSE;
}

BOOL Source_TsFile::GotoSync()
{
	while(m_BufferSync.m_nDataSize >= TS_PACKET_SIZE_MAX * 3)
	{
		if (m_BufferSync.m_pData[0] == 0x47 &&
			m_BufferSync.m_pData[TS_PACKET_188] == 0x47 &&
			m_BufferSync.m_pData[TS_PACKET_188*2] == 0x47)
		{
			m_nTsPacketSize = TS_PACKET_188;
			return TRUE;
		}

		if (m_BufferSync.m_pData[0] == 0x47 &&
			m_BufferSync.m_pData[TS_PACKET_196] == 0x47 &&
			m_BufferSync.m_pData[TS_PACKET_196*2] == 0x47)
		{
			m_nTsPacketSize = TS_PACKET_196;
			return TRUE;
		}

		if (m_BufferSync.m_pData[0] == 0x47 &&
			m_BufferSync.m_pData[TS_PACKET_204] == 0x47 &&
			m_BufferSync.m_pData[TS_PACKET_204*2] == 0x47)
		{
			m_nTsPacketSize = TS_PACKET_204;
			return TRUE;
		}

		m_BufferSync.m_pData++;
		m_BufferSync.m_nDataSize--;
	}

	return FALSE;
}

BOOL Source_TsFile::ReadBuffer()
{
	DWORD nReadSize;	

	nReadSize = fread(m_Buffer.m_pBuffer, 1, m_Buffer.m_nBufferSize, m_hFile);

	if(0 == nReadSize)
		return FALSE;

	m_BufferSync.AppendData(m_Buffer.m_pBuffer, nReadSize);

	if (nReadSize != m_Buffer.m_nBufferSize)
	{
		//fseek(m_hFile, 0, SEEK_SET);
		if(feof(m_hFile))
		{
			m_bFinish = TRUE;
		}
	}

	if(m_BufferSync.m_nDataSize < TS_PACKET_SIZE_MAX*3)
		return FALSE;

	return TRUE;
}

BOOL Source_TsFile::GetTsPacketData( BYTE*& p_data, DWORD& data_size )
{
	if (m_vTsPacket.GetDataSize() < m_PacketGetSize)
		return FALSE;

	m_TsPacketData.ClearData();

	Buffer* p_buffer;
	while(m_TsPacketData.m_nDataSize < m_PacketGetSize)
	{
		p_buffer = m_vTsPacket.GetFullBuffer();
		m_TsPacketData.AppendData(p_buffer->m_pData, p_buffer->m_nDataSize);
		m_vTsPacket.AddEmptyBuffer(p_buffer);
	}

	p_data = m_TsPacketData.m_pData;
	data_size = m_TsPacketData.m_nDataSize;

	return TRUE;
}