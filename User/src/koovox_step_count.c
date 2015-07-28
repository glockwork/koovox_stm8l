/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    koovox_step_count.c

*/

#include "koovox_step_count.h"
#include "koovox_lis3dh_sensor.h"
#include "koovox_message_handle.h"
#include "koovox_uart.h"
#include "koovox_const_seat.h"
#include "koovox_neck_protect.h"


#include <stdlib.h>
#include <string.h>

#define STEP_STATE_CHECK_TIME	10	// 10 s
#define STEP_THRESHOLD			5

bool step_count_enable  	= FALSE;
uint16_t step_count = 0;
uint32_t step_time_cnt = 0;

/**
* @brief  Koovox_enable_step_count
* @param  
* @retval none
*/
uint8_t Koovox_enable_step_count(void)
{
	if(!step_count_enable)
	{
		step_count_enable = TRUE;
		
		LIS3DH_Init_Config();
	}
	else
		return PROCESS;

	return SUC;
}

/**
* @brief  Koovox_disable_step_count
* @param  
* @retval none
*/
uint8_t Koovox_disable_step_count(void)
{
	if(step_count_enable)
	{
		step_count_enable = FALSE;
	}
	else
		return PROCESS;

	return SUC;
}



/**
* @brief  count step
* @param  
* 		axis_x: the accelerate of the axis_x
*		axis_y: the accelerate of the axis_y
*		index_acc:
* @retval none
*/
void Koovox_step_count(int16_t axis_x, int16_t axis_y, int16_t axis_z, uint32_t index_acc)
{
	uint16_t sum_value = 0, step_value = 0;

	sum_value = axis_x*axis_x;
	sum_value += axis_y*axis_y;
	step_value = sum_value;
	sum_value += axis_z*axis_z;

	sum_value /= 100;
	step_value /= 100;

	if((sum_value > STEP_COUNT_MAX_THRESHOLD) || (sum_value < STEP_COUNT_MIN_THRESHOLD))
	{
		uint8_t value[6] = {0};

		Koovox_fill_three_param(value, step_value, 0x7fff, index_acc);
		
		// send curr_value and index_acc to csr8670
		Koovox_fill_and_send_packet(ENV, STEP_COUNT, value, 6);
	}
		
}


/**
* @brief Koovox_step_count_event
* @param  
* 		value: 
* @retval none
*/
void Koovox_step_count_event(void)
{	
	if(step_count == 0)
	{
		step_time_cnt = curr_time;
	}

	step_count++;
}


/**
* @brief Koovox_step_count_event
* @param  
* 		value: 
* @retval none
*/
void Koovox_judge_walk_status(void)
{
	uint32_t time_curr = curr_time;

	if(time_curr - step_time_cnt >= STEP_STATE_CHECK_TIME)
	{
		// 走路状态判断
		if(step_count >= STEP_THRESHOLD)
		{
			// 清零颈椎计时和久坐计时
			neck_protect_count = time_curr;
			const_seat_count = time_curr;
		}

		// 重新判断走路状态
		step_count = 0;
	}
}


