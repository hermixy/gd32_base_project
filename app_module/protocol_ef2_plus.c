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

REG_PROTOCOL(   PROTOCOL_EF02_PLUS,
                get_header_len,
                get_data_len,
                check_header,
                check_data,
                pack_data,
                unpack_data
            ) ;


#pragma pack(1)
typedef struct
{
    uint8_t 	sof;                 //1 byte
	  uint8_t		version;
    uint16_t 	len;                 //2 byte
    uint8_t 	head_crc; 

    uint8_t		need_ack		  : 1; //1 byte
    uint8_t		is_ack			  : 1;
    uint8_t		check_type		: 3;
    uint8_t		enc_type		  : 2;
    uint8_t		seq_type		  : 1;
	
    uint32_t  seq;                 //4 byte
	  uint16_t  product;             //2 byte
    uint8_t		src;                 //1 byte
    uint8_t		dest;                //1 byte
    uint8_t 	cmdfunc;             //1 byte
    uint8_t 	cmdid;               //1 byte
    uint8_t 	pdata[1];            //1 byte
} protocol_ef02_plus_t;
#pragma pack()

#define CUR_PACK_TYPE  protocol_ef02_plus_t

#define HEAD_SOF		(0xAA)
#define VERSION			(2)
#define HEAD_LEN		(5)   
#define BODY_LEN		(sizeof(CUR_PACK_TYPE)-1)  //9+2 crc16
#define NONE_CHECK   	(0xBB)

#define NON_CHECK_TYPE   0x00
#define SUM_CHECK_TYPE   0x01
#define CRC8_CHECK_TYPE  0x02
#define CRC16_CHECK_TYPE 0x03

#define NON_ENC_TYPE  0x00
#define XOR_ENC_TYPE  0x01

#define CHECK_TYPE     CRC16_CHECK_TYPE
#define ENC_TYPE       XOR_ENC_TYPE


extern   uint32_t chk_sum(uint8_t* pbuf,uint32_t len);
static uint16_t get_header_len(uint8_t *pbuf,uint32_t len)
{
    return HEAD_LEN + 1;
}

static uint16_t get_data_len(uint8_t *pbuf,uint32_t len)
{
    CUR_PACK_TYPE *p_pack = (CUR_PACK_TYPE *)pbuf;
    uint16_t all_pack_len = 0;
    all_pack_len = BODY_LEN + p_pack->len + 2;
    return all_pack_len;
}

static uint8_t check_header(uint8_t *pbuf,uint32_t len)
{
    CUR_PACK_TYPE *p_pack = (CUR_PACK_TYPE *)pbuf;
    if( (p_pack->sof == HEAD_SOF) && (p_pack->version == VERSION))
    {
			//uint8_t i = 0;
			uint32_t head_chk = 0;
			head_chk = calc_chk_val(CRC8_CHECK_TYPE,pbuf,HEAD_LEN-1);
			
			if(head_chk==p_pack->head_crc) {return 1;}
    }
    return 0;
}

static uint8_t check_data(uint8_t *pbuf,uint32_t len)
{
    CUR_PACK_TYPE *p_pack = (CUR_PACK_TYPE *)pbuf;
    uint8_t  *pbuf_check = NULL;
	uint32_t  all_pack_len;
	all_pack_len = BODY_LEN + p_pack->len + 2;
    pbuf_check = &pbuf[all_pack_len-2];

	//-------------------enc
	if(XOR_ENC_TYPE == p_pack->enc_type)
	{
		uint32_t i = 0;
		for(i=0;i<p_pack->len;i++)
		{
			p_pack->pdata[i] = p_pack->pdata[i]^p_pack->seq;
		}
	}

	//-------------chk
	{
		uint32_t frame_chk = 0;
		uint16_t* p_chk = (uint16_t*)pbuf_check;
		frame_chk = calc_chk_val(p_pack->check_type,pbuf,all_pack_len-2); 
		if(frame_chk == (*p_chk))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
  
}


static uint16_t  pack_data	(uint8_t *p_raw_buf, hal_frame_info_t *p_frame_info)
{
    uint8_t *pcheck_buf = NULL;
    uint16_t pack_len = 0;
		uint32_t crc16_val = 0;
    CUR_PACK_TYPE *p_pack     = (CUR_PACK_TYPE *)p_raw_buf;

    p_pack->sof				    = HEAD_SOF;
	  p_pack->version			    = VERSION;
	  p_pack->len				    = p_frame_info->data_len;
	  p_pack->head_crc			= calc_chk_val(CRC8_CHECK_TYPE,p_raw_buf,HEAD_LEN-1)&0xff;
	
    p_pack->need_ack		    = (p_frame_info->need_ack > 0) ? 1 : 0;
    p_pack->is_ack			    = (p_frame_info->is_ack > 0)	? 1 : 0;
    p_pack->check_type		    = CHECK_TYPE;
		p_pack->enc_type		    = ENC_TYPE	& (0x07);
	
	
		p_pack->product         = 0x1234;

    //------------------data
    p_pack->seq					= p_frame_info->seq       ;
    p_pack->cmdid				= p_frame_info->cmd_id      & 0xff ;
    p_pack->cmdfunc				= p_frame_info->cmd_func    & 0xff;
    p_pack->src					= p_frame_info->src         & 0xff;
    p_pack->dest				= p_frame_info->dest        & 0xff;

    memcpy(p_pack->pdata, p_frame_info->pdata,  p_frame_info->data_len);
	
	//-------------------enc
	if(XOR_ENC_TYPE == p_pack->enc_type)
	{
		uint32_t i = 0;
		for(i=0;i<p_pack->len;i++)
		{
			p_pack->pdata[i] = p_pack->pdata[i]^p_pack->seq;
		}
	}

	  pack_len = BODY_LEN + p_pack->len + 2;
    pcheck_buf = &p_raw_buf[pack_len - 2];
    //----------------check
    crc16_val = calc_chk_val(p_pack->check_type,p_raw_buf,pack_len - 2); 
		pcheck_buf[0] = crc16_val &0xff;
		pcheck_buf[1] = (crc16_val >>8)&0xff;
    return pack_len;
}


static uint16_t  unpack_data	(uint8_t *p_raw_buf, hal_frame_info_t *p_frame_info)
{
    CUR_PACK_TYPE *p_pack = (CUR_PACK_TYPE *)p_raw_buf;

    p_frame_info->enc_type			= p_pack->enc_type;
    p_frame_info->check_type		= p_pack->check_type;
    p_frame_info->cmd_id			= p_pack->cmdid;
    p_frame_info->data_len			= p_pack->len;

    p_frame_info->is_ack			= p_pack->is_ack;
    p_frame_info->need_ack			= p_pack->need_ack;

    p_frame_info->src				= p_pack->src;
    p_frame_info->dest				= p_pack->dest;
    p_frame_info->cmd_func			= p_pack->cmdfunc;
    p_frame_info->seq				= p_pack->seq;
    p_frame_info->pdata				= p_pack->pdata;

    return 1;
}
