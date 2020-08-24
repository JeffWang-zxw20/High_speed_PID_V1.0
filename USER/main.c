#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "timer.h"
/************************************************
 ALIENTEK 战舰STM32F103开发板 实验10
 输入捕获实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

extern u8  TIM5CH1_CAPTURE_STA;		//输入捕获状态		    				
extern u32	TIM5CH1_CAPTURE_VAL;	//输入捕获值 

int main(void)
{
	long long temp=0; 
	
    HAL_Init();                    	 	//初始化HAL库    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	uart_init(115200);					// chuangkou
	LED_Init();							//初始化LED	
    //TIM3_PWM_Init(500-1,72-1);       	//72M/72=1M的计数频率，自动重装载为500，那么PWM频率为1M/500=2kHZ
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

