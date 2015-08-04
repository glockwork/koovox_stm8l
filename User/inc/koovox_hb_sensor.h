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

#define HB_MIN_POINT         8
#define HB_MAX_POINT         25
#define HB_MAX_MIN_LEN		 4
#define HB_MIN_MAX_LEN		 2
#define HB_POINT_THRESHOLD	 2

#define HR_SAMPLE_POINT_NUM 120
#define HR_SAMPLE_TIMES     50
#define HR_SAMPLE_PERIOD    (HR_SAMPLE_TIMES*1000) /*50ms, so we get 20 samples per second.*/
#define HR_SAMPLE_FREQUENCE	20

#define  HB_VALIDE_POINT	 5
#define	 HB_MAX_BUFF_SIZE	 16
#define  HB_MIN_VALUE    	 50
#define  HB_MAX_VALUE    	 150



uint8_t Koovox_enable_heart_rate(void);
uint8_t Koovox_disable_heart_rate(void);

void Koovox_send_heart_rate_value(void);

extern bool heart_rate_enable;

#endif



