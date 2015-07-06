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


bool nod_head_enable		= FALSE;
bool shake_head_enable		= FALSE;


/**
* @brief  Koovox_enable_nod_head
* @param none
* @retval none
*/
uint8_t Koovox_enable_nod_head(void)
{
	if(!nod_head_enable)
	{
		nod_head_enable = TRUE;
	}
	else
		return PROCESS;

	return SUC;
}

/**
* @brief  Koovox_disable_nod_head
* @param none
* @retval none
*/
uint8_t Koovox_disable_nod_head(void)
{

	if(nod_head_enable)
	{
		nod_head_enable = FALSE;
	}
	else
		return PROCESS;

	return SUC;
	
}


/**
* @brief  Koovox_disable_nod_head
* @param none
* @retval none
*/
void Koovox_nod_head(uint16_t axis_x, uint16_t axis_y, uint32_t index)
{
	uint8_t value[8] = {0};

#if 1
	//Koovox_fill_value_param(value, axis_x, axis_y, 0xffff, index);
	Koovox_fill_three_param(value, axis_x, axis_y, index);
#else
	value[0] = axis_x & 0xff;
	value[1] = (axis_x >> 8) & 0xff;
	value[2] = axis_y & 0xff;
	value[3] = (axis_y >> 8) & 0xff;
	value[4] = index & 0xff;
	value[5] = (index >> 8) & 0xff;
	value[6] = (index >> 16) & 0xff;
	value[7] = (index >> 24) & 0xff;

#endif
	
	Koovox_fill_and_send_packet(ENV, NOD_HEAD, value, 8);
}

/**
* @brief  Koovox_enable_shake_head
* @param none
* @retval none
*/
uint8_t Koovox_enable_shake_head(void)
{
	if(!shake_head_enable)
	{
		shake_head_enable = TRUE;
	}
	else
		return PROCESS;

	return SUC;
}

/**
* @brief  Koovox_disable_nod_head
* @param none
* @retval none
*/
uint8_t Koovox_disable_shake_head(void)
{
	if(shake_head_enable)
	{
		shake_head_enable = FALSE;
	}
	else
		return PROCESS;

	return SUC;
}

/**
* @brief  Koovox_shake_head
* @param none
* @retval none
*/
void Koovox_shake_head(uint16_t axis_x, uint16_t axis_z, uint32_t index)
{
	uint8_t value[8] = {0};

#if 1
	Koovox_fill_three_param(value, axis_x, axis_z, index);
#else
	value[0] = axis_x & 0xff;
	value[1] = (axis_x >> 8) & 0xff;
	value[2] = axis_z & 0xff;
	value[3] = (axis_z >> 8) & 0xff;
	value[4] = index & 0xff;
	value[5] = (index >> 8) & 0xff;
	value[6] = (index >> 16) & 0xff;
	value[7] = (index >> 24) & 0xff;

#endif
	
	Koovox_fill_and_send_packet(ENV, SHAKE_HEAD, value, 8);
}

