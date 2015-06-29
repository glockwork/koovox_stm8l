/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    koovox_hb_sensor.c

*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "koovox_uart.h"
#include "koovox_hb_sensor.h"
#include "koovox_message_handle.h"


uint16_t ad_hb[HR_SAMPLE_FREQUENCE] = {0};
uint16_t index = 0;
bool hb_adc_valid = FALSE;
bool heart_rate_enable		= FALSE;

#if 0
/**
* @brief  Koovox_init_hb_sensor	
* @param  none
* @retval none.
*/
void Koovox_init_hb_sensor(void)
{
	GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Slow);
}

/**
* @brief  Koovox_enable_hb_sensor	
* @param  none
* @retval none.
*/
void Koovox_enable_hb_sensor(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_2);
}

/**
* @brief  Koovox_enable_hb_sensor	
* @param  none
* @retval none.
*/
void Koovox_disable_hb_sensor(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_2);
}

/**
* @brief  Koovox_init_adc
* @param  none 
* @retval none
*/
static void Koovox_init_adc(void)
{
	CLK_PeripheralClockConfig (CLK_Peripheral_ADC1,ENABLE);//开启ADC/USART时钟

	ADC_Init (ADC1,ADC_ConversionMode_Single,ADC_Resolution_12Bit,ADC_Prescaler_1);
	ADC_Cmd(ADC1,ENABLE);
	ADC_ChannelCmd (ADC1,ADC_Channel_18,ENABLE);	
}

/**
* @brief  Koovox_disable_adc
* @param  none 
* @retval none
*/
static void Koovox_disable_adc(void)
{
	ADC_ChannelCmd (ADC1,ADC_Channel_18,DISABLE);		
	ADC_Cmd(ADC1, DISABLE);
}

/**
* @brief  Koovox_enable_hb_sample_timer (TIM2)
* @param  none
* @retval none
*/
static void Koovox_enable_hb_sample_timer(void)
{
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
	TIM2_TimeBaseInit(TIM2_Prescaler_64, TIM2_CounterMode_Up,0x186A);	// 50ms
	TIM2_ClearFlag(TIM2_FLAG_Update);
	TIM2_ITConfig(TIM2_IT_Update, ENABLE);

	TIM2_Cmd(ENABLE);	

	index = 0;
}

/**
* @brief  Koovox_disable_hb_sample_timer (TIM2)
* @param  none
* @retval none
*/
static void Koovox_disable_hb_sample_timer(void)
{
	TIM2_Cmd(DISABLE);
}
#endif


/**
* @brief  Koovox_enable_heart_rate
* @param  none
* @retval none
*/
uint8_t Koovox_enable_heart_rate(void)
{
	if(!heart_rate_enable)
	{
		// 初始化ADC
		CLK_PeripheralClockConfig (CLK_Peripheral_ADC1,ENABLE);//开启ADC/USART时钟
		
		ADC_Init (ADC1,ADC_ConversionMode_Single,ADC_Resolution_12Bit,ADC_Prescaler_1);
		ADC_Cmd(ADC1,ENABLE);
		ADC_ChannelCmd (ADC1,ADC_Channel_18,ENABLE);	

		// 启动传感器
		GPIO_SetBits(GPIOB, GPIO_Pin_2);

		// 启动定时器采集数据
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
		TIM2_TimeBaseInit(TIM2_Prescaler_64, TIM2_CounterMode_Up,0x186A);	// 50ms
		TIM2_ClearFlag(TIM2_FLAG_Update);
		TIM2_ITConfig(TIM2_IT_Update, ENABLE);
		
		TIM2_Cmd(ENABLE);	
		
		index = 0;
		
		heart_rate_enable = TRUE;
	}
	else
		return PROCESS;

	return SUC;
}

/**
* @brief  Koovox_enable_heart_rate
* @param  none
* @retval none
*/
uint8_t Koovox_disable_heart_rate(void)
{
	if(heart_rate_enable)
	{
		// 关闭心率采样定时器
		TIM2_Cmd(DISABLE);

		// 关闭心率传感器
		GPIO_ResetBits(GPIOB, GPIO_Pin_2);

		// 关闭ADC
		ADC_ChannelCmd (ADC1,ADC_Channel_18,DISABLE);		
		ADC_Cmd(ADC1, DISABLE);
		
		heart_rate_enable = FALSE;
		hb_adc_valid = FALSE;
	}
	else
		return PROCESS;

	return SUC;
}


/**
* @brief  Koovox_receive_message
* @param  none
* @retval none.
*/
void Koovox_send_hb_adc_value(void)
{
	if(hb_adc_valid)
	{
		// send heart rate adc value to csr8670
		uint8_t length = 2*HR_SAMPLE_FREQUENCE;
		uint16_t value[HR_SAMPLE_FREQUENCE] = {0};
		uint8_t i = 0;

		disableInterrupts();
		for(; i < HR_SAMPLE_FREQUENCE; i++)
			value[i] = ad_hb[i];
		
		index = 0;
		enableInterrupts();

		Koovox_fill_and_send_packet(ENV , HEART_RATE, (uint8_t*)value, length);
	
		hb_adc_valid = FALSE;
	}
}



/**
  * @brief TIM2 Update/Overflow/Trigger/Break /USART2 TX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler,19)
{
	disableInterrupts();
	
	TIM2_ClearFlag(TIM2_FLAG_Update);

	ADC_SoftwareStartConv (ADC1);	
	while(!ADC_GetFlagStatus (ADC1,ADC_FLAG_EOC));

	ADC_ClearFlag (ADC1,ADC_FLAG_EOC);

	if(index == HR_SAMPLE_FREQUENCE)
		index = HR_SAMPLE_FREQUENCE - 1;

	ad_hb[index++] = ADC_GetConversionValue (ADC1);

	if(index == HR_SAMPLE_FREQUENCE)
		hb_adc_valid = TRUE;

	enableInterrupts();
	
}




