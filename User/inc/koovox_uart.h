/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    koovox_uart.h

*/

#ifndef __KOOVOX_UART_H
#define __KOOVOX_UART_H

#include "stm8l15x.h"

#define UART_DEBUGx

#define USART_MSG_SIZE		12
#define FRAME_TAIL			"{}"
#define FRAME_SIZE			6

#ifdef UART_DEBUG
#define KOOVOX_UART_DEBUG(x)	{Usart_send_str x;};
#else
#define KOOVOX_UART_DEBUG(x)
#endif


void Init_usart1(uint32_t BaudRate);
uint16_t Koovox_check_frame_tail(const uint8_t* msg, const char* frame_tail, uint16_t size);
void Koovox_send_message(uint8_t* msg, uint8_t len);
void Koovox_receive_message(void);

#ifdef UART_DEBUG
void Usart_send_str(unsigned char *str);

#endif	// UART_DEBUG

#endif	// __KOOVOX_UART_H



