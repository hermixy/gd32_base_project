#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#if ENABLE_RECORDER == 1
#include "rec_func_entity.h"
#include "x_frame.h"

#define GEN_REC_CFG_ENTITY
    #include "rec_db.h"
#undef GEN_REC_CFG_ENTITY

#define GEN_REC_DAT_ENTITY
    #include "rec_db.h"
#undef GEN_REC_DAT_ENTITY


static list_t rec_init_head;
const char* get_var_name(const char* var_name)
{
    char *p;
    p = strrchr(var_name, '>');
    if(p !=NULL)
    {
       return p+1;
    }

    p = strrchr(var_name, '.');
    if(p !=NULL)
    {
       return p+1;
    }

    return var_name;
}


void add_rec_node(list_t* p_add_node)
{
    list_add(p_add_node,&rec_init_head);
}


#define GEN_REC_INIT_ENTITY
    #include "rec_db.h"
#undef GEN_REC_INIT_ENTITY

void rec_init(void)
{
    INIT_LIST_HEAD(&rec_init_head);
    
    #define GEN_REC_INIT_CALL
        #include "rec_db.h"
    #undef GEN_REC_INIT_CALL    
}

REG_INIT_MOD(rec_init);


list_t* p_last_exe_list = &rec_init_head;
void rec_cfg_task(void)
{
    list_t      *cur_node = NULL;
    rec_node_t  *p_cfg_node;
    cur_node = p_last_exe_list->next;
    if(cur_node == &rec_init_head){cur_node = cur_node->next;}

    p_cfg_node = list_entry(cur_node,rec_node_t,rec_init_node);
    p_cfg_node->rec_cfg_func();
    p_last_exe_list = cur_node;
    
}

REG_TASK(ID_TASK,2000,rec_cfg_task);

uint8_t enable_rec_dat = 0;
REG_CFG_ITEM(enable_rec_dat,enable_rec_dat             ,eu_uint8      ,PARM_RW        ,DEF_REC_DAT_FLG         ,0.0f       ,1.0f  );

#endif //ENABLE_RECORDER
