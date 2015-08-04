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
Acc_step_var step_var = {0, 1, 1, 100, 100, 100, 0, 0};
uint32_t koovox_step = 0;


/**
* @brief  Koovox_enable_step_count
* @param  
* @retval none
*/
uint8_t Koovox_enable_step_count(void)
{
	if(!step_count_enable)
	{
		uint8_t step[4] = {0};
		
		step_count_enable = TRUE;
		
		LIS3DH_Init_Config();

		// 从E2PROM中读取步数值
		Koovox_read_data_from_e2prom(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS, STEP_VALUE_OFFSET, step, 4);

		koovox_step += (uint32_t)step[0];
		koovox_step += (uint32_t)step[1] << 8;
		koovox_step += (uint32_t)step[2] << 16;
		koovox_step += (uint32_t)step[3] << 24;
		
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
		uint8_t step[4] = {0};
		
		step_count_enable = FALSE;

		step_var.status = 0;
		step_var.index_max = 0;
		step_var.index_min = 0;
		step_var.max_flag = 1;
		step_var.min_flag = 1;
		step_var.min_value = 100;
		step_var.max_value = 100;
		step_var.pre_value = 100;

		step[0] = koovox_step & 0xff;
		step[1] = (koovox_step >> 8) & 0xff;
		step[2] = (koovox_step >> 16) & 0xff;
		step[3] = (koovox_step >> 24) & 0xff;

		// 存储步数值到E2PROM
		Koovox_store_data_to_e2prom(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS, STEP_VALUE_OFFSET, step, 4);

		koovox_step = 0;
	}
	else
		return PROCESS;

	return SUC;
}

/****************************************************************************
NAME 
  	Koovox_calc_step_value

DESCRIPTION
 	count the step value
 
RETURNS
  	void
*/ 
static void Koovox_calc_step_value(uint16_t value, uint32_t index)
{

	{
		uint16_t curr_value = value;
		uint32_t curr_index = index;
		
		uint8_t status = step_var.status;
		uint16_t pre_value = step_var.pre_value;
		
		if(curr_value > pre_value)
		{
			/*检测极小值*/
			if(status)
			{
				uint16_t min_value = step_var.min_value;
				step_var.status = 0;/*设置为上升沿*/

				if(step_var.max_flag)
				{
					uint32_t index_min = step_var.index_min;
					if((pre_value < STEP_COUNT_MIN_THRESHOLD)&&(curr_index - index_min >= TIME_THRESHOLD))
					{
						step_var.max_flag = 0;
						step_var.min_flag = 1;
						step_var.min_value = pre_value;
						step_var.index_min = curr_index;
					}
				}
				else
				{
					/* 更新极小值 */
					if(pre_value < min_value)
					{
						step_var.min_value = pre_value;
						step_var.index_min = curr_index;
					}
				}
		
			}
		}
		else
		{
			/*检测极大值*/
			if(!status)
			{
				uint16_t max_value = step_var.max_value;
				step_var.status = 1; /*设置为下降沿*/

				if(step_var.min_flag)
				{
					uint32_t index_max = step_var.index_max;

					if((pre_value > STEP_COUNT_MAX_THRESHOLD)&&(curr_index - index_max >= TIME_THRESHOLD))
					{
						koovox_step++;
						step_var.max_flag = 1;
						step_var.min_flag = 0;
						step_var.index_max = curr_index;
						
						/**********久坐计时清零**********/
						Koovox_step_count_event();
					}
					
				}
				else
				{
					/* 更新极大值 */
					if(pre_value > max_value)
					{
						step_var.max_value = pre_value;
						step_var.index_max = curr_index;
					}
					
				}
				
			}
		}
		
		step_var.pre_value = curr_value;
	}
	
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
		/* 计步计算 */
		Koovox_calc_step_value(sum_value, index_acc);
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


