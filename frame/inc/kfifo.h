#ifndef __KFIFO_H__
#define __KFIFO_H__

typedef struct
{
    const char  *name;
	uint32_t    size;
    uint8_t     *p_buf;
    uint32_t    index_w;
    uint32_t    index_r;
} kfifo_t;

int32_t kfifo_init(const char* name, kfifo_t *p_kfifo, uint8_t *p_buf, uint32_t len);
int32_t kfifo_push_in(kfifo_t *p_kfifo, uint8_t *p_buf, uint32_t len);
int32_t kfifo_pull_out(kfifo_t *p_kfifo, uint8_t *p_buf, uint32_t len);

void kfifo_reset(kfifo_t *pfifo);
uint32_t kfifo_get_data_len(kfifo_t *p_kfifo);
uint32_t kfifo_get_rest_len(kfifo_t *p_kfifo);
uint32_t kfifo_get_w_index(kfifo_t *p_kfifo);
uint32_t kfifo_get_r_index(kfifo_t *p_kfifo);

#endif //----__KFIFO_H__
