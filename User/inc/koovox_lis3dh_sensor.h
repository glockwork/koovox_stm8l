/****************************************************************************
 Copyright (C) JoySoft . 2015-2025
 Part of KOOVOX 1.0.1
 @brief  LIS3DH Accelerate Sensor I2C Interface pins

FILE NAME
    koovox_lis3dh_sensor.h
 
*/

#ifndef __KOOVOX_LIS3DH_SENSOR_H
#define __KOOVOX_LIS3DH_SENSOR_H 

#include "stm8l15x.h"

#define ACC_READ_SIZE		6


#define I2C_TIMEOUT         (uint32_t)0xFF /*!< I2C Time out */
  
#define LIS3DH_I2C                         I2C1
#define LIS3DH_I2C_CLK                     CLK_Peripheral_I2C1
#define LIS3DH_I2C_SCL_PIN                 GPIO_Pin_1                  /* PC.01 */
#define LIS3DH_I2C_SCL_GPIO_PORT           GPIOC                       /* GPIOC */
#define LIS3DH_I2C_SDA_PIN                 GPIO_Pin_0                  /* PC.00 */
#define LIS3DH_I2C_SDA_GPIO_PORT           GPIOC                       /* GPIOC */
#define LIS3DH_I2C_SMBUSALERT_PIN          GPIO_Pin_4                  /* PC.04 */
#define LIS3DH_I2C_SMBUSALERT_GPIO_PORT    GPIOC                       /* GPIOC */

#define LIS3DH_I2C_SPEED      	100000 /*!< I2C Speed 100kHz*/

// address of th accelerate sensor
#define LIS3DH_ADDR			0x30	

// WHO AM I
#define WHO_AM_I				0x0f

// STATUS REGISTER
#define LIS3DH_STATUS_REG		0x27

// TEMP_CFG_REG 
#define LIS3DH_TEMP_CFG_REG		0x1f

#define	LIS3DH_FIFO_CTRL_REG	0x2E	/*	FiFo control reg	*/
#define	LIS3DH_INT_CFG1			0x30	/*	interrupt 1 config	*/
#define	LIS3DH_INT_THS1			0x32	/*	interrupt 1 threshold	*/
#define	LIS3DH_INT_DUR1			0x33	/*	interrupt 1 duration	*/


#define	LIS3DH_TT_CFG			0x38	/*	tap config		*/
#define	LIS3DH_TT_THS			0x3A	/*	tap threshold		*/
#define	LIS3DH_TT_LIM			0x3B	/*	tap time limit		*/
#define	LIS3DH_TT_TLAT			0x3C	/*	tap time latency	*/
#define	LIS3DH_TT_TW			0x3D	/*	tap time window		*/

// CONTROL REGISTER 1
#define LIS3DH_CTRL_REG1		0x20
#define LIS3DH_CTRL_REG1_VALUE 	0x37	// 25HZ

#define LIS3DH_CTRL_REG2		0x21
#define LIS3DH_CTRL_REG3		0x22
#define LIS3DH_CTRL_REG5		0x24
#define LIS3DH_CTRL_REG6		0x25

//CONTROL REGISTER 4
#define LIS3DH_CTRL_REG4		0x23
#define LIS3DH_CTRL_REG4_VALUE 	0x0

//OUTPUT REGISTER
#define LIS3DH_ADC_L			0x0c
#define LIS3DH_ADC_H			0x0d


#define LIS3DH_OUT_MUL			0xA8	// multiple read address
#define LIS3DH_OUT_X_L			0x28
#define LIS3DH_OUT_X_H			0x29
#define LIS3DH_OUT_Y_L			0x2A
#define LIS3DH_OUT_Y_H			0x2B
#define LIS3DH_OUT_Z_L			0x2C
#define LIS3DH_OUT_Z_H			0x2D


/** @defgroup koovox_lis3dh_sensor_Private_Functions ****/
void LIS3DH_Init(void);
uint8_t LIS3DH_ReadReg(uint8_t RegName);
void LIS3DH_WriteReg(uint8_t RegName, uint8_t RegValue);
void LIS3DH_ReadAccData(uint8_t* data_buff, uint16_t data_len);
void LIS3DH_Init_Config(void);
void LIS3DH_status(void);


bool Koovox_read_acc_value(uint8_t* data, uint16_t size_data);
void Koovox_calc_accelerate(void);
void Koovox_enable_time_counter(void);


extern uint32_t curr_time;

#endif
