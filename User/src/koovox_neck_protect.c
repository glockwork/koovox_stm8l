/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    Koovox_neck_protect.c

*/

#include <stdlib.h>
#include <string.h>

#include "koovox_neck_protect.h"
#include "koovox_message_handle.h"
#include "koovox_uart.h"
#include "koovox_lis3dh_sensor.h"
#include "koovox_step_count.h"


#define NECK_PROTECT_ALARM_TIME		((uint16_t)300)	// 5min
#define NECK_PROTECT_ALARM_EVENT	((uint8_t)0x01)
#define NECK_PROTECT_UPDATE_TIME	(2)		// 2s

#define NECK_PERFECT_TIME			(10)

#define HIGH_LEVEL_VALUE	143
#define LOW_LEVEL_VALUE		81

uint32_t neck_protect_count = 0;		// ͳ���쳣����ʱ��
uint32_t neck_perfect_count = 0;		// ͳ����ȷ����ʱ��
uint16_t neck_protect_time = NECK_PROTECT_ALARM_TIME;
uint32_t neck_updata_count = 0;

bool neck_protect_lock		= FALSE;	// �쳣������
bool neck_perfect_lock		= FALSE;	// ��ȷ������
bool neck_protect_enable 	= FALSE;



/**
* @brief  Koovox_enable_neck_protect
* @param 
* @retval none
*/
uint8_t Koovox_enable_neck_protect(void)
{

	if(!neck_protect_enable)
	{
		uint8_t alarm_time = 0;
		
		neck_protect_enable = TRUE;
		Koovox_read_data_from_e2prom(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS, NECK_PROTECT_CFG_OFFSET, &alarm_time, 1);

		if(alarm_time)
			neck_protect_time = alarm_time*60;

		neck_updata_count = curr_time;
		neck_protect_count = curr_time;
		
	}
	else
		return PROCESS;

	return SUC;

}


/**
* @brief  Koovox_disable_neck_protect
* @param 
* @retval none
*/
uint8_t Koovox_disable_neck_protect(void)
{

	if(neck_protect_enable)
	{
		neck_protect_enable = FALSE;
	}
	else
		return PROCESS;

	return SUC;

}


/**
* @brief  Koovox_neck_protect
* @param 
* @retval none
*/
void Koovox_neck_protect(int16_t axis_x, int16_t axis_y, int16_t axis_z, int16_t g_x, int16_t g_y, int16_t g_z)
{
	uint32_t time_curr = curr_time;
		
	// �ϴ�������ٶ�ֵ
	if(time_curr - neck_updata_count > NECK_PROTECT_UPDATE_TIME)
	{
		uint16_t sum = g_x*g_x;

		sum += g_y*g_y;
		sum += g_z*g_z;

		sum /= 100;

		if((sum <= HIGH_LEVEL_VALUE)&&(sum >= LOW_LEVEL_VALUE))
		{
			uint8_t value[6] = {0};
			
			value[0] = axis_x & 0xff;
			value[1] = (axis_x >> 8) & 0xff;
			value[2] = axis_y & 0xff;
			value[3] = (axis_y >> 8) & 0xff;
			value[4] = axis_z & 0xff;
			value[5] = (axis_z >> 8) & 0xff;
			
			Koovox_fill_and_send_packet(ENV, NECK_PROTECT, value, 6);
		}

		neck_updata_count = time_curr;
	}

	if(neck_protect_lock)
	{
		// ��׵��ʾ���
		if(time_curr - neck_protect_count > neck_protect_time)
		{
			// �ϱ���׵�����¼�
			uint8_t value = NECK_PROTECT_ALARM_EVENT;
			Koovox_fill_and_send_packet(ENV, NECK_PROTECT, &value, 1);
		
			neck_protect_count = time_curr;
		}
	}

	Koovox_judge_walk_status();
	
}


/**
* @brief  Koovox_neck_protect_event
* @param  value
* @retval none
*/
void Koovox_neck_protect_event(uint8_t value)
{
	uint32_t time_curr = curr_time;

	if(value)
	{
		// ʹ���쳣��ʱ
		if(!neck_protect_lock)
		{
			neck_perfect_lock = FALSE;
			neck_protect_lock = TRUE;

			if(time_curr - neck_perfect_count > NECK_PERFECT_TIME)
			{
				// ��ȡ��ǰʱ��
				neck_protect_count = time_curr;
			}
		}

		neck_perfect_count = time_curr;
	}
	else
	{
		// ʹ��������ʱ
		if(!neck_perfect_lock)
		{
			neck_protect_lock = FALSE;
			neck_perfect_lock = TRUE;

			// ��ȡ��ǰʱ��
			neck_perfect_count = time_curr;
		}
	}
}




