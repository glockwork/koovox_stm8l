/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    koovox_step_count.h

*/

#ifndef	__KOOVOX_STEP_COUNT_H
#define __KOOVOX_STEP_COUNT_H

#include "stm8l15x.h"

#define STEP_COUNT_MIN_THRESHOLD	49
#define STEP_COUNT_MAX_THRESHOLD	120


uint8_t Koovox_enable_step_count(void);
uint8_t Koovox_disable_step_count(void);
void Koovox_step_count(int8_t axis_x, int8_t axis_y, uint32_t index_acc);
void Koovox_step_count_event(void);


extern bool step_count_enable;

#endif

