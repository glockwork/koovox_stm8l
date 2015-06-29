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

int main( void )
{
	/*********** 初始化内部系统时钟 **********/
    CLK_HSICmd(ENABLE);//开始内部高频RC
    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);//HSI为系统时钟
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_2);	/* 8Mhz 主频 */

	enableInterrupts();	

	/******** 初始化串口 ****************/
	Init_usart1(19200);

	/*********** 初始化hb_sensor ***************/
	GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Slow);


	/*********** I2C 初始化 ***************/

	/*!< LIS3DH_I2C Periph clock enable */
	CLK_PeripheralClockConfig(LIS3DH_I2C_CLK, ENABLE);
		
	/* Configure PC.4 as Input pull-up, used as TemperatureSensor_INT */
	GPIO_Init(LIS3DH_I2C_SMBUSALERT_GPIO_PORT, LIS3DH_I2C_SMBUSALERT_PIN, GPIO_Mode_In_FL_No_IT);

	I2C_DeInit(LIS3DH_I2C);	
	I2C_Init(LIS3DH_I2C, LIS3DH_I2C_SPEED, 0x00, I2C_Mode_SMBusHost,
			 I2C_DutyCycle_2, I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);
	I2C_Cmd(LIS3DH_I2C, ENABLE);
	
	while(1)
	{
	
		// 接收串口数据
		Koovox_receive_message();
		
		// 加速度相关计算
		Koovox_calc_accelerate();

		// 发送hb_sensor原始数据到CSR8670
		Koovox_send_hb_adc_value();
		
	}

}





