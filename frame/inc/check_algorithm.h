#ifndef __CHECK_ALGORITHM_H__
#define __CHECK_ALGORITHM_H__

#include <stdint.h>

//check_alg
enum
{
	u_chk_none = 0,
	u_chk_sum  = 1,
	u_chk_crc8 = 2,
	u_chk_crc16 = 3,
	
	u_chk_err,
};

#define CHK_VAL_ERR  0xffff

uint32_t calc_chk_val(uint8_t chk_type,uint8_t* pbuf,uint32_t len);

#endif
