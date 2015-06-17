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
				
			case CONST_SEAT:		// ������������
				ret = Koovox_enable_const_seat();
				break;

			case NECK_PROTECT:		// ������׵����
				ret = Koovox_enable_neck_protect();
				break;

			case HEAD_ACTION:		// ����ͷ����̬ʶ��
				ret = Koovox_enable_head_action();
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
				
			case CONST_SEAT:		// ֹͣ��������
				ret = Koovox_disable_const_seat();
				break;

			case NECK_PROTECT:		// ֹͣ��׵����
				ret = Koovox_disable_neck_protect();
				break;

			case HEAD_ACTION:		// ֹͣͷ����̬ʶ��
				ret = Koovox_disable_head_action();
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

	case ENV:
		switch(message->obj)
		{
		case CONST_SEAT:
			Koovox_const_seat_event();
			break;
		case NECK_PROTECT:
			Koovox_neck_protect_event(message->data[0]);
			break;
		case STEP_COUNT:
			Koovox_step_count_event();
			break;
		default:
			break;
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
	uint8_t length = sizeof(Usart_msg) + (size_value ? (size_value - 1): 0);
	Usart_msg *msg = (Usart_msg *)malloc(length);

	if(msg == NULL)
		return;

	// fill the packet
	msg->cmd = cmd;
	msg->obj = obj;
	msg->len = size_value;
	if(value)
		memmove(msg->data, value, size_value);
	else
		msg->data[0] = 0x0;

	// send the packet
	Koovox_send_message((uint8_t*)msg, length);
	free(msg);
}



