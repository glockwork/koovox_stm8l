/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    koovox_hb_sensor.h

*/

#ifndef __KOOVOX_HB_SENSOR_H
#define __KOOVOX_HB_SENSOR_H

#include "stm8l15x.h"

#define HR_SAMPLE_FREQUENCE		20

void Koovox_init_hb_sensor(void);
void Koovox_send_hb_adc_value(void);
uint8_t Koovox_enable_heart_rate(void);
uint8_t Koovox_disable_heart_rate(void);


#endif



