/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    main.c

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "koovox_uart.h"
#include "koovox_message_handle.h"
#include "koovox_lis3dh_sensor.h"
#include "koovox_hb_sensor.h"


bool system_sleep_enable = FALSE;

static void Koovox_system_sleep(void);

int main( void )
{

	/*********** ��ʼ���ڲ�ϵͳʱ�� **********/
    CLK_HSICmd(ENABLE);//��ʼ�ڲ���ƵRC
    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);//HSIΪϵͳʱ��
    CLK_SYSCLKDivConfig(CLK_SYSCLK_DIV);	/* ��Ƶ */

	/******* �����жϻ���MCU ***********/
	GPIO_Init(GPIOA, GPIO_Pin_4, GPIO_Mode_In_PU_IT);
    EXTI_DeInit (); //�ָ��жϵ��������� 
    EXTI_SetPinSensitivity (EXTI_Pin_4,EXTI_Trigger_Falling);//�ⲿ�ж�4���½��ش���

	enableInterrupts();	

	/******** ��ʼ������ ****************/
	Init_usart1(19200);

	/*********** ��ʼ��hb_sensor ***************/
	GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Slow);

	/*********** I2C ��ʼ�� ***************/
	LIS3DH_Init();

	/**************** ������ʱ��ʱ�� ***************/
	Koovox_enable_time_counter();
	
	while(1)
	{
	
		// ���մ�������
		Koovox_receive_message();

		// �������
		Koovox_health_monitor();
		
		// ���ٶ���ؼ���
		Koovox_calc_accelerate();

		// ��������ֵ��CSR8670
		Koovox_send_heart_rate_value();

		// �����ж�
		Koovox_system_sleep();

	}
}


/**
* @brief  Koovox_system_sleep
* @param  none
* @retval none
*/
static void Koovox_system_sleep(void)
{
	uint8_t result = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);

	if((!result)&&(!health_monitor_enable)&&(!heart_rate_enable))
	{
		system_sleep_enable = TRUE;
 	}

	if(system_sleep_enable)
	{
		halt();
	}
}


/**
  * @brief External IT PIN4 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI4_IRQHandler,12)
{
	system_sleep_enable = FALSE;
}



