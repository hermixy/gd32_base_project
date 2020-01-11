#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "line_buf.h"
#include "list.h"

#include "x_frame_typedefine.h"
#include "debug.h"


///--------shell
#define REG_SHELL_CMD(name,maxargs,cmd,usage,detail)  \
					const shell_cmd_t    cmd_##name = {#name,maxargs,cmd,usage,detail};\
					AUTO_REG_ITEM(SHELL_##name,AUTO_REG_TYPE_SHELL,(void*)&cmd_##name)
#define HANDLE_SHELL_CMD(name)  &(__auto_reg_##SHELL_##name)
#define EXT_SHELL_CMD(name)   extern auto_reg_t (__auto_reg_##SHELL_##name)

#define REG_SHELL_CMD_X(name,maxargs,cmd,usage,detail)  \
					const shell_cmd_t    cmd_##name = {#name,maxargs,cmd,usage,detail};\
					AUTO_REG_ITEM(SHELL_X_##name,AUTO_REG_TYPE_SHELL_X,(void*)&cmd_##name)
#define HANDLE_SHELL_CMD_X(name)  &(__auto_reg_##SHELL_X_##name)
#define EXT_SHELL_CMD_X(name)   extern auto_reg_t (__auto_reg_##SHELL_X_##name)


//===========shell
typedef int32_t (*pf_cmd)(uint8_t argc, uint8_t *argv[]);
typedef struct __shell_cmd
{
    const char 	*name;
    uint8_t			max_args;
    pf_cmd			cmd_func;
    const char		*usage;		/* Usage message	(short)	*/
    const char		*help;		/* Help  message	(long)	*/

}shell_cmd_t;


extern uint8_t enable_protocol_shell_out; //debug.h

typedef uint32_t (*pf_init_drv)(uint32_t dirver_id);
void init_shell_item(pf_init_drv init_drv);
void show_buf(uint8_t *pbuf, uint8_t len);

#endif
