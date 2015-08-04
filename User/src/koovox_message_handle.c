/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    koovox_message_handle.c

*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "koovox_lis3dh_sensor.h"
#include "koovox_hb_sensor.h"
#include "koovox_message_handle.h"
#include "koovox_uart.h"

#include "koovox_const_seat.h"
#include "koovox_neck_protect.h"
#include "koovox_head_action.h"
#include "koovox_step_count.h"

#define HEALTH_MONITOR_TIME		(3600)		// 1 hour

bool health_monitor_enable = FALSE;
uint32_t health_monitor_time = 0;


/**
* @brief  Koovox_enable_health_monitor
* @param  none
* @retval none
*/
static uint8_t Koovox_enable_health_monitor(void)
{
	if(!health_monitor_enable)
	{
		uint8_t result = 0;
		
		health_monitor_enable = TRUE;

		Koovox_enable_const_seat();
		Koovox_enable_neck_protect();

		health_monitor_time = curr_time;

		/* �����ٶȴ������ж�������ж��Ƿ����˶�״̬ */
		result = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);
		if(result)
		{
			Koovox_enable_heart_rate();
		}

		return SUC;
	}
	else
		return PROCESS;
}

/**
* @brief  Koovox_disable_health_monitor
* @param  none
* @retval none
*/
static uint8_t Koovox_disable_health_monitor(void)
{
	if(health_monitor_enable)
	{
		health_monitor_enable = FALSE;

		Koovox_disable_const_seat();
		Koovox_disable_neck_protect();

		return SUC;
	}
	else
		return PROCESS;
}

/**
* @brief  Koovox_health_monitor
* @param  none
* @retval none
*/
void Koovox_health_monitor(void)
{
	if(health_monitor_enable)
	{
		uint32_t time_curr = curr_time;
		uint8_t  result = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);
		if((time_curr - health_monitor_time >= HEALTH_MONITOR_TIME)
			&&(result))
		{
			Koovox_enable_heart_rate();
			health_monitor_time = time_curr;
		}

	}
}


/**
* @brief  koovox_message_handle
* @param  
*		msg:the message receive from the csr8670
*		size: the size of the message received
* @retval none
*/
void Koovox_message_handle(uint8_t* msg, uint8_t size)
{
	Usart_msg* message = (Usart_msg*)msg;
	uint8_t ret = SUC;

	if((msg == NULL)||(size < FRAME_SIZE))
	{
		// ����֡����
		Koovox_fill_and_send_packet(CFM, FRAME_ERR, 0, 0);
		return;
	}

	switch(message->cmd)
	{
	case START:
		{			
			switch(message->obj)
			{
			case SYSTEM:
				break;

			case HEALTH_MONITOR:
				ret = Koovox_enable_health_monitor();
				break;
				
			case CONST_SEAT:		// ������������
				ret = Koovox_enable_const_seat();
				break;

			case NECK_PROTECT:		// ������׵����
				ret = Koovox_enable_neck_protect();
				break;

			case NOD_HEAD:		// ������ͷ��̬ʶ��
				ret = Koovox_enable_nod_head();
				break;

			case SHAKE_HEAD:	// ����ҡͷ��̬ʶ��
				ret = Koovox_enable_shake_head();
				break;

			case STEP_COUNT:		// �����Ʋ�����
				ret = Koovox_enable_step_count();
				break;

			case HEART_RATE:		// �������ʼ��
				ret = Koovox_enable_heart_rate();
				break;

			case I2C_TEST:
				LIS3DH_status();
				break;
				
			default:
				break;
			}

		}
		break;
		
	case STOP:
		{
			switch(message->obj)
			{
			case SYSTEM:
				break;

			case HEALTH_MONITOR:
				Koovox_disable_health_monitor();
				break;
				
			case CONST_SEAT:		// ֹͣ��������
				ret = Koovox_disable_const_seat();
				break;

			case NECK_PROTECT:		// ֹͣ��׵����
				ret = Koovox_disable_neck_protect();
				break;

			case NOD_HEAD:		// ֹͣͷ����̬ʶ��
				ret = Koovox_disable_nod_head();
				break;

			case SHAKE_HEAD:	// ֹͣҡͷ��̬ʶ��
				ret = Koovox_disable_shake_head();
				break;

			case STEP_COUNT:		// ֹͣ�Ʋ�����
				ret = Koovox_disable_step_count();
				break;

			case HEART_RATE:		// ֹͣ���ʼ��
				ret = Koovox_disable_heart_rate();
				break;
				
			default:
				break;
			}
		}
		break;
				
	case CNF:	// ��������
		{
			switch(message->obj)
			{
			case CONST_SEAT:
				// ��ȡ���ò�����E2PROM
				Koovox_store_data_to_e2prom(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS, CONST_SEAT_CFG_OFFSET, message->data, 1);
				break;

			case NECK_PROTECT:
				// ��ȡ���ò�����E2PROM
				Koovox_store_data_to_e2prom(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS, NECK_PROTECT_CFG_OFFSET, message->data, 1);
				break;

			default:
				break;
			
			}
		}
		break;

	case GET:
		// ��ȡ����ֵ
		if(message->obj == STEP_COUNT)
		{

		}
		break;
	
	default:
		break;
	
	}

	{
		uint8_t value[2] = {0};

		value[0] = message->cmd;
		value[1] = ret;
		// Ӧ������
		Koovox_fill_and_send_packet(CFM, message->obj, value, 2);
	}

}



/**
* @brief  Koovox_fill_and_send_packet
* @param  
*		cmd:
*		obj: 
*		value:
* @retval none
*/
void Koovox_fill_and_send_packet(uint8_t cmd, uint8_t obj, uint8_t* value, uint8_t size_value)
{
	uint8_t msg[FRAME_MAX_SIZE] = {0};
	uint8_t i = 0;

	// fill the packet
	msg[i++] = cmd;
	msg[i++] = obj;
	msg[i++] = size_value;

	if(value)
	{
		uint8_t j = 0;
		for(; j<size_value; j++ )
			msg[i++] = value[j];
	}
	else
		msg[i++] = 0x0;

	// send the packet
	Koovox_send_message(msg, i);
	
}


void Koovox_fill_three_param(uint8_t* value, uint16_t param1, uint16_t param2, uint32_t param3)
{
	uint8_t i = 0;

	value[i++] = param1 & 0xff;
	value[i++] = (param1 >> 8) & 0xff;

	if(param2 != 0x7fff)
	{
		value[i++] = param2 & 0xff;
		value[i++] = (param2 >> 8) & 0xff;
	}

	value[i++] = param3 & 0xff;
	value[i++] = (param3 >> 8) & 0xff;
	value[i++] = (param3 >> 16) & 0xff;
	value[i++] = (param3 >> 24) & 0xff;
	
}


