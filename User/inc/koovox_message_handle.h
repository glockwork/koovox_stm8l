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
	HEAD_ACTION		= 0x08,
	STEP_COUNT		= 0x10,
	HEART_RATE		= 0x20,
	I2C_TEST		= 0x40,
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


#endif



