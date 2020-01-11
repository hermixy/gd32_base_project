#ifndef __FLASH_PART_H__
#define __FLASH_PART_H__

#include "x_frame.h"


//---------flash_part
enum
{
	SYS_LOADER_PART=1,
	SYS_APP_PART,
	SYS_APP_CFG_PART,
    SYS_BOOT_CFG_PART,
	SYS_PRODUCT_PART,
	
	//-------------------------user part
	USER_REC_PART,
};

#define FLASH_ADDR   		(0x8000000)
#define FLASH_SIZE			(128*1024)
#define FLASH_BLOCK_SIZE	(1*1024)

/*
    +------------------------------------- 128K  FLASH ---------------------------------------+
   /                                                                                           \
  +-----------------+----+------------------------------------+------------+------+------+------+
  |       24K       | 1K |              89K                   |     8K     |  2K  |  2K  |  2K  |
  +-------+---------+-+--+---------------+--------------------+-----+------+---+--+---+--+---+--+
          |           |                  |                          |          |      |      |
     bootloader       |               app_bin                       |       APP_CFG   |       \
                   app_file_info                                 recorder         boot_cfg    product_info

*/

#define ERASE_FLASH_CMD    0x01
#define CHG_IO_MODE_IN     0x02
#define CHG_IO_MODE_OUT    0x03
#define RELOAD_IWDT        0x04

#define SYS_RESET_CMD           0x01
#define SYS_JUMP_CMD            0x02
#define SYS_IAP_APP_INIT_CMD    0x03


typedef struct 
{
	const char *		part_name;
	uint32_t			part_id;
	uint32_t			part_size;
	uint32_t 			part_address;
}part_item_t;

typedef struct
{
	uint32_t   			start_address;
	uint32_t 			flash_size;
	uint32_t			block_size;
	uint32_t   			part_num;
	part_item_t*		p_part_item;			
}flash_part_core_t;


void flash_part_init(void);
part_item_t* get_flash_item_by_part_id(uint32_t part_id);

void flash_part_erase(uint32_t part_id);
void flash_part_write(uint32_t part_id,uint32_t offset,uint8_t *pbuf,uint32_t len);
void flash_part_read(uint32_t part_id,uint32_t offset,uint8_t *pbuf,uint32_t len);


#endif //__FLASH_PART_H__
