/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    Koovox_const_seat.c

*/

//#include <stdlib.h>
//#include <string.h>

#include "koovox_const_seat.h"
#include "koovox_message_handle.h"
#include "koovox_uart.h"
#include "koovox_lis3dh_sensor.h"
#include "koovox_step_count.h"


#define CONST_SEAT_ALARM_TIME		((uint16_t)300)	// 5min
#define CONST_SEAT_ALARM_EVENT	((uint8_t)0x01)


bool const_seat_enable 		= FALSE;
uint32_t const_seat_count = 0;
uint16_t const_seat_time = CONST_SEAT_ALARM_TIME;
Const_seat_var seat_var = {0, 0, 0, 100, 0};



/**
* @brief  Koovox_enable_const_seat
* @param none
* @retval none
*/
uint8_t Koovox_enable_const_seat(void)
{
	if(!const_seat_enable)
	{
		uint8_t alarm_time = 0;
		
		const_seat_enable = TRUE;
		const_seat_count = curr_time;
		
		Koovox_read_data_from_e2prom(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS, CONST_SEAT_CFG_OFFSET, &alarm_time, 1);

		if(alarm_time)
			const_seat_time = alarm_time*60;
		
	}
	else
		return PROCESS;

	return SUC;
}

/**
* @brief  Koovox_disable_const_seat
* @param none
* @retval none
*/
uint8_t Koovox_disable_const_seat(void)
{
	if(const_seat_enable)
	{
		const_seat_enable = FALSE;
		
		seat_var.max_flag = 0;
		seat_var.max_value = 0;
		seat_var.p_value = 100;
		seat_var.status = 0;
		seat_var.max_index = 0;
	}
	else
		return PROCESS;

	return SUC;
}

/**
* @brief  Koovox_const_seat_event
* @param  
* @retval none
*/
void Koovox_const_seat_event(void)
{
	const_seat_count = curr_time;
}

/****************************************************************************
NAME 
  	KoovoxConstSeat

DESCRIPTION
 	const seat prompt
 
RETURNS
  	void
*/ 
static void KoovoxConstSeat(uint16_t value, uint32_t index)
{
	/* 完成起身动作检测 */
	uint16_t curr_value = value;
	uint16_t pre_value = seat_var.p_value;
	uint32_t curr_index = index;
	uint8_t status = seat_var.status;

	seat_var.p_value = curr_value;

	if(curr_value > pre_value)
	{
		/* 极小值检测 */
		if(status)
		{
			/* 设置为上升沿 */
			seat_var.status = 0;

			if((pre_value < GET_UP_MIN_THRESHOLD)&&
				(seat_var.max_flag)&&
				(seat_var.max_value - pre_value > GET_UP_VALUE_THRESHOLD)&&
				(curr_index - seat_var.max_index < INDEX_THRESHOLD))
			{
				/* 检测到起身动作 */
				Koovox_const_seat_event();
			}
		}
	}
	else
	{
		/* 极大值检测 */
		if(!status)
		{
			/* 设置为下降沿 */
			seat_var.status = 1;

			if(pre_value > GET_UP_MAX_THRESHOLD)
			{
				seat_var.max_flag = 1;
				seat_var.max_index = curr_index;
				seat_var.max_value = pre_value;
			}
		}
	}
	
}



/**
* @brief  Koovox_const_seat
* @param none
* @retval none
*/
void Koovox_const_seat(int16_t axis_x, int16_t axis_y, int16_t axis_z, uint32_t index_acc)
{
	uint16_t curr_value = 0;
	uint32_t time_curr = curr_time;

	curr_value = axis_x*axis_x ;
	curr_value += axis_y*axis_y;
	curr_value += axis_z*axis_z;

	curr_value /= 100;

	if((curr_value > CONST_SEAT_MAX_THRESHOLD) || (curr_value < CONST_SEAT_MIN_THRESHOLD))
	{
		// 久坐检测
		KoovoxConstSeat(curr_value, index_acc);
		
	}


	// 久坐检测
	if(time_curr - const_seat_count >= const_seat_time)
	{
		// 上报久坐提醒事件
		uint8_t value = CONST_SEAT_ALARM_EVENT;
		Koovox_fill_and_send_packet(ENV, CONST_SEAT, &value, 1);
		
		const_seat_count = time_curr;
	}

	Koovox_judge_walk_status();
}


