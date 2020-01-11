#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
//#include "debug.h"
#include "x_frame.h"

#if USE_REG_SECTION==1

#if defined(__ARMCC_VERSION)
    #define CY_SYS_INITIAL_STACK_POINTER ((cyisraddress)(uint32)&Image$$ARM_LIB_STACK$$ZI$$Limit)
    extern unsigned char Load$$AUTO_REG_SECTION$$Base[];			//AUTO_REG_SECTION的起始地址
    extern unsigned char Load$$AUTO_REG_SECTION$$Length[];			//AUTO_REG_SECTION的长度
	#define AUTO_SECTION_START (Load$$AUTO_REG_SECTION$$Base)
    #define AUTO_SECTION_LEN   (Load$$AUTO_REG_SECTION$$Length)
	
//    extern unsigned char Load$$AUTO_REG_SECTION$$RO$$Base[];
//	extern unsigned char Load$$AUTO_REG_SECTION$$RO$$Limit[];
//    extern unsigned char Load$$AUTO_REG_SECTION$$RO$$Length[];
//	#define AUTO_SECTION_START (Load$$AUTO_REG_SECTION$$RO$$Base)
//	#define AUTO_SECTION_LIMIT (Load$$AUTO_REG_SECTION$$RO$$Limit)
//    #define AUTO_SECTION_LEN   (Load$$AUTO_REG_SECTION$$RO$$Length)

//	extern unsigned char Load$$AUTO_REG_SECTION$$RW$$Base[];
//	extern unsigned char Load$$AUTO_REG_SECTION$$RW$$Limit[];
//    extern unsigned char Load$$AUTO_REG_SECTION$$RW$$Length[];
//	#define AUTO_SECTION_START (Load$$AUTO_REG_SECTION$$RW$$Base)
//	#define AUTO_SECTION_LIMIT (Load$$AUTO_REG_SECTION$$RW$$Limit)
//    #define AUTO_SECTION_LEN   (Load$$AUTO_REG_SECTION$$RW$$Length)
#elif defined (__GNUC__)
    extern unsigned char __auto_reg_section[];
    extern unsigned char __auto_reg_len[];
    extern unsigned char __auto_reg_section_end[];
    #define AUTO_SECTION_START (__auto_reg_section)
    #define AUTO_SECTION_LEN   (__auto_reg_len)
    #define AUTO_SECTION_END   (__auto_reg_section_end)
#elif defined (__ICCARM__)
    #pragma language=extended
    #pragma segment="CSTACK"
    #define CY_SYS_INITIAL_STACK_POINTER  { .__ptr = __sfe( "CSTACK" ) }

    extern void __iar_program_start( void );
    extern void __iar_data_init3 (void);
#endif  /* (__ARMCC_VERSION) */



void *auto_reg_get_func_cont_by_index(uint8_t func_type, uint32_t index)
{
    auto_reg_t *p_auto_reg = NULL;
    uint32_t i;
    uint32_t match_cur_type_cnt = 0;
    /*加载所有需要自动初始化的模块所有设备*/
    unsigned long auto_reg_section_addr = (unsigned long)AUTO_SECTION_START;
    unsigned long auto_reg_section_len  = (unsigned long)AUTO_SECTION_LEN;
    //unsigned long auto_reg_section_end  = (unsigned long)(AUTO_SECTION_END);
    uint32_t   auto_reg_item = (auto_reg_section_len / sizeof(auto_reg_t));
	
	if(index > auto_reg_item) { return NULL;}
	
	p_auto_reg = (auto_reg_t*)auto_reg_section_addr;
	for(i = 0;i < auto_reg_item;i++)
	{
		if(p_auto_reg[i].type == func_type)
		{
			if(match_cur_type_cnt == index)
			{
				return p_auto_reg[i].p_user_reg_data;
			}
			match_cur_type_cnt++;
		}
	}
	return NULL;
}

int32_t auto_reg_get_func_type_cont(uint8_t func_type, void *p_cont[], uint8_t max_read_item)
{
	auto_reg_t *p_auto_reg = NULL;
    uint32_t i;
    uint32_t match_cur_type_cnt = 0;
    /*加载所有需要自动初始化的模块所有设备*/
    unsigned long auto_reg_section_addr = (unsigned long)AUTO_SECTION_START;
    unsigned long auto_reg_section_len  = (unsigned long)AUTO_SECTION_LEN;
	uint32_t   auto_reg_item = (auto_reg_section_len / sizeof(auto_reg_t));
	
	p_auto_reg = (auto_reg_t*)auto_reg_section_addr;
	for(i = 0;i < auto_reg_item;i++)
	{
		if(p_auto_reg[i].type == func_type)
		{
			if(match_cur_type_cnt < max_read_item)
			{
				p_cont[match_cur_type_cnt] = p_auto_reg[i].p_user_reg_data;
				match_cur_type_cnt++;
			}
			else
			{
				break;
			}
		}
	}
	return match_cur_type_cnt;
}



typedef void (*pf_init_func)(void);

void auto_init_modules(uint32_t init_type)
{
	auto_reg_t *p_auto_reg = NULL;
    uint32_t i;
    pf_init_func p_init_func = NULL;
    /*加载所有需要自动初始化的模块所有设备*/
    unsigned long auto_reg_section_addr = (unsigned long)AUTO_SECTION_START;
    unsigned long auto_reg_section_len  = (unsigned long)AUTO_SECTION_LEN;
	uint32_t   auto_reg_item = (auto_reg_section_len / sizeof(auto_reg_t));
	
	p_auto_reg = (auto_reg_t*)auto_reg_section_addr;
	for(i = 0;i < auto_reg_item;i++)
	{
		if(p_auto_reg[i].type == init_type)
		{
			p_init_func = (pf_init_func)p_auto_reg[i].p_user_reg_data;
			
			if(p_init_func!=NULL)	{p_init_func();}
		}
	}
}


#if TINY_FUNC_CODE == 0
int32_t show_modules(uint8_t argc, uint8_t *argv[])
{
    auto_reg_t *p_auto_reg = NULL;
    uint32_t i;
    unsigned long auto_reg_section_addr = (unsigned long)AUTO_SECTION_START;
    unsigned long auto_reg_section_len  = (unsigned long)(AUTO_SECTION_LEN);
    uint32_t   auto_reg_item = (auto_reg_section_len / sizeof(auto_reg_t));

    (void)p_auto_reg;

    p_auto_reg = (auto_reg_t *)(auto_reg_section_addr);
    for(i = 0; i < auto_reg_item; i++)
    {
        DBG_I("[%2d]:type:%3d addr:0x%8x - %s ", i, p_auto_reg[i].type, p_auto_reg[i].p_user_reg_data, p_auto_reg[i].name);
    }


    return RET_OK;
}
REG_SHELL_CMD_X(moduls, 0, show_modules, "show_modules", "show all auto reg modules \r\n ");
#endif 

#else
    
extern auto_reg_t* p_auto_reg_tab[];
extern uint32_t    auto_reg_num;    


void *auto_reg_get_func_cont_by_index(uint8_t func_type, int32_t index)
{
    auto_reg_t *p_auto_reg = NULL;
    uint32_t i;
    uint32_t match_cur_type_cnt = 0;

    if(index > auto_reg_num) { return NULL;}

    for(i = 0; i < auto_reg_num; i++)
    {
		p_auto_reg = p_auto_reg_tab[i];
        if(p_auto_reg->type == func_type)
        {
            if(match_cur_type_cnt == index)
            {
                //DBG_I("[%d]: %s type:%d addr:0x%x",i,p_auto_reg->name,p_auto_reg->type,p_auto_reg->p_user_reg_data);
                return p_auto_reg->p_user_reg_data;
            }
            match_cur_type_cnt++;
        }
    }
    return NULL;
}

int32_t auto_reg_get_func_type_cont(uint8_t func_type, void *p_cont[], uint8_t max_read_item)
{
    auto_reg_t *p_auto_reg = NULL;
    uint32_t i;
    int32_t     match_cur_type_cnt = 0;

    for(i = 0; i < auto_reg_num; i++)
    {
		p_auto_reg = p_auto_reg_tab[i];
        if(p_auto_reg->type == func_type)
        {
            p_cont[match_cur_type_cnt] = p_auto_reg->p_user_reg_data;
            match_cur_type_cnt++;
            //DBG_I("[%d]: %s type:%d addr:0x%x",i,p_auto_reg->name,p_auto_reg->type,p_auto_reg->p_user_reg_data);
        }
        if(match_cur_type_cnt >= max_read_item)
        {
            break;
        }
    }
    return match_cur_type_cnt;
}



typedef void (*pf_init_func)(void);

void auto_init_modules(uint32_t init_type)
{
    auto_reg_t *p_auto_reg = NULL;
    uint32_t i;
    pf_init_func p_init_func = NULL;

    DBG_I("\r\n ----------------init modules---------\r\n");
    for(i = 0; i < auto_reg_num; i++)
    {
		p_auto_reg = p_auto_reg_tab[i];
        if(p_auto_reg->type == init_type)
        {
            DBG_I("init mod [%d]: %15s type:%d addr:0x%x", i, p_auto_reg->name, p_auto_reg->type, p_auto_reg->p_user_reg_data);
            p_init_func = (pf_init_func)p_auto_reg->p_user_reg_data;
            p_init_func();
        }

    }
}

#if TINY_FUNC_CODE == 0

int32_t show_modules(uint8_t argc, uint8_t *argv[])
{
    auto_reg_t *p_auto_reg = NULL;
    uint32_t i;

    for(i = 0; i < auto_reg_num; i++)
    {
		p_auto_reg = p_auto_reg_tab[i];
        DBG_I("[%2d]:type:%3d addr:0x%8x - %s ", i, p_auto_reg->type, p_auto_reg->p_user_reg_data, p_auto_reg->name);
    }
    return RET_OK;
}
REG_SHELL_CMD_X(moduls, 0, show_modules, "demo show_modulesfault", "show all auto reg modules \r\n ");
#endif
    
#endif 








