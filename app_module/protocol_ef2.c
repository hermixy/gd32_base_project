#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
//#include "debug.h"
#include "x_frame.h"


static uint16_t 	v1_get_header_len		(uint8_t *pbuf,uint32_t len);
static uint16_t 	v1_get_data_len			(uint8_t *pbuf,uint32_t len);
static uint8_t 	    v1_check_header			(uint8_t *pbuf,uint32_t len);
static uint8_t 	    v1_check_data			(uint8_t *pbuf,uint32_t len);
static uint16_t     v1_pack_data			(uint8_t *p_raw_buf, hal_frame_info_t *p_frame_info);
static uint16_t     v1_unpack_data			(uint8_t *p_raw_buf, hal_frame_info_t *p_frame_info);

REG_PROTOCOL(   PROTOCOL_EF02,
                v1_get_header_len,
                v1_get_data_len,
                v1_check_header,
                v1_check_data,
                v1_pack_data,
                v1_unpack_data
            ) ;

#pragma pack(1)
typedef struct
{
    uint8_t 	sof;                 //1 byte
    uint8_t 	len;                 //1 byte
    uint8_t		version		    : 4; //1 byte
    uint8_t 	resv		    : 4;
    uint8_t		need_ack		: 1; //1 byte
    uint8_t		is_ack			: 1;
    uint8_t		check_type		: 2;
    uint8_t		enc_type		: 3;
    uint8_t		seq_type		: 1;
    uint8_t   	seq;                 //1 byte
    uint8_t		src;                 //1 byte
    uint8_t		dest;                //1 byte
    uint8_t 	cmdfunc;             //1 byte
    uint8_t 	cmdid;               //1 byte
    uint8_t 	pdata[1];            //1 byte
} v1_data_struct_t;
#pragma pack()


#define v1_HEAD_SOF		(0xAA)
#define v1_VERSION		(1)
#define v1_HEAD_HEN		(sizeof(v1_data_struct_t)-1)  //9+1 crc8
#define v1_NONE_CHECK   (0xBB)



#define NON_CHECK_TYPE  0x00
#define SUM_CHECK_TYPE  0x01
#define CRC_CHECK_TYPE  0x02

#define NON_ENC_TYPE  0x00
#define XOR_ENC_TYPE  0x01

#define CHECK_TYPE     SUM_CHECK_TYPE
#define ENC_TYPE       NON_ENC_TYPE


static uint16_t v1_get_header_len(uint8_t *pbuf,uint32_t len)
{
    return v1_HEAD_HEN + 1;
}

static uint16_t v1_get_data_len(uint8_t *pbuf,uint32_t len)
{
    v1_data_struct_t *p_v1_pack = (v1_data_struct_t *)pbuf;
    uint16_t all_pack_len = 0;
    all_pack_len = v1_HEAD_HEN + p_v1_pack->len + 1;
    return all_pack_len;
}

static uint8_t v1_check_header(uint8_t *pbuf,uint32_t len)
{
    v1_data_struct_t *p_v1_pack = (v1_data_struct_t *)pbuf;
    if( (p_v1_pack->sof == v1_HEAD_SOF) && (p_v1_pack->version == v1_VERSION))
    {
        return 1;
    }
    return 0;
}

static uint8_t v1_check_data(uint8_t *pbuf,uint32_t len)
{
    v1_data_struct_t *p_v1_pack = (v1_data_struct_t *)pbuf;
    uint8_t  *pbuf_check = NULL;

    pbuf_check = &pbuf[v1_HEAD_HEN + p_v1_pack->len];
	
	
	//-------------------enc
	if(XOR_ENC_TYPE == p_v1_pack->enc_type)
	{
		uint8_t i = 0;
		for(i=0;i<p_v1_pack->len;i++)
		{
			p_v1_pack->pdata[i] = p_v1_pack->pdata[i]^p_v1_pack->seq;
		}
	}

	//-------------chk
    if(p_v1_pack->check_type == NON_CHECK_TYPE)
    {
        if(pbuf_check[0] == v1_NONE_CHECK)
        {
            return 1;
        }
    }
    else if(p_v1_pack->check_type == SUM_CHECK_TYPE)
    {
        uint16_t i = 0;
        uint8_t calc_result = 0;
        
        for(i=0;i<(v1_HEAD_HEN+p_v1_pack->len);i++)
        {
            calc_result+=pbuf[i];
        }
        
        if(pbuf_check[0] == calc_result)
        {
            return 1;
        }
    }
    else
    {
        return 0;
    }


    return 0;
}

static uint16_t  v1_pack_data	(uint8_t *p_raw_buf, hal_frame_info_t *p_frame_info)
{
    uint8_t *pcheck_buf = NULL;
    uint16_t pack_len = 0;
    v1_data_struct_t *p_v1_pack     = (v1_data_struct_t *)p_raw_buf;

    p_v1_pack->sof				    = v1_HEAD_SOF;
    p_v1_pack->need_ack		        = (p_frame_info->need_ack > 0) 	? 1 : 0;
    p_v1_pack->is_ack			    = (p_frame_info->is_ack > 0)	? 1 : 0;
    p_v1_pack->check_type		    = CHECK_TYPE;
    //p_v1_pack->enc_type		        = p_frame_info->enc_type	& (0x07);
	p_v1_pack->enc_type		        = ENC_TYPE	& (0x07);
    p_v1_pack->version			    = v1_VERSION;
    p_v1_pack->resv			        = 0;

    p_v1_pack->len				    = p_frame_info->data_len;
    //------------------data
    p_v1_pack->seq					= p_frame_info->seq         & 0xff;
	
    p_v1_pack->cmdid				= p_frame_info->cmd_id      & 0xff;
    p_v1_pack->cmdfunc				= p_frame_info->cmd_func    & 0xff;
    p_v1_pack->src					= p_frame_info->src         & 0xff;
    p_v1_pack->dest					= p_frame_info->dest        & 0xff;
	
	//show_buf(p_raw_buf,10);
    memcpy(p_v1_pack->pdata, p_frame_info->pdata,  p_frame_info->data_len);
	
	//-------------------enc
	if(XOR_ENC_TYPE == p_v1_pack->enc_type)
	{
		uint8_t i = 0;
		for(i=0;i<p_v1_pack->len;i++)
		{
			p_v1_pack->pdata[i] = p_v1_pack->pdata[i]^p_v1_pack->seq;
		}
	}

    //----------------check
    pack_len = v1_HEAD_HEN + p_v1_pack->len + 1;
    pcheck_buf = &p_raw_buf[pack_len - 1];
    if(p_v1_pack->check_type == NON_CHECK_TYPE)
    {
        pcheck_buf[0] = v1_NONE_CHECK;
    }
    else  //only support sum chk now
    {
        uint16_t i = 0;
        uint8_t calc_result = 0;
        
        for(i=0;i<(v1_HEAD_HEN+p_v1_pack->len);i++)
        {
            calc_result+=p_raw_buf[i];
        }
        
        pcheck_buf[0] = calc_result;
    }
    
//    if(p_frame_info->cmd_func == 0x08 ||
//       p_frame_info->cmd_func == REC_CMDSET_REC 
//      )
//    {
//        DBG_I("cmd_func:0x%x id:0x%x",p_frame_info->cmd_func,p_frame_info->cmd_id);
//        show_buf(p_raw_buf,pack_len);
//    }

    return pack_len;
}


static uint16_t  v1_unpack_data	(uint8_t *p_raw_buf, hal_frame_info_t *p_frame_info)
{
    v1_data_struct_t *p_v1_pack = (v1_data_struct_t *)p_raw_buf;

    //p_frame_info->link_id 			= 0; /*ÓÉÉÏ²ãÈ¥Ìî³ä*/
    p_frame_info->enc_type			= p_v1_pack->enc_type;
    p_frame_info->check_type		= p_v1_pack->check_type;
    p_frame_info->cmd_id			= p_v1_pack->cmdid;
    p_frame_info->data_len			= p_v1_pack->len;

    p_frame_info->is_ack			= p_v1_pack->is_ack;
    p_frame_info->need_ack			= p_v1_pack->need_ack;

    p_frame_info->src				= p_v1_pack->src;
    p_frame_info->dest				= p_v1_pack->dest;
    p_frame_info->cmd_func			= p_v1_pack->cmdfunc;
    p_frame_info->seq				= p_v1_pack->seq;
    p_frame_info->pdata				= p_v1_pack->pdata;

    return 1;
}
