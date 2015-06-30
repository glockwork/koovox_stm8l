/****************************************************************************
 Copyright (C) JoySoft . 2015-2025
 Part of KOOVOX 1.0.1
 @brief  LIS3DH Accelerate Sensor I2C Interface pins

FILE NAME
    koovox_lis3dh_sensor.c
 
*/

//#include <stdlib.h>
//#include <string.h>

#include "koovox_lis3dh_sensor.h"
#include "koovox_uart.h"
#include "koovox_message_handle.h"

#include "koovox_neck_protect.h"
#include "koovox_head_action.h"
#include "koovox_const_seat.h"
#include "koovox_step_count.h"


#define ACC_LSD		((int16_t)1638)
#define LIS3DH_TEST_SUC	((uint8_t)0x01)
#define LIS3DH_TEST_FAIL	((uint8_t)0x0)


uint32_t index_acc = 0;
uint32_t curr_time  = 0;


/** @defgroup koovox_lis3dh_sensor_Private_Functions ****/

/**
  * @brief  Initializes the LIS3DH_I2C..
  * @param  None
  * @retval None
  */
static void LIS3DH_LowLevel_Init(void)
{
  /*!< LIS3DH_I2C Periph clock enable */
  CLK_PeripheralClockConfig(LIS3DH_I2C_CLK, ENABLE);

  /*!< Configure LIS3DH_I2C pins: SCL */
  GPIO_Init(LIS3DH_I2C_SCL_GPIO_PORT, LIS3DH_I2C_SCL_PIN, GPIO_Mode_In_PU_No_IT);

  /*!< Configure LIS3DH_I2C pins: SDA */
  GPIO_Init(LIS3DH_I2C_SDA_GPIO_PORT, LIS3DH_I2C_SDA_PIN, GPIO_Mode_In_PU_No_IT);

  /* Configure PC.4 as Input pull-up, used as TemperatureSensor_INT */
  GPIO_Init(LIS3DH_I2C_SMBUSALERT_GPIO_PORT, LIS3DH_I2C_SMBUSALERT_PIN, GPIO_Mode_In_FL_No_IT);
}


/**
  * @brief  DeInitializes the LIS3DH_I2C.
  * @param  None
  * @retval None
  */
static void LIS3DH_LowLevel_DeInit(void)
{
  /*!< Disable LIS3DH_I2C */
  I2C_Cmd(LIS3DH_I2C, DISABLE);
  /*!< DeInitializes the LIS3DH_I2C */
  I2C_DeInit(LIS3DH_I2C);

  /*!< LIS3DH_I2C Periph clock disable */
  CLK_PeripheralClockConfig(LIS3DH_I2C_CLK, DISABLE);

  /*!< Configure LIS3DH_I2C pins: SCL */
  GPIO_Init(LIS3DH_I2C_SCL_GPIO_PORT, LIS3DH_I2C_SCL_PIN, GPIO_Mode_In_PU_No_IT);

  /*!< Configure LIS3DH_I2C pins: SDA */
  GPIO_Init(LIS3DH_I2C_SDA_GPIO_PORT, LIS3DH_I2C_SDA_PIN, GPIO_Mode_In_PU_No_IT);

  /*!< Configure LIS3DH_I2C pin: SMBUS ALERT */
  GPIO_Init(LIS3DH_I2C_SMBUSALERT_GPIO_PORT, LIS3DH_I2C_SMBUSALERT_PIN, GPIO_Mode_In_FL_No_IT);
}


/**
  * @brief  DeInitializes the LIS3DH_I2C.
  * @param  None
  * @retval None
  */
void LIS3DH_DeInit(void)
{
  LIS3DH_LowLevel_DeInit();
}


/**
  * @brief  Initializes the LIS3DH_I2C.
  * @param  None
  * @retval None
  */
void LIS3DH_Init(void)
{

  LIS3DH_LowLevel_Init();

  /* I2C DeInit */
  I2C_DeInit(LIS3DH_I2C);

  /* I2C configuration */
  I2C_Init(LIS3DH_I2C, LIS3DH_I2C_SPEED, 0x00, I2C_Mode_SMBusHost,
           I2C_DutyCycle_2, I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);

  /*!< LIS3DH_I2C Init */
  I2C_Cmd(LIS3DH_I2C, ENABLE);
}

/**
  * @brief  Write to the specified register of the LIS3DH.
  * @param  RegName: specifies the LIS3DH register to be written.
  * @param  RegValue: value to be written to LIS3DH register.
  * @retval None
  */
void LIS3DH_WriteReg(uint8_t RegName, uint8_t RegValue)
{
	uint32_t I2C_TimeOut = I2C_TIMEOUT;

  /*-------------------------------- Transmission Phase -----------------------*/
  /* Send LIS3DH_I2C START condition */
  I2C_GenerateSTART(LIS3DH_I2C, ENABLE);

  /* Test on LIS3DH_I2C EV5 and clear it */
  while ((!I2C_CheckEvent(LIS3DH_I2C, I2C_EVENT_MASTER_MODE_SELECT))&&(I2C_TimeOut))  /* EV5 */
  {
	  I2C_TimeOut--;
  }

  /* Send STLIS3DH slave address for write */
  I2C_Send7bitAddress(LIS3DH_I2C, LIS3DH_ADDR, I2C_Direction_Transmitter);

  I2C_TimeOut = I2C_TIMEOUT;

  /* Test on LIS3DH_I2C EV6 and clear it */
  while ((!I2C_CheckEvent(LIS3DH_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))&&(I2C_TimeOut)) /* EV6 */
  {
	  I2C_TimeOut--;
  }

  /* Send the specified register data pointer */
  I2C_SendData(LIS3DH_I2C, RegName);

  I2C_TimeOut = I2C_TIMEOUT;

  /* Test on LIS3DH_I2C EV8 and clear it */
  while ((!I2C_CheckEvent(LIS3DH_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))&&(I2C_TimeOut)) /* EV8 */
  {
	  I2C_TimeOut--;
  }

  /* Send LIS3DH_I2C data */
  I2C_SendData(LIS3DH_I2C, RegValue );

  I2C_TimeOut = I2C_TIMEOUT;

  /* Test on LIS3DH_I2C EV8 and clear it */
  while ((!I2C_CheckEvent(LIS3DH_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))&&(I2C_TimeOut)) /* EV8 */
  {
	  I2C_TimeOut--;
  }

  /* Send LIS3DH_I2C STOP Condition */
  I2C_GenerateSTOP(LIS3DH_I2C, ENABLE);

}

/**
  * @brief  Read Axis register of LIS3DH
  * @param  :
  *		data_buff:the receive buff address of payload
  *		data_len: the length of receive data
  * @retval None.
  */
void LIS3DH_ReadAccData(uint8_t regAdrress, uint8_t* data_buff, uint16_t data_len)
{

  uint16_t i = 0;
  uint32_t I2C_TimeOut = I2C_TIMEOUT;

  /* Enable LIS3DH_I2C acknowledgement if it is already disabled by other function */
  I2C_AcknowledgeConfig(LIS3DH_I2C, ENABLE);

  /*------------------------------------- Transmission Phase ------------------*/
  /* Send LIS3DH_I2C START condition */
  I2C_GenerateSTART(LIS3DH_I2C, ENABLE);

  /* Test on LIS3DH_I2C EV5 and clear it */
  while ((!I2C_CheckEvent(LIS3DH_I2C, I2C_EVENT_MASTER_MODE_SELECT))&&(I2C_TimeOut))  /* EV5 */
  {
	  I2C_TimeOut--;
  }

  /* Send STLIS3DH slave address for write */
  I2C_Send7bitAddress(LIS3DH_I2C, LIS3DH_ADDR, I2C_Direction_Transmitter);

  I2C_TimeOut = I2C_TIMEOUT;

  /* Test on LIS3DH_I2C EV6 and clear it */
  while ((!I2C_CheckEvent(LIS3DH_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))&&(I2C_TimeOut)) /* EV6 */
  {
	  I2C_TimeOut--;
  }

  /* Send the temperature register data pointer */
  I2C_SendData(LIS3DH_I2C, regAdrress);
  
  I2C_TimeOut = I2C_TIMEOUT;
  
  /* Test on LIS3DH_I2C EV8 and clear it */
  while ((!I2C_CheckEvent(LIS3DH_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))&&(I2C_TimeOut)) /* EV8 */
  {
	  I2C_TimeOut--;
  }

  /*-------------------------------- Reception Phase --------------------------*/
  /* Send Re-STRAT condition */
  I2C_GenerateSTART(LIS3DH_I2C, ENABLE);

  I2C_TimeOut = I2C_TIMEOUT;

  /* Test on EV5 and clear it */
  while ((!I2C_CheckEvent(LIS3DH_I2C, I2C_EVENT_MASTER_MODE_SELECT))&&(I2C_TimeOut))  /* EV5 */
  {
	  I2C_TimeOut--;
  }

  /* Send STLIS3DH slave address for read */
  I2C_Send7bitAddress(LIS3DH_I2C, LIS3DH_ADDR, I2C_Direction_Receiver);

  I2C_TimeOut = I2C_TIMEOUT;
  /* Test on EV6 and clear it */
  while ((!I2C_CheckEvent(LIS3DH_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))&&(I2C_TimeOut))	/* EV6 */
  {
	  I2C_TimeOut--;
  }
  
  for(i = 0; i < (data_len - 1); i++)
  {
	  I2C_TimeOut = I2C_TIMEOUT;
	  /* Test on EV6 and clear it */
	  while ((!I2C_CheckEvent(LIS3DH_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))&&(I2C_TimeOut))  /* EV7 */
	  {
		  I2C_TimeOut--;
	  }

	  /* Store LIS3DH_I2C received data */
	  data_buff[i] = I2C_ReceiveData(LIS3DH_I2C) ;
  }

  /* Disable LIS3DH_I2C acknowledgement */
  I2C_AcknowledgeConfig(LIS3DH_I2C, DISABLE);

  /* Send LIS3DH_I2C STOP Condition */
  I2C_GenerateSTOP(LIS3DH_I2C, ENABLE);

  I2C_TimeOut = I2C_TIMEOUT;

  /* Test on RXNE flag */
  while ((I2C_GetFlagStatus(LIS3DH_I2C, I2C_FLAG_RXNE) == RESET)&&(I2C_TimeOut))
  {
	I2C_TimeOut--;
  }

	/* Store LIS3DH_I2C received data */
	data_buff[i] = I2C_ReceiveData(LIS3DH_I2C) ;

}

/**
  * @brief  Initializes the LIS3DH config register.
  * @param  None
  * @retval None
  */
void LIS3DH_Init_Config(void)
{
	// set CTRL_REG1
	LIS3DH_WriteReg(LIS3DH_CTRL_REG1, LIS3DH_CTRL_REG1_VALUE);

	// set CTRL_REG4
	LIS3DH_WriteReg(LIS3DH_CTRL_REG4, LIS3DH_CTRL_REG4_VALUE);
}

/**
  * @brief  LIS3DH_status
  * @param  None
  * @retval None
  */
void LIS3DH_status(void)
{
	uint8_t value = 0;

	LIS3DH_ReadAccData(WHO_AM_I, &value, 1);
	if(value == 0x33)
	{
		// i2c is ok
		value = LIS3DH_TEST_SUC;
	}
	else
	{
		// i2c is bad
		value = LIS3DH_TEST_FAIL;
		
	}
	
	Koovox_fill_and_send_packet(ENV, I2C_TEST, &value, 1);
}



/**
* @brief  Koovox_ad_to_acc
* @param  
*		ad_value : adc value
* @retval accelerate value
*/
static int16_t Koovox_ad_to_acc(uint16_t ad_value)
{
	int16_t acc_value = 0, temp = 0, value = 0;
	uint8_t i = 0;

	value = (int16_t)ad_value;
	
	for(; i<3; i++)
	{
		temp = value / ACC_LSD;
		value %= ACC_LSD;
		value *= 10;
		acc_value = acc_value*10 + temp;
	}

	return acc_value;

}

/**
* @brief  Koovox_convert_acc_value (实际加速度的1000倍)
* @param  
*		ad_data:
*		axis_x:
*		axis_y:
*		axis_z:
* @retval none
*/
static void Koovox_convert_acc_value(uint8_t* ad_data, int16_t* axis_x, int16_t* axis_y, int16_t* axis_z)
{
	uint16_t ad_x, ad_y, ad_z;
	ad_x = (uint16_t)ad_data[0] + ((uint16_t)ad_data[1] << 8);
	ad_y = (uint16_t)ad_data[2] + ((uint16_t)ad_data[3] << 8);
	ad_z = (uint16_t)ad_data[4] + ((uint16_t)ad_data[5] << 8);

	*axis_x = Koovox_ad_to_acc(ad_x);
	*axis_y = Koovox_ad_to_acc(ad_y);
	*axis_z = Koovox_ad_to_acc(ad_z);
	
}

/**
* @brief  Koovox_ad_to_acc
* @param  
*		ad_value : adc value
* @retval accelerate value
*/
bool Koovox_read_acc_value(uint8_t* data, uint16_t size_data)
{
	bool result = TRUE;
	uint8_t ret = 0;
	
	LIS3DH_ReadAccData(LIS3DH_STATUS_REG, &ret, 1);
	if(ret >= 0x0f)
	{
		LIS3DH_ReadAccData(LIS3DH_OUT_MUL, data, size_data);
	}
	else
	{
		result = FALSE;
	}

	return result;
}


/**
* @brief  Koovox_calc_accelerate
* @param  none
* @retval none.
*/
void Koovox_calc_accelerate(void)
{
	uint8_t acc_data[ACC_READ_SIZE] = {0};

	if(!step_count_enable)
	{
		//return;
	}

	if(Koovox_read_acc_value(acc_data, ACC_READ_SIZE))
	{
		
		int16_t axis_x = 0, axis_y = 0, axis_z = 0;
		int16_t acc_x , acc_y, acc_z;
			
		// 将原始AD采样值转换为重力加速度值	(加速度的1000倍)
		Koovox_convert_acc_value(acc_data, &axis_x, &axis_y, &axis_z);

		acc_x = axis_x / 10;
		acc_y = axis_y / 10;
		acc_z = axis_z / 10;

		index_acc++;
		
		// 计步功能
		if(step_count_enable)
		{
			Koovox_step_count(acc_x, acc_y, acc_z,index_acc);
		}

		// 颈椎保护
		if(neck_protect_enable)
		{
			Koovox_neck_protect(axis_x, axis_y, axis_z, acc_x, acc_y, acc_z);
		}

		// 久坐提醒
		if(const_seat_enable)
		{
			Koovox_const_seat(acc_x, acc_y, acc_z,index_acc);
		}

		// 头部动作识别
		if(head_action_enable)
		{
			Koovox_head_action(acc_x, acc_y, acc_z);
		}		
	}
}


/**
* @brief  Koovox_calc_accelerate
* @param  none
* @retval none.
*/
void Koovox_enable_time_counter(void)
{
	// 初始化定时器
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);
	TIM3_TimeBaseInit(TIM3_Prescaler_128, TIM3_CounterMode_Up,0xf424);	//  1000ms
	TIM3_ClearFlag(TIM3_FLAG_Update);
	TIM3_ITConfig(TIM3_IT_Update, ENABLE);
	
	TIM3_Cmd(ENABLE);
}


/**
  * @brief Timer3 Update/Overflow/Trigger/Break Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQHandler,21)
{	
	disableInterrupts();

	TIM3_ClearFlag(TIM3_FLAG_Update);

	curr_time++;

	enableInterrupts();

}



