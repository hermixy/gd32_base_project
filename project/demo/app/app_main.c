#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
//#include "debug.h"
#include "x_frame.h"

REG_DEBUG(DRV_RTT);
REG_DEBUG(DRV_UART0);
REG_DEBUG(DRV_UART1);
//REG_DEBUG(DRV_UART2);
//REG_DEBUG(DRV_UART3);
//REG_DEBUG(DRV_UART4);

//REG_LINK(PC_LINK_ID,	1, 	DRV_UART0, 	PROTOCOL_EF02_PLUS, 	512,0,0);		//��λ��ͨѶ UART0 	 PROTOCOL_EF02_PLUS    >1024+20

ef_state_t ef_state;

uint8_t enable_dbg_mode = 0;
REG_CFG_ITEM(enable_dbg_mode,  enable_dbg_mode,	eu_uint8,	PARM_RW,	0u,	0u,	1u );

static void feed_iwdg(void)
{
	hal_app_ctrl(DRV_IWDG, RELOAD_IWDT, NULL, 0, NULL);
}
REG_TASK(0, 200, feed_iwdg);


#define LED_TOGGLE_BY_NUM(num)										\
{																	\
	static uint32_t state = 0;										\
	state = !state;													\
	hal_app_write(DRV_GPIO, (uint8_t*)&state, sizeof(uint32_t), num, NULL);		\
}

EXT_REC(TEST_CTRL_DAT);
static void led_toggle(void)
{
	hal_app_ctrl(DRV_GPIO, GPIO_TOGGLE_MODE, NULL, IDX_GPIO_LED, NULL);
	
	RUN_REC(TEST_CTRL_DAT);
}
REG_TASK(0, 500, led_toggle);

void DBG_test_func(void)
{
	DBG_I_LIMIT(1000, "get_sys_s() = %d s", get_sys_s());
}
REG_TASK(3, 1000, DBG_test_func);

static void read_adc(void)
{
	uint32_t rd_ad_dat[MAX_AD_CH];
	hal_app_read(DRV_ADC,(uint8_t*)&rd_ad_dat,sizeof(uint32_t)*MAX_AD_CH,0,NULL);
	
	DBG_I("rd_ad_dat[0]=%d,rd_ad_dat[1]=%d", rd_ad_dat[0],rd_ad_dat[1]);
}
//REG_TASK(0, 500, read_adc);

void uart_protocol_send(void)
{
	const char buf[] = {0x11,0x22,0x33,0x44,0x55};
	hal_frame_info_t frame_info = {0};
	
	frame_info.link_id	= PC_LINK_ID;
	frame_info.cmd_func	= TEST_CMD_SET;
	frame_info.cmd_id	= TEST_CMD_ID;
	frame_info.src		= HOST_ADDR;
	frame_info.dest		= PC_ADDR;
	frame_info.pdata		= (uint8_t*)buf;
	frame_info.data_len	= sizeof(buf);
	
	protocol_send_by_id(&frame_info);
}
REG_TASK(1, 1000, uart_protocol_send);

void debug_func_exec(void)
{
	
}
REG_TASK(3, 1000, debug_func_exec);









//---------------------------FSM test


//--------- normal | sleep | power off
static void power_on_in_func				(fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len);
static uint32_t power_on_condition_func		(fsm_item_t *p_fsm, uint32_t event, uint8_t *pbuf, uint32_t len);

static void power_off_in_func					(fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len);
static uint32_t power_off_condition_func		(fsm_item_t *p_fsm, uint32_t event, uint8_t *pbuf, uint32_t len);


//===========
REG_FSM(FSM_ID_PD_STATE, 10, 0, FSM_POWER_ON_STATE);
REG_FSM_STATE(FSM_ID_PD_STATE, FSM_POWER_ON_STATE,		power_on_in_func,	NULL,	NULL,	power_on_condition_func);
REG_FSM_STATE(FSM_ID_PD_STATE, FSM_POWER_OFF_STATE,		power_off_in_func,	NULL,	NULL,	power_off_condition_func);

static void power_on_in_func				(fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len)
{
	DBG_I("power_on_in_func");
}
static uint32_t power_on_condition_func		(fsm_item_t *p_fsm, uint32_t event, uint8_t *pbuf, uint32_t len)
{
	if(event == EV_POWER_OFF)
	{
		p_fsm->next_state = FSM_POWER_OFF_STATE;
		return 1;
	}
	return 0;
}

static void power_off_in_func				(fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len)
{
	DBG_I("power_off_in_func---");
}
static uint32_t power_off_condition_func	(fsm_item_t *p_fsm, uint32_t event, uint8_t *pbuf, uint32_t len)
{
	if(event == EV_POWER_ON)
	{
		p_fsm->next_state = FSM_POWER_ON_STATE;
		return 1;
	}
	return 0;
}


void demo_test_in(void)
{
	
}
uint8_t demo_start(void)
{
	return 1;
}
void demo_exe(auto_test_item_t* p_item,uint32_t err_code,uint8_t cur_step,uint8_t total)
{
	
}
void demo_test_NG(auto_test_item_t* p_item,uint32_t err_code,uint8_t cur_step,uint8_t total)
{
	
}
void demo_test_OK(auto_test_item_t* p_item,uint32_t err_code,uint8_t cur_step,uint8_t total)
{
	
}

REG_AUTO_TEST_CTRL(	demo_test_in,             //auto_test_ start func
										demo_exe,                 //auto_test_ exe func
										demo_start,				  //auto test start
										demo_test_NG,             //auto test NG
										demo_test_OK              //auto test OK
									);

#define IDX_TEST_BMS_CELL	1
static void cell_test_item_in(void)
{
	
}
static void cell_test_item_exe(void)
{
	
}

REG_AUTO_TEST_ITEM(	cell_test,                //test item name
										IDX_TEST_BMS_CELL,    		//test seq
										cell_test_item_in,        //test_case in_func
										cell_test_item_exe);      //test_case exe_func
