/*
 * Lab04_DAC.c
 *
 *  Created on: Oct 15, 2019
 *      Author: hyperiontiger
 */
#include "init.h"
#include "math.h"

void configureDAC();
void configureADC();
DAC_HandleTypeDef __hdac;
DAC_ChannelConfTypeDef dac_ch;
ADC_HandleTypeDef adc1;

unsigned int inc;
float v_dac;

int main(void)
{
	/*main code
	 *
	 */
	//Initialize the system
	Sys_Init();
	configureDAC();
	configureADC();
	printf("\r\033[2J\033[1;1HLab4-2!\r\n");

	while (1)
	{

		HAL_DAC_Init(&__hdac);
		HAL_DAC_Start(&__hdac, DAC_CHANNEL_1);
		while (inc < 4000)
		{
			HAL_DAC_SetValue(&__hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, inc);
		    HAL_Delay(1);
		    inc++;
		}

		while (inc > 1)
		{
			HAL_DAC_SetValue(&__hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, inc);
		    HAL_Delay(1);
		    inc--;
		}
//		HAL_ADC_Start(&adc1);
//		HAL_DAC_Start(&__hdac, DAC_CHANNEL_1);
//
//		if (HAL_ADC_PollForConversion(&adc1, 10000) == HAL_OK)
//		{
//			//	Get Raw ADC reading
//			__HAL_ADC_CLEAR_FLAG(&adc1, ADC_SR_EOC);
//			inc = HAL_ADC_GetValue(&adc1);
//			HAL_DAC_SetValue(&__hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, inc);
//			HAL_Delay(1);
//		}
	}

}

void configureDAC()
{
	/*configure the DAC
	 *
	 */
	__HAL_RCC_DAC_CLK_ENABLE();
	__hdac.Instance = DAC1;

	HAL_DAC_Init(&__hdac);

	dac_ch.DAC_Trigger = DAC_TRIGGER_NONE;
	dac_ch.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;

	HAL_DAC_ConfigChannel(&__hdac, &dac_ch, DAC_CHANNEL_1);

}

void configureADC()
{
	// Enable the ADC Clock.ExternalTrigConv
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	//ADC_HandleTypeDef adc1;//movetop
	adc1.Instance = ADC1;
	adc1.Init.Resolution = ADC_RESOLUTION_12B;
	adc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	adc1.Init.ContinuousConvMode = ENABLE;
	adc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

	HAL_ADC_Init(&adc1); // Initialize the ADC

	/* Available sampling times:

		ADC_SAMPLETIME_3CYCLES
	    ADC_SAMPLETIME_15CYCLES
		ADC_SAMPLETIME_28CYCLES
		ADC_SAMPLETIME_56CYCLES
		ADC_SAMPLETIME_84CYCLES
		ADC_SAMPLETIME_112CYCLES
		ADC_SAMPLETIME_144CYCLES
		ADC_SAMPLETIME_480CYCLES

	*/

	// Configure the ADC channel

	ADC_ChannelConfTypeDef _ADC1_IN6;
	_ADC1_IN6.Channel = ADC_CHANNEL_6;
	_ADC1_IN6.Offset = 0;
	_ADC1_IN6.Rank = ADC_REGULAR_RANK_1;
	_ADC1_IN6.SamplingTime = ADC_SAMPLETIME_3CYCLES;

	HAL_ADC_ConfigChannel(&adc1, &_ADC1_IN6);

}

void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
	/*configure the DAC GPIO
	 *
	 */
	GPIO_InitTypeDef  GPIO_InitStruct;
	__GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin       = GPIO_PIN_4; //Analog output PA4
	GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin       = GPIO_PIN_6; //Analog input ADC1IN6
	GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}
