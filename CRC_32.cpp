#include <stdio.h>

#include "IntTypes.h"
#include "CRC_32.h"

#define POLYNOMIAL_VALUE			0x04c11db7

U32 crc32_table[256];
S32 crc32_uninitialized = 1;

void init_crc32_table()
{
	S32 i, j;
	U32 nData;

	U32 flag, flag1, flag2;

	// 256 个值
	for (i = 0; i <= 0xFF; i++)
	{
		nData = (U32)(i << 24);
		crc32_table[i] = 0x00000000;

		for (j = 0; j < 8; j++)
		{
			flag1 = (nData & 0x80000000);
			flag2 = crc32_table[i] & 0x80000000;
			flag = flag1 ^ flag2;

			crc32_table[i] <<= 1;

			if (flag)
			{
				crc32_table[i] ^= POLYNOMIAL_VALUE;
			}

			nData <<= 1;
		}
	}
}


S32 Decode_CRC_32(U8* buf, S32 length)
{
	U32 oldcrc32;
	U32 crc32;
	U32 oldcrc;
	S32	 t;
	S32	 c;
	S32  i;

	if (crc32_uninitialized)
	{
		init_crc32_table();
		crc32_uninitialized = 0;
	}

	oldcrc32 = 0xffffffff;				 //初值为0xffffffff
	
	for (i = 0; i < length; i++)
	{
		c = buf[i];						 //新移进来的字节值
		t= (oldcrc32 >> 24) ^ c;				 //要移出的字节的值
		oldcrc = crc32_table[t];		 //根据移出的字节的值查表
		oldcrc32 <<= 8;
		oldcrc32 ^= oldcrc;		 //将寄存器与查出的值进行xor 运算
	}

	crc32 = oldcrc32;

	return crc32;
}

S32 Encode_CRC_32(U8* buf, S32 length)
{
	U32 oldcrc32;
	U32 crc32;
	U32 oldcrc;
	S32	 t;
	S32	 c;
	S32  i;

	if (crc32_uninitialized)
	{
		init_crc32_table();
		crc32_uninitialized = 0;
	}

	oldcrc32 = 0xffffffff;				 //初值为0xffffffff
	
	for (i = 0; i < length; i++)
	{
		c = buf[i];						 //新移进来的字节值
		t= (oldcrc32 >> 24) ^ c;				 //要移出的字节的值
		oldcrc = crc32_table[t];		 //根据移出的字节的值查表
		oldcrc32 <<= 8;
		oldcrc32 ^= oldcrc;		 //将寄存器与查出的值进行xor 运算
	}

	crc32 = oldcrc32;

	return crc32;
}

/*
S32 Decode_CRC_32(U8* buf, S32 length)
{
	S32 flag_src;
	S32 flag_dst;
	S32 flag;

	U32 remain = 0xFFFFFFFF;

	int8_t	ch;
	S32 i, j;

	for (i = 0; i < length; i++)
	{
		ch = *(buf + i);

		for (j = 0; j < 8; j++)
		{
//			flag_src = (ch & 0x80) >> 7;
//			flag_dst = (remain & 0x80000000) >> 31;
			flag_src = ch & 0x80;
			flag_dst = (remain >> 24) & 0x80;

			flag = flag_src ^ flag_dst;

			remain <<= 1;

			if (flag)
			{
				remain ^= POLYNOMIAL_VALUE;
			}

			ch <<= 1;
		}
	}

	return remain;
}

S32 Encode_CRC_32(U8* buf, S32 length)
{
	S32 flag_src;
	S32 flag_dst;
	S32 flag;

	U32 remain = 0xFFFFFFFF;

	int8_t	ch;
	S32 i, j;

	for (i = 0; i < length; i++)
	{
		ch = *(buf + i);

		for (j = 0; j < 8; j++)
		{
			flag_src = (ch & 0x80) >> 7;
			flag_dst = (remain & 0x80000000) >> 31;

			flag = flag_src ^ flag_dst;

			remain <<= 1;

			if (flag)
			{
				remain ^= POLYNOMIAL_VALUE;
			}

			ch <<= 1;
		}
	}

	return remain;
}
*/

/*计算校验和的函数 zdj added 20071210*/
U8 CalculateCheckSum(U8* pdata, S32 data_length)
{
	U8		check_sum = 0;
	S32		i;

	if ((pdata != NULL) && (data_length > 0))
	{
		for (i=0; i < data_length; i++) 
		{
			check_sum ^= pdata[i];
		}
	}

	return check_sum;
}
