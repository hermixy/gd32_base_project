#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
//#include "debug.h"

#include "protocol_hal.h"

#if ENABLE_PROTCOL == 1

static link_obj_t       sg_link_obj = {0};
static pf_check_route   sg_sys_check_route   = NULL;
static uint8_t flag_init_call = 0;
static uint8_t send_buf[MAX_SEND_PACK_LEN] = {0};
int32_t protocol_send_by_id(hal_frame_info_t *p_frame_info)
{
    static volatile uint8_t lock = 0;
    protocol_obj_t *p_protocol = NULL;				//协议名，包头，校验，数据
    reg_protocol_t *p_reg_protocol = NULL;          //protocol_obj_t，list_t
    link_hal_item  *p_link = NULL;                  //链路信息
    int32_t ret = 0;
    uint16_t pack_len = 0;
    list_t   *cur_list;
    list_t   *tag_list_head;
    uint8_t  find_link = 0;
    uint8_t  find_protocol = 0;
    
    if(flag_init_call==0){return 0;}

    if(lock)
    {
        return -1;
    }
    lock = 1;

    tag_list_head = &sg_link_obj.link_all_head;
    list_for_each(cur_list, tag_list_head)			//找链路
    {
        p_link = list_entry(cur_list, link_hal_item, link_all_list);
        if(p_link->link_id == p_frame_info->link_id)
        {
            find_link = 1;
            break;
        }
    }

    if(find_link == 0)
    {
        ret =  -1;
        goto end;
    }

    tag_list_head = &sg_link_obj.protocol_head;
    list_for_each(cur_list, tag_list_head)			//找协议
    {
        p_reg_protocol = list_entry(cur_list, reg_protocol_t, protocol_node);
        if(p_reg_protocol->protocol_ptr->protocol_num == p_link->protocol_type)
        {
            find_protocol = 1;
            break;
        }
    }
    if(find_protocol == 0)
    {
        ret =  -2;
        goto end;
    }

    p_protocol = (protocol_obj_t *)p_reg_protocol->protocol_ptr;
    p_frame_info->seq  = p_link->send_seq++;
    pack_len = p_protocol->pack_data(send_buf, p_frame_info);
    ret = hal_app_write(p_link->write_dev, send_buf, pack_len, 0, NULL);

end:
    lock = 0;
    return ret;
}


void protocol_send_ack(hal_frame_info_t* p_frame_info,uint8_t* pbuf,uint16_t len)
{
    uint8_t addr = p_frame_info->dest;
    if(flag_init_call==0){return ;}
    p_frame_info->dest = p_frame_info->src;
    p_frame_info->src  = addr;
	p_frame_info->is_ack = 1;
    p_frame_info->pdata = pbuf;
    p_frame_info->data_len = len;
    protocol_send(p_frame_info);
}

int32_t  protocol_send(hal_frame_info_t *p_frame_info)
{
#if ENABLE_PROTCOL_ROUTE == 1
    link_hal_item *p_link = NULL;
    uint8_t     pack_type = ERR_ADDR_PACK;
    uint8_t     tag_link_id = 0;
    list_t   *cur_list;
    list_t   *tag_list_head;
    uint8_t  find_link = 0;

    if(flag_init_call==0){return 0;}
    tag_list_head = &sg_link_obj.link_all_head;
    list_for_each(cur_list, tag_list_head)
    {
        p_link = list_entry(cur_list, link_hal_item, link_all_list);
        if(p_link->link_id == p_frame_info->link_id)
        {
            find_link = 1;
            break;
        }
    }

    if(find_link == 0)
    {
        return -1;
    }

    //if(p_frame_info->check_type == 0)
    //{
    //    p_frame_info->check_type = DEFAUT_V2_CHECK_TYPE;
    //}


    /*

      路由逻辑

            链路支持路由  |     ok    |    ok    |    no    |    no     |
    ----------------------+-----------+----------+----------+-----------+
        系统以及注册路由表|     ok    |    no    |    ok    |    no     |
    ----------------------+-----------+----------+----------+-----------+
           此包的发送结果 |    route  | dis_route| dis_route| dis_route |

    */

    if((p_link->en_route & EN_ROUTE) && (sg_sys_check_route != NULL))
    {
        //---需要进行路由的包
        pack_type = sg_sys_check_route(p_frame_info, &tag_link_id);
        if(pack_type == IS_ROUTE_PACK)
        {
            p_frame_info->link_id = tag_link_id;
        }
        else
        {
            //DBG_I("unchange pack!!");
        }
    }
#endif
    return protocol_send_by_id(p_frame_info);
}


static void find_and_exec_action(hal_frame_info_t *p_frame_info)
{
    link_hal_item *p_link = NULL;
    list_t *tag_list_head = NULL;
    uint8_t en_debug      = 0;
    list_t  *cur_list;
    action_tab_t   *p_action_item = NULL;
    uint8_t flag_is_exec = 0;
    uint8_t  find_link = 0;

    tag_list_head = &sg_link_obj.link_all_head;
    list_for_each(cur_list, tag_list_head)
    {
        p_link = list_entry(cur_list, link_hal_item, link_all_list);
        if(p_link->link_id == p_frame_info->link_id)
        {
            find_link = 1;
            break;
        }
    }

    if(find_link == 0)
    {
        return ;
    }

    tag_list_head = &p_link->action_list_head;
    en_debug      =  p_link->en_debug;
    //DBG_I("recv [cmd_set:0x%02x ][cmd_id:0x%02x ]",p_frame_info->cmd_func,p_frame_info->cmd_id);
    if(p_link->flag_init)
    {
        list_for_each(cur_list, tag_list_head)
        {
            p_action_item = list_entry(cur_list, action_tab_t, action_list);
            if( (p_action_item->cmd_func == p_frame_info->cmd_func ) &&
                    (p_action_item->cmd_id   == p_frame_info->cmd_id   ) )
            {
                
//                if(p_frame_info->src == HOST_ADDR)
//                {
//                    DBG_I("pack is fall in while!");
//                    break;
//                }
                
                p_action_item->action_func(p_frame_info);
                flag_is_exec = 1;
                //DBG_I("action [cmd_set:%4d ][cmd_id:%4d ]: %s",p_frame_info->cmd_func,p_frame_info->cmd_id,p_action_item->func_name);
                break;
            }
        }
    }


    if(en_debug)
    {
        if(flag_is_exec == 0)
        {
            DBG_W("unknow pack (%s) [cmd_set:0x%02x cmd_id:0x%02x]",p_link->name, p_frame_info->cmd_func, p_frame_info->cmd_id);
        }
        else
        {
            DBG_I("recv (%s) [cmd_set:0x%02x ][cmd_id:0x%02x ] : %s",p_link->name, p_frame_info->cmd_func, p_frame_info->cmd_id, p_action_item->func_name);
        }
    }
}

static uint8_t read_buf[MAX_RECV_PACK_LEN];
static void run_unpack(link_hal_item *p_link)
{
    protocol_obj_t *p_protocol = NULL;
    uint32_t read_len = 0;
    uint32_t real_len = 0;
    uint16_t lb_rest_len = 0;
    uint16_t lb_have_len = 0;
    int32_t  ret = 0;
    uint8_t *p_buf = NULL;
    uint8_t    find_protocol = 0;
    static volatile uint8_t lock = 0;
    
    list_t *tag_list_head = NULL;
    list_t  *cur_list;
    reg_protocol_t *p_reg_protocol = NULL;

    if(flag_init_call==0){return ;}
    tag_list_head = &sg_link_obj.protocol_head;
    list_for_each(cur_list, tag_list_head)
    {
        p_reg_protocol = list_entry(cur_list, reg_protocol_t, protocol_node);
        p_protocol = (protocol_obj_t *)p_reg_protocol->protocol_ptr;
        if(p_reg_protocol->protocol_ptr->protocol_num == p_link->protocol_type)
        {
            find_protocol = 1;
            break;
        }
    }
    if(find_protocol == 0)
    {
        return;
    }
    if(lock)
    {
        return;
    }

    lock = 1;

    lb_move(&p_link->line_buf_obj, p_link->line_buf_obj.r_index);
    lb_rest_len = lb_get_rest_len(&p_link->line_buf_obj);
    read_len = (lb_rest_len < MAX_RECV_PACK_LEN) ? lb_rest_len : MAX_RECV_PACK_LEN;

    if(read_len > 0)
    {
        ret = hal_app_read(p_link->read_dev, read_buf, read_len, 0, &real_len);
    }

    if( (ret >= 0) && (real_len > 0) )
    {
        //DBG_I("dev :%d read:%d  real:%d",p_link->read_dev,read_len,real_len);
        lb_push(&p_link->line_buf_obj, read_buf, real_len);
    }

    lock = 0;

    do
    {
        lb_have_len = lb_get_data_len(&p_link->line_buf_obj);
        switch(p_link->unpack_step)
        {
        case UNPACK_HEAD:
            if(lb_have_len >= p_protocol->get_head_len(NULL,0))
            {
                p_buf = lb_get_r_ptr(&p_link->line_buf_obj);
                if(p_protocol->check_header(p_buf,lb_have_len))
                {
                    p_link->unpack_step = UNPACK_BODY;
                }
                else
                {
					if(p_link->en_debug)
					{
						DBG_E_LIMIT(1000, "[%s]just head err pack!!", p_link->name);
						//show_buf(p_buf,32);
					}
                    p_link->line_buf_obj.r_index++;
                }
            }
            else
            {
                goto end;   //字节数不够,等待下一个周期
            }
            break;

        case UNPACK_BODY:
            p_buf = lb_get_r_ptr(&p_link->line_buf_obj);
            if(lb_have_len >= p_protocol->get_pack_len(p_buf,lb_have_len))
            {
                p_link->unpack_step = UNPACK_HEAD;
                if(p_protocol->check_data(p_buf,lb_have_len))
                {
                    hal_frame_info_t frame_info = {0};
                    uint8_t tag_link_id = 0;
                    p_protocol->unpack_data(p_buf, &frame_info);
                    p_link->recv_seq = frame_info.seq;
                    frame_info.link_id = p_link->link_id;


                    if( (p_link->en_route & EN_ROUTE) && (sg_sys_check_route != NULL))
                    {
                        uint8_t pack_type;
                        //---需要进行路由的包
                        pack_type = sg_sys_check_route(&frame_info, &tag_link_id);
						
						if( (p_link->en_route&EN_HACK) || (pack_type == IS_HOST_PACK) )
						{
							find_and_exec_action(&frame_info);
						}
						
                        if(pack_type == IS_ROUTE_PACK)
                        {
                            frame_info.link_id = tag_link_id;
                            protocol_send_by_id(&frame_info);
                        }
                    }
                    else
                    {
                        //p_link->user_action(&frame_info,p_link->p_action__tab,p_link->remap_tab_num);
                        //查找并且调用响应函数
                        find_and_exec_action(&frame_info);
                    }

                    p_link->line_buf_obj.r_index += p_protocol->get_pack_len(p_buf,lb_have_len);

                }
                else
                {
					if(p_link->en_debug)
					{
						DBG_E_LIMIT(1000, "[%s]just have err pack!!####! -len:%d ", p_link->name, p_protocol->get_pack_len(p_buf,lb_have_len));
						//show_buf(p_buf,p_protocol->get_pack_len(p_buf));
					}
                    
                    p_link->line_buf_obj.r_index += p_protocol->get_head_len(p_buf,lb_have_len);
                }
            }
            else
            {
                goto end;   //字节数不够,等待下一个周期
            }
            break;

        default:
            DBG_E("unpack err!!!");
            while(1) ;  //此处不会出现
        }

    }
    while(1);

end:
    return;
}


static int32_t  protocol_ontick(uint32_t prio)
{
    /*运行不同优先级的解包*/
    list_t      *p_link_list_head = NULL;
    link_hal_item  *p_cur_link;
    list_t    *cur_node = NULL;
    
    if(flag_init_call==0){return 0;}

    if(prio >= MAX_TASKS_GROUP)
    {
        return -1;
    }

    p_link_list_head = &sg_link_obj.link_ctrl[prio].link_head;

    list_for_each(cur_node, p_link_list_head)
    {
        p_cur_link = list_entry(cur_node, link_hal_item, link_list);
        if(p_cur_link->buf_len < 3)
        {
            return 0;   /// 此链路不需要解包
        }
        run_unpack(p_cur_link);
        //DBG_I("run_unpack :%s",p_cur_link->name);
    }

    return 0;
}

void  protocol_init(void)
{
    uint32_t i = 0;
    reg_protocol_t *p_reg_protocol;
    link_hal_item  *p_cur_link;
    list_t          *tag_list_head;
    list_t          *cur_list;
    uint8_t          find_link = 0;

	//----初始化链表头
    DBG_I("protocol_init !!!");
    INIT_LIST_HEAD(&sg_link_obj.link_all_head);		//链路
    INIT_LIST_HEAD(&sg_link_obj.protocol_head);		//具体协议
    for(i = 0; i < MAX_PROTCOL_PRIO; i++)
    {
        sg_link_obj.link_ctrl[i].link_num = 0;
        INIT_LIST_HEAD(&sg_link_obj.link_ctrl[i].link_head);
    }

    /*1:加载协议*/
    for(i = 0; i < 100; i++)
    {
        p_reg_protocol = (reg_protocol_t *)auto_reg_get_func_cont_by_index(AUTO_REG_PROTOCOL, i);
        if(p_reg_protocol != NULL)
        {
            list_add(&p_reg_protocol->protocol_node, &sg_link_obj.protocol_head);
        }
    }

    /*2:加载链路*/
    for(i = 0; i < 100; i++)
    {
        p_cur_link = (link_hal_item *)auto_reg_get_func_cont_by_index(AUTO_REG_TYPE_LINK, i);
        if  (p_cur_link == NULL)
        {
            break;
        }
        if  (p_cur_link->prio < MAX_PROTCOL_PRIO)
        {

            list_add_tail(&p_cur_link->link_all_list, &sg_link_obj.link_all_head);

            //list_add_tail(&p_cur_link->link_list, &sg_link_obj.link_ctrl[p_cur_link->link_id].link_head);
			
			list_add_tail(&p_cur_link->link_list, &sg_link_obj.link_ctrl[p_cur_link->prio].link_head);


            lb_init(NULL, &p_cur_link->line_buf_obj, p_cur_link->p_buf, p_cur_link->buf_len);
            sg_link_obj.real_reg_link_num++;
            INIT_LIST_HEAD(&p_cur_link->action_list_head);
        }
    }

    /*3:加载action_tab*/
    for(i = 0; i < 10000; i++) //
    {
        action_tab_t *p_action_item = NULL;
        p_action_item = (action_tab_t *)auto_reg_get_func_cont_by_index(AUTU_REG_TYPE_LINK_ACTION, i);

        if(p_action_item == NULL)
        {
            break;
        }

        tag_list_head = &sg_link_obj.link_all_head;
        find_link = 0;
        list_for_each(cur_list, tag_list_head)
        {
            p_cur_link = list_entry(cur_list, link_hal_item, link_all_list);
            if(p_cur_link->link_id == p_action_item->link_id)
            {
                find_link = 1;
                break;
            }
        }

        if(find_link == 1)
        {
            INIT_LIST_HEAD(&p_action_item->action_list);
            list_add_tail(&p_action_item->action_list, &p_cur_link->action_list_head);
        }
        else
        {
            DBG_W("ADD action item- cmd_set:%d cmd_id:%d link_id:%d is err!!!", p_action_item->cmd_func, p_action_item->cmd_id, p_action_item->link_id);
        }
    }

    DBG_I("system max interface action:%d", i);

    /*4:加载路由信息*/
    auto_reg_get_func_type_cont(AUTO_REG_ROUTE_CFG, (void **)&sg_sys_check_route, 1);

    /*5:初始化链路*/
    tag_list_head = &sg_link_obj.link_all_head;
    list_for_each(cur_list, tag_list_head)
    {
        p_cur_link = list_entry(cur_list, link_hal_item, link_all_list);
        p_cur_link->send_seq = 0;
        p_cur_link->recv_seq = 0;
        p_cur_link->unpack_step = UNPACK_HEAD;
        p_cur_link->flag_init  = 1;
        DBG_I("init link  [%s]",p_cur_link->name);
    }
    
    flag_init_call = 1;
}

//-----tasks.h中已经定义
//#define VH_TASK   0
//#define HI_TASK   1
//#define LW_TASK   2
//#define ID_TASK   3

void run_protocol_vh(void)
{
    protocol_ontick(VH_TASK);
}
void run_protocol_hi(void)
{
    protocol_ontick(HI_TASK);
}
void run_protocol_lw(void)
{
    protocol_ontick(LW_TASK);
}
void run_protocol_id(void)
{
    protocol_ontick(ID_TASK);
}

REG_TASK(VH_TASK, PROTOCOL_RUN_PERIO, run_protocol_vh);
REG_TASK(HI_TASK, PROTOCOL_RUN_PERIO, run_protocol_hi);
REG_TASK(LW_TASK, PROTOCOL_RUN_PERIO, run_protocol_lw);
REG_TASK(ID_TASK, PROTOCOL_RUN_PERIO, run_protocol_id);

uint32_t judge_link_write_dev(uint32_t debug_dev)
{
	list_t *cur_node = NULL;
	list_t *tag_list_head = NULL;
	link_hal_item *p_cur_link = NULL;
	
	if(flag_init_call)
	{
		tag_list_head = &sg_link_obj.link_all_head;
		list_for_each(cur_node, tag_list_head)
		{
			p_cur_link = list_entry(cur_node, link_hal_item, link_all_list);
			
			if( p_cur_link->write_dev == debug_dev )
			{
				return 1;
			}
		}
	}
	return 0;
}

#if TINY_FUNC_CODE == 0
int32_t list_links(uint8_t argc, uint8_t *argv[])
{
    list_t          *tag_list_head;
    list_t          *cur_list;
    link_hal_item  *p_cur_link;
	(void)p_cur_link;
    DBG_I("*cur system reg  %d links ", sg_link_obj.real_reg_link_num);
    tag_list_head = &sg_link_obj.link_all_head;
    list_for_each(cur_list, tag_list_head)
    {
        p_cur_link = list_entry(cur_list, link_hal_item, link_all_list);
        DBG_I("[link_id:%d] prio:%d -  %-12s  protocol-type:%d   bufsize:%d ", p_cur_link->link_id, p_cur_link->prio, p_cur_link->name, p_cur_link->protocol_type, p_cur_link->buf_len);
    }

    return RET_OK;
}

REG_SHELL_CMD_X(ls_links, 0, list_links, "list_links", "list_links.  eg list_links \r\n ");


int32_t list_action(uint8_t argc, uint8_t *argv[])
{
    if(argc >= 2)
    {
        uint32_t link_id = 0;
        list_t          *tag_list_head;
        list_t          *cur_list;
        link_hal_item  *p_cur_link;
        uint8_t         find_link = 0;
        sscanf((const char *)argv[1], "%d", &link_id);


        {
            action_tab_t     *p_action_item = NULL;
			(void)p_action_item;
            tag_list_head = &sg_link_obj.link_all_head;
            list_for_each(cur_list, tag_list_head)
            {
                p_cur_link = list_entry(cur_list, link_hal_item, link_all_list);
                if(p_cur_link->link_id  == link_id)
                {
                    find_link = 1;
                    break;
                }
            }

            if(find_link == 0)
            {
                DBG_W("LINK_ID %d is not exist!!!", link_id);
                return 0;
            }

            if(p_cur_link->flag_init)
            {
                DBG_I("The Link ID:%d action function as follow:", link_id);
                tag_list_head = &p_cur_link->action_list_head;
                list_for_each(cur_list, tag_list_head)
                {
                    p_action_item = list_entry(cur_list, action_tab_t, action_list);
                    DBG_I("[cmd_set:0x%02x ][cmd_id:0x%02x ]: %s ", p_action_item->cmd_func, p_action_item->cmd_id, p_action_item->func_name);
                }
            }
            else
            {
                DBG_W("LINK_ID %d is not exist!!!", link_id);
            }
        }
    }
    else
    {
        DBG_W("param is error, [ eg. ] list_action link_id ");
    }
    return RET_OK;
}

REG_SHELL_CMD_X(ls_action, 0, list_action, "list_action", "list_action.  eg list_action link_id\r\n ");
#endif

#endif


