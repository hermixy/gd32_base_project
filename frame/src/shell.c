#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
//#include "debug.h"
//#include "x_frame.h"

#include "shell.h"


#define  SHELL_WRITE	PRF

#define SHELL_BUF_SIZE  300
//#define SHELL_DEV_IN    sys_debug_drv_id
#define SHELL_MAX_ARGS  8
#define SHELL_X_MAX_ITEM  12
#define SHELL_MAX_ITEM    32


uint8_t enable_protocol_shell_out = 1;
typedef struct
{
    uint8_t  shell_init_flag;

    uint8_t  shell_x_item ;
    shell_cmd_t  *sg_shell_x_item[SHELL_X_MAX_ITEM];

    uint8_t  shell_item;
    shell_cmd_t  *sg_shell_item[SHELL_MAX_ITEM];

    list_t	 shell_dev_head;
} shell_obj_t;



static shell_obj_t shell_obj = {0};
int32_t parse_line (uint8_t *line, uint8_t *argv[], uint8_t *p_args, uint8_t max_len);
uint8_t 	*shell_dec_line	(linebuf_t   *p_lb);
static shell_cmd_t *find_cmd		(uint8_t *pcmd);
static uint8_t find_EOF(uint8_t *pbuf, uint32_t len);


uint8_t shell_action(uint8_t *pbuf_start, uint8_t max_len)
{
    uint8_t  cur_args = 0;
    uint8_t  read_len = 0;
    uint8_t *shell_args[SHELL_MAX_ARGS];
    shell_cmd_t  *cur_cmd = NULL;

    read_len = parse_line(pbuf_start, shell_args, &cur_args, max_len);

    /*寻找对应的cmd，并且执行*/

    if(cur_args)
    {
        cur_cmd = find_cmd(shell_args[0]);
        if(!cur_cmd)
        {
            SHELL_WRITE("can not find the cmd: %s", shell_args[0]);
        }
        else
        {
            cur_cmd->cmd_func(cur_args, shell_args);
        }
    }
	
    return read_len;
}

#if 1
static int32_t         action_shell_in       (hal_frame_info_t* p_frame_info)
{
	shell_action(p_frame_info->pdata,p_frame_info->data_len);
    return 0;
}
REG_LINK_ACTION(LINK_SHELL_ID      ,COMM_CMD_SET    ,CMD_ID_SHELL_IN          ,action_shell_in);

//static int32_t         action_shell_ctrl       (hal_frame_info_t* p_frame_info)
//{
//    if(p_frame_info->data_len==1)
//    {
//        memcpy(&enable_protocol_shell_out,p_frame_info->pdata,sizeof(uint8_t));
//    }
//    return 0;
//}
//REG_LINK_ACTION(LINK_SHELL_ID      ,COMM_CMD_SET    ,CMD_ID_SHELL_CTRL          ,action_shell_ctrl);
#endif 

REG_CFG_ITEM(en_shell,enable_protocol_shell_out, eu_uint32,  PARM_RW, DEF_SHELL_BY_PROTOCOL, 0.0f, 1.0f  );


void shell_run_on_tick(void)
{
    list_t *cur_list;
    list_t *tag_list_head = &shell_obj.shell_dev_head;
    debug_obj_t *p_dev_obj = NULL;

    if(shell_obj.shell_init_flag == 0)
    {
        return;
    }

    list_for_each(cur_list, tag_list_head)
    {
        uint16_t lb_rest_len = 0;
        uint32_t real_len = 0;
        int32_t  ret = 0;
        uint8_t  temp_buf[SHELL_BUF_SIZE];
        uint8_t *pbuf_start = NULL;
        uint8_t  read_len = 0;
        uint32_t cur_len = 0;

        p_dev_obj = list_entry(cur_list, debug_obj_t, shell_debug_node);

        /*1:从fifo中读取数据到linebuf*/
        lb_rest_len = lb_get_rest_len(p_dev_obj->p_line_buf);

        ret = hal_app_read(p_dev_obj->debug_dev, temp_buf, lb_rest_len, 0, &real_len);
        //DBG_I("hal_app_read  dev:%d read:%d len:%d ",p_dev_obj->debug_dev,lb_rest_len,real_len);
        if( (ret >= 0) && (real_len > 0) )
        {
            lb_push(p_dev_obj->p_line_buf, temp_buf, real_len);
        }

        /*2:不断的解析linebuf*/
        do
        {
            cur_len = lb_get_data_len(p_dev_obj->p_line_buf);
            if(find_EOF(p_dev_obj->p_line_buf->pbuf, cur_len))
            {
                pbuf_start = shell_dec_line(p_dev_obj->p_line_buf);
                read_len = shell_action(pbuf_start, cur_len);
            }
            else
            {
                break;
            }
            /*3:移除已经处理的过的linebuf数据*/
            lb_move(p_dev_obj->p_line_buf, read_len);
        }
        while(real_len);
    }
}
REG_TASK(0, 100, shell_run_on_tick);

int32_t  is_ascii(uint8_t ch)
{
    if( ( (ch > 'a') && (ch < 'z') ) ||
        ( (ch > 'A') && (ch < 'Z') ) ||
        ( (ch > '0') && (ch < '9') )
      )
    {
        return 1;
    }
    return 0;
}


uint8_t *shell_dec_line(linebuf_t   *p_lb)
{
    uint16_t 	buf_len = 0;
    uint8_t	*cur_ptr = NULL;
    uint16_t 	cur_index = 0;

    buf_len = lb_get_data_len(p_lb);
    cur_ptr = lb_get_r_ptr(p_lb);

    while(cur_index < buf_len)
    {
        if(is_ascii(cur_ptr[cur_index]))
        {
            return &cur_ptr[cur_index];
        }
        cur_index++;
        p_lb->r_index++;
    }

    return NULL;
}


int32_t parse_line (uint8_t *line, uint8_t *argv[], uint8_t *p_args, uint8_t max_len)
{
    int nargs = 0;
    int16_t read_len = 0;
    while ((nargs < SHELL_MAX_ARGS) && (read_len < max_len))
    {
        /* skip any white space */
        while ( ((*line == ' ') || (*line == '\t') || (*line == '\r') || (*line == '\n')) && (read_len < max_len))
        {
            ++line;
            read_len++;
        }

        if (*line == '\0')  	/* end of line, no more args	*/
        {
            argv[nargs] = NULL;
            *p_args = nargs;
            return (read_len);
        }

        if(read_len < max_len)
        {
            argv[nargs++] = line;	/* begin of argument string	*/
        }


        /* find end of string */
        while (*line && (*line != ' ') && (*line != '\t') && (*line != '\r') && (*line != '\n') && (read_len < max_len) )
        {
            ++line;
            read_len++;
        }

        *p_args = nargs;

        if ((*line == '\0') )  	/* end of line, no more args	*/
        {
            argv[nargs] = NULL;
            return (read_len);
        }

        if(read_len < max_len)
        {
            *line++ = '\0';		/* terminate current arg	 */
            read_len++;
        }
    }

    //SHELL_WRITE ("** Too many args (max. %d) **\n", SHELL_MAX_ARGS);
    return (read_len);
}


static uint8_t find_EOF(uint8_t *pbuf, uint32_t len)
{
    uint32_t i = 0;

    for(i = 0; i < len; i++)
    {
        if((pbuf[i] == '\r') || ((pbuf[i] == '\n')))
        {
            return 1;
        }
    }

    return 0;
}

void init_shell_item(pf_init_drv init_drv)
{
    debug_obj_t *p_dev_obj = NULL;

    INIT_LIST_HEAD(&shell_obj.shell_dev_head);
    if(shell_obj.shell_init_flag == 0)
    {
        shell_obj.shell_init_flag = 1;
        //init debug dev
		#if 0		//0:直接拷贝dbg.c文件中已经找好的调试驱动设备	1:在AUTO_REG_SECTION中，寻找调试驱动设备
			uint8_t i = 0;
			for(i = 0; i < MAX_DRV_NUM; i++)
			{
				p_dev_obj = (debug_obj_t *)auto_reg_get_func_cont_by_index(AUTO_REG_TYPE_DBG_DEV, i);
				if(p_dev_obj == NULL)
				{
					break;
				}

				list_add(&p_dev_obj->shell_debug_node, &shell_obj.shell_dev_head);
				init_drv(p_dev_obj->debug_dev);
			}
		#else
			{
				list_t *tag_list_head = &debug_ctrl_obj.all_debug_head;
				list_t  *cur_node = NULL;
				list_for_each(cur_node, tag_list_head)
				{
					p_dev_obj = list_entry(cur_node, debug_obj_t, debug_node);
					
					list_add(&p_dev_obj->shell_debug_node, &shell_obj.shell_dev_head);
					init_drv(p_dev_obj->debug_dev);
				}
			}
		#endif
		
        shell_obj.shell_x_item   = auto_reg_get_func_type_cont(AUTO_REG_TYPE_SHELL_X, (void **)shell_obj.sg_shell_x_item, SHELL_X_MAX_ITEM);
        shell_obj.shell_item     = auto_reg_get_func_type_cont(AUTO_REG_TYPE_SHELL  , (void **)shell_obj.sg_shell_item  , SHELL_MAX_ITEM  );
    }
}
//REG_INIT_MOD(init_shell_item);


shell_cmd_t *find_cmd(uint8_t *pcmd)
{
    shell_cmd_t *pShell_cmd = NULL;
    uint32_t i = 0;

    for(i = 0; i < shell_obj.shell_item; i++)
    {
        pShell_cmd = (shell_cmd_t *)(shell_obj.sg_shell_item[i]);
        if(strcmp((const char *)pShell_cmd->name, (const char *)pcmd) == 0)
        {
            return pShell_cmd;
        }
    }

    for(i = 0; i < shell_obj.shell_x_item; i++)
    {
        pShell_cmd = (shell_cmd_t *)(shell_obj.sg_shell_x_item[i]);
        if(strcmp((const char *)pShell_cmd->name, (const char *)pcmd) == 0)
        {
            return pShell_cmd;
        }
    }
    return NULL;
}


int32_t shell_helpx(uint8_t argc, uint8_t *argv[])
{
    shell_cmd_t *pShell_cmd = NULL;
    uint32_t i = 0;

    /*help 后面还有参数，说明是显示具体函数的帮助信息所有就不要显示list信息了*/
    if(argc == 1)
    {
        SHELL_WRITE("[sys reg cmd list]:");
        SHELL_WRITE(" --name--    ----usestage----");
    }

    for(i = 0; i < shell_obj.shell_x_item; i++)
    {
        pShell_cmd = (shell_cmd_t *)shell_obj.sg_shell_x_item[i];
        if(argc > 1)
        {
            if(strcmp((const char *)pShell_cmd->name, (const char *)argv[1]) == 0)
            {
                SHELL_WRITE("%-10s  :\r\n  %s", pShell_cmd->name, pShell_cmd->help);
                return RET_OK;
            }
        }
        else
        {
            SHELL_WRITE("%-15s  : %-15s", pShell_cmd->name, pShell_cmd->usage);
        }
    }

    if(argc > 1)
    {
        SHELL_WRITE("Not find the shell cmd: %s", argv[1]);
    }

    return RET_OK;
}

REG_SHELL_CMD_X(helpx, 0, shell_helpx, "help to list sys_cmds",
                "this is the dtail help for help \r\n \
    help - list all func.\r\n \
    help - cmd_name  show the detail info for cmd");





int32_t shell_help(uint8_t argc, uint8_t *argv[])
{
    shell_cmd_t *pShell_cmd = NULL;
    uint32_t i = 0;

    /*help 后面还有参数，说明是显示具体函数的帮助信息所有就不要显示list信息了*/
    if(argc == 1)
    {
        SHELL_WRITE("[user reg cmd list]:");
        SHELL_WRITE(" --name--    ----usestage----");
    }

    for(i = 0; i < shell_obj.shell_item; i++)
    {
        pShell_cmd = (shell_cmd_t *)shell_obj.sg_shell_item[i];
        if(argc > 1)
        {
            if(strcmp((const char *)pShell_cmd->name, (const char *)argv[1]) == 0)
            {
                SHELL_WRITE("%-10s  :\r\n  %s", pShell_cmd->name, pShell_cmd->help);
                return RET_OK;
            }
        }
        else
        {
            SHELL_WRITE("%-15s  : %-15s", pShell_cmd->name, pShell_cmd->usage);
        }
    }

    if(argc > 1)
    {
        SHELL_WRITE("Not find the shell cmd:%s", argv[1]);
    }

    return RET_OK;
}

REG_SHELL_CMD(help, 0, shell_help, "help to list user_cmds",
              "this is the dtail help for help \r\n \
    help - list all func.\r\n \
    help - cmd_name  show the detail info for cmd");


#define MAX_SHOW_BUF_LEN 200
static uint8_t show_buf_buf[MAX_SHOW_BUF_LEN];
void show_buf(uint8_t *pbuf, uint8_t len)
{
	uint8_t i = 0;
	uint32_t index = 0;
	uint32_t cnt = 0;
	uint8_t *p_buf = show_buf_buf;
	cnt = sprintf((char*)&p_buf[index],"\r\n"); index+=cnt;
    for(i = 0; i < len; i++)
    {
    cnt = sprintf((char*)&p_buf[index],"%02x ", pbuf[i]);  index+=cnt;
        if(i % 16 == 15)
        {
            cnt = sprintf((char*)&p_buf[index],"\r\n"); if(index+cnt<(MAX_SHOW_BUF_LEN-2)) {index+=cnt;}
        }
    }
    cnt = sprintf((char*)&p_buf[index],"\r\n"); index+=cnt;
  
  DBG_RAW_SER("%s",p_buf);
}


void show_buf_ascii(uint8_t *pbuf, uint8_t len)
{
    uint8_t i = 0;
    DBG_RAW_SER("\r\n");

    for(i = 0; i < len; i++)
    {
        DBG_RAW_SER("%c ", pbuf[i]);
        if(i % 16 == 15)
        {
            DBG_RAW_SER("\r\n");
        }
    }
    DBG_RAW_SER("\r\n");
}

void show_buf_limit(uint32_t ms, uint8_t *pbuf, uint8_t len)
{
    static uint32_t last_ms = 0;
    uint8_t i = 0;

    DBG_RAW_SER("\r\n");

    if((SYS_MS_FUNC - last_ms) < ms)
    {
        return ;
    }

    last_ms = SYS_MS_FUNC;

    for(i = 0; i < len; i++)
    {
        DBG_RAW_SER("%02x ", pbuf[i]);
        if(i % 28 == 27)
        {
            DBG_RAW_SER("\r\n");
        }
    }
    DBG_RAW_SER("\r\n");
}



//void debug_out(char *fmt, ...)
//{
//    va_list ap;
//    uint16_t len = 0;
//    char str[DEBUG_BUF_SIZE];
//    debug_obj_t *p_dev_obj = NULL;
//    list_t  *cur_list;
//    list_t *tag_list_head = &shell_obj.shell_dev_head;

//    if(lock == 1)
//    {
//        return;
//    }
//    if(shell_obj.shell_init_flag == 0)
//    {
//        return;
//    }

//    lock = 1;
//    va_start(ap, fmt);
//    len = vsnprintf(str, DEBUG_BUF_SIZE - 1, fmt, ap);
//    str[len] = '\0';
//    len = len + 1;

//     hal_app_write(DRV_RTT, (uint8_t *)str, len, 0, NULL);
//  

//    lock = 0;
//    va_end(ap);
//}
