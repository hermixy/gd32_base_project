#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E

#include "flash_part.h"

#if ENABLE_FLASH_PART == 1
static flash_part_core_t* p_flash_part=NULL;

#define FLASH_PART_ERR  0xffffffff

void flash_part_init(void)
{
	uint32_t i = 0;
	
	for(i = 0; i < 100; i++)
    {
        p_flash_part = (flash_part_core_t *)auto_reg_get_func_cont_by_index(AUTO_REG_FLASH_PART, i);
        if(p_flash_part != NULL) 
		{
			p_flash_part->p_part_item[0].part_address = p_flash_part->start_address;
			for(i=1;i<p_flash_part->part_num;i++)
			{
				p_flash_part->p_part_item[i].part_address = p_flash_part->p_part_item[i-1].part_address + p_flash_part->p_part_item[i-1].part_size;
			}
			return;
		}
    }
}

part_item_t* get_flash_item_by_part_id(uint32_t part_id)
{
	uint32_t i = 0;
	
	if(p_flash_part==NULL){DBG_E("flash part is not init!!");   return NULL;}
	
	for(i=0;i<p_flash_part->part_num;i++)
	{
		if(part_id==p_flash_part->p_part_item[i].part_id)
		{
			return &p_flash_part->p_part_item[i];
		}
	}
	DBG_W("not find the flash part:%d",part_id); 
	return NULL;
}

void flash_part_erase(uint32_t part_id)
{
	part_item_t* p_part_item=NULL;
	p_part_item = get_flash_item_by_part_id(part_id);
	if(p_part_item!=NULL)
	{
		hal_app_ctrl(DRV_FALSH,ERASE_FLASH_CMD,(uint8_t*)p_part_item->part_address,p_part_item->part_size,NULL);
	}
}

void flash_part_write(uint32_t part_id,uint32_t offset,uint8_t *pbuf,uint32_t len)
{
	part_item_t* p_part_item=NULL;
	p_part_item = get_flash_item_by_part_id(part_id);
	
	if(p_part_item!=NULL)
	{
		offset = offset+p_part_item->part_address;
		hal_app_write(DRV_FALSH,pbuf,len,offset,NULL);
	}
}

void flash_part_read(uint32_t part_id,uint32_t offset,uint8_t *pbuf,uint32_t len)
{
	part_item_t* p_part_item=NULL;
	p_part_item = get_flash_item_by_part_id(part_id);
	
	if(p_part_item!=NULL)
	{
		offset = offset+p_part_item->part_address;
		hal_app_read(DRV_FALSH,pbuf,len,offset,NULL);
	}
}

#if TINY_FUNC_CODE == 0

int32_t show_flash_part(uint8_t argc, uint8_t *argv[])
{
	uint32_t i = 0;
    for(i=0;i<p_flash_part->part_num;i++)
	{
		DBG_I("FLASH_PART: %d   addr:0x%x   size:%8d   name:%s", p_flash_part->p_part_item[i].part_id,p_flash_part->p_part_item[i].part_address,p_flash_part->p_part_item[i].part_size,p_flash_part->p_part_item[i].part_name  );
	}

    return RET_OK;
}

REG_SHELL_CMD_X(show_flash_part, 0, show_flash_part, "show_flash_part!!", "show_flash_part \r\n ");


int32_t flash_part_addr(uint8_t argc, uint8_t *argv[])
{
	if(argc == 2)
    {
        uint32_t part_id = 0;
		part_item_t* part_item = NULL;
        sscanf((const char *)argv[1], "%d", &part_id);
        part_item = get_flash_item_by_part_id(part_id);
		if(part_item!=NULL)
		{
			DBG_I("part id:%d  addr:0x%x",part_id, part_item->part_address);
		}
    }
    return RET_OK;
}

REG_SHELL_CMD(flash_part_addr, 0, flash_part_addr, "flash_part_addr!!", "flash_part_addr \r\n ");


int32_t flash_part_e(uint8_t argc, uint8_t *argv[])
{
    uint32_t part_id = 0;

    if(argc >= 2)
    {
        sscanf((const char *)argv[1], "%d", &part_id);

        DBG_I("flash_part_e part:%d", part_id);
        flash_part_erase(part_id);
    }
    else
    {
        DBG_W("flash_part_e param error! eg: flash_part_e part_id");
    }
    return RET_OK;
}

REG_SHELL_CMD(flash_part_e, 0, flash_part_e, "flash_part_e !!", "eg:flash_part_e part_id \r\n ");

int32_t flash_part_w(uint8_t argc, uint8_t *argv[])
{
    uint32_t part_id = 0;
	uint32_t offset  = 0;
	uint32_t len = 0;
	uint8_t  read_buf[100];

    if(argc >= 4)
    {
        sscanf((const char *)argv[1], "%d", &part_id);
		sscanf((const char *)argv[2], "%d", &offset);
        len = strlen((const char *)argv[3]);

        flash_part_write(part_id, offset,argv[3],len);
		len = (len<90)?len:90;
		flash_part_read(part_id, offset,read_buf,len);
        DBG_I("flash_part_w part_id:%d offset: %d ", part_id, offset);
		show_buf(read_buf,len);
    }
    else
    {
        DBG_W("flash_part_w param error! eg: flash_part_w part_id offset str");
    }
    return RET_OK;
}

REG_SHELL_CMD(flash_part_w, 0, flash_part_w, "flash_part_w!!", "eg:flash_part_w part_id offset str \r\n ");


int32_t flash_part_r(uint8_t argc, uint8_t *argv[])
{
    uint32_t part_id = 0;
	uint32_t offset  = 0;
	uint32_t len = 0;
	uint8_t  read_buf[100];

    if(argc >= 4)
    {
        sscanf((const char *)argv[1], "%d", &part_id);
		sscanf((const char *)argv[2], "%d", &offset);
		sscanf((const char *)argv[3], "%d", &len);
		len = (len<90)?len:90;
		flash_part_read(part_id, offset,read_buf,len);
        DBG_I("flash_part_r part_id:%d offset: %d  ", part_id, offset);
		show_buf(read_buf,len);
    }
    else
    {
        DBG_W("flash_part_r ! eg: flash_part_r id  len");
    }
    return RET_OK;
}

REG_SHELL_CMD(flash_part_r, 0, flash_part_r, "flash_part_r read!!", "eg:flash_part_r id  len \r\n ");



#endif
#endif 
