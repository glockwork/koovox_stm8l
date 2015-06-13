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


#define I2C_TEST_SUC	((uint8_t)0x01)
#define I2C_TEST_FAIL	((uint8_t)0x0)

int main( void )
{
	/*********** ��ʼ���ڲ�ϵͳʱ�� **********/
    CLK_HSICmd(ENABLE);//��ʼ�ڲ���ƵRC
    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);//HSIΪϵͳʱ��
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_2);	/* 8Mhz ��Ƶ */

	enableInterrupts();	

	/******** ��ʼ������ ****************/
	Init_usart1(19200);

	/*********** ��ʼ��hb_sensor ***************/
	Koovox_init_hb_sensor();


	/*********** I2C ��ʼ�� ***************/
	LIS3DH_Init();
	LIS3DH_Init_Config();
	

	/*********** I2c test *******************/
	{
		uint8_t value = 0;
		
		if(LIS3DH_ReadReg(WHO_AM_I) == 0x33)
		{
			// i2c is ok
			value = I2C_TEST_SUC;
		}
		else
		{
			// i2c is bad
			value = I2C_TEST_FAIL;
		}
		
		Koovox_fill_and_send_packet(ENV, I2C_TEST, &value, 1);

	}


	while(1)
	{
	
		// ���մ�������
		Koovox_receive_message();
		
		// ���ٶ���ؼ���
		Koovox_calc_accelerate();

		// ����hb_sensorԭʼ���ݵ�CSR8670
		Koovox_send_hb_adc_value();
		
	}

}





