/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    koovox_step_count.h

*/

#ifndef	__KOOVOX_STEP_COUNT_H
#define __KOOVOX_STEP_COUNT_H

#include "stm8l15x.h"

#define STEP_COUNT_MIN_THRESHOLD	82
#define STEP_COUNT_MAX_THRESHOLD	143
#define VALUE_THRESHOLD				15


uint8_t Koovox_enable_step_count(void);
uint8_t Koovox_disable_step_count(void);
void Koovox_step_count(int16_t axis_x, int16_t axis_y, int16_t axis_z, uint32_t index_acc);
void Koovox_step_count_event(void);


extern bool step_count_enable;

#endif

