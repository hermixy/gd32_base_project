#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "x_frame.h"



static uint16_t 	v2_get_header_len		(uint8_t *pbuf);
static uint16_t 	v2_get_data_len			(uint8_t *pbuf);
static uint8_t 	    v2_check_header			(uint8_t *pbuf);
static uint8_t 	    v2_check_data			(uint8_t *pbuf);
static uint16_t     v2_pack_data			(uint8_t *p_raw_buf, hal_frame_info_t *p_frame_info);
static uint16_t     v2_unpack_data			(uint8_t *p_raw_buf, hal_frame_info_t *p_frame_info);

REG_PROTOCOL(   PROTOCOL_EF01_INV,
                v2_get_header_len,
                v2_get_data_len,
                v2_check_header,
                v2_check_data,
                v2_pack_data,
                v2_unpack_data
            ) ;


#pragma pack(1)
typedef struct
{
    uint8_t 	sof1;                 //1 byte
	uint8_t 	sof2;                 //1 byte
    uint8_t 	cmdid;                //1 byte
    uint16_t 	pdata[1];             //2 byte
} v2_data_struct_t;
#pragma pack()


#define INV_HEAD_SOF1   (0xAB)
#define INV_HEAD_SOF2   (0xBA)
#define v2_HEAD_HEN		(3)
#define v2_DATA_LEN		(10)

#define v2_ID			0x55

//#define CRC_CHECK_TYPE  0x02
//#define CHECK_TYPE      CRC_CHECK_TYPE


static uint16_t v2_get_header_len(uint8_t *pbuf)	//包头的长度
{
    return v2_HEAD_HEN;
}

static uint16_t v2_get_data_len(uint8_t *pbuf)		//包头+数据+校验的长度
{
    return v2_DATA_LEN+v2_HEAD_HEN+2;
}

static uint8_t v2_check_header(uint8_t *pbuf)
{
    v2_data_struct_t *p_v2_pack = (v2_data_struct_t *)pbuf;
	
	if(  (p_v2_pack->sof1 == INV_HEAD_SOF1) && 
	     (p_v2_pack->sof2 == INV_HEAD_SOF2) &&
		 (p_v2_pack->cmdid == v2_ID) 
	   )
	
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}
//多项式是 0xA001 CRC16 初始值 0xFFFF
uint16_t InCrc16Calc(uint8_t *data_arr)
{
	uint16_t crc16 = 0xFFFF;
	uint8_t i,j;
	
	for(i = 0; i < v2_DATA_LEN+v2_HEAD_HEN; i++)
	{
		crc16^=*(data_arr+i);
		for(j=0;j<8;j++)
		{
			if(crc16&0x01)
			{
				crc16>>=1;
				crc16^=0xA001;
			}
			else
			{
				crc16>>=1;
			}
		}
	}
	return crc16;
}


static uint8_t v2_check_data(uint8_t *pbuf)
{
	uint16_t calc_result = 0;
	uint16_t dat = 0;
    //v2_data_struct_t *p_v2_pack = (v2_data_struct_t *)pbuf;
    uint8_t  *pbuf_check = NULL;
    pbuf_check = &pbuf[v2_HEAD_HEN + v2_DATA_LEN];

	calc_result = InCrc16Calc(pbuf);
	//DBG_I("calc_result = 0x%x",calc_result);
	dat = pbuf_check[1];
	dat = (dat<<8) + pbuf_check[0];
	//DBG_I("dat = 0x%x",dat);
	//DBG_I("dat = 0x%x",pbuf_check[1]);
	//DBG_I("dat = 0x%x",pbuf_check[0]);
	if( dat == calc_result)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static uint16_t  v2_pack_data	(uint8_t *p_raw_buf, hal_frame_info_t *p_frame_info)
{
	uint8_t i = 0;
    uint8_t *pcheck_buf = NULL;
    uint16_t pack_len = 0;
    v2_data_struct_t *p_v2_pack     = (v2_data_struct_t *)p_raw_buf;
	
	
    p_v2_pack->sof1				    = INV_HEAD_SOF1;
	p_v2_pack->sof2				    = INV_HEAD_SOF2;


    //------------------data
    p_v2_pack->cmdid				= p_frame_info->cmd_id      & 0xff;
	
    memcpy(p_v2_pack->pdata, p_frame_info->pdata,  p_frame_info->data_len);
	//DBG_I("calc_result = %d",p_frame_info->data_len);
	//小端模式
	for(i = 0;i < p_frame_info->data_len;i++)
	{
		p_v2_pack->pdata[i] = SWAP_U16(p_v2_pack->pdata[i]);
	}
	
    //----------------check
    pack_len = v2_HEAD_HEN + v2_DATA_LEN + 2;
    pcheck_buf = &p_raw_buf[pack_len - 2];

    {
        uint16_t calc_result = 0;
        
		calc_result = InCrc16Calc(p_raw_buf);
		
        //DBG_I("calc_result = 0x%x",calc_result);
        pcheck_buf[1] = (uint8_t)(calc_result>>8);
		pcheck_buf[0] = (uint8_t)calc_result;
    }
    
    return pack_len;
}


static uint16_t  v2_unpack_data	(uint8_t *p_raw_buf, hal_frame_info_t *p_frame_info)
{
    v2_data_struct_t *p_v2_pack = (v2_data_struct_t *)p_raw_buf;

    //p_frame_info->link_id 			= 0; /*由上层去填充*/
    p_frame_info->cmd_id			= p_v2_pack->cmdid;
	p_frame_info->data_len			= v2_DATA_LEN;
    p_frame_info->pdata				= (uint8_t*)p_v2_pack->pdata;

    return 1;
}
