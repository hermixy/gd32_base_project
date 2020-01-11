#ifdef GEN_REC_CFG_ENTITY
    #undef REC_START
    #undef REC_ITEM
    #undef REC_END
	
	#undef REC_CACHE_START
    #undef REC_CACHE_ITEM
    #undef REC_CACHE_END
    
    #include "user_data_def.h"
    #include "x_frame.h"

    #define REC_START(name,rec_cmd_set,rec_cmd_id)      void rec_cfg_##name(void)                           \
                                                { uint8_t _rec_buf[MAX_RECORDER_PACK_LEN]={0};              \
                                                  uint16_t rec_w_index = sizeof(rec_cfg_item_t)-1;          \
                                                  rec_cfg_item_t* p_cfg_item=(rec_cfg_item_t*)(_rec_buf);   \
                                                  uint8_t i = 0;                                            \
                                                  const char* pack_name = #name;                            \
                                                  uint8_t max_var_item = 0;                                 \
                                                  const char* p_var_name[MAX_REC_ITEM_NUM]={0};             \
                                                  rec_var_item_t var_item_cfg[MAX_REC_ITEM_NUM]={0};        \
                                                  rec_var_item_t* p_var_item_cfg=NULL;                      \
                                                  hal_frame_info_t  rec_cfg_frame={0};                      \
                                                  uint8_t   rec_item_len= 0;                                \
                                                  p_cfg_item->pack_cmd_set = rec_cmd_set;                   \
                                                  p_cfg_item->pack_cmd_id  = rec_cmd_id;                    \
                                                  rec_cfg_frame.link_id = REC_LINK_ID;                      \
                                                  rec_cfg_frame.src     = HOST_ADDR;                        \
                                                  rec_cfg_frame.dest    = PC_ADDR;                          \
                                                  rec_cfg_frame.cmd_func= REC_CMDSET_REC;                   \
                                                  rec_cfg_frame.cmd_id  = REC_CMDID_REC_CFG;                \
                                                  p_cfg_item->pack_name_len = strlen(pack_name);             \
                                                  memcpy(p_cfg_item->pack_name,pack_name,strlen(pack_name));\
                                                  rec_w_index+=strlen(pack_name);
                                                
                                                
     #define REC_ITEM(type,var)                   var_item_cfg[max_var_item].rec_var_index      = max_var_item;                     \
                                                  var_item_cfg[max_var_item].rec_var_type       = type;                             \
                                                  var_item_cfg[max_var_item].rec_var_len        = 0;                                \
                                                  p_var_name[max_var_item]                      = get_var_name(#var);               \
                                                  var_item_cfg[max_var_item].rec_var_name_len   = strlen(p_var_name[max_var_item]); \
                                                  max_var_item++;
                                                  
                                                
                                    
    #define REC_END()                             p_cfg_item->pack_item_num = max_var_item;                                            \
                                                  for(i=0;i<max_var_item;i++)                                                       \
                                                  {                                                                                 \
                                                    rec_item_len = sizeof(rec_var_item_t)-1+var_item_cfg[i].rec_var_name_len;         \
                                                    if(rec_w_index+rec_item_len>(MAX_RECORDER_PACK_LEN))                            \
                                                    {                                                                               \
                                                        rec_cfg_frame.pdata    =_rec_buf;                                           \
                                                        rec_cfg_frame.data_len = rec_w_index;                                       \
                                                        protocol_send_by_id(&rec_cfg_frame);                                        \
                                                        rec_w_index = sizeof(rec_cfg_item_t)-1;                                       \
														memcpy(p_cfg_item->pack_name,pack_name,strlen(pack_name));					\
														rec_w_index+=strlen(pack_name);												\
                                                    }                                                                               \
                                                    p_var_item_cfg = (rec_var_item_t*)&_rec_buf[rec_w_index];                       \
                                                    memcpy(p_var_item_cfg,&var_item_cfg[i],sizeof(rec_var_item_t)-1);               \
                                                    memcpy(&_rec_buf[rec_w_index+sizeof(rec_var_item_t)-1],p_var_name[i],p_var_item_cfg->rec_var_name_len);\
                                                    rec_w_index+=rec_item_len;                                                      \
                                                  }                                                                                 \
                                                  rec_cfg_frame.pdata    =_rec_buf;                                                 \
                                                  rec_cfg_frame.data_len = rec_w_index;                                             \
                                                  if(enable_rec_dat){protocol_send_by_id(&rec_cfg_frame);} }
												  
												  
												  
	#define REC_CACHE_START(name,rec_cmd_set,rec_cmd_id,cache_size)     \
													static uint32_t rec_pack_cache_##name##_len=0;\
													static uint8_t flg_pack_##name##_init=0;\
													void rec_cfg_##name(void)                          			 \
                                                { uint8_t _rec_buf[MAX_RECORDER_PACK_LEN]={0};              \
                                                  uint16_t rec_w_index = sizeof(rec_cfg_item_t)-1;          \
                                                  rec_cfg_item_t* p_cfg_item=(rec_cfg_item_t*)(_rec_buf);   \
                                                  uint8_t i = 0;                                            \
                                                  const char* pack_name = #name;                            \
                                                  uint8_t max_var_item = 0;                                 \
                                                  const char* p_var_name[MAX_REC_ITEM_NUM]={0};             \
                                                  rec_var_item_t var_item_cfg[MAX_REC_ITEM_NUM]={0};        \
                                                  rec_var_item_t* p_var_item_cfg=NULL;                      \
                                                  hal_frame_info_t  rec_cfg_frame={0};                      \
                                                  uint8_t   rec_item_len= 0;                                \
                                                  p_cfg_item->pack_cmd_set = rec_cmd_set;                   \
                                                  p_cfg_item->pack_cmd_id  = rec_cmd_id;                    \
                                                  rec_cfg_frame.link_id = REC_LINK_ID;                      \
                                                  rec_cfg_frame.src     = HOST_ADDR;                        \
                                                  rec_cfg_frame.dest    = PC_ADDR;                          \
                                                  rec_cfg_frame.cmd_func= REC_CMDSET_REC;                   \
                                                  rec_cfg_frame.cmd_id  = REC_CMDID_REC_CFG;                \
                                                  p_cfg_item->pack_name_len = strlen(pack_name);             \
                                                  memcpy(p_cfg_item->pack_name,pack_name,strlen(pack_name));\
                                                  rec_w_index+=strlen(pack_name);
                                                
                                                
     #define REC_CACHE_ITEM(name,type,var)             var_item_cfg[max_var_item].rec_var_index      = max_var_item;                     \
                                                  var_item_cfg[max_var_item].rec_var_type       = type;                             \
                                                  var_item_cfg[max_var_item].rec_var_len        = 0;                                \
                                                  p_var_name[max_var_item]                      = get_var_name(#var);               \
                                                  var_item_cfg[max_var_item].rec_var_name_len   = strlen(p_var_name[max_var_item]); \
                                                  max_var_item++;\
												  if(flg_pack_##name##_init==0){rec_pack_cache_##name##_len+=(sizeof(var)+4);}
												  
                                                  
                                                
                                    
    #define REC_CACHE_END(name)                       p_cfg_item->pack_item_num = max_var_item;                                            \
                                                  for(i=0;i<max_var_item;i++)                                                       \
                                                  {                                                                                 \
                                                    rec_item_len = sizeof(rec_var_item_t)-1+var_item_cfg[i].rec_var_name_len;         \
                                                    if(rec_w_index+rec_item_len>(MAX_RECORDER_PACK_LEN))                            \
                                                    {                                                                               \
                                                        rec_cfg_frame.pdata    =_rec_buf;                                           \
                                                        rec_cfg_frame.data_len = rec_w_index;                                       \
                                                        protocol_send_by_id(&rec_cfg_frame);                                        \
                                                        rec_w_index = sizeof(rec_cfg_item_t)-1;                                       \
														memcpy(p_cfg_item->pack_name,pack_name,strlen(pack_name));					\
														rec_w_index+=strlen(pack_name);												\
                                                    }                                                                               \
                                                    p_var_item_cfg = (rec_var_item_t*)&_rec_buf[rec_w_index];                       \
                                                    memcpy(p_var_item_cfg,&var_item_cfg[i],sizeof(rec_var_item_t)-1);               \
                                                    memcpy(&_rec_buf[rec_w_index+sizeof(rec_var_item_t)-1],p_var_name[i],p_var_item_cfg->rec_var_name_len);\
                                                    rec_w_index+=rec_item_len;                                                      \
                                                  }                                                                                 \
												  flg_pack_##name##_init=1;															\
                                                  rec_cfg_frame.pdata    =_rec_buf;                                                 \
                                                  rec_cfg_frame.data_len = rec_w_index;                                             \
                                                  if(enable_rec_dat){protocol_send_by_id(&rec_cfg_frame);} }

#endif
                                                  
                                                  
#ifdef GEN_REC_CFG_DECLARE
    #undef REC_START
    #undef REC_ITEM
    #undef REC_END
	
	#undef REC_CACHE_START
    #undef REC_CACHE_ITEM
    #undef REC_CACHE_END

    #define REC_START(name,cmd_set,cmd_id)       		extern void rec_cfg_##name(void);   
    #define REC_ITEM(type,var)
    #define REC_END()
	
	#define REC_CACHE_START(name,rec_cmd_set,rec_cmd_id,cache_size)        extern void rec_cfg_##name(void);   
    #define REC_CACHE_ITEM(type,var)
    #define REC_CACHE_END()
	
#endif
                                                  
                                                  
#ifdef GEN_REC_CFG_CALL
    #undef REC_START
    #undef REC_ITEM
    #undef REC_END
	
	#undef REC_CACHE_START
    #undef REC_CACHE_ITEM
    #undef REC_CACHE_END

    #define REC_START(name,cmd_set,cmd_id)        	rec_cfg_##name();
    #define REC_ITEM(type,var)
    #define REC_END()
	
	#define REC_CACHE_START(name,rec_cmd_set,rec_cmd_id,cache_size)        rec_cfg_##name();
    #define REC_CACHE_ITEM(type,var)
    #define REC_CACHE_END(name)
	
#endif


#ifdef GEN_REC_DAT_ENTITY
    #undef REC_START
    #undef REC_ITEM
    #undef REC_END
	
	#undef REC_CACHE_START
    #undef REC_CACHE_ITEM
    #undef REC_CACHE_END
    
    #include "x_frame.h"

    #define REC_START(name,rec_cmd_set,rec_cmd_id)      void rec_dat_##name(void)                           \
                                                { uint8_t _rec_buf[MAX_RECORDER_PACK_LEN]={0};      \
                                                  uint16_t rec_w_index = sizeof(uint32_t);          \
                                                  hal_frame_info_t  rec_dat_frame={0};              \
                                                  uint8_t   if_full = 0;                            \
                                                  uint32_t cur_ms = REC_TIME_STAMP;                  \
                                                  rec_dat_frame.link_id = REC_LINK_ID;              \
                                                  rec_dat_frame.src     = HOST_ADDR;                \
                                                  rec_dat_frame.dest    = PC_ADDR;                  \
                                                  rec_dat_frame.cmd_func= rec_cmd_set;                  \
                                                  rec_dat_frame.cmd_id  = rec_cmd_id;                   \
                                                  memcpy(_rec_buf,&cur_ms,sizeof(uint32_t));
                                                
                                                
                                                  
     #define REC_ITEM(type,var)                   if(if_full == 0)\
                                                  {\
                                                    memcpy(&_rec_buf[rec_w_index],(uint8_t*)&var,sizeof(var));      \
                                                    if(rec_w_index>MAX_RECORDER_PACK_LEN){rec_w_index = MAX_RECORDER_PACK_LEN; if_full = 1; rec_w_index=MAX_RECORDER_PACK_LEN;}\
                                                    else{rec_w_index+=sizeof(var);}            \
                                                  }
                                                  
                                    
     #define REC_END()                            rec_dat_frame.pdata    =_rec_buf;\
                                                  rec_dat_frame.data_len = rec_w_index;\
                                                  if(enable_rec_dat){protocol_send_by_id(&rec_dat_frame);}}
												  
												  
												  
	 #define REC_CACHE_START(name,rec_cmd_set,rec_cmd_id,cache_size)    \
												void rec_cache_dat_##name(void)        		                   \
                                                {   static uint8_t  rec_cache_dat[cache_size];\
													static uint32_t cur_cnt= 0;          \
													uint32_t cur_cache_size = cache_size;\
													hal_frame_info_t  rec_dat_frame={0};              \
													uint32_t cur_ms= REC_TIME_STAMP; \
													if(flg_pack_##name##_init==0){return;}\
													rec_dat_frame.cmd_func= rec_cmd_set;                  \
													 rec_dat_frame.cmd_id  = rec_cmd_id;    \
													if( (cur_cnt+rec_pack_cache_##name##_len) > (cur_cache_size )){\
													rec_dat_frame.pdata    = rec_cache_dat;					\
													 rec_dat_frame.data_len = cur_cnt;					\
													 cur_cnt = 0;\
													 rec_dat_frame.link_id = REC_LINK_ID;              \
													 rec_dat_frame.src     = HOST_ADDR;                \
													 if(enable_rec_dat){protocol_send_by_id(&rec_dat_frame);}}\
													 memcpy(&rec_cache_dat[cur_cnt],&cur_ms,sizeof(uint32_t)); \
													cur_cnt+=4;  
													
                                              
     #define REC_CACHE_ITEM(name,type,var)           memcpy(&rec_cache_dat[cur_cnt],(uint8_t*)&var,sizeof(var));      \
                                                    cur_cnt+=sizeof(var);      
													
                                                  
                                    
     #define REC_CACHE_END(name)        			}				
												

#endif
          


#ifdef GEN_REC_DAT_DECLARE
    #undef REC_START
    #undef REC_ITEM
    #undef REC_END
	
	#undef REC_CACHE_START
    #undef REC_CACHE_ITEM
    #undef REC_CACHE_END

    #define REC_START(name,cmd_set,cmd_id)       extern void rec_dat_##name(void);   
    #define REC_ITEM(type,var)
    #define REC_END()
	
	
	#define REC_CACHE_START(name,rec_cmd_set,rec_cmd_id,cache_size)        extern void rec_cache_dat_##name(void);   
    #define REC_CACHE_ITEM(name,type,var)
    #define REC_CACHE_END(name)
	
#endif
                                                  
                                                  
#ifdef GEN_REC_DAT_CALL
    #undef REC_START
    #undef REC_ITEM
    #undef REC_END
	
	#undef REC_CACHE_START
    #undef REC_CACHE_ITEM
    #undef REC_CACHE_END

    #define REC_START(name,cmd_set,cmd_id)        rec_dat_##name();   
    #define REC_ITEM(type,var)
    #define REC_END()
	
	#define REC_CACHE_START(name,rec_cmd_set,rec_cmd_id,cache_size)          rec_cache_dat_##name();   
    #define REC_CACHE_ITEM(name,type,var)
    #define REC_CACHE_END(name)
	
#endif



#ifdef GEN_REC_INIT_ENTITY
    #undef REC_START
    #undef REC_ITEM
    #undef REC_END
	
	#undef REC_CACHE_START
    #undef REC_CACHE_ITEM
    #undef REC_CACHE_END
    
    #include "x_frame.h"
    #include "rec_func_entity.h"
    #define REC_START(name,cmd_set,cmd_id)      static rec_node_t  rec_init_##name##_node={0};                   \
                                                void rec_init_##name(void)                                \
                                                {                                                         \
                                                    INIT_LIST_HEAD(&rec_init_##name##_node.rec_init_node);\
                                                    rec_init_##name##_node.rec_cfg_func = rec_cfg_##name;\
                                                    add_rec_node(&rec_init_##name##_node.rec_init_node);   \
                                                }
                                                                                           
     #define REC_ITEM(type,var)                                                  
     #define REC_END()    

	 #define REC_CACHE_START(name,rec_cmd_set,rec_cmd_id,cache_size)       static rec_node_t  rec_init_##name##_node={0};                   \
																			void rec_init_##name(void)                                \
																			{                                                         \
																				INIT_LIST_HEAD(&rec_init_##name##_node.rec_init_node);\
																				rec_init_##name##_node.rec_cfg_func = rec_cfg_##name;\
																				add_rec_node(&rec_init_##name##_node.rec_init_node);   \
																			}
                                                                                           
     #define REC_CACHE_ITEM(name,type,var)                                                  
     #define REC_CACHE_END(name)    

#endif
          


#ifdef GEN_REC_INIT_DECLARE
    #undef REC_START
    #undef REC_ITEM
    #undef REC_END

	#undef REC_CACHE_START
    #undef REC_CACHE_ITEM
    #undef REC_CACHE_END

    #define REC_START(name,cmd_set,cmd_id)       extern void rec_init_##name(void);   
    #define REC_ITEM(type,var)
    #define REC_END()
	
	
	 #define REC_CACHE_START(name,rec_cmd_set,rec_cmd_id,cacfhe_size)       extern void rec_init_##name(void);            
     #define REC_CACHE_ITEM(name,type,var)                                                  
     #define REC_CACHE_END(name)    
	
#endif
                                                  
                                                  
#ifdef GEN_REC_INIT_CALL
    #undef REC_START
    #undef REC_ITEM
    #undef REC_END
	
	#undef REC_CACHE_START
    #undef REC_CACHE_ITEM
    #undef REC_CACHE_END

    #define REC_START(name,cmd_set,cmd_id)        rec_init_##name();   
    #define REC_ITEM(type,var)
    #define REC_END()
	
	#define REC_CACHE_START(name,rec_cmd_set,rec_cmd_id,cache_size)       rec_init_##name();              
    #define REC_CACHE_ITEM(name,type,var)                                                  
    #define REC_CACHE_END(name) 
	 
#endif






