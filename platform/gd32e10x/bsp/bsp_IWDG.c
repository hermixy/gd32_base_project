#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "x_frame.h"

#include "bsp_IWDG.h"



void bsp_IWDG_init(void)
{
     /* enable IRC40K */
    rcu_osci_on(RCU_IRC40K);
    
    /* wait till IRC40K is ready */
    while(SUCCESS != rcu_osci_stab_wait(RCU_IRC40K)){
    }
	/* confiure FWDGT counter clock: 40KHz(IRC40K) / 64 = 0.625 KHz */
    fwdgt_config(3000, FWDGT_PSC_DIV64);	//1.6ms * 2000 = 3.2s
    
    /* after 1.6 seconds to generate a reset */
    fwdgt_enable();
}

void bsp_IWDG_reload(void)
{
	fwdgt_counter_reload();
}



