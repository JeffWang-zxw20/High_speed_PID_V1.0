#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "timer.h"
/************************************************
 ALIENTEK ս��STM32F103������ ʵ��10
 ���벶��ʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

extern u8  TIM5CH1_CAPTURE_STA;		//���벶��״̬		    				
extern u32	TIM5CH1_CAPTURE_VAL;	//���벶��ֵ 

int main(void)
{
	long long temp=0; 
	
    HAL_Init();                    	 	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					// chuangkou
	LED_Init();							//��ʼ��LED	
    //TIM3_PWM_Init(500-1,72-1);       	//72M/72=1M�ļ���Ƶ�ʣ��Զ���װ��Ϊ500����ôPWMƵ��Ϊ1M/500=2kHZ
	//Time3 here is pwm input 
	TIM3_PWM_Init(899,0);   //so pwm freq = 72Mhz/900 = 0.08Mhz  
    TIM5_CH1_Cap_Init(0XFFFF,72-1);		//count at a freq of 1MZ  / it is got by
										//  72MHZ(sys clock) / ((72-1)+1)   = 1Mhz 
	//OXFFFF is the ARR value -- set it to max, so count from zero to 65535
	//so an overflow is in 65535/1 000 000 = 0.065535 seconds 
	
	while(1)
    {
        delay_ms(10);
		TIM_SetTIM3Compare2(TIM_GetTIM3Capture2()+1); 
		if(TIM_GetTIM3Capture2()==300)TIM_SetTIM3Compare2(0);   
		//TIm3 is irrelevant here!!!! ignore 
		
        if(TIM5CH1_CAPTURE_STA&0X80)        //means finish the whole capture, ready to calculate data
		{
			temp=TIM5CH1_CAPTURE_STA&0X3F; //Note OX3F means from bit0 to 5, not the single bit on bit 5
											// so it refers to how many overflows 
			temp*=65535;		 	    	//65536 is OXFFFF	
			temp+=(TIM5CH1_CAPTURE_VAL);      //THE time when the falling edge has occured
			
			printf("HIGH:%lld us\r\n",temp);//print to chuangkou via uart
			TIM5CH1_CAPTURE_STA=0;          //start the next cpature 
		}
    }
}

