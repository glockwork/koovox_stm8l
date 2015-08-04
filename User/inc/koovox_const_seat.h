/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    koovox_const_seat.h

*/

#ifndef __Koovox_const_seat_H
#define __Koovox_const_seat_H


#include "stm8l15x.h"


#define CONST_SEAT_MIN_THRESHOLD	49
#define CONST_SEAT_MAX_THRESHOLD	120

#define GET_UP_MIN_THRESHOLD	37
#define GET_UP_MAX_THRESHOLD	144
#define GET_UP_VALUE_THRESHOLD	120
#define INDEX_THRESHOLD			10


typedef struct{
	uint8_t status;/* 0:表示上升沿； 1:表示下降沿	*/
	uint8_t max_flag;
	uint8_t max_value;
	uint16_t p_value;/* 前一个加速度矢量和 */
	uint32_t max_index;
}Const_seat_var;




uint8_t Koovox_enable_const_seat(void);
uint8_t Koovox_disable_const_seat(void);
void Koovox_const_seat_event(void);
void Koovox_const_seat(int16_t axis_x, int16_t axis_y, int16_t axis_z,uint32_t index_acc);


extern bool const_seat_enable;
extern uint32_t const_seat_count;

#endif

