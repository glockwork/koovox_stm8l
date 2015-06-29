/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    koovox_head_action.c

*/
#include <stdlib.h>
#include <string.h>

#include "koovox_head_action.h"
#include "koovox_message_handle.h"


bool head_action_enable		= FALSE;


/**
* @brief  Koovox_enable_head_action
* @param none
* @retval none
*/
uint8_t Koovox_enable_head_action(void)
{
	if(!head_action_enable)
	{
		head_action_enable = TRUE;
	}
	else
		return PROCESS;

	return SUC;
}

/**
* @brief  Koovox_disable_head_action
* @param none
* @retval none
*/
uint8_t Koovox_disable_head_action(void)
{

	if(head_action_enable)
	{
		head_action_enable = FALSE;
	}
	else
		return PROCESS;

	return SUC;
	
}


/**
* @brief  Koovox_disable_head_action
* @param none
* @retval none
*/
void Koovox_head_action(uint16_t axis_x, uint16_t axis_y, uint16_t axis_z)
{
	uint8_t value[6] = {0};

	value[0] = axis_x & 0xff;
	value[1] = (axis_x >> 8) & 0xff;
	value[2] = axis_y & 0xff;
	value[3] = (axis_y >> 8) & 0xff;
	value[4] = axis_z & 0xff;
	value[5] = (axis_z >> 8) & 0xff;
	
	Koovox_fill_and_send_packet(ENV, HEAD_ACTION, value, 6);
}




