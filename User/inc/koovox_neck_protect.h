/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    Koovox_neck_protect.h

*/

#ifndef __Koovox_neck_protect_H
#define __Koovox_neck_protect_H

#include "stm8l15x.h"

uint8_t Koovox_enable_neck_protect(void);
uint8_t Koovox_disable_neck_protect(void);

void Koovox_neck_protect(int16_t acc_x, int16_t acc_y, int16_t acc_z, int16_t g_x, int16_t g_y, int16_t g_z);
void Koovox_neck_protect_event(uint8_t value);


extern bool neck_protect_enable;
extern uint32_t neck_protect_count;

#endif

