#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "bsp_i2c_hw.h"

//----------------------------------xyh
#define I2C0_SLAVE_ADDRESS7    0x82
#define I2C1_SLAVE_ADDRESS7    0x72

typedef struct
{
	uint32_t 			i2c_num;		//I2C0
	uint32_t			port;			//GPIOB
	uint32_t			pin_scl;		//GPIO_PIN_6
	uint32_t			pin_sda;		//GPIO_PIN_7
	rcu_periph_enum		port_clock;		//RCU_GPIOB
	rcu_periph_enum		i2c_clock;		//RCU_I2C0
	
	uint32_t 			i2c_is_remap;			//GPIO_AF_1
	uint32_t 			i2c_speed;		//100000
	uint32_t			i2c_addr;		//I2C0_SLAVE_ADDRESS7
	
}i2c_item_t;

#define ADD_I2C_ITEM(i2c_num, port, pin_scl, pin_sda, i2c_is_remap, i2c_speed, i2c_addr)	\
					{							\
						i2c_num,				\
						port,					\
						GPIO_PIN_##pin_scl,		\
						GPIO_PIN_##pin_sda,		\
						RCU_##port,				\
						RCU_##i2c_num,			\
						i2c_is_remap,			\
						i2c_speed,				\
						i2c_addr,				\
					}
						



#define  I2C_CFG  g_i2c_cfg_tab[ch]

static i2c_item_t g_i2c_cfg_tab[]=
{
			// i2c_x,  port, pin_scl,pin_sda,	af,			speed,       addr
	ADD_I2C_ITEM(I2C0,	GPIOB,	6 ,	7 ,	0,	100000, I2C0_SLAVE_ADDRESS7),
	//ADD_I2C_ITEM(I2C0,	GPIOB,	8 ,	9 ,	1,	100000, I2C0_SLAVE_ADDRESS7),
	//ADD_I2C_ITEM(I2C1,	GPIOB,	10,	11,	0,	100000, I2C1_SLAVE_ADDRESS7),
	
};

typedef struct
{
	uint32_t 		err_code;
	const char*   	err_info;
}i2c_err_tab_t;


const i2c_err_tab_t i2c_err_tab[]=
{
	{I2C_FLAG_I2CBSY,							"I2C bus busy!"},
	{I2C_FLAG_SBSEND,							"I2C start is not ACK!"},
	{I2C_FLAG_ADDSEND,							"I2C send addr is not ACK!"},
	{I2C_FLAG_BTC,								"I2C send byte is not ACK!"},
	{I2C_FLAG_RBNE,								"I2C recv byte time_out!"},
};

const char* find_err_msg(uint32_t msg_code)
{
	const char* not_find_msg = "not_find_msg!!";
	uint8_t i = 0;
	for(i=0;i<ARRY_ITEMS_NUM(i2c_err_tab);i++)
	{
		if(msg_code == i2c_err_tab[i].err_code)
		{
			return i2c_err_tab[i].err_info;
		}
	}
	return not_find_msg;
}


void I2C_hw_init(uint8_t ch)
{
	//-------1.------------------clock config
	/* enable GPIOB clock */
    rcu_periph_clock_enable(I2C_CFG.port_clock);
    /* enable I2C0 clock */
    rcu_periph_clock_enable(I2C_CFG.i2c_clock);
	
	rcu_periph_clock_enable(RCU_AF);
	//-------2.------------------gpio config
	gpio_init(I2C_CFG.port, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, I2C_CFG.pin_scl);
	gpio_init(I2C_CFG.port, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, I2C_CFG.pin_sda);
	
	if(I2C_CFG.i2c_is_remap == 1)
	{
		if(I2C_CFG.i2c_num == I2C0)
		{
			gpio_pin_remap_config(GPIO_I2C0_REMAP, ENABLE);
		}
	}
	
	
	//------3.------------------i2c config
	/* configure I2C0 clock */
    i2c_clock_config(I2C_CFG.i2c_num, I2C_CFG.i2c_speed, I2C_DTCY_2);
    /* configure I2C0 address */
    i2c_mode_addr_config(I2C_CFG.i2c_num, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C_CFG.i2c_addr);
	
	//i2c_smbus_type_config (I2C0, I2C_SMBUS_HOST);		//smbus
	
    /* enable I2C0 */
    i2c_enable(I2C_CFG.i2c_num);
    /* enable acknowledge */
    i2c_ack_config(I2C_CFG.i2c_num, I2C_ACK_ENABLE);
}

void bsp_i2c_init(void)
{
	I2C_hw_init(0);
	I2C_hw_init(1);
	DBG_I("I2C_hw_init(0)");
}
//REG_INIT_MOD(bsp_i2c_init);


#define CHK_TIMEOUT(flag)   time_out_cnt = 1000; 								\
							while(!i2c_flag_get(I2C_CFG.i2c_num,flag))			\
							{   												\
								time_out_cnt--;									\
								if(time_out_cnt==0)								\
								{												\
								 err_flag = flag;								\
								 goto err_end;									\
								}												\
							}
							
#define BUSY_CHK_TIMEOUT(flag)   time_out_cnt = 1000; 							\
							while(i2c_flag_get(I2C_CFG.i2c_num,flag))			\
							{   												\
								time_out_cnt--;									\
								if(time_out_cnt==0)								\
								{												\
								 err_flag = flag;								\
								 goto err_end;									\
								}												\
							}

uint8_t bsp_I2C_read(uint8_t ch,uint8_t addr,uint8_t reg,uint8_t* pbuf, uint8_t len)
{
	static uint32_t read_err_cnt = 0;	
	//---------------------------xyh
	uint32_t err_flag = 0;
	uint32_t time_out_cnt = 0;
	uint8_t i = 0;
	(void)err_flag;
	
	if(ch>=ARRY_ITEMS_NUM(g_i2c_cfg_tab)){return 1;}
	
	/* wait until I2C bus is idle */
	//while(i2c_flag_get(I2C_CFG.i2c_num, I2C_FLAG_I2CBSY));								//等待I2C总线空闲：当总线空闲: SDA和SCL都处于高电平状态
	BUSY_CHK_TIMEOUT(I2C_FLAG_I2CBSY);
	
	/* send a start condition to I2C bus */
	i2c_start_on_bus(I2C_CFG.i2c_num);													//在I2C总线上生成起始位								ok

	/* wait until SBSEND bit is set */
	//while(!i2c_flag_get(I2C_CFG.i2c_num, I2C_FLAG_SBSEND));							//等待起始位发送完成								ok
	CHK_TIMEOUT(I2C_FLAG_SBSEND);
	
	/* send slave address to I2C bus*/
	i2c_master_addressing(I2C_CFG.i2c_num, addr, I2C_TRANSMITTER);						//主机模式下 发送从机地址到I2C总线上（器件地址）	ok

	/* wait until ADDSEND bit is set*/
	//while(!i2c_flag_get(I2C_CFG.i2c_num, I2C_FLAG_ADDSEND));							//等待地址发送完成									ok
	CHK_TIMEOUT(I2C_FLAG_ADDSEND);
	
	/* clear ADDSEND bit */
	i2c_flag_clear(I2C_CFG.i2c_num, I2C_FLAG_ADDSEND);									//清除地址发送完成标志								ok
	
	i2c_data_transmit(I2C_CFG.i2c_num, reg);											//发送寄存器地址									ok
	
	/* wait until the transmission data register is empty*/
	//while(!i2c_flag_get(I2C_CFG.i2c_num, I2C_FLAG_BTC));								//等待字节传输完成									ok
	CHK_TIMEOUT(I2C_FLAG_BTC);
	//---------------------
	
	/* send a start condition to I2C bus */
	i2c_start_on_bus(I2C_CFG.i2c_num);													//在I2C总线上生成起始位 restart					

	/* wait until SBSEND bit is set */
	//while(!i2c_flag_get(I2C_CFG.i2c_num, I2C_FLAG_SBSEND));							//等待起始位发送完成								
	CHK_TIMEOUT(I2C_FLAG_SBSEND);
	/* send slave address to I2C bus*/
	//i2c_master_addressing(I2C0, addr, I2C_TRANSMITTER);						//作为主机发送器：发送设备地址写操作		//设置为写操作成功
	i2c_master_addressing(I2C_CFG.i2c_num, addr, I2C_RECEIVER);						//作为主机接收器：发送设备地址读操作		//设置为读操作失败
	
	/* wait until ADDSEND bit is set*/
	//while(!i2c_flag_get(I2C_CFG.i2c_num, I2C_FLAG_ADDSEND));							//等待设置成功
	CHK_TIMEOUT(I2C_FLAG_ADDSEND);
	/* clear ADDSEND bit */
	i2c_flag_clear(I2C_CFG.i2c_num, I2C_FLAG_ADDSEND);									//清除地址发送完成标志
	
	for(i=0;i<(len-1);i++)
	{
		i2c_ack_config(I2C_CFG.i2c_num, I2C_ACK_ENABLE);
		//while(!i2c_flag_get(I2C_CFG.i2c_num, I2C_FLAG_RBNE));
		CHK_TIMEOUT(I2C_FLAG_RBNE);
		pbuf[i] = i2c_data_receive(I2C_CFG.i2c_num);
	}
	
	//---------------------
	
	//设置ACK失能和停止位产生
	i2c_ack_config(I2C_CFG.i2c_num, I2C_ACK_DISABLE);									//最后一个字节关闭应答，非常重要，否则会一直接收数据
	i2c_stop_on_bus(I2C_CFG.i2c_num);													//产生停止信号										ok
	
	//while(!i2c_flag_get(I2C_CFG.i2c_num, I2C_FLAG_RBNE));
	CHK_TIMEOUT(I2C_FLAG_RBNE);
	pbuf[i] = i2c_data_receive(I2C_CFG.i2c_num);										//接收最后一个字节
	
	i2c_ack_config(I2C_CFG.i2c_num, I2C_ACK_ENABLE);
	
	return 0;
	
	err_end:
	read_err_cnt++;
	DBG_E_LIMIT(200, "IIC[%d] read err!  code:0x%x  addr:0x%x err_msg:%s read_err_cnt:%d",ch,err_flag,addr,find_err_msg(err_flag),read_err_cnt);
	
	/* send a stop condition to I2C bus*/
	i2c_stop_on_bus(I2C_CFG.i2c_num);
	I2C_hw_init(ch);
	//DBG_E("IIC[%d] again init---------");
	return 1;
}

uint8_t  bsp_I2C_write(uint8_t ch,uint8_t addr,uint8_t reg,uint8_t* pbuf, uint8_t len)
{
	static uint32_t write_err_cnt = 0;
	//---------------------------xyh
	uint32_t err_flag = 0;
	uint32_t time_out_cnt = 0;
	uint8_t i = 0;
	(void)err_flag;
	
	if(ch>=ARRY_ITEMS_NUM(g_i2c_cfg_tab)){return 1;}
	
	/* wait until I2C bus is idle */
    //while(i2c_flag_get(I2C_CFG.i2c_num, I2C_FLAG_I2CBSY));								//等待I2C总线空闲
	BUSY_CHK_TIMEOUT(I2C_FLAG_I2CBSY);
	
	/* send a start condition to I2C bus */
    i2c_start_on_bus(I2C_CFG.i2c_num);													//在I2C总线上生成起始位
	
	/* wait until SBSEND bit is set */
    //while(!i2c_flag_get(I2C_CFG.i2c_num, I2C_FLAG_SBSEND));							//等待起始位发送完成
	CHK_TIMEOUT(I2C_FLAG_SBSEND);
	
	/* send slave address to I2C bus*/
    i2c_master_addressing(I2C_CFG.i2c_num, addr, I2C_TRANSMITTER);						//主机模式下 发送从机地址到I2C总线上（器件地址）
	
	/* wait until ADDSEND bit is set*/
    //while(!i2c_flag_get(I2C_CFG.i2c_num, I2C_FLAG_ADDSEND));							//等待地址发送完成
	CHK_TIMEOUT(I2C_FLAG_ADDSEND);
	
	/* clear ADDSEND bit */
    i2c_flag_clear(I2C_CFG.i2c_num, I2C_FLAG_ADDSEND);									//清除地址发送完成标志
	
	i2c_data_transmit(I2C_CFG.i2c_num, reg);											//发送寄存器地址
	
	/* wait until the transmission data register is empty*/
    //while(!i2c_flag_get(I2C_CFG.i2c_num, I2C_FLAG_BTC));								//等待字节传输完成
	CHK_TIMEOUT(I2C_FLAG_BTC);
	
	for(i=0;i<len;i++)
	{
		i2c_data_transmit(I2C_CFG.i2c_num, pbuf[i]);									//发送寄存器地址
		/* wait until the transmission data register is empty*/
		//while(!i2c_flag_get(I2C_CFG.i2c_num, I2C_FLAG_BTC));							//等待字节传输完成
		CHK_TIMEOUT(I2C_FLAG_BTC);
	}
	
	/* send a stop condition to I2C bus*/
	i2c_stop_on_bus(I2C_CFG.i2c_num);
	
	return 0;
	
	err_end:
	write_err_cnt++;
	DBG_E_LIMIT(200, "IIC[%d] write err!  code:0x%x  addr:0x%x err_msg:%s write_err_cnt:%d",ch,err_flag,addr,find_err_msg(err_flag),write_err_cnt);
	
	/* send a stop condition to I2C bus*/
	i2c_stop_on_bus(I2C_CFG.i2c_num);
	I2C_hw_init(ch);
	//DBG_E("IIC[%d] again init---------");
	return 1;
}

uint8_t I2C_read(uint8_t ch,uint8_t addr,uint8_t reg,uint8_t* pbuf, uint8_t len)
{
	uint8_t retry_cnt = 0;
	uint8_t err_flg = 0;
	while(retry_cnt<3)
	{
		err_flg = bsp_I2C_read(ch,addr,reg,pbuf,len);
		if(err_flg == 0)
		{
			//if(retry_cnt>0){DBG_E("iic_r_confirm err[%d]:0x%x reg:0x%x  cnt:%d",ch,addr,reg,retry_cnt);}	
		  return 0;
		}
		retry_cnt++;
	}
	DBG_E_LIMIT(500,"iic_r_confirm err[%d]:0x%x reg:0x%x ",ch,addr,reg);
	return 1;
}

uint8_t I2C_write(uint8_t ch,uint8_t addr,uint8_t reg,uint8_t* pbuf, uint8_t len)
{
	uint8_t retry_cnt = 0;
	uint8_t err_flg = 0;
	while(retry_cnt<3)
	{
		err_flg = bsp_I2C_write(ch,addr,reg,pbuf,len);
		if(err_flg == 0)
		{
			//if(retry_cnt>0){DBG_E("iic_w_confirm err[%d]:0x%x reg:0x%x  cnt:%d",ch,addr,reg,retry_cnt);}	
			return 0;
		}
		retry_cnt++;
	}
	DBG_E_LIMIT(500,"iic_w_confirm err[%d]:0x%x reg:0x%x",ch,addr,reg);
	return 1;
}

uint8_t I2C_write_reg(uint8_t ch,uint8_t addr,uint8_t reg,uint8_t reg_val)
{
	uint8_t wr_reg_val = reg_val;
	return I2C_write(ch,addr,reg,&wr_reg_val,1);
}

#if 1

//void iic_debug_init(void)
//{
//	I2C_hw_init(0);
//	I2C_hw_init(1);
//}

//REG_INIT_MOD(iic_debug_init);

//void i2c_test(void)
//{
//	static uint8_t cnt = 0;
//	uint8_t buf[20] = {0x12,0x34,0x56,0x78,0x90};
//	uint8_t buf_r[20] = {0};
//	
//	cnt++;
//	cnt = cnt%2;
//	if(cnt==0)
//	{
//		//bsp_I2C_write(0,0xa0,0x00,buf,5);					//ok
//		I2C_write(0,0xa0,0x00,buf,5);
//	}
//	else
//	{
//		//bsp_I2C_read(0,0xa0,0x00,buf_r,5);
//		I2C_read(0,0xa0,0x00,buf_r,5);
//		show_buf(buf_r,5);
//	}
//}
////REG_TASK(1,500,i2c_test);

int32_t i2c_r(uint8_t argc, uint8_t *argv[])
{
	if(argc >= 5)
	{
		uint32_t addr;
		uint32_t ch={0};
		uint32_t cmd = 0;
		uint32_t read_len = 0;
		uint8_t buf_rd[100]={0};

		sscanf((const char*)argv[1],"%d",&ch);
		sscanf((const char*)argv[2],"%x",&addr);
		sscanf((const char*)argv[3],"%x",&cmd);
		sscanf((const char*)argv[4],"%d",&read_len);

		read_len = (read_len<90)?read_len:90;
		I2C_read((uint8_t)ch, (uint8_t)addr, (uint8_t)cmd,buf_rd,(uint8_t)read_len);

		DBG_I("i2c_read-->ch:%d  addr:0x%x  reg:0x%x  len:%d",ch,addr,cmd,read_len);
		show_buf(buf_rd,read_len);
	}
	else
	{
		DBG_W("warn the i2c_r param is error: eg:i2c_r ch addr reg len");
	}
	return RET_OK;
}

REG_SHELL_CMD(i2c_r,0,i2c_r,"i2c_r", "eg:i2c_r ch addr reg len\r\n ");



int32_t i2c_w(uint8_t argc, uint8_t *argv[])
{
	if(argc >= 5)
	{
		uint32_t addr;
		uint32_t ch={0};
		uint32_t cmd = 0;
		uint32_t wr_reg = 0;

		sscanf((const char*)argv[1],"%d",&ch);
		sscanf((const char*)argv[2],"%x",&addr);
		sscanf((const char*)argv[3],"%x",&cmd);
		sscanf((const char*)argv[4],"%x",&wr_reg);
		

		I2C_write((uint8_t)ch, (uint8_t)addr, (uint8_t)cmd,(uint8_t*)&wr_reg,1);

		DBG_I("i2c_write-->ch:0x%x  addr:0x%x  reg:0x%x  dat:0x%x len(fix):%d",ch,addr,cmd,wr_reg,1);
	}
	else
	{
		DBG_W("warn the i2c_w param is error: eg:i2c_w ch addr reg byte");
	}
	return RET_OK;
}

REG_SHELL_CMD(i2c_w,0,i2c_w,"i2c_w", "eg:i2c_w ch addr reg byte\r\n ");
#endif
