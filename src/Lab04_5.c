/*
 * Lab04_5.c
 *
 *  Created on: Oct 24, 2019
 *      Author: hyperiontiger
 */

#include "init.h"
#include "math.h"

void configureDAC();
void configureADC();
void Init_Timer();
DAC_HandleTypeDef __hdac;
DAC_ChannelConfTypeDef dac_ch;
ADC_HandleTypeDef adc1;
ADC_HandleTypeDef adc3;
TIM_HandleTypeDef htim7;

int flag_tim6;
int f_adc1;
int f_adc3;
uint16_t raw_adc1;
uint16_t raw_adc3;
uint16_t x_t[5];
uint16_t y_t[5];
uint16_t tempx;
uint16_t tempy;

int main (void)
{
	/*main code
	 *
	 */
	Sys_Init();
	configureADC();
	configureDAC();
	Init_Timer();
	printf("\r\033[2J\033[1;1HLab4-5---------------------------------!\r\n");
	while(1){

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
	// Enable the ADC Clock.ExternalTrigConv for ADC1 and ADC3 __HAL_ADC_ENABLE_IT
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;

	NVIC->ISER[18 / 32] = (uint32_t) 1 << (18 % 32); //ADC Global, priority 25
 	HAL_NVIC_EnableIRQ(ADC_IRQn); // Enable IRQ
 	HAL_NVIC_SetPriority(ADC_IRQn, 10, 10);

	//ADC_HandleTypeDef adc1;
	adc1.Instance = ADC1;
	adc1.Init.Resolution = ADC_RESOLUTION_12B;
	adc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	adc1.Init.ContinuousConvMode = ENABLE;
	adc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

	HAL_ADC_Init(&adc1); // Initialize the ADC1

	ADC_ChannelConfTypeDef _ADC1_IN6;
	_ADC1_IN6.Channel = ADC_CHANNEL_6;
	_ADC1_IN6.Offset = 0;
	_ADC1_IN6.Rank = ADC_REGULAR_RANK_1;
	_ADC1_IN6.SamplingTime = ADC_SAMPLETIME_3CYCLES;

	HAL_ADC_ConfigChannel(&adc1, &_ADC1_IN6);

	//ADC_HandleTypeDef adc3;
	adc1.Instance = ADC3;
	adc1.Init.Resolution = ADC_RESOLUTION_12B;
	adc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	adc1.Init.ContinuousConvMode = ENABLE;
	adc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

	HAL_ADC_Init(&adc3); // Initialize the ADC3

	ADC_ChannelConfTypeDef _ADC3_IN8;
	_ADC1_IN6.Channel = ADC_CHANNEL_8;
	_ADC1_IN6.Offset = 0;
	_ADC1_IN6.Rank = ADC_REGULAR_RANK_1;
	_ADC1_IN6.SamplingTime = ADC_SAMPLETIME_3CYCLES;

	HAL_ADC_ConfigChannel(&adc3, &_ADC3_IN8);

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
	__GPIOF_CLK_ENABLE();

	GPIO_InitStruct.Pin       = GPIO_PIN_10; //Analog input ADC3IN8
	GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

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

void Init_Timer()
{
	 //Enable Timer 7: overflow at 1.211E-5s
	 	HAL_Init();
	 	htim7.Instance = TIM7;
	 	htim7.Init.Prescaler = 2;
	 	htim7.Init.Period = 436;

	 	__HAL_RCC_TIM7_CLK_ENABLE();

	 	HAL_NVIC_SetPriority(TIM7_IRQn, 5, 5);
	 	HAL_NVIC_EnableIRQ(TIM7_IRQn);

	 	HAL_TIM_Base_Init(&htim7);
	 	HAL_TIM_Base_Start_IT(&htim7);


}

void TIM7_IRQHandler() {
	//HAL_GPIO_EXTI_IRQHandler(???);
	HAL_TIM_IRQHandler(&htim7);
}

void ADC_IRQHandler(void)
{

	//HAL_ADC_IRQHandler(&adc1);
	HAL_ADC_IRQHandler(&adc3);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc->Instance == ADC3)
	{
		//adc3 value
		printf("\r\n333\r\n");
		f_adc3 = 1;
		__HAL_ADC_CLEAR_FLAG(&adc3, ADC_SR_EOC);
		raw_adc3 = HAL_ADC_GetValue(&adc3);
		//__HAL_ADC_CLEAR_FLAG(&adc3, ADC_SR_EOC);
//		HAL_ADC_Stop_IT(&adc3);
	}
	if (f_adc1 && f_adc3)
	{
		//multiply
		f_adc1 = 0;
		f_adc3 = 0;
		tempx = x_t[3];
		x_t[3] = x_t[2];
		x_t[2] = x_t[1];
		x_t[1] = x_t[0];
		x_t[4] = tempx;

		x_t[0] = (raw_adc1-620) * (raw_adc3-620);
		//filter code
		tempy = y_t[3];
		y_t[3] = y_t[2];
		y_t[2] = y_t[1];
		y_t[1] = y_t[0];
		y_t[4] = tempy;

		y_t[0] = 0.001*x_t[0] - 0.002*x_t[2] + 0.001*x_t[4] + 3.166*y_t[1] - 4.418*y_t[2] + 3.028*y_t[3] - 0.915*y_t[4];
		 //DAC
		HAL_DAC_Init(&__hdac);
		HAL_DAC_Start(&__hdac, DAC_CHANNEL_1);

		HAL_DAC_SetValue(&__hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (y_t[0]+620));
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if(htim->Instance == TIM7)
	{
		HAL_ADC_Start_IT(&adc3);
		HAL_Delay(1);
		HAL_ADC_Start(&adc1);
		if (HAL_ADC_PollForConversion(&adc1, 10000) == HAL_OK)
		{
			//	Get Raw ADC reading
			printf("\r\n111\r\n");
			f_adc1 = 1;
			__HAL_ADC_CLEAR_FLAG(&adc1, ADC_SR_EOC);
			raw_adc1 = HAL_ADC_GetValue(&adc1);
		}
		HAL_Delay(1);
		HAL_ADC_Stop(&adc1);
	}
		//counter_10++;
}
