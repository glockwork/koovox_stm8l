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



uint8_t Koovox_enable_const_seat(void);
uint8_t Koovox_disable_const_seat(void);
void Koovox_const_seat_event(void);
void Koovox_const_seat(int16_t axis_x, int16_t axis_y, int16_t axis_z,uint32_t index_acc);


extern bool const_seat_enable;
extern uint32_t const_seat_count;

#endif

