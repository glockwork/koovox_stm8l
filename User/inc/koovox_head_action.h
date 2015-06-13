/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    koovox_head_action.h

*/

#ifndef __KOOVOX_HEAD_ACTION_H
#define __KOOVOX_HEAD_ACTION_H

#include "stm8l15x.h"

uint8_t Koovox_enable_head_action(void);
uint8_t Koovox_disable_head_action(void);
void Koovox_head_action(uint8_t axis_x, uint8_t axis_y, uint8_t axis_z);


extern bool head_action_enable;


#endif

