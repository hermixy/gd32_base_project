#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
//#include "debug.h"
#include "x_frame.h"


#define PRODUCT_TYPE_EF01    1
#define PRODUCT_TYPE_EF02    2
#define PRODUCT_TYPE_EF03    3

#define CUR_SUPPORT_PRODUCT_TYPE   PRODUCT_TYPE_EF02


#define SUP_PWD_HI_KEY      0xaa
#define SUP_PWD_LO_KEY      0x55

#define APP_CHK_TYPE_SUM     0x0
#define APP_CHK_TYPE_CRC8    0x1
#define APP_CHK_TYPE_CRC16   0x2
#define APP_CHK_TYPE_CRC32   0x3

#define APP_ENC_TYPE_NONE   0
#define APP_ENC_TYPE_XOR    1
#define APP_ENC_TYPE_AES    2

#define APP_CHECK_ENABLE    1


#define ENABLE_LOADER_MODULE 1

#define LOADER_APP_INFO_CFG_VER  1 //第一版本的附加参数

#if  ENABLE_LOADER_MODULE == 1

extern void jump_to_app(uint32_t app_address);

#define LOADER_VER          (0x01000000)
#define DEF_APP_ADDRESS     (0x800a000)
#define DEF_BOOT_DELAY      (10)
#define MAX_TRANS_PACK_LEN  (512)
#define APP_ENTRY_OFFSET    (0x800) //前面2K存储loader的固定信息
#define JUMP_TO_APP_FUNC    jump_to_app

typedef uint8_t (*pf_force_update_check)(void);
pf_force_update_check g_force_check_update=NULL;

#pragma pack(1)
typedef struct
{
	uint32_t		max_trans_per_pack;	
}update_prepare_t;

typedef struct
{
	int8_t  		sta;
	uint32_t		trans_per_pack;	
    uint16_t        loader_support_product_type;
    uint16_t        loader_support_product_detail;
    uint32_t        loader_ver;
}update_prepare_ack_t;

typedef struct
{
    uint32_t        trans_per_block;
    uint32_t        total_blocks;
	uint32_t 		fw_size;  			
	uint32_t 		fw_sum_chk_val;    		
}update_trans_cfg_t;


typedef struct
{
	int8_t          sta;
    uint8_t         sn_len;
    uint8_t         sn[20];	
}update_trans_cfg_ack_t;


typedef struct
{
	int8_t  sta;
	uint16_t recv_cur_block;
}update_trans_ack_t;

typedef struct
{
	uint32_t real_checksum;
}update_check_t;

#pragma pack()



enum
{
    upadte_sta_ok = 0,
    upadte_sta_check_err,
    upadte_sta_param_err,
    upadte_sta_fw_size_too_large,
    upadte_sta_cur_block_is_been_write,
	upadte_sta_flash_is_not_define,
	upadte_sta_boot_addr_err,
};

#define LOADER_FSM_ID       0xf1
#define NEED_IN_LOADER      0x01
#define ENTRY_APP_KEY   	(0x12345678)
#define LOADER_APP_KEY  	(0x87654321)
//enum  FSM state
enum
{
	LD_S_CHECK=1,
    LD_S_UPDATE_IDLE,   //此时可以获取CPUID SN
    LD_S_UPDATE_TRANS,
    LD_S_GOTO_APP,
};


//-----enum FSM event
enum
{
	EV_force_stay_in_loader = 1,
    EV_boot_param_err ,
    EV_flag_in_loader ,
	EV_app_err ,
    EV_recv_update_err ,
	EV_recv_update_cmd ,
    EV_recv_update_dat ,
	EV_loader_time_out,
	EV_update_success,
};

typedef struct
{
    uint32_t  boot_param_len;
    uint32_t  need_in_loader_flag;
    uint32_t  boot_delay;
    uint32_t  fw_addr;
    uint32_t  boot_addr;
    uint32_t  loader_ver;
    uint32_t  boot_cfg_chk_val; 
}boot_cfg_t;


typedef struct
{
    uint32_t  app_info_chk_val;
    uint32_t  app_info_ver;     //此长度为 app_header的2K
    uint32_t  app_info_block_len;
    uint32_t  app_info_real_len;
	uint32_t  app_info_chk_type;
	
    
	uint32_t  app_len;
    uint32_t  app_chk_type;
	uint32_t  app_chk_val;
    
    uint32_t  app_enc_type;
    uint32_t  app_enc_key;
	uint32_t  app_auth_type;
    uint32_t  app_auth_key;
    
    uint16_t  product_type;  //大类
    uint16_t  product_detail;     //明细
    uint32_t  app_ver;
    
    uint8_t   app_code_ver[32];
    uint8_t   app_code_time[32];
    uint8_t   app_product_info[32];
    uint8_t   app_product_company[32];
    uint8_t   app_custom_info[32];
    
}app_info_t;


typedef struct
{
    boot_cfg_t      boot_cfg;
    uint32_t        fw_size;        //此ENC长度包含 app_header的2K
    uint32_t        fw_sum_chk_val; //ENC的和校验
    uint32_t		trans_per_pack;	
    uint32_t        total_block_num;
    uint32_t        recv_block_num;
    uint32_t        timeout_calc_snap  ; //超时开始计时的时间点
    uint32_t        flag_wait_time_out ;
    uint8_t         flag_is_force_update;
    uint8_t         flag_is_boot_cfg_err;
    uint8_t         flag_is_app_err;
}ef_boot_t;


static void     load_boot_cfg           (boot_cfg_t* p_boot_cfg);
static void     reset_boot_cfg          (boot_cfg_t* p_boot_cfg);
static void     save_boot_cfg           (boot_cfg_t* p_boot_cfg);
static uint32_t check_app_is_err         (boot_cfg_t* p_boot_cfg);
static uint32_t check_boot_param_is_err  (boot_cfg_t* p_boot_cfg);

void            ld_s_check_in           (fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len);
void            ld_s_check_exe          (fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len);
uint32_t        ld_s_check_cond         (fsm_item_t *p_fsm, uint32_t event, uint8_t *pbuf, uint32_t len);


void            ld_s_idle_in            (fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len);
void 			ld_s_idle_exe			(fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len);
uint32_t        ld_s_idle_cond          (fsm_item_t *p_fsm, uint32_t event, uint8_t *pbuf, uint32_t len);


uint32_t        ld_s_trans_cond         (fsm_item_t *p_fsm, uint32_t event, uint8_t *pbuf, uint32_t len);

void            ld_s_goto_app_in         (fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len);
void            ld_s_goto_app_exe        (fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len);
uint32_t ld_s_goto_app_cond(fsm_item_t *p_fsm, uint32_t event, uint8_t *pbuf, uint32_t len);

static ef_boot_t    g_ef_boot            ={0};
app_info_t* p_app_info = NULL;
part_item_t* p_app_part= NULL;

static uint8_t  enable_led_flag = 0;
//       fsm_id,       fsm_run_time,  max_param_len,  init_state) 
REG_FSM(LOADER_FSM_ID, 1, 0,LD_S_CHECK);
REG_FSM_STATE(LOADER_FSM_ID, LD_S_CHECK	            , ld_s_check_in		        , ld_s_check_exe	        , NULL					, ld_s_check_cond);
REG_FSM_STATE(LOADER_FSM_ID, LD_S_UPDATE_IDLE	    , ld_s_idle_in  		    , ld_s_idle_exe				, NULL					, ld_s_idle_cond);
REG_FSM_STATE(LOADER_FSM_ID, LD_S_UPDATE_TRANS	    , NULL		                , NULL						, NULL					, ld_s_trans_cond);
REG_FSM_STATE(LOADER_FSM_ID, LD_S_GOTO_APP	        , ld_s_goto_app_in          , ld_s_goto_app_exe			, NULL		            , ld_s_goto_app_cond);    


static void led_task(void)
{
	static uint32_t state = 0;
    state ^=0x01;
	
	if(enable_led_flag)
	{
		hal_app_write(DRV_GPIO,(uint8_t*)&state,sizeof(uint32_t),LED_PIN_PIN,NULL);
	}
	
    //DBG_I("ef_state.wf_is_in:%d",ef_state.wf_is_in);
    
    //send_demo();

}
REG_TASK(ID_TASK, 200, led_task);


void ld_s_check_in(fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len)
{
    //1:force_check
    //2:check_boot_param is ok
    //3:check_boot_param flag 
    //4:check_app_is ok
    DBG_I("ld_s_check_in");
    g_ef_boot.flag_is_force_update  = 0;
    if(g_force_check_update!=NULL) {g_ef_boot.flag_is_force_update = g_force_check_update();}
    
    //g_ef_boot.flag_is_force_update  = 1;
    if(g_ef_boot.flag_is_force_update){ fsm_send_event(LOADER_FSM_ID,EV_force_stay_in_loader,NULL,0);  return; }
//    
    load_boot_cfg(&g_ef_boot.boot_cfg);
    if( check_boot_param_is_err(&g_ef_boot.boot_cfg) != 0){fsm_send_event(LOADER_FSM_ID,EV_boot_param_err,NULL,0);  return;}
    
    if( g_ef_boot.boot_cfg.need_in_loader_flag!=0)   {fsm_send_event(LOADER_FSM_ID,EV_flag_in_loader,NULL,0);  return;}

//    //check app is ok
    if( check_app_is_err(&g_ef_boot.boot_cfg) != 0){fsm_send_event(LOADER_FSM_ID,EV_app_err,NULL,0);  return;}
    
    g_ef_boot.timeout_calc_snap  = get_sys_ms();
    
    //g_ef_boot.boot_cfg.boot_delay = DEF_BOOT_DELAY;
    
}
void ld_s_check_exe(fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len)
{
    DBG_I_LIMIT(1000,"flag_wait_time_out");
    if( (get_sys_ms() ) > (g_ef_boot.timeout_calc_snap + g_ef_boot.boot_cfg.boot_delay) )
    {
        fsm_send_event(LOADER_FSM_ID,EV_loader_time_out,NULL,0);
        
    }
}


uint32_t ld_s_check_cond(fsm_item_t *p_fsm, uint32_t event, uint8_t *pbuf, uint32_t len)
{
    uint32_t ret = 0;
    DBG_I("1:ld_s_check_cond evt:%d",event);
    if( (event == EV_force_stay_in_loader) || 
		(event == EV_app_err		     ) ||
		(event == EV_recv_update_cmd	 ) ||
        (event == EV_boot_param_err      ) ||
		(event == EV_flag_in_loader      ) 
		)
    {
		enable_led_flag = 1;
        DBG_I("sys will goto loader_state_update");
        p_fsm->next_state = LD_S_UPDATE_IDLE;
        ret = 1;
    }
    
    if(event == EV_loader_time_out)
    {
        p_fsm->next_state = LD_S_GOTO_APP;
        ret = 1;
    }


	if(event == EV_update_success)
	{
	   p_fsm->next_state = LD_S_CHECK;
	   ret = 1;
	}

	
    return ret;
}



//此状态在接收到 第一个block数据的时候跳转
void ld_s_idle_in(fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len)
{
    g_ef_boot.recv_block_num = 0;
    
}

static void send_in_loader_info(void)
{
	uint8_t str[]="  I am in Loader!!";
	hal_frame_info_t send_info_frame={0};
	str[0]=0;
	str[1]=0;
	send_info_frame.link_id  =  PC_LINK_ID;
	send_info_frame.cmd_func =  COMM_CMD_SET;
	send_info_frame.src      =  HOST_ADDR;
	send_info_frame.dest     =  PC_ADDR;
	send_info_frame.cmd_id   =  CMD_ID_SHELL_OUT;  
	send_info_frame.pdata    =  (uint8_t *)str;
	send_info_frame.data_len =  sizeof(str)-1;    //协议传输不用后面的\0    add log_type  log_win  sub 0x0d  0x0a
	protocol_send_by_id(&send_info_frame);
	
	//send_info_frame.link_id  =  GPRS_LINK_ID;
	//protocol_send_by_id(&send_info_frame);
}

void ld_s_idle_exe(fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len)
{
    RUN_BY_LIMIT_FUNC(1000,send_in_loader_info);
}



uint32_t ld_s_idle_cond(fsm_item_t *p_fsm, uint32_t event, uint8_t *pbuf, uint32_t len)
{
    uint32_t ret = 0;
    if( event == EV_recv_update_dat      ) 
    {
        DBG_I("sys will goto trans_dat");
        p_fsm->next_state = LD_S_UPDATE_TRANS;
        ret = 1;
    }
    return ret;
}


//EV_recv_update_cmd ,
//EV_recv_update_dat ,
uint32_t ld_s_trans_cond(fsm_item_t *p_fsm, uint32_t event, uint8_t *pbuf, uint32_t len)
{
    uint32_t ret = 0;
    
    if( event == EV_recv_update_cmd	 )
    {
        p_fsm->next_state = LD_S_UPDATE_IDLE;
        ret = 1;
    }
    
    
    if(event == EV_recv_update_err)
    {
        DBG_I("update err!! ");
        p_fsm->next_state = LD_S_UPDATE_IDLE;
        ret = 1;
    }
    
    if(event == EV_update_success)
    {
        DBG_I("EV_update_success!! ");
        p_fsm->next_state = LD_S_CHECK;//LD_S_GOTO_APP;  虽然升级ok但是还是需要在检查一次
        g_ef_boot.boot_cfg.need_in_loader_flag = 0;
        save_boot_cfg(&g_ef_boot.boot_cfg);
        ret = 1;
    }
    
   
    return ret;
}

void            ld_s_goto_app_in           (fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len)
{
    g_ef_boot.flag_wait_time_out = 0;
    g_ef_boot.timeout_calc_snap  = get_sys_ms();
    DBG_I("ld_s_goto_app_in");
}


void            ld_s_goto_app_exe           (fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len)
{
    //if( (get_sys_ms() ) > (g_ef_boot.timeout_calc_snap + g_ef_boot.boot_cfg.boot_delay) )
    {
		uint32_t addr = 0;
        part_item_t* p_app_part=NULL;
        //跳转到APP的命令 jump_app
        

        
        p_app_part = get_flash_item_by_part_id(SYS_APP_PART);
        if(p_app_part==NULL){DBG_E("flash part app is not exist"); return;}
		
		addr = p_app_part->part_address + (p_app_info->app_info_block_len);
		
		DBG_I("jump_app,and never come back! 0x%x",addr);
		
        //block_ms(500);   //不知道为什么去除打印之后，此代码会一直卡在这里
        //block_ms(5000); //为了打印完成，之后在跳转到APP
        
        JUMP_TO_APP_FUNC(p_app_part->part_address + (p_app_info->app_info_block_len));
    }   
}

uint32_t ld_s_goto_app_cond(fsm_item_t *p_fsm, uint32_t event, uint8_t *pbuf, uint32_t len)
{
    uint32_t ret = 0;
    if( event == EV_recv_update_cmd	 )
    {
        p_fsm->next_state = LD_S_UPDATE_IDLE;
        ret = 1;
    }
    return ret;
}

//--------------------------------------------boot param option

void show_boot_cfg(boot_cfg_t* p_boot_cfg)
{
    DBG_I("need_in_loader_flag:0x%x",p_boot_cfg->need_in_loader_flag);
    DBG_I("boot_chk           :0x%x",p_boot_cfg->boot_cfg_chk_val);
}

static void load_boot_cfg(boot_cfg_t* p_boot_cfg)
{
	part_item_t* p_boot_cfg_part=NULL;
	
	p_boot_cfg_part = get_flash_item_by_part_id(SYS_BOOT_CFG_PART);
	if(p_boot_cfg_part==NULL){DBG_E("flash part boot cfg is not exist"); return ;}
	
	memcpy(p_boot_cfg,(void*)p_boot_cfg_part->part_address,sizeof(boot_cfg_t));
    DBG_I("load_boot_cfg");
    show_boot_cfg(p_boot_cfg);
}


static void reset_boot_cfg(boot_cfg_t* p_boot_cfg)
{
    uint32_t start_addr = (uint32_t)p_boot_cfg;
    uint32_t end_addr   = (uint32_t)start_addr+sizeof(boot_cfg_t)-4;
   
	part_item_t* p_boot_cfg_part=NULL;
    part_item_t* p_app_part     =NULL;
	
	p_boot_cfg_part = get_flash_item_by_part_id(SYS_BOOT_CFG_PART);
	if(p_boot_cfg_part==NULL){DBG_E("flash part boot cfg  is not exist"); return ;}
    
    p_app_part      = get_flash_item_by_part_id(SYS_APP_PART);
	if(p_app_part==NULL){DBG_E("flash part app  is not exist"); return ;}
    
    memset(p_boot_cfg,0,sizeof(boot_cfg_t));
    
    p_boot_cfg->boot_param_len       = sizeof(boot_cfg_t);
    p_boot_cfg->need_in_loader_flag  =  0x1;
    p_boot_cfg->boot_addr            =  p_app_part->part_address + APP_ENTRY_OFFSET;
    p_boot_cfg->fw_addr              =  p_app_part->part_address;
    p_boot_cfg->boot_delay           =  DEF_BOOT_DELAY;
    p_boot_cfg->loader_ver           = 	LOADER_VER;
    
    p_boot_cfg->boot_cfg_chk_val = calc_chk_val(u_chk_sum,(uint8_t*)start_addr,(end_addr - start_addr));
    save_boot_cfg(p_boot_cfg);
    DBG_I("reset_boot_cfg");
    show_boot_cfg(p_boot_cfg);
}


static void save_boot_cfg(boot_cfg_t* p_boot_cfg)
{
    uint32_t start_addr = (uint32_t)p_boot_cfg;
    uint32_t end_addr   = (uint32_t)start_addr+sizeof(boot_cfg_t)-4;
    part_item_t* p_boot_cfg_part=NULL;

	
	p_boot_cfg_part = get_flash_item_by_part_id(SYS_BOOT_CFG_PART);
	if(p_boot_cfg_part==NULL){DBG_E("flash part boot cfg is not exist"); return ;}
    
    p_boot_cfg->boot_cfg_chk_val = calc_chk_val(u_chk_sum,(uint8_t*)start_addr,(end_addr - start_addr));
    
    flash_part_erase(SYS_BOOT_CFG_PART);
    flash_part_write(SYS_BOOT_CFG_PART,0,(uint8_t*)p_boot_cfg,sizeof(boot_cfg_t));
    DBG_I("save_boot_cfg");
    show_boot_cfg(p_boot_cfg);
}

static void show_app_info(app_info_t* p_cur_app_info)
{
    DBG_I("app_info_chk_val   :0x%x ",p_cur_app_info->app_info_chk_val);
    DBG_I("app_info_ver       :0x%x ",p_cur_app_info->app_info_ver);
    DBG_I("app_info_block_len :0x%x ",p_cur_app_info->app_info_block_len);
    DBG_I("app_info_chk_type  :0x%x ",p_cur_app_info->app_info_chk_type);
    
    DBG_I("app_len            :0x%x ",p_cur_app_info->app_len);
    DBG_I("app_chk_type       :0x%x ",p_cur_app_info->app_chk_type);
    DBG_I("app_chk_val        :0x%x ",p_cur_app_info->app_chk_val);
    
    DBG_I("app_enc_type       :0x%x ",p_cur_app_info->app_enc_type);
    DBG_I("app_enc_key        :0x%x ",p_cur_app_info->app_enc_key);
    DBG_I("app_auth_type      :0x%x ",p_cur_app_info->app_auth_type);
    DBG_I("app_auth_key       :0x%x ",p_cur_app_info->app_auth_key);
    
    DBG_I("product_type       :0x%x  product_detail   :0x%x",p_cur_app_info->product_type,p_cur_app_info->product_detail);
    DBG_I("app_ver            :0x%x ",p_cur_app_info->app_ver);
    
    DBG_I("app_code_time      :%s ",p_cur_app_info->app_code_time);
    DBG_I("app_product_company:%s ",p_cur_app_info->app_product_company);
    DBG_I("app_product_info   :%s ",p_cur_app_info->app_product_info);
    DBG_I("app_custom_info    :%s ",p_cur_app_info->app_custom_info);
    
}

static uint32_t check_app_is_err(boot_cfg_t* p_boot_cfg)
{
	uint32_t check_result = 1;  //1-err 
    uint32_t check_val = 1;
    uint8_t* p_start_ptr = 0;
    
    
    
    p_app_part = get_flash_item_by_part_id(SYS_APP_PART);
	if(p_app_part==NULL){DBG_E("flash part app-part is not exist"); return check_result;}

    //1-check app_info_section
    p_app_info = (app_info_t*)p_app_part->part_address;
    
    DBG_I("p_app_part->part_address:0x%x",p_app_part->part_address);
    show_app_info(p_app_info);
	//show_buf((uint8_t*)p_app_info,sizeof(app_info_t));
    
    if(p_app_info->app_info_chk_type>APP_CHK_TYPE_CRC32) {DBG_E("app_info_check_type is err"); return check_result;}
    if(p_app_info->app_chk_type     >APP_CHK_TYPE_CRC32) {DBG_E("app_check_type      is err"); return check_result;}
    if( (p_app_info->app_info_block_len)<sizeof(app_info_t)){DBG_E("app_info_BLOCK_size       is err"); return check_result;}
    if( (p_app_info->app_info_real_len)!=sizeof(app_info_t)){DBG_E("app_info_size       is err"); return check_result;}
    
    
    p_start_ptr = (uint8_t*)&p_app_info->app_info_ver;
    check_val = calc_chk_val(p_app_info->app_info_chk_type,p_start_ptr,sizeof(app_info_t)-4);

	//show_buf(p_start_ptr,sizeof(app_info_t)-4);
	
    if(check_val!=p_app_info->app_info_chk_val)          {DBG_E("app_info           is err[calc:_val:0x%x real:0x%x] size:%d",check_val,p_app_info->app_info_chk_val,sizeof(app_info_t)-4); return check_result;}


	
    //2-check app-bin 
    p_start_ptr = (uint8_t*)(p_app_part->part_address+(p_app_info->app_info_block_len));
    check_val = calc_chk_val(p_app_info->app_chk_type,p_start_ptr,p_app_info->app_len);
    if(check_val!=p_app_info->app_chk_val)               {DBG_E("app_bin           is err[calc:chk:0x%x rec_chk_val:0x%x]",check_val,p_app_info->app_chk_val); return check_result;}
    
    //3-check is sup key 
//    if(  //(p_app_info->product_type!= PRODUCT_TYPE_EF02) &&
//        ( (p_app_info->product_type!= SUP_PWD_HI_KEY) && (p_app_info->product_detail != SUP_PWD_LO_KEY))         
//      )
//    {
//        DBG_E("product_type           is err:type:0x%x  detail:0x%x",p_app_info->product_type,p_app_info->product_detail); 
//        return check_result;
//    }
//    

	return 0;
}


static uint32_t check_boot_param_is_err(boot_cfg_t* p_boot_cfg)
{
	uint32_t check_val = 0;
	uint32_t start_addr = (uint32_t)p_boot_cfg;
    uint32_t end_addr   = (uint32_t)start_addr+sizeof(boot_cfg_t)-4;
    part_item_t* p_boot_cfg_part=NULL;

	p_boot_cfg_part = get_flash_item_by_part_id(SYS_BOOT_CFG_PART);
	if(p_boot_cfg_part==NULL){DBG_E("flash part boot cfg is not exist"); return 0;}
    
    check_val = calc_chk_val(u_chk_sum,(uint8_t*)start_addr,(end_addr - start_addr));
    
    if( (check_val!=p_boot_cfg->boot_cfg_chk_val)  || (p_boot_cfg->boot_param_len!=sizeof(boot_cfg_t)))
    {
        DBG_I("real_chk:0x%x   boot_chk:0x%x",check_val,p_boot_cfg->boot_cfg_chk_val);
        reset_boot_cfg(p_boot_cfg);
        DBG_I("---------------------------reload boot cfg");
        load_boot_cfg(p_boot_cfg);
    }
   
	return 0;
}


//--------------------------------------------
static int32_t              fw_update_prepare       (hal_frame_info_t* p_frame_info);
static int32_t              fw_update_trans_cfg     (hal_frame_info_t* p_frame_info);
static int32_t              fw_update_trans_data    (hal_frame_info_t* p_frame_info);
static int32_t              fw_update_check         (hal_frame_info_t* p_frame_info);

uint8_t get_cpu_id(uint8_t* pbuf);

REG_LINK_ACTION(PC_LINK_ID      ,0x1    ,0x10          ,fw_update_prepare);
REG_LINK_ACTION(PC_LINK_ID      ,0x1    ,0x11          ,fw_update_trans_cfg);
REG_LINK_ACTION(PC_LINK_ID      ,0x1    ,0x12          ,fw_update_trans_data);
REG_LINK_ACTION(PC_LINK_ID      ,0x1    ,0x13          ,fw_update_check);

//REG_LINK_ACTION(GPRS_LINK_ID      ,0x1    ,0x10          ,fw_update_prepare);
//REG_LINK_ACTION(GPRS_LINK_ID      ,0x1    ,0x11          ,fw_update_trans_cfg);
//REG_LINK_ACTION(GPRS_LINK_ID      ,0x1    ,0x12          ,fw_update_trans_data);
//REG_LINK_ACTION(GPRS_LINK_ID      ,0x1    ,0x13          ,fw_update_check);

//REG_LINK_ACTION(DBG_LINK_ID      ,0x1    ,0x10          ,fw_update_prepare);
//REG_LINK_ACTION(DBG_LINK_ID      ,0x1    ,0x11          ,fw_update_trans_cfg);
//REG_LINK_ACTION(DBG_LINK_ID      ,0x1    ,0x12          ,fw_update_trans_data);
//REG_LINK_ACTION(DBG_LINK_ID      ,0x1    ,0x13          ,fw_update_check);


static int32_t         fw_update_prepare       (hal_frame_info_t* p_frame_info)
{
	update_prepare_t*     p_update_prepare =NULL;
    update_prepare_ack_t  update_ack ={0};
    if(p_frame_info->data_len!=sizeof(update_prepare_t)){update_ack.sta = upadte_sta_param_err;}
    else
    {
		p_update_prepare = (update_prepare_t*)p_frame_info->pdata;
		update_ack.trans_per_pack = (p_update_prepare->max_trans_per_pack<MAX_TRANS_PACK_LEN)?(p_update_prepare->max_trans_per_pack):(MAX_TRANS_PACK_LEN);
		update_ack.sta = upadte_sta_ok;
        DBG_I("TRANS_PER_PACK:%d   APP:%d  CUR:%d  link_id:%d src:%d dest:%d"  ,update_ack.trans_per_pack,p_update_prepare->max_trans_per_pack,MAX_TRANS_PACK_LEN,p_frame_info->link_id,p_frame_info->src,p_frame_info->dest);
	}
    protocol_send_ack(p_frame_info,(uint8_t*)&update_ack,sizeof(update_prepare_ack_t));
	if(update_ack.sta == upadte_sta_ok){fsm_send_event(LOADER_FSM_ID,EV_recv_update_cmd,NULL,0);}
    return 0;
}

static int32_t         fw_update_trans_cfg       (hal_frame_info_t* p_frame_info)
{
	update_trans_cfg_t*  p_update_cfg=NULL; 
    update_trans_cfg_ack_t update_ack={0}; 

    if(p_frame_info->data_len!=sizeof(update_trans_cfg_t))
    {
        DBG_E("p_frame_info->data_len:%d  recv:%d",sizeof(update_trans_cfg_t),p_frame_info->data_len);
        update_ack.sta = upadte_sta_param_err;
    }
    else
    {
		part_item_t* p_app_part=NULL;
		p_app_part = get_flash_item_by_part_id(SYS_APP_PART);
		
		update_ack.sta = upadte_sta_ok;
		if(p_app_part==NULL){DBG_E("flash part app is not exist"); update_ack.sta = upadte_sta_flash_is_not_define;}
		else 
	    {
			p_update_cfg = (update_trans_cfg_t*)p_frame_info->pdata;
		
            g_ef_boot.trans_per_pack          = p_update_cfg->trans_per_block;
            if(g_ef_boot.trans_per_pack > MAX_TRANS_PACK_LEN){update_ack.sta = upadte_sta_param_err;}
            g_ef_boot.total_block_num         = p_update_cfg->total_blocks;
			g_ef_boot.fw_size                 = p_update_cfg->fw_size;
            if(g_ef_boot.fw_size > p_app_part->part_size){update_ack.sta = upadte_sta_fw_size_too_large;}
            g_ef_boot.fw_sum_chk_val          = p_update_cfg->fw_sum_chk_val;
            
            DBG_I("trans_per_block:%d",p_update_cfg->trans_per_block);
			DBG_I("total_blocks :%d"  ,p_update_cfg->total_blocks);
			DBG_I("fw_file_size :0x%x",p_update_cfg->fw_size);
            DBG_I("fw_file_chk  :0x%x",p_update_cfg->fw_sum_chk_val);
			
			update_ack.sn_len = get_cpu_id(update_ack.sn);
		}
    }
	
	protocol_send_ack(p_frame_info,(uint8_t*)&update_ack,sizeof(update_trans_cfg_ack_t));
	if(update_ack.sta == upadte_sta_ok){fsm_send_event(LOADER_FSM_ID,EV_recv_update_cmd,NULL,0);}
	else                               {fsm_send_event(LOADER_FSM_ID,EV_recv_update_err,NULL,0);}
    
    DBG_I("fw_update_trans_cfg  update_ack.sta:%d",update_ack.sta);
 
    return 0;
}


static int32_t         fw_update_trans_data       (hal_frame_info_t* p_frame_info)
{
	
    update_trans_ack_t update_trans_ack = {0};
	uint16_t cur_recv_block = 0;
    uint32_t wr_addr        = 0;

	memcpy(&cur_recv_block,p_frame_info->pdata,2);
	//DBG_I("trans![%d]: %d --len:%d --",update_obj.total_block,recv_block,p_frame_info->data_len-2);
    
    if(g_ef_boot.recv_block_num < cur_recv_block)
    {
        if(cur_recv_block==1)
        {
            part_item_t* p_app_part=NULL;
            p_app_part = get_flash_item_by_part_id(SYS_APP_PART);
            if(p_app_part==NULL){DBG_E("flash part app is not exist"); update_trans_ack.sta = upadte_sta_flash_is_not_define;}
            
            g_ef_boot.boot_cfg.fw_addr = p_app_part->part_address;

            flash_part_erase(SYS_APP_PART);
			//DBG_I("---flash_part_erase----");
        }
        
        wr_addr = ((cur_recv_block-1)*g_ef_boot.trans_per_pack);
        flash_part_write(SYS_APP_PART,wr_addr,(uint8_t*)&p_frame_info->pdata[2],p_frame_info->data_len-2);
        g_ef_boot.recv_block_num = cur_recv_block;
        update_trans_ack.sta = upadte_sta_ok;
        update_trans_ack.recv_cur_block = cur_recv_block;
    }
    else
    {
        update_trans_ack.sta = upadte_sta_cur_block_is_been_write;
        update_trans_ack.recv_cur_block = cur_recv_block;
    }
    
    //if(cur_recv_block>0){update_obj.is_erase_flash=0;}
 
     fsm_send_event(LOADER_FSM_ID,EV_recv_update_dat,NULL,0);

	protocol_send_ack(p_frame_info,(uint8_t*)&update_trans_ack,sizeof(update_trans_ack_t));
    return 0;
}

static int32_t         fw_update_check       (hal_frame_info_t* p_frame_info)
{
    int8_t update_ack                 = upadte_sta_ok;
    uint32_t 						calc_checksum = 0;
    uint8_t* pbuf =    NULL;
    part_item_t* p_app_part=NULL;
    p_app_part = (part_item_t*)get_flash_item_by_part_id(SYS_APP_PART);
    pbuf =  (uint8_t*)(p_app_part->part_address);
    
    DBG_I("check:addr :0x%x len:%d",g_ef_boot.boot_cfg.fw_addr,g_ef_boot.fw_size);
    calc_checksum = calc_chk_val(0,pbuf,g_ef_boot.fw_size);

    if(calc_checksum!=g_ef_boot.fw_sum_chk_val)
    {
        DBG_E("err chk:0x%x   recv_chk:0x%x",calc_checksum,g_ef_boot.fw_sum_chk_val);
        update_ack = upadte_sta_check_err;
		fsm_send_event(LOADER_FSM_ID,EV_recv_update_err,NULL,0);
		
    }
    else
    {
        DBG_E("update success!!");
		fsm_send_event(LOADER_FSM_ID,EV_update_success,NULL,0);
    }
    
    protocol_send_ack(p_frame_info,(uint8_t*)&update_ack,sizeof(int8_t));
    return 0;
}


//=====================================================
//int32_t load_boot(uint8_t argc, uint8_t *argv[])
//{
//    boot_cfg_t  cur_boot_cfg={0};
//    load_boot_cfg(&cur_boot_cfg);
//    show_boot_cfg(&cur_boot_cfg);
//    return RET_OK;
//}

//REG_SHELL_CMD(load_boot, 0, load_boot, "load_boot", "load_boot.  eg load_boot \r\n ");

//int32_t save_boot(uint8_t argc, uint8_t *argv[])
//{
//    save_boot_cfg(&g_ef_boot.boot_cfg);
//    return RET_OK;
//}

//REG_SHELL_CMD(save_boot, 0, save_boot, "save_boot", "save_boot.  eg save_boot \r\n ");


//int32_t show_boot(uint8_t argc, uint8_t *argv[])
//{
//    show_boot_cfg(&g_ef_boot.boot_cfg);
//    return RET_OK;
//}

//REG_SHELL_CMD(show_boot, 0, show_boot, "show_boot", "show_boot.  eg show_boot \r\n ");

//int32_t reset_boot(uint8_t argc, uint8_t *argv[])
//{
//    reset_boot_cfg(&g_ef_boot.boot_cfg);
//    return RET_OK;
//}

//REG_SHELL_CMD(reset_boot, 0, reset_boot, "reset_boot", "reset_boot.  eg reset_boot \r\n ");

#endif 
