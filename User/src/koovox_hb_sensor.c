/****************************************************************************
Copyright (C) JoySoft . 2015-2025
Part of KOOVOX 1.0.1

FILE NAME
    koovox_hb_sensor.c

*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "koovox_uart.h"
#include "koovox_hb_sensor.h"
#include "koovox_message_handle.h"


uint16_t ad_hb[HR_SAMPLE_FREQUENCE] = {0};
uint16_t hb_calc_buff[HR_SAMPLE_POINT_NUM] = {0};
uint16_t index = 0;
bool hb_adc_valid = FALSE;
bool heart_rate_enable		= FALSE;



/**
* @brief  Koovox_enable_heart_rate
* @param  none
* @retval none
*/
uint8_t Koovox_enable_heart_rate(void)
{
	if(!heart_rate_enable)
	{
		// ��ʼ��ADC
		CLK_PeripheralClockConfig (CLK_Peripheral_ADC1,ENABLE);//����ADC/USARTʱ��
		
		ADC_Init (ADC1,ADC_ConversionMode_Single,ADC_Resolution_12Bit,ADC_Prescaler_1);
		ADC_Cmd(ADC1,ENABLE);
		ADC_ChannelCmd (ADC1,ADC_Channel_18,ENABLE);	

		// ����������
		GPIO_SetBits(GPIOB, GPIO_Pin_2);

		// ������ʱ���ɼ�����
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
		TIM2_TimeBaseInit(TIM2_PRESCALER, TIM2_CounterMode_Up,0x186A);	// 50ms
		TIM2_ClearFlag(TIM2_FLAG_Update);
		TIM2_ITConfig(TIM2_IT_Update, ENABLE);
		
		TIM2_Cmd(ENABLE);	
		
		index = 0;
		
		heart_rate_enable = TRUE;
	}
	else
		return PROCESS;

	return SUC;
}

/**
* @brief  Koovox_enable_heart_rate
* @param  none
* @retval none
*/
uint8_t Koovox_disable_heart_rate(void)
{
	if(heart_rate_enable)
	{
		// �ر����ʲ�����ʱ��
		TIM2_Cmd(DISABLE);

		// �ر����ʴ�����
		GPIO_ResetBits(GPIOB, GPIO_Pin_2);

		// �ر�ADC
		ADC_ChannelCmd (ADC1,ADC_Channel_18,DISABLE);		
		ADC_Cmd(ADC1, DISABLE);
		
		heart_rate_enable = FALSE;
		hb_adc_valid = FALSE;
	}
	else
		return PROCESS;

	return SUC;
}

/**
  * @brief  calc heart rate value
  * @param  
  * 		data: the peload of the data
  *		len: the length of the data
  * @retval heart rate value
  */
static uint8_t Koovox_heartrate_calc(uint16_t* data, uint16_t len)
{
	
	uint8_t hb_max_buff[HB_MAX_BUFF_SIZE] = {0};
	uint8_t hb_point_buff[HB_MAX_BUFF_SIZE] = {0};
	uint8_t temp = 0;
	uint8_t count_index = 0;
	
	uint8_t i, j;

	/************ Ѱ�Ҽ���ֵ����ȡ����ֵ������ *****************/
	{
		uint8_t status = 0; 	/* 0:��ʾ�����أ� 1:��ʾ�½���*/	
		uint8_t max_flag = 1;
		uint8_t min_flag = 1;
		int8_t max_index = -(HB_MIN_POINT);
		int8_t min_index = -(HB_MIN_POINT);
		uint16_t max_value = 0;
		uint16_t min_value = 4096;
		
		uint16_t curr_data = 0;
		uint16_t prev_data = *data;
		int8_t curr_index = 0;

		for(i=1; i<len; i++)
		{
			curr_data = *(data + i);
			curr_index = i;

			if(curr_data > prev_data)
			{
				/* ��Сֵ���*/
				if(status)
				{
					status = 0;	/*����Ϊ������*/

					if(max_flag)
					{
						/* ��ȡ��Сֵ*/
						if((curr_index > (HB_MAX_MIN_LEN + max_index))&&(curr_index >(HB_MIN_POINT + min_index)))
						{
							max_flag = 0;
							min_flag = 1;
							min_value = prev_data;
							min_index = curr_index;
						}
					}
					else
					{
						/* ���¼�Сֵ*/
						if((prev_data <= min_value)&&(curr_index <= (HB_MIN_POINT + min_index)))
						{
							min_index = curr_index;
							min_value = prev_data;
						}
					}
						
				}
			}
			else 
			{
				/* ����ֵ���*/
				if(!status)
				{
					status = 1; /*����Ϊ�½���*/

					if(min_flag)
					{
						/* ��ȡ����ֵ*/
						if((curr_index > (HB_MIN_MAX_LEN + min_index))&&(curr_index > (HB_MIN_POINT + max_index)))
						{
							min_flag = 0;
							max_flag = 1;
							hb_max_buff[count_index++] = curr_index - max_index;
							max_index = curr_index;
							max_value = prev_data;
						}			
					}
					else
					{
						temp = curr_index - max_index;
						/* ���¼���ֵ*/
						if((max_value <= prev_data)&&(temp <= HB_MIN_POINT))
						{
							count_index--;
							temp += hb_max_buff[count_index];
							hb_max_buff[count_index++] = temp;
							max_index = curr_index;
							max_value = prev_data;
						}
					}
				}
			}

			prev_data = curr_data;
		}
	}

	/* �����������ڸ����Լ���ʱ�� */
	if((count_index - HB_VALIDE_POINT) >= 0)
	{
		/****** �ܼ������� *****************/
		{
			uint8_t curr_out = 0;/*��ѭ����ǰֵ*/
			uint8_t curr_in = 0; /*��ѭ����ǰֵ*/
			uint8_t add_cnt = 0;
			
			for(i = 0; i<count_index; i++)
			{
				add_cnt = 0;
				curr_out = hb_max_buff[i];
				for(j=0; j<count_index; j++)
				{
					curr_in = hb_max_buff[j];
					if(curr_out > curr_in)
						temp = curr_out - curr_in;
					else
						temp = curr_in - curr_out;
			
					if(temp <= HB_POINT_THRESHOLD)
						add_cnt++;
				}
			
				hb_point_buff[i] = add_cnt;
			}
		}
		
		/************* �ܼ�����Ч���ж� ****************/
		{
			uint8_t density_num = 0;		/*�ܼ�ֵ����*/
			uint8_t period_num = 0;		/*ͳ����Ч���ڸ���*/
			uint16_t time_sum = 0;		/*ͳ����Ч������ʱ��*/
			uint8_t period_min = 0;
			uint8_t curr_point =0;		/*��ǰ�����*/

			if(count_index % 2)
			{
				period_min = (count_index / 2) + 1;
			}
			else
			{
				period_min = count_index / 2;
			}

			for(i=0; i<count_index; i++)
			{
				curr_point = hb_max_buff[i];
				density_num = hb_point_buff[i];

				if(density_num >= period_min)
				{
					temp = HR_SAMPLE_POINT_NUM;
					temp /= curr_point;		/* �ܼ����Ӧ��������*/	

					if((temp < 10 )||(count_index >= temp))
					{
						period_num++;
						time_sum += curr_point;
					}
				}
			}


			/* ����ƽ������ֵ */	
			if(period_num >= period_min)
			{
				uint16_t sum = 60000;
				uint16_t div = time_sum*HR_SAMPLE_TIMES;
				uint8_t hb_value = 0;

				div /= period_num;

				hb_value = sum / div;
				if((hb_value > HB_MAX_VALUE)||(hb_value < HB_MIN_VALUE))
					return 0;
				else
					return hb_value;
			}
			else
			{
				return 0;
			}
			
		}
		
	}
	else
	{
		return 0;		
	}	
}

/**
* @brief  Koovox_send_heart_rate_value
* @param  none
* @retval none.
*/
void Koovox_send_heart_rate_value(void)
{
	if(hb_adc_valid)
	{
		// send heart rate value to csr8670
		uint8_t i = 0, j = 0;
		uint8_t hb_value = 0;
		
		for(; i<(HR_SAMPLE_POINT_NUM - HR_SAMPLE_FREQUENCE); i++)
		{
			hb_calc_buff[i] = hb_calc_buff[i + HR_SAMPLE_FREQUENCE];
		}

		for(; i<HR_SAMPLE_POINT_NUM; i++)
		{
			hb_calc_buff[i] = ad_hb[j++];
		}
		index = 0;
		
		/* ��������ֵ */
		hb_value = Koovox_heartrate_calc(hb_calc_buff, HR_SAMPLE_POINT_NUM);

		Koovox_fill_and_send_packet(ENV , HEART_RATE, &hb_value, 1);
	
		hb_adc_valid = FALSE;
	}
}



/**
  * @brief TIM2 Update/Overflow/Trigger/Break /USART2 TX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler,19)
{
	disableInterrupts();
	
	TIM2_ClearFlag(TIM2_FLAG_Update);

	ADC_SoftwareStartConv (ADC1);	
	while(!ADC_GetFlagStatus (ADC1,ADC_FLAG_EOC));

	ADC_ClearFlag (ADC1,ADC_FLAG_EOC);

	if(index == HR_SAMPLE_FREQUENCE)
		index = HR_SAMPLE_FREQUENCE - 1;

	ad_hb[index++] = ADC_GetConversionValue (ADC1);

	if(index == HR_SAMPLE_FREQUENCE)
		hb_adc_valid = TRUE;

	enableInterrupts();
	
}




