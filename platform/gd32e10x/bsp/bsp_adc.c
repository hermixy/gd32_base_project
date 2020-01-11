#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "bsp_adc.h"

typedef struct
{
    uint32_t      		port;
    uint32_t            RCU_port;
    uint32_t            pin;
    uint32_t            ADC_ch;
} adc_item_t;

#define ADC1_DR_Address    0x4001244C//0x40012440
#define AD_COUNT   5			//AD滤波采样次数	
#define ADD_ADC_ITEM(port,pin,ch) {port,RCU_##port,GPIO_PIN_##pin,ch}

uint32_t AD_Data[MAX_AD_CH];      //AD采集值
volatile uint16_t ADC_ConvertedValue[AD_COUNT][MAX_AD_CH];

const adc_item_t adc_tab[MAX_AD_CH] =
{

    //    ADD_ADC_ITEM(GPIOA,0,ADC_Channel_0),
    //    ADD_ADC_ITEM(GPIOA,1,ADC_Channel_1),
    //    ADD_ADC_ITEM(GPIOA,2,ADC_Channel_2),
    //    ADD_ADC_ITEM(GPIOA,3,ADC_Channel_3),
    //    ADD_ADC_ITEM(GPIOA,4,ADC_Channel_4),
    //    ADD_ADC_ITEM(GPIOA,5,ADC_Channel_5),                                      
	
    //--------------必须按照ADC ch 的顺序排列
	//----------
	ADD_ADC_ITEM(GPIOA, 1,	ADC_CHANNEL_1),	//AD_CH1
//	ADD_ADC_ITEM(GPIOA, 0,	ADC_CHANNEL_0),	//AD_CH2
//	ADD_ADC_ITEM(GPIOC, 3,	ADC_CHANNEL_13),//AD_CH3
//	ADD_ADC_ITEM(GPIOC, 2,	ADC_CHANNEL_12),//AD_CH4
//	ADD_ADC_ITEM(GPIOC, 1,	ADC_CHANNEL_11),//AD_CH5
//	ADD_ADC_ITEM(GPIOC, 0,	ADC_CHANNEL_10),//AD_CH6
//	ADD_ADC_ITEM(GPIOA, 4,	ADC_CHANNEL_4),	//AD_CH7
//	ADD_ADC_ITEM(GPIOA, 5,	ADC_CHANNEL_5),	//AD_CH8
//	ADD_ADC_ITEM(GPIOA, 6,	ADC_CHANNEL_6),	//AD_CH9
//	ADD_ADC_ITEM(GPIOA, 7,	ADC_CHANNEL_7),	//AD_CH10
//	ADD_ADC_ITEM(GPIOC, 4,	ADC_CHANNEL_14),//AD_CH11
//	ADD_ADC_ITEM(GPIOC, 5,	ADC_CHANNEL_15),//AD_CH12
//	ADD_ADC_ITEM(GPIOB, 0,	ADC_CHANNEL_8),	//AD_CH13
//	ADD_ADC_ITEM(GPIOB, 1,	ADC_CHANNEL_9),	//AD_CH14
};


static void dma_config(void)
{
    /* ADC_DMA_channel configuration */
    dma_parameter_struct dma_data_parameter;

    /* enable DMA0 clock */
    rcu_periph_clock_enable(RCU_DMA0);  
	nvic_irq_enable(DMA0_Channel0_IRQn , 0, 0);

	
    /* ADC DMA_channel configuration */
    dma_deinit(DMA0, DMA_CH0);
    dma_struct_para_init(&dma_data_parameter);
	
    /* initialize DMA single data mode */
    dma_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr = (uint32_t)(&ADC_ConvertedValue);	
    dma_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;  
    dma_data_parameter.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number = MAX_AD_CH *AD_COUNT;
    dma_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_data_parameter);


	dma_interrupt_enable(DMA0, DMA_CH0, DMA_INT_FTF);
	
    dma_circulation_enable(DMA0, DMA_CH0);
    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH0);
}
static void adc_config(void)
{
	uint32_t i;
   /* enable ADC clock */
   rcu_periph_clock_enable(RCU_ADC0);
   /* config ADC clock */
   rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);

   /* reset ADC */
   adc_deinit(ADC0);
   /* ADC mode config */
   adc_mode_config(ADC_DAUL_REGULAL_PARALLEL);
   /* ADC scan mode disable */
   adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
   adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
   /* ADC data alignment config */
   adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
   
   /* ADC channel length config */
   adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, MAX_AD_CH);
   for(i = 0; i < MAX_AD_CH; i++)  
   {
	   /* enable GPIOC clock */
	   rcu_periph_clock_enable((rcu_periph_enum)adc_tab[i].RCU_port);
	   /* config the GPIO as analog mode */
	   gpio_init(adc_tab[i].port, GPIO_MODE_AIN, GPIO_OSPEED_MAX, adc_tab[i].pin);

	   /* ADC regular channel config */
	   adc_regular_channel_config(ADC0, i, adc_tab[i].ADC_ch, ADC_SAMPLETIME_239POINT5);

   }

   /* ADC trigger config */
   adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_NONE); 
   adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);
   
   /* enable ADC interface */
   adc_enable(ADC0);
   
//	 delay_1ms(1);
   
   /* ADC calibration and reset calibration */
   adc_calibration_enable(ADC0);

   /* ADC DMA function enable */
   adc_dma_mode_enable(ADC0);
   /* ADC software trigger enable */
   adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
}

void bsp_adc_init(void)
{
	dma_config();
    adc_config();  
}


uint32_t bsp_adc_read(uint8_t *pSrc, uint32_t len, uint32_t offset)
{
    uint32_t real_len = 0;
    if( (uint32_t)pSrc % 4 != 0)
    {
        DBG_W_LIMIT(1000, "bsp_adc_read address must uint32_addr cur:0x%x", pSrc);
        return 0;
    }
    if(           len % 4 != 0)
    {
        DBG_W_LIMIT(1000, "bsp_adc_read len  must uint32_t cur:%d", len);
        return 0;
    }
    if(        offset > (MAX_AD_CH-1))
    {
        DBG_W_LIMIT(1000, "bsp_adc_read offset must < MAX_AD_CH:%d", offset);
        return 0;
    }

    real_len = len;
    if(real_len > (MAX_AD_CH * sizeof(uint32_t)) )
    {
        real_len = (MAX_AD_CH * sizeof(uint32_t));
    }

    memcpy(pSrc, (uint8_t*)&AD_Data[offset], len);

    return real_len;
}

uint32_t flag_is_ok = 0;
static uint32_t cnt = 0;
uint32_t DMA_IRQ_CNT = 0;
void DMA0_Channel0_IRQHandler(void)
{
    //float vol = 0;
    uint32_t i, j;
    cnt++;
    DMA_IRQ_CNT++;
    
    if(cnt > MAX_AD_CH)
    {
        cnt = 0;
        /*判断DMA传输完成中断*/
        if(dma_interrupt_flag_get(DMA0, DMA_CH0, DMA_INT_FLAG_FTF) != RESET)
        {
            /*取值求和取平均*/
            for(i = 0; i < MAX_AD_CH; i++)
            {
                AD_Data[i] = 0;
                for(j = 0; j < AD_COUNT; j++)
                {
                    AD_Data[i] += ADC_ConvertedValue[j][i];
                    //DBG_I("ADC_ConvertedValue[%d][%d]:%d",j,i,ADC_ConvertedValue[j][i]);
                }
                AD_Data[i] = AD_Data[i] / AD_COUNT;
            }
        }
        //DBG_I_LIMIT(1000,"CH1:%d  CH5:%d  CH6:%d  CH7:%d",AD_Data[0],AD_Data[1],AD_Data[2],AD_Data[3]);
        //vol = AD_Data[0];
        //vol = vol*3300/4096;
        //DBG_I_LIMIT(1000,"note boot:%d   vol:%f",AD_Data[1],vol);
        //DBG_I_LIMIT(1000,"usb-a:%d   vol:%f",AD_Data[0],vol);
        
        
        for(i = 0; i < MAX_AD_CH; i++)
        {
            //adc_regular_channel_config(ADC0, adc_tab[i].ADC_ch, i , ADC_SAMPLETIME_239POINT5);	//错误
			adc_regular_channel_config(ADC0, i, adc_tab[i].ADC_ch, ADC_SAMPLETIME_239POINT5);
        }
    }

    /*清除DMA中断标志位*/
	dma_interrupt_flag_clear(DMA0, DMA_CH0, DMA_INT_FLAG_G);
}


uint16_t Get_val(uint16_t ch)
{
    uint16_t DataValue; //??????????    
    /* ADC1 regular channel14 configuration */ 

  adc_regular_channel_config(ADC0, 1, ch , ADC_SAMPLETIME_239POINT5);
    
    /* Start ADC1 Software Conversion */ 
  //ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    
        /* Test if the ADC1 EOC flag is set or not */ 

        //while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
        //FlagStatus Status;
        //Status = ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC);
        //while(!Status);---->???????????
    /*Returns the ADC1 Master data value of the last converted channel*/
     DataValue = adc_regular_data_read(ADC0); 
    return DataValue; 
} 

void ADC_read(void)
{
	//DBG_I("AD->%d  %d",AD_Data[0],AD_Data[1]);
    DBG_I("AD->%d  %d",AD_Data[0],AD_Data[1]);
}
//REG_TASK(0,1000,ADC_read);

#if 1
int32_t read_adc(uint8_t argc,uint8_t *argv[])
{
	if(argc > 1)
	{
		uint32_t ch = 0;
		uint32_t val = 0;
		sscanf((const char *)argv[1],"%d",&ch);
		
		val = AD_Data[ch];
		DBG_I("Get_val success! ch:%d  val:%d",ch,val);
	}
	return 0;
}
REG_SHELL_CMD(read_adc,0,read_adc,"read_adc","eg:read_adc ch");
#endif 
