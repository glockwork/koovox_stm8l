/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    koovox_uart.c

*/

#include <string.h>
#include <stdlib.h>

#include "koovox_uart.h"
#include "koovox_message_handle.h"


uint8_t usart_msg[USART_MSG_SIZE] = {0};
uint8_t size = 0;
bool frame_valid = FALSE;

/**
  * @brief  init usart1 
  * @param  None
  * @retval None
  */
void Init_usart1(uint32_t BaudRate)
{
	CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);
	USART_Init(USART1, BaudRate, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, USART_Mode_Tx|USART_Mode_Rx);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
}


/**
  * @brief Koovox_check_frame_tail.
  * @param  
  *		msg:
  *		frame_tail:
  *		size:
  * @retval true is find or false is not find frame tail
  */
uint16_t Koovox_check_frame_tail(const uint8_t* msg, const char* frame_tail, uint16_t size)
{
	uint16_t i = 0, j = 0;

	if((msg == NULL)||(frame_tail == NULL))
		return FALSE;

	while(frame_tail[j])
	{
		if(msg[i++] == frame_tail[j])
			j++;
		else
			j = 0;

		if(i >= size)
			break;
	}
	
	if(frame_tail[j] == '\0')
		return i;
	else
		return 0;
}


/**
* @brief  Koovox_receive_message
* @param  none
* @retval none.
*/
void Koovox_receive_message(void)
{
	if(frame_valid)
	{
		// message handle
		Koovox_message_handle(usart_msg, size);
		size = 0;	
		frame_valid = FALSE;
	}
}

/**
  * @brief Koovox_send_message
  * @param  
  *		msg: the address point of the message
  *		len: the length of the message
  * @retval None
  */
void Koovox_send_message(uint8_t* msg, uint8_t len)
{
	uint8_t i = 0;

	for(; i<len; i++)
	{
		USART_SendData8(USART1, msg[i]);
		while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
	}

	USART_SendData8(USART1, '{');
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
	
	USART_SendData8(USART1, '}');
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));

}


/**
  * @brief USART1 RX / Timer5 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(USART1_RX_TIM5_CC_IRQHandler,28)
{
	disableInterrupts();
	
    USART_ClearITPendingBit (USART1,USART_IT_RXNE);

	if(size >= USART_MSG_SIZE)
		size = 0;
	
	usart_msg[size++] = USART_ReceiveData8(USART1);

	if(size >= FRAME_SIZE)
		frame_valid = TRUE;

	enableInterrupts();
}



#ifdef UART_DEBUG

/**
  * @brief  Usart_send_str
  * @param  str: send string
  * @retval none.
  */
void Usart_send_str(unsigned char *str)
{
	while(*str != 0)
	{
		USART_SendData8(USART1, *str);
		str++;
		while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
	}
}


#endif


