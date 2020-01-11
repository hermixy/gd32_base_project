#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "kfifo.h"

#define KFIFO_MIN(a,b) ((a)<(b))?(a):(b)
#define KFIFO_MAX(a,b) ((a)>(b))?(a):(b)

uint32_t  calc_real_size(uint32_t len)  //考虑2的n次方
{
    uint8_t i = 0;
    for(i = 0;len > 0;i++)
    {
        len = len >> 1;
    }
    return (0x01<<(i-1));
}

int32_t kfifo_init(const char* name, kfifo_t *p_kfifo, uint8_t *p_buf, uint32_t len)
{
    p_kfifo->name   = name;
    p_kfifo->p_buf  = p_buf;
    p_kfifo->size   = calc_real_size(len);

    p_kfifo->index_w = 0;
    p_kfifo->index_r = 0;

    return 1;
}

/*
    1.-------w-------------------r-----------
    2.-------r-------------------w-----------
*/
int32_t kfifo_push_in(kfifo_t *p_kfifo, uint8_t *dest_buf, uint32_t len)
{
    uint32_t real_len = 0;
    uint32_t rest_len_w = p_kfifo->size - (p_kfifo->index_w & (p_kfifo->size - 1));           //index_w 一直增加, 保证写指针不超过p_kfifo->size
	
	if(p_kfifo->size != calc_real_size(p_kfifo->size))	{p_kfifo->size=calc_real_size(p_kfifo->size);} //避免size不是2的n次方，导致出错
    if(dest_buf == NULL)   {return 0;}

    real_len = KFIFO_MIN(len,       p_kfifo->size - p_kfifo->index_w + p_kfifo->index_r );    //获取能够写入的长度
    len      = KFIFO_MIN(real_len,  rest_len_w );

    memcpy(p_kfifo->p_buf + (p_kfifo->index_w & (p_kfifo->size - 1)), dest_buf, len );         //第1种情况（全部数据拷贝完成），第2种情况（先拷贝一部分数据）
    memcpy(p_kfifo->p_buf, (dest_buf+len), real_len-len );                                     //第1种情况（拷贝数据长度为0）， 第2种情况（拷贝剩余长度）

    p_kfifo->index_w += real_len;

    return real_len;
}

int32_t kfifo_pull_out(kfifo_t *p_kfifo, uint8_t *dest_buf, uint32_t len)
{
    uint32_t real_len = 0;
    uint32_t rest_len_r = p_kfifo->size - (p_kfifo->index_r & (p_kfifo->size - 1));
	
	if(p_kfifo->size != calc_real_size(p_kfifo->size))	{p_kfifo->size=calc_real_size(p_kfifo->size);} //避免size不是2的n次方，导致出错
    if(dest_buf == NULL)   {return 0;}

    real_len = KFIFO_MIN(len,       (p_kfifo->index_w - p_kfifo->index_r) );
    len      = KFIFO_MIN(real_len,  rest_len_r );

    memcpy(dest_buf, p_kfifo->p_buf + (p_kfifo->index_r & (p_kfifo->size - 1)), len);          //第1种情况（先拷贝一部分数据），第2种情况（全部数据拷贝完成）
    memcpy(dest_buf+len, p_kfifo->p_buf, real_len-len);                                            //第1种情况（拷贝剩余长度），    第2种情况（拷贝数据长度为0） 修复bug: memcpy(dest_buf, p_kfifo->p_buf, real_len-len);

    p_kfifo->index_r += real_len;

    return real_len;
}

void kfifo_reset(kfifo_t *p_kfifo)
{
    p_kfifo->index_w  = 0;
    p_kfifo->index_r   = 0;
}
uint32_t kfifo_get_data_len(kfifo_t *p_kfifo)   //available data len
{
    return (p_kfifo->index_w - p_kfifo->index_r);
}
uint32_t kfifo_get_rest_len(kfifo_t *p_kfifo)   //rest len
{
    return (p_kfifo->size - kfifo_get_data_len(p_kfifo));
}
uint32_t kfifo_get_w_index(kfifo_t *p_kfifo)
{
    return (p_kfifo->index_w & (p_kfifo->size - 1));
}
uint32_t kfifo_get_r_index(kfifo_t *p_kfifo)
{
    return (p_kfifo->index_r & (p_kfifo->size - 1));
}
