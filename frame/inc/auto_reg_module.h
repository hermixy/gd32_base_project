#ifndef __AUTO_REG_MODULE_H__
#define __AUTO_REG_MODULE_H__

#include <stdint.h>



void   *auto_reg_get_func_cont_by_index(uint8_t func_type, uint32_t index);
int32_t auto_reg_get_func_type_cont(uint8_t func_type, void *p_cont[], uint8_t max_read_item);
void    auto_init_modules(uint32_t init_type);


#endif
