/*
 * LAb04_4, assembly filter
 Created on: Oct 21, 2019
 Author: Tiger Liu and Frank Ji
     */

#include "init.h"
#include "math.h"

void configureDAC();
void configureADC();
DAC_HandleTypeDef __hdac;
DAC_ChannelConfTypeDef dac_ch;
ADC_HandleTypeDef adc1;

uint16_t x_t[3];//X array for the filter, index 0 = current reading, index 1 = previous reading
uint16_t y_t[2];//Y array for the filter, same
uint16_t temp;
double inx[4];//coefficients
double volatile var;

int main(void)
{
	/*
	 * main  code
	 */
	Sys_Init();
	configureADC();
	configureDAC();
	printf("\r\033[2J\033[1;1HLab4-2!\r\n");

	double inx[4] = {0.3125, 0.240385, 0.312500, 0.296875};
	/*
	 * ADC reading
	 */
	while(1)
	{
		HAL_ADC_Start(&adc1);

		//Take ADC input
		if (HAL_ADC_PollForConversion(&adc1, 10000) == HAL_OK)
		{
			temp = x_t[1];
			x_t[1] = x_t[0];
			x_t[2] = temp;

			__HAL_ADC_CLEAR_FLAG(&adc1, ADC_SR_EOC);
			x_t[0] = HAL_ADC_GetValue(&adc1);
		}

		//Transfer function / filter
		double x_t0 = x_t[0];
		double x_t1 = x_t[1];
		double x_t2 = x_t[2];

		double y_t0 = y_t[0];
		double y_t1 = y_t[1];

		double inx0 = inx[0];
		double inx1 = inx[1];
		double inx2 = inx[2];
		double inx3 = inx[3];

		//Apply the filter
		y_t1 = y_t[0];
	    asm("VMUL.F64 %P[out],%P[in1],%P[in2]"
	           :[out] "=w" (var)
	           :[in1] "w" (x_t0),[in2] "w" (inx0));
	    asm volatile ("VMLA.F64 %P[out],%P[in1],%P[in2]"
	           :[out] "+&w" (var)
	           :[in1] "w" (x_t1),[in2] "w" (inx1));
	    asm volatile ("VMLA.F64 %P[out],%P[in1],%P[in2]"
	           :[out] "+&w" (var)
	           :[in1] "w" (x_t2),[in2] "w" (inx2));
	    asm volatile ("VMLA.F64 %P[out],%P[in1],%P[in2]"
	           :[out] "+&w" (var)
	           :[in1] "w" (y_t1),[in2] "w" (inx3));
	    y_t[0] = var;

	    //DAC
		HAL_DAC_Init(&__hdac);
		HAL_DAC_Start(&__hdac, DAC_CHANNEL_1);

		HAL_DAC_SetValue(&__hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, y_t[0]);
		//HAL_Delay(1);
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

}

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{

// GPIO init
	GPIO_InitTypeDef  GPIO_InitStruct;
	__GPIOA_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();


	GPIO_InitStruct.Pin       = GPIO_PIN_6; //Analog input ADC1IN6
	GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	  __HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitStruct.Pin       = GPIO_PIN_7; //Pushbutton C7
	GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}
