#include "timer.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������
//��ʱ���ж���������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2019/9/17
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
//�޸�˵��
//V1.1 20170526
//����TIM3_PWM_Init����,����PWM���
//V1.2 20170526
//����TIM5_CH1_Cap_Init����,�������벶��
////////////////////////////////////////////////////////////////////////////////// 
TIM_HandleTypeDef 	TIM3_Handler;      	//��ʱ����� 
TIM_OC_InitTypeDef 	TIM3_CH2Handler;		//��ʱ��3ͨ��2���

//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Init(u16 arr,u16 psc)
{  
    TIM3_Handler.Instance=TIM3;                          //ͨ�ö�ʱ��3
    TIM3_Handler.Init.Prescaler=psc;                     //��Ƶϵ��  //determines the clock, the time diff bet tk and tk+1
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM3_Handler.Init.Period=arr;                        //�Զ�װ��ֵ  //arr determines the period of CNT counter
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    HAL_TIM_Base_Init(&TIM3_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM3_Handler); // start interrupt
}

//TIM3 PWM���ֳ�ʼ�� 
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
void TIM3_PWM_Init(u16 arr,u16 psc)
{  
    TIM3_Handler.Instance=TIM3;          	//��ʱ��3
    TIM3_Handler.Init.Prescaler=psc;       //��ʱ����Ƶ
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;//���ϼ���ģʽ
    TIM3_Handler.Init.Period=arr;          //�Զ���װ��ֵ
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM3_Handler);       //��ʼ��PWM
    
    TIM3_CH2Handler.OCMode=TIM_OCMODE_PWM1; //ģʽѡ��PWM1
    TIM3_CH2Handler.Pulse=arr/2;            //���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�Ĭ�ϱȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50%
    TIM3_CH2Handler.OCPolarity=TIM_OCPOLARITY_LOW; //����Ƚϼ���Ϊ�� 
    HAL_TIM_PWM_ConfigChannel(&TIM3_Handler,&TIM3_CH2Handler,TIM_CHANNEL_2);//����TIM3ͨ��2
	
    HAL_TIM_PWM_Start(&TIM3_Handler,TIM_CHANNEL_2);//����PWMͨ��2
	 	   
}

//��ʱ���ײ�����������ʱ�ӣ������ж����ȼ�
//�˺����ᱻHAL_TIM_Base_Init()��������
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //ʹ��TIM3ʱ��
		HAL_NVIC_SetPriority(TIM3_IRQn,1,3);    //�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //����ITM3�ж�   
	}
}

//��ʱ���ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_TIM_PWM_Init()����
//htim:��ʱ�����
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_Initure;
	
    if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();			//ʹ�ܶ�ʱ��3
		__HAL_AFIO_REMAP_TIM3_PARTIAL();		//TIM3ͨ�����Ų�����ӳ��ʹ��
		__HAL_RCC_GPIOB_CLK_ENABLE();			//����GPIOBʱ��
		
		GPIO_Initure.Pin=GPIO_PIN_5;           	//PB5
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	//�����������
		GPIO_Initure.Pull=GPIO_PULLUP;          //����
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//����
		HAL_GPIO_Init(GPIOB,&GPIO_Initure); 	
	}
}

//����TIMͨ��2��ռ�ձ�
//compare:�Ƚ�ֵ
void TIM_SetTIM3Compare2(u32 compare)
{
	TIM3->CCR2=compare; 
}

//��ȡTIM����/�ȽϼĴ���ֵ
u32 TIM_GetTIM3Capture2(void)
{
	return  HAL_TIM_ReadCapturedValue(&TIM3_Handler,TIM_CHANNEL_2);
}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}

////�ص���������ʱ���жϷ���������
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//    if(htim==(&TIM3_Handler))
//    {
//        LED1=!LED1;        //LED1��ת
//    }
//}

/***************************************************************************
****************************************************************************
  ���������벶�����Դ��ʵ����غ���Դ��
****************************************************************************
****************************************************************************/

TIM_HandleTypeDef TIM5_Handler;         //��ʱ��5���

//��ʱ��5ͨ��1���벶������
//arr���Զ���װֵ(TIM5��16λ��!!)
//psc��ʱ��Ԥ��Ƶ��
void TIM5_CH1_Cap_Init(u32 arr,u16 psc)
{  
    TIM_IC_InitTypeDef TIM5_CH1Config;  //Tim init func specific for capture pwm mode
										//this init func will use HAL_TIM_IC_MspInit to callback it ??
    
    TIM5_Handler.Instance=TIM5;                          //ͨ�ö�ʱ��5
    TIM5_Handler.Init.Prescaler=psc;                     //��Ƶϵ��
    TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM5_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    HAL_TIM_IC_Init(&TIM5_Handler);						//init cpature para
    
    TIM5_CH1Config.ICPolarity=TIM_ICPOLARITY_RISING;    //  capture when rising
    TIM5_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//map to TI1  yes, directly from T11 to IC1	
    TIM5_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;          // We should set to 1, pub interruput for every one rising edge 
    TIM5_CH1Config.ICFilter=1;                          //don't filter input wave  ---- set to 0010
    HAL_TIM_IC_ConfigChannel(&TIM5_Handler,&TIM5_CH1Config,TIM_CHANNEL_1);//Tim5 has four channels, we set up ch1
	
    HAL_TIM_IC_Start_IT(&TIM5_Handler,TIM_CHANNEL_1);   //init ch1 capture intterrupt
    __HAL_TIM_ENABLE_IT(&TIM5_Handler,TIM_IT_UPDATE);   //enable update interrupt,, so we can count how many overflow happens during the counting and hence determines the period
	
	HAL_NVIC_SetPriority(TIM5_IRQn,2,0);    //Set be interrupt priority
    HAL_NVIC_EnableIRQ(TIM5_IRQn);          //open TIM5 interrupt ch
}

//��ʱ��5�ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_TIM_IC_Init()����
//htim:��ʱ��5���
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)  //this init the call back func 
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_TIM5_CLK_ENABLE();            // enable time5 
    __HAL_RCC_GPIOA_CLK_ENABLE();			// enable gpioA
	
    GPIO_Initure.Pin=GPIO_PIN_0;            //PA0
    GPIO_Initure.Mode=GPIO_MODE_AF_INPUT; 	//  input mode 
    GPIO_Initure.Pull=GPIO_PULLDOWN;        // 
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;// high speed
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);   //init GPIOA0

    HAL_NVIC_SetPriority(TIM5_IRQn,2,0);    //set interrupt priority
    HAL_NVIC_EnableIRQ(TIM5_IRQn);          // enable interrupt channel 
}

//����״̬
//[7]:0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
//[6]:0,��û���񵽵͵�ƽ;1,�Ѿ����񵽵͵�ƽ��.
//[5:0]:����͵�ƽ������Ĵ���
u8  TIM5CH1_CAPTURE_STA=0;							//capture state, 
//ox40 -> 0100 0000  means captured the rising edge
//ox80 -> 1000 0000 means whole capture has completed(falling edge got)
//  bit 5to bit0   record ARR reached number after the rising edged has been captured . 

u16	TIM5CH1_CAPTURE_VAL;							//capture the value of CNT counter when the faling edge has been captured 
//the value of CNT counter when the rising edge is set to zero as in fig15.1.1, the counter is immediatedly reset to count from zero

void TIM5_IRQHandler(void)   //interrupt service func
{
	HAL_TIM_IRQHandler(&TIM5_Handler);				//��ʱ�����ô�����
}
 
//��ʱ�������жϣ�����������жϴ���ص������� �ú�����HAL_TIM_IRQHandler�лᱻ����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
	// function when CNT reach ARR!!!!!since it is a callback func!!!can't be called when volt is in low volt platum.
{ //this function is delt with the case that
	//1.the rising edge has already been detected but falling edge has not 

	if((TIM5CH1_CAPTURE_STA&0X80)==0)				// if not capture the falling edge //pid--- not capture the second rising edge
	{
		if(TIM5CH1_CAPTURE_STA&0X40)				// if capture high volt rising edge already  ---- pid captured first rising edge before
		{
			if((TIM5CH1_CAPTURE_STA&0X3F)==0X3F)	//3F = 63   it is the bit 5(the sixth bit) 
													//if this bit is 1 means too many overflow has occured 
													//beyond what it can remember
			{
				TIM5CH1_CAPTURE_STA|=0X80;			//80 =128  , set capture once  ---- unlikely ---- ignore  
				TIM5CH1_CAPTURE_VAL=0XFFFF;
			}else TIM5CH1_CAPTURE_STA++;  //  can still remember , ok then since this callback func is called for a overflow
											//then just remember one overflow
											// since we just add one, it will only affect bit 0 to 5 in most cases
										//so it will not affect the capture state flag at bit 6 and 7
		}	 
	}		
}

//The following func is called when a capture is detected --- either falling or rising 
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//capture handler ---- call when IRQH is used 
{
	if((TIM5CH1_CAPTURE_STA&0X80)==0)				//not cpature the whole high volt range
	{
		if(TIM5CH1_CAPTURE_STA&0X40)				//means the rising edge has already been captured before, so this time, we have captured the falling edge
		{	  										//
			//FOR pid, this means we now have just captured the second rising edge
			
			TIM5CH1_CAPTURE_STA|=0X80;				//Flag capture the whole high vol
            TIM5CH1_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM5_Handler,TIM_CHANNEL_1);//GET CAPTURE VALUE!!!!!!
			TIM_RESET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1);   //RESET!
            TIM_SET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);//Reset to cpature rising edge
																						//ready to do the next capture
		}else  										//means this capture is rising edge, we need immediately set count to zero
													//so that we can count how long it is.
		{
			TIM5CH1_CAPTURE_STA=0;					//SET status to zero 
			TIM5CH1_CAPTURE_VAL=0;					//SET capture value to zero
			TIM5CH1_CAPTURE_STA|=0X40;				//flag that captured the rising edge
			__HAL_TIM_DISABLE(&TIM5_Handler);      	//close timer 5 
			__HAL_TIM_SET_COUNTER(&TIM5_Handler,0);
			TIM_RESET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1);   //clear setting 
			//TIM_SET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);//reset to capture the falling edge
			
			//for pid, set this to capture rising edge again
			TIM_SET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);//reset to capture the falling edge
			__HAL_TIM_ENABLE(&TIM5_Handler);		//open timer 5 again.
		}		    
	}		
}

