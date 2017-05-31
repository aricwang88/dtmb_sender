#ifndef _CRC_32_H_
#define _CRC_32_H_

#include "IntTypes.h"

void init_crc32_table(void);
S32 Decode_CRC_32(U8* buf, S32 length);
S32 Encode_CRC_32(U8* buf, S32 length);

U8 CalculateCheckSum(U8* pdata, S32 data_length);

#endif	//_CRC_32_H_



