/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    Koovox_const_seat.c

*/

#include <stdlib.h>
#include <string.h>

#include "koovox_const_seat.h"
#include "koovox_message_handle.h"
#include "koovox_uart.h"
#include "koovox_lis3dh_sensor.h"


#define CONST_SEAT_ALARM_TIME		((uint16_t)300)	// 5min
#define CONST_SEAT_ALARM_EVENT	((uint8_t)0x01)


bool const_seat_enable 		= FALSE;
uint32_t const_seat_count = 0;
uint16_t const_seat_time = CONST_SEAT_ALARM_TIME;



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

/**
* @brief  Koovox_const_seat
* @param none
* @retval none
*/
void Koovox_const_seat(int8_t axis_x, int8_t axis_y, uint32_t index_acc)
{
	uint16_t curr_value = 0;
	uint32_t time_curr = curr_time;

	curr_value = axis_x*axis_x + axis_y*axis_y;

	if((curr_value > CONST_SEAT_MAX_THRESHOLD) || (curr_value < CONST_SEAT_MIN_THRESHOLD))
	{
		uint8_t value[6] = {0};

		value[0] = curr_value & 0xff;
		value[1] = (curr_value >> 8) & 0xff;
		value[2] = index_acc & 0xff;
		value[3] = (index_acc >> 8) & 0xff;
		value[4] = (index_acc >> 16) & 0xff;
		value[5] = (index_acc >> 24) & 0xff;
		
		// send curr_value and index_acc to csr8670
		Koovox_fill_and_send_packet(ENV, CONST_SEAT, value, 6);
	}


	// 久坐检测
	if(time_curr - const_seat_count >= const_seat_time)
	{
		// 上报久坐提醒事件
		uint8_t value = CONST_SEAT_ALARM_EVENT;
		Koovox_fill_and_send_packet(ENV, CONST_SEAT, &value, 1);
		
		const_seat_count = time_curr;
	}

	
}


