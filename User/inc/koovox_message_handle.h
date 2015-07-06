/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    koovox_message_handle.h

*/

#ifndef __KOOVOX_MESSAGE_HANDLE_H
#define __KOOVOX_MESSAGE_HANDLE_H

#include "stm8l15x.h"

#define FRAME_MIN_SIZE	4
#define FRAME_MAX_SIZE	45

typedef enum{
	START = 0x01,
	STOP,
	CFM,	// ”¶¥√¸¡Ó
	CNF,	// ≤Œ ˝≈‰÷√
	GET,
	ENV		
}enumCmd;

typedef enum{
	SYSTEM 			= 0x01,
	CONST_SEAT 		= 0x02,
	NECK_PROTECT 	= 0x04,
	NOD_HEAD		= 0x08,
	SHAKE_HEAD		= 0x10,
	STEP_COUNT		= 0x20,
	HEART_RATE		= 0x40,
	I2C_TEST		= 0x80,
	FRAME_ERR		= 0xff
}enumObj;


typedef enum{
	SUC,
	ERR,
	PROCESS
}enumFrameDetail;



typedef struct{
	uint8_t cmd;
	uint8_t obj;
	uint8_t len;
	uint8_t data[1];
}Usart_msg;

void Koovox_message_handle(uint8_t* msg, uint8_t size);
void Koovox_fill_and_send_packet(uint8_t cmd, uint8_t obj, uint8_t* value, uint8_t size_value);
void Koovox_fill_three_param(uint8_t* value, uint16_t param1, uint16_t param2, uint32_t param3);

#endif



