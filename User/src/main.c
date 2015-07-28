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
	uint32_t time = 0;

	/*********** ��ʼ���ڲ�ϵͳʱ�� **********/
    CLK_HSICmd(ENABLE);//��ʼ�ڲ���ƵRC
    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);//HSIΪϵͳʱ��
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_4);	/* 8Mhz ��Ƶ */

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
		
		// ���ٶ���ؼ���
		Koovox_calc_accelerate();

		// ����hb_sensorԭʼ���ݵ�CSR8670
		Koovox_send_hb_adc_value();

		// ����������csr8670
		if(curr_time - time >= 30)
		{
			time = curr_time;
			Koovox_fill_and_send_packet(ENV , SYSTEM, 0, 0);
		}

	}
}





