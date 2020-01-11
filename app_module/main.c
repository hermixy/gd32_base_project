#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
//#include "debug.h"
#include "x_frame.h"


#include MCU_HEADER	//if use default compiler version 6, you need open it

int main(void)
{
    __enable_irq();//此时打开总中断（由于程序执行到此处前，关闭了所有中断）
    hal_init();

  	DBG_I("\r\n \r\n");
	DBG_I("=====================================================");
	DBG_I("Build Time: %s   %s  (%s)",__DATE__,__TIME__,(USE_REG_SECTION>0)?"USE_REG_SECTION":"USE_REG_ARRY_TAB");
	DBG_I("=====================================================\r\n \r\n");
	
    while(1)
    {
        task_sch(0);
		task_sch(1);
		task_sch(2);
		task_sch(3);
    }
}

