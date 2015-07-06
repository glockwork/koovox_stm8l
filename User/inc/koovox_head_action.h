/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    Koovox_head_action.h

*/

#ifndef __KOOVOX_HEAD_ACTION_H
#define __KOOVOX_HEAD_ACTION_H

#include "stm8l15x.h"

uint8_t Koovox_enable_nod_head(void);
uint8_t Koovox_disable_nod_head(void);
void Koovox_nod_head(uint16_t axis_x, uint16_t axis_y, uint32_t index);

uint8_t Koovox_enable_shake_head(void);
uint8_t Koovox_disable_shake_head(void);
void Koovox_shake_head(uint16_t axis_x, uint16_t axis_z, uint32_t index);


extern bool nod_head_enable;
extern bool shake_head_enable;


#endif

