/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    koovox_head_action.c

*/
#include <stdlib.h>
#include <string.h>

#include "koovox_head_action.h"
#include "koovox_message_handle.h"


bool nod_head_enable		= FALSE;
bool shake_head_enable		= FALSE;


Extremum_param axis_x_var;
Extremum_param axis_y_var;
Extremum_param axis_z_var;

/****************************************************************************
NAME 
  	Koovox_fill_min_param

DESCRIPTION
 	free the head action business variable
 
RETURNS
  	void
*/ 
static void Koovox_fill_min_param(Extremum_param* param)
{
	param->status = 0;
	param->min_flag = 0;
	param->max_flag = 1;
	param->min_value = 0;
	param->max_value = 0;
	param->pre_value = 0;
	param->min_index = 0;
	param->max_index = 0;
}

/****************************************************************************
NAME 
  	Koovox_fill_max_param

DESCRIPTION
 	free the head action business variable
 
RETURNS
  	void
*/ 
static void Koovox_fill_max_param(Extremum_param* param)
{
	param->status = 0;
	param->min_flag = 1;
	param->max_flag = 0;
	param->min_value = 0;
	param->max_value = 0;
	param->pre_value = 10;
	param->min_index = 0;
	param->max_index = 0;
}


/****************************************************************************
NAME 
  	Koovox_find_max_min_status

DESCRIPTION
 	free the head action business variable
 
RETURNS
  	void
*/ 
static void Koovox_find_max_min_value(uint32_t index, int16_t curr_value, Extremum_param* param)
{
	uint8_t status = param->status;
	uint8_t min_flag = param->min_flag;
	uint8_t max_flag = param->max_flag;
	int16_t pre_value = param->pre_value;
	
	if(curr_value > pre_value)
	{
		/* 检测极小值 */
		if(status)
		{
			param->status = 0;/* 0:表示上升沿； 1:表示下降沿 */
			
			if(max_flag)
			{
				uint32_t max_index = param->max_index;
				int16_t max_value = param->max_value;
				/* 存取极小值*/
				if((index - max_index > HEAD_LEN_MAX_MIN)&&(max_value - pre_value >= HEAD_VALUE_THRESHOLD))
				{
					param->min_flag = 1;
					param->max_flag = 0;
					param->min_index = index;
					param->min_value = pre_value;
				}
			}
			else
			{
				int16_t min_value = param->min_value;
				
				/* 更新极小值*/
				if(pre_value <= min_value)
				{
					param->min_value = pre_value;
					param->min_index = index;
				}
			}
			
		}
		
	}
	else if(curr_value < pre_value)
	{
		/* 检测极大值 */
		if(!status)
		{
			param->status = 1;/* 0:表示上升沿； 1:表示下降沿 */ 

			if(min_flag)
			{
				uint32_t min_index = param->min_index;
				int16_t min_value = param->min_value;
				/* 存取极大值*/
				if((index - min_index > HEAD_LEN_MAX_MIN)&&(pre_value - min_value >= HEAD_VALUE_THRESHOLD))
				{
					param->min_flag = 0;
					param->max_flag = 1;
					param->max_index = index;
					param->max_value = pre_value;					
				}			
			}
			else
			{
				int16_t max_value = param->max_value;
				
				/* 更新极大值*/
				if(pre_value + 1>= max_value)
				{
					param->max_value = pre_value;
					param->max_index = index;
				}
			}
			
		}
		
	}
	else
	{
		int16_t min_value = param->min_value;
		int16_t max_value = param->max_value;
		
		if(curr_value >= max_value)
		{
			/* 更新极大值*/
			param->max_value = pre_value;
			param->max_index = index;			
		}
		else if(curr_value <= min_value)
		{
			/* 更新极小值*/
			param->min_value = pre_value;
			param->min_index = index;			
		}
	}
	
}


/**
* @brief  Koovox_enable_nod_head
* @param none
* @retval none
*/
uint8_t Koovox_enable_nod_head(void)
{
	if(!nod_head_enable)
	{
		nod_head_enable = TRUE;

		Koovox_fill_min_param(&axis_x_var);

		Koovox_fill_max_param(&axis_y_var);
		
	}
	else
		return PROCESS;

	return SUC;
}

/**
* @brief  Koovox_disable_nod_head
* @param none
* @retval none
*/
uint8_t Koovox_disable_nod_head(void)
{

	if(nod_head_enable)
	{
		nod_head_enable = FALSE;
	}
	else
		return PROCESS;

	return SUC;
	
}

/**
* @brief  Koovox_disable_nod_head
* @param none
* @retval none
*/
void Koovox_nod_head(int16_t axis_x, int16_t axis_y, uint32_t index)
{
	axis_x /= 10;
	axis_y /= 10;

	/* find the axis_x min value */
	Koovox_find_max_min_value(index, axis_x, &axis_x_var);
	
	/* find the axis_y max value */
	Koovox_find_max_min_value(index, axis_y, &axis_y_var);
	
	axis_x_var.pre_value = axis_x;
	axis_y_var.pre_value = axis_y;
	
	
	/* check the nod head action */
	if((axis_x_var.min_flag)&&(axis_y_var.max_flag))
	{
		uint32_t x_index = axis_x_var.min_index;
		uint32_t y_index = axis_y_var.max_index;
	
		if((x_index < y_index + HEAD_ACTION_CHECK)
			||(y_index < x_index + HEAD_ACTION_CHECK))
		{
	
			/* 接听电话 */
			
		}
		else
		{
			axis_x_var.min_flag = 0;
			axis_y_var.max_flag = 0;
		}
	}
}

/**
* @brief  Koovox_enable_shake_head
* @param none
* @retval none
*/
uint8_t Koovox_enable_shake_head(void)
{
	if(!shake_head_enable)
	{
		shake_head_enable = TRUE;

		Koovox_fill_min_param(&axis_x_var);

		Koovox_fill_max_param(&axis_z_var);
	}
	else
		return PROCESS;

	return SUC;
}

/**
* @brief  Koovox_disable_nod_head
* @param none
* @retval none
*/
uint8_t Koovox_disable_shake_head(void)
{
	if(shake_head_enable)
	{
		shake_head_enable = FALSE;
	}
	else
		return PROCESS;

	return SUC;
}

/**
* @brief  Koovox_shake_head
* @param none
* @retval none
*/
void Koovox_shake_head(int16_t axis_x, int16_t axis_z, uint32_t index)
{
	axis_x /= 10;
	axis_z /= 10;

	/* find the axis_x min value */
	Koovox_find_max_min_value(index, axis_x, &axis_x_var);
	
	/* find the axis_z max value */
	Koovox_find_max_min_value(index, axis_z, &axis_z_var);
	
	axis_x_var.pre_value = axis_x;
	axis_z_var.pre_value = axis_z;
	
	
	/* check the shake head action */
	if((axis_x_var.min_flag)&&(axis_z_var.max_flag))
	{
		uint32_t x_index = axis_x_var.min_index;
		uint32_t z_index = axis_z_var.max_index;
	
		if((x_index < z_index + HEAD_ACTION_CHECK)
			||(z_index < x_index + HEAD_ACTION_CHECK))
		{
	
			/* 挂断电话 */
			
		}
		else
		{
			axis_x_var.min_flag = 0;
			axis_z_var.max_flag = 0;
		}
	}
}

