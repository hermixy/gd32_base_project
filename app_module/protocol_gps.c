#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "x_frame.h"


static uint16_t 	get_header_len		(uint8_t *pbuf,uint32_t len);
static uint16_t 	get_data_len		(uint8_t *pbuf,uint32_t len);
static uint8_t 	    check_header		(uint8_t *pbuf,uint32_t len);
static uint8_t 	    check_data			(uint8_t *pbuf,uint32_t len);
static uint16_t     pack_data			(uint8_t *p_raw_buf, hal_frame_info_t *p_frame_info);
static uint16_t     unpack_data			(uint8_t *p_raw_buf, hal_frame_info_t *p_frame_info);

REG_PROTOCOL(   PROTOCOL_GPS,
                get_header_len,
                get_data_len,
                check_header,
                check_data,
                pack_data,
                unpack_data
            ) ;




static uint16_t get_header_len(uint8_t *pbuf,uint32_t len)
{
    return 1;
}

static uint16_t get_data_len(uint8_t *pbuf,uint32_t len)
{
    uint16_t i = 0;
	for(i=0;i<len-1;i++)
	{
		if((pbuf[i]==0x0d) && (pbuf[i+1]==0x0a)) {return i+1+1;}
	}
    return 120;
}

static uint8_t check_header(uint8_t *pbuf,uint32_t len)
{
	if(pbuf[0]=='$'){return 1;}
    return 0;
}

static uint8_t check_data(uint8_t *pbuf,uint32_t len)
{
    uint16_t i = 0;
	uint8_t chk_val = pbuf[1];
	char chk_str[3] = {0};
	
	chk_str[0] = '*';
	for(i=2;i<(len-5);i++)
	{
		chk_val = chk_val^pbuf[i];
	}
	
	sprintf(&chk_str[1],"%02X",chk_val);  // action letter big
	//DBG_I("chk_val:0x%x  [0]:%c [1]:%c [2]:%c",chk_val,pbuf[len-5],pbuf[len-4],pbuf[len-3]);
	//show_buf_ascii(pbuf,80);
	
	if( (chk_str[0]==pbuf[len-5]) && (chk_str[1]==pbuf[len-4]) && (chk_str[2]==pbuf[len-3])  ) {return 1;}

    return 0;
}

static uint16_t  pack_data	(uint8_t *p_raw_buf, hal_frame_info_t *p_frame_info)
{
    

    return 0;
}


static uint16_t  unpack_data	(uint8_t *p_raw_buf, hal_frame_info_t *p_frame_info)
{
	char gps_type_str[6]={0};//$GNRMC
    p_frame_info->cmd_func = CMD_SET_GPS;
	
	sscanf((const char*)p_raw_buf,"%6s",gps_type_str);
	
	     if(strcmp(gps_type_str,"$GNRMC")==0) { p_frame_info->cmd_id = CMD_ID_RMC; }
	else if(strcmp(gps_type_str,"$GPGSV")==0) { p_frame_info->cmd_id = CMD_ID_GSV; }
	else if(strcmp(gps_type_str,"$GNGGA")==0) { p_frame_info->cmd_id = CMD_ID_GGA; }
	else if(strcmp(gps_type_str,"$GNVTG")==0) { p_frame_info->cmd_id = CMD_ID_VTG; }
	else                                      { p_frame_info->cmd_id = CMD_ID_ERR; }
	
	
	p_frame_info->pdata 	= p_raw_buf;//&p_raw_buf[7];
	p_frame_info->data_len  = 0;

    return 1;
}
