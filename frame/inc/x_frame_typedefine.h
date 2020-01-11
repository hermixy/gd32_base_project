#ifndef __X_FRAME_TYPEDEF_H__
#define __X_FRAME_TYPEDEF_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "line_buf.h"
#include "list.h"

enum
{
    AUTO_REG_TYPE_RESV   = 0x0,

	//DEV_AUTO_INIT,
	
	AUTO_REG_TYPE_DRIVER,
	
	AUTO_REG_INIT_MOD,
	AUTO_REG_TYPE_TASK,
	
	AUTO_REG_TYPE_DRIVER_PUSH,
	AUTO_REG_TYPE_DBG_DEV,
	AUTO_REG_PROTOCOL,
	AUTO_REG_TYPE_LINK,
	AUTU_REG_TYPE_LINK_ACTION,
	AUTO_REG_ROUTE_CFG,
	AUTO_REG_TYPE_SHELL_X,
	AUTO_REG_TYPE_SHELL,
	AUTO_REG_TYPE_PARAM_CFG,
	AUTO_REG_FSM,
	AUTO_REG_FSM_STATE,
	AUTO_REG_AUTOTEST,
	AUTO_REG_AUTOTEST_CTRL,
	AUTO_REG_FLASH_PART,
	
    AUTO_REG_TYPE_ERR,
};


typedef struct __auto_reg_
{
    const char 		*name;
    uint8_t			type;
    void			*p_user_reg_data;
}auto_reg_t;

/*
	INFO_SECTION ROM table
	
	+---------------------------------------+
	|  name1_addr |	type1	|	data_addr1  |
	+---------------------------------------+
	|  name2_addr |	type1	|	data_addr2  |
	+---------------------------------------+
	|  name3_addr |	type2	|	data_addr3  |
	+---------------------------------------+

*/







#endif  //__X_FRAME_TYPEDEF_H__
