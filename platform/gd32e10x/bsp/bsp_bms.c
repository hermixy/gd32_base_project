#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "bsp_i2c_hw.h"
#include "bsp_bms.h"

#define BMS_I2C_CH  	0
#define BMS_I2C_ADDR	0x10		//0x08左移一位后的7位地址

#define SMBUS_INIT 					I2C_hw_init(BMS_I2C_CH)
#define SMBUS_RD(reg,pbuf,len) 		I2C_read(BMS_I2C_CH,BMS_I2C_ADDR,reg,pbuf,len);
#define SMBUS_WR(reg,pbuf,len)  	I2C_write(BMS_I2C_CH,BMS_I2C_ADDR,reg,pbuf,len);

void bsp_bms_init(void)
{
    SMBUS_INIT;
	//I2C_hw_init(0);
	//I2C_hw_init(1);
}


/* 原始函数 */
//unsigned char CRC8(unsigned char *ptr, unsigned char len,unsigned char key)
//{
//	unsigned char i;
//	unsigned char crc=0;
//	while(len--!=0)
//	{
//		for(i=0x80; i!=0; i/=2)
//		{
//			if((crc & 0x80) != 0)
//			{
//				crc *= 2;
//				crc ^= key;
//			}
//			else
//				crc *= 2;

//			if((*ptr & i)!=0)
//				crc ^= key;
//		}
//		ptr++;
//	}
//	return(crc);
//}

enum
{
	CRC8_OK		= 0 ,
	CRC8_ERR		,
};

#define CRC8_POLY	0x07
#define CRC8_INIT	0x00
unsigned char bms_crc8(unsigned char *ptr, unsigned char len)
{
	unsigned char i;
	unsigned char crc=CRC8_INIT;
	while(len--!=0)
	{
		for(i=0x80; i!=0; i/=2)
		{
			if((crc & 0x80) != 0)
			{
				crc *= 2;
				crc ^= CRC8_POLY;
			}
			else
				crc *= 2;

			if((*ptr & i)!=0)
				crc ^= CRC8_POLY;
		}
		ptr++;
	}
	return(crc);
}

//------------bms read

uint8_t bms_read_first_byte_crc8(uint8_t *ptr)	//bq76940 first data crc8
{
	const uint8_t len = 2;
	uint8_t buf[3] = {0};		//临时存储区
	uint8_t res = 0;
	uint8_t i = 0;
	
	buf[0] = BMS_I2C_ADDR + 1;	//read addr
	
	for(i = 1;i < len+1;i++)
	{
		buf[i] = ptr[i-1];		//data + crc
	}
	//show_buf(buf,2);
	
	res = bms_crc8(buf,len);	//addr + data --> crc
	
	if(res == buf[len])
	{
		return CRC8_OK;
	}
	else
	{
		return CRC8_ERR;
	}
}

uint8_t smbus_read_with_crc(uint8_t reg,uint8_t* pbuf, uint8_t len)
{
	uint8_t buf_r[100] = {0};		//临时存储区
	uint8_t real_len = 0;			
	uint8_t ok_cnt = 0;
	uint8_t i = 0;
	
	if(len > 50)
	{
		DBG_E("smbus_read len > 50 error");
		return 0xff;
	}
	
	real_len = len*2;					//len:实际数据长度		real_len：带crc8数据长度
	
	SMBUS_RD(reg,buf_r,real_len);
	//show_buf(buf_r,real_len);
	
	//-------------------------crc8 calc
	//------first byte crc calc: addr+dat1
	if( bms_read_first_byte_crc8(buf_r) )		//CRC8_ERR
	{
		DBG_E_LIMIT(200, "bms_first_byte_crc8 err");
		memset(&pbuf, 0, real_len);
		return CRC8_ERR;
	}
	else
	{
		ok_cnt += 1;
	}
	
	//------remain data crc calc: dat_n
	for(i = 1;i < len;i++)
	{
		if( buf_r[i*2+1] == bms_crc8(&buf_r[i*2],1) )
		{
			ok_cnt += 1;
		}
		else
		{
			DBG_E("bms_crc8 err  i = %d,data = %d",i,buf_r[i]);
			return CRC8_ERR;
		}
	}
	//DBG_I("ok_cnt = %d",ok_cnt);
	
	//------拷贝实际数据（丢掉crc8）
	if(ok_cnt == len)						//all data crc8 check ok
	{
		for(i = 0;i < len;i++)
		{
			pbuf[i] = buf_r[i*2];			
		}
		//show_buf(pbuf, len);
	}
	else
	{
		DBG_E("bms_crc8 err  ok_cnt = %d,len = %d",ok_cnt,len);	//不会执行到此处
	}
	
	return CRC8_OK;
}

//-----------bms write

uint8_t smbus_write_with_crc(uint8_t reg,uint8_t* pbuf, uint8_t len)
{
	uint8_t buf_w[102] = {0};		//临时存储区
	uint8_t real_len = 0;			
	//uint8_t ok_cnt = 0;
	uint8_t i = 0;
	uint8_t ret = 0xff;
	
	if(len > 50)
	{
		DBG_E("smbus_write len > 50 error");
		return 0xff;
	}
	
	real_len = len*2;
	
	/*
	+--0---1---2----3----4----5----6-----7--------2n---2n+1-+
	|ADDR|REG|dat1|crc1|dat2|crc2|dat3|crc3|....|dat_n|crc_n|
	+-------------------------------------------------------+
	*/
	
	//------first byte crc calc: ADDR+REG+dat1
	buf_w[0] = BMS_I2C_ADDR;
	buf_w[1] = reg;
	buf_w[2] = pbuf[0];
	
	buf_w[3] = bms_crc8(buf_w,3);
	
	//------remain data crc calc: dat_n
	for(i = 1;i < len;i++)
	{
		buf_w[i*2+2] = pbuf[i];					//存储dat
		buf_w[i*2+3] = bms_crc8(&pbuf[i],1);	//存储crc
	}
	
	ret = SMBUS_WR(reg,&buf_w[2],real_len);
	
	//show_buf(buf_w,real_len+2);
	
	return ret;
}



//K=273.15+C
int32_t bsp_bms_read		(uint8_t *p_dest, uint8_t len)
{
	return 0;
}






#if TINY_FUNC_CODE == 0

int32_t smbus_r(uint8_t argc, uint8_t *argv[])
{
	if(argc == 3)
	{
		uint32_t reg = 0;
		uint32_t read_len = 0;
		uint8_t buf_rd[100]={0};

		sscanf((const char*)argv[1],"%x",&reg);
		sscanf((const char*)argv[2],"%d",&read_len);

		read_len = (read_len<90)?read_len:90;
		smbus_read_with_crc((uint8_t)reg, buf_rd, (uint8_t)read_len);

		DBG_I("smbus_r-->reg:0x%x  len:%d",reg,read_len);
		show_buf(buf_rd,read_len);
	}
	else
	{
		DBG_W("warn the smbus_r param is error: eg:smbus_r reg len");
	}
	return RET_OK;
}
REG_SHELL_CMD(smbus_r, 0, smbus_r, "smbus_r", "smbus_r reg len (only bq76940 with crc)\r\n ");

int32_t smbus_w(uint8_t argc, uint8_t *argv[])
{
	if(argc == 3)
	{
		uint32_t reg = 0;
		uint32_t dat = 0;

		sscanf((const char*)argv[1],"%x",&reg);
		sscanf((const char*)argv[2],"%x",&dat);
		
		smbus_write_with_crc(reg,(uint8_t*)&dat,1);

		DBG_I("smbus_write-->addr:0x10(fix) reg:0x%x  dat:0x%x len(fix):%d\r\n",reg,dat,1);
	}
	else
	{
		DBG_W("warn the smbus_w param is error: eg:smbus_w reg byte\r\n");
	}
	return RET_OK;
}

REG_SHELL_CMD(smbus_w,0,smbus_w,"smbus_w", "eg:smbus_w reg byte(only use bq76940 with crc)\r\n ");

#endif
