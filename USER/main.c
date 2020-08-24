#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "timer.h"

#include "motor.h"
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
u32 RPM_now;
u32 RPM_pre;
u32 RPM_ref;
float e_pre;
float e_now;
double I_e;
float pwm_out;
double delta_t;
double delta_e;
float kp = 20;
float ki = 0.1;
float kd = 0.1;
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
	TIM3_PWM_Init(7200,0);   //so pwm freq = 72Mhz/900 = 0.08Mhz // for motor the pwm is from 6-16khz
// so 72e6/10e3 =7200	---- pwm freq is 10khz
    TIM5_CH1_Cap_Init(0XFFFF,72-1);		//count at a freq of 1MZ  / it is got by
										//  72MHZ(sys clock) / ((72-1)+1)   = 1Mhz 
	//OXFFFF is the ARR value -- set it to max, so count from zero to 65535
	//so an overflow is in 65535/1 000 000 = 0.065535 seconds 
	
	e_now = 0;
	e_pre=0;
	I_e =0;
	delta_t=0;
	delta_e=0;
	while(1)
    {
        //delay_ms(10);
		//TIM_SetTIM3Compare2(TIM_GetTIM3Capture2()+1); 
		//if(TIM_GetTIM3Capture2()==300)TIM_SetTIM3Compare2(0);  ---- just clear  
		//TIm3 is irrelevant here!!!! ignore 
		
        if(TIM5CH1_CAPTURE_STA&0X80)        //means finish the whole capture, ready to calculate data
		{
			temp=TIM5CH1_CAPTURE_STA&0X3F; //Note OX3F means from bit0 to 5, not the single bit on bit 5
											// so it refers to how many overflows 
			temp*=65535;		 	    	//65536 is OXFFFF	
			temp+=(TIM5CH1_CAPTURE_VAL);      //THE time when the falling edge has occured
			
			printf("HIGH:%lld us\r\n",temp);//print to chuangkou via uart
			
			
			
			//pid part RPM = 60/T
			RPM_now = 60/(temp*1e-6);  //possible bugs here as temp is long
			e_pre = e_now; // a possible bug here
			
			e_now = RPM_ref - RPM_now;  //positive means low speed, so kp is positive
			I_e += e_now;   //ki positive 
			delta_t = temp*1e-6;
			delta_e = (e_now - e_pre)/delta_t;   /// watch sign   ---- possible bugs here 
			//if the gradient is increasing

			pwm_out = kp*e_now + ki*I_e + kd*delta_e;  
			if(pwm_out>1.0)
			{
				pwm_out = 1.0;
			}				
			if(pwm_out<0.0) pwm_out=0.0;	
			TIM_SetTIM3Compare2(pwm_out);  //generate the pwm wave 
			
			TIM5CH1_CAPTURE_STA=0;          //start the next cpature 
		}
    }
}

