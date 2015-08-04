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

	/*********** 初始化内部系统时钟 **********/
    CLK_HSICmd(ENABLE);//开始内部高频RC
    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);//HSI为系统时钟
    CLK_SYSCLKDivConfig(CLK_SYSCLK_DIV);	/* 主频 */

	/******* 配置中断唤醒MCU ***********/
	GPIO_Init(GPIOA, GPIO_Pin_4, GPIO_Mode_In_PU_IT);
    EXTI_DeInit (); //恢复中断的所有设置 
    EXTI_SetPinSensitivity (EXTI_Pin_4,EXTI_Trigger_Falling);//外部中断4，下降沿触发

	enableInterrupts();	

	/******** 初始化串口 ****************/
	Init_usart1(19200);

	/*********** 初始化hb_sensor ***************/
	GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Slow);

	/*********** I2C 初始化 ***************/
	LIS3DH_Init();

	/**************** 启动计时定时器 ***************/
	Koovox_enable_time_counter();
	
	while(1)
	{
	
		// 接收串口数据
		Koovox_receive_message();

		// 健康监测
		Koovox_health_monitor();
		
		// 加速度相关计算
		Koovox_calc_accelerate();

		// 发送心率值到CSR8670
		Koovox_send_heart_rate_value();

		// 休眠判断
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



