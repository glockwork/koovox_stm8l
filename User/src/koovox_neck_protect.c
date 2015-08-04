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

#define ANGLE_INIT_COUNT			(5)

static int16_t angle_x_int = 0;
static int16_t angle_y_int = 0;
static int16_t angle_z_int = 0;
uint8_t angle_init_cnt = 0;


uint32_t neck_protect_count = 0;		// 统计异常坐姿时间
uint32_t neck_perfect_count = 0;		// 统计正确坐姿时间
uint16_t neck_protect_time = NECK_PROTECT_ALARM_TIME;
uint32_t neck_updata_count = 0;

bool neck_protect_lock		= FALSE;	// 异常坐姿锁
bool neck_perfect_lock		= FALSE;	// 正确坐姿锁
bool neck_protect_enable 	= FALSE;


const int16_t sin_table[] = {
17,  34,  52,  69,  87, 104, 121, 139, 156, 173,
190, 207, 224, 241, 258, 275, 292, 309, 325, 342,
358, 374, 390, 406, 422, 438, 453, 469, 484, 500,
515, 529, 544, 559, 573, 587, 601, 615, 629, 642,
656, 669, 681, 694, 707, 719, 731, 743, 754, 766,
777, 788, 798, 809, 819, 829, 838, 848, 857, 866,
874, 882, 891, 898, 906, 913, 920, 927, 933, 939,
945, 951, 956, 961, 965, 970, 974, 978, 981, 984,
987, 990, 992, 994, 996, 997, 998, 999, 999, 1000	
};


/**
  * @Angle_search
  * @param  
  * 		table: the address point of the table
  *		size: the length of the table
  *		key: search the key
  * @retval the angle value
  */
static int8_t Angle_search(const int16_t* table, uint16_t size, int16_t key)
{
	uint16_t low = 0;
	uint16_t high = size - 1;
	uint16_t mid = (low + high) / 2;

	if(key < 0)
		key = -key;
	

	if(key > table[size - 1])
		return (int8_t)90;

	while(low < high)
	{
		if(key > table[mid])
		{
			low = mid + 1;
			mid = (low + high) / 2;
		}
		else if(key < table[mid])
		{
			high = mid - 1;
			mid = (low + high) / 2;
		}
		else
			return (int8_t)mid;
	}

	return (int8_t)low;
}


/****************************************************************************
NAME 
  	KoovoxProtectNeck

DESCRIPTION
 	neck protect
 
RETURNS
  	void
*/ 
static void KoovoxProtectNeck(int16_t axis_x, int16_t axis_y, int16_t axis_z)
{
	int8_t angle_x_curr = 0;
	int8_t angle_y_curr = 0;
	int8_t angle_z_curr = 0;

	angle_x_curr = Angle_search(sin_table, 90, axis_x);
	angle_y_curr = Angle_search(sin_table, 90, axis_y);
	angle_z_curr = 90 - Angle_search(sin_table, 90, axis_z);


	if(angle_init_cnt == ANGLE_INIT_COUNT)
	{
		angle_x_int /= ANGLE_INIT_COUNT;
		angle_y_int /= ANGLE_INIT_COUNT;
		angle_z_int /= ANGLE_INIT_COUNT;

		angle_init_cnt++;
					
	}
	else if(angle_init_cnt < ANGLE_INIT_COUNT)
	{
		angle_init_cnt++;
		angle_x_int += angle_x_curr;
		angle_y_int += angle_y_curr;
		angle_z_int += angle_z_curr;
		return;
	}

	if(axis_x < 0)
	{
		angle_x_curr = - angle_x_curr;
	}

	if(axis_y < 0)
	{
		angle_y_curr = - angle_y_curr;
	}
	
	if(axis_z < 0)
	{
		angle_z_curr = - angle_z_curr;
	}

	if((angle_x_curr - (int8_t)angle_x_int > ANGLE_VALUE_THRESHOLD )
		||(((int8_t)angle_x_int - angle_x_curr) > ANGLE_VALUE_THRESHOLD))
	{
		/********* 启动定时器 ********/
		Koovox_neck_protect_event(TRUE);
	}

	if((angle_y_curr - (int8_t)angle_y_int > ANGLE_VALUE_THRESHOLD )
		||(((int8_t)angle_y_int - angle_y_curr) > ANGLE_VALUE_THRESHOLD))
	{
		/********* 启动定时器 ********/
		Koovox_neck_protect_event(TRUE);
	}

	if((angle_z_curr - (int8_t)angle_z_int > ANGLE_VALUE_THRESHOLD )
		||(((int8_t)angle_z_int - angle_z_curr) > ANGLE_VALUE_THRESHOLD))
	{
		/********* 启动定时器 ********/
		Koovox_neck_protect_event(TRUE);
	}

	/***** 关闭定时器 ****/
	{
		Koovox_neck_protect_event(FALSE);
	}
	
}




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
		
	// 上传各轴加速度值
	if(time_curr - neck_updata_count > NECK_PROTECT_UPDATE_TIME)
	{
		uint16_t sum = g_x*g_x;

		sum += g_y*g_y;
		sum += g_z*g_z;

		sum /= 100;

		if((sum <= HIGH_LEVEL_VALUE)&&(sum >= LOW_LEVEL_VALUE))
		{
			// 计算各轴角度
			KoovoxProtectNeck(axis_x, axis_y, axis_z);
		}

		neck_updata_count = time_curr;
	}

	if(neck_protect_lock)
	{
		// 颈椎提示检测
		if(time_curr - neck_protect_count > neck_protect_time)
		{
			// 上报颈椎保护事件
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
		// 使能异常计时
		if(!neck_protect_lock)
		{
			neck_perfect_lock = FALSE;
			neck_protect_lock = TRUE;

			if(time_curr - neck_perfect_count > NECK_PERFECT_TIME)
			{
				// 获取当前时间
				neck_protect_count = time_curr;
			}
		}

		neck_perfect_count = time_curr;
	}
	else
	{
		// 使能正常计时
		if(!neck_perfect_lock)
		{
			neck_protect_lock = FALSE;
			neck_perfect_lock = TRUE;

			// 获取当前时间
			neck_perfect_count = time_curr;
		}
	}
}




