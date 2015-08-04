/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    Koovox_head_action.h

*/

#ifndef __KOOVOX_HEAD_ACTION_H
#define __KOOVOX_HEAD_ACTION_H

#include "stm8l15x.h"

#define HEAD_LEN_MAX_MIN				(3)
#define HEAD_VALUE_THRESHOLD			(5)
#define HEAD_ACTION_CHECK				(3)


/* 求极值的参数 */
typedef struct{
	uint8_t status;/* 0:表示上升沿； 1:表示下降沿	*/
	uint8_t min_flag;
	uint8_t max_flag;
	int16_t min_value;
	int16_t max_value;
	int16_t pre_value;
	uint32_t min_index;
	uint32_t max_index;
}Extremum_param;


uint8_t Koovox_enable_nod_head(void);
uint8_t Koovox_disable_nod_head(void);
void Koovox_nod_head(int16_t axis_x, int16_t axis_y, uint32_t index);

uint8_t Koovox_enable_shake_head(void);
uint8_t Koovox_disable_shake_head(void);
void Koovox_shake_head(int16_t axis_x, int16_t axis_z, uint32_t index);


extern bool nod_head_enable;
extern bool shake_head_enable;


#endif

