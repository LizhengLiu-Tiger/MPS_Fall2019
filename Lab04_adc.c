//--------------------------------
// Lab 4 - Sample - Lab04_sample.c
//--------------------------------
//
//

#include "init.h"
#include "math.h"

void configureADC();
float count = 0;
float high = 0;
float low = 5;
float total;
float ave=0;
int flag =0;

ADC_HandleTypeDef adc1;//movetop
// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();
	configureADC();

	uint16_t raw_value;
	float vol;

	printf("\r\033[2J\033[1;1HLab4-1!\r\n");
	while(1)
	{
		if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7))
		{
			flag =1;
			HAL_Delay(400);
		}
		HAL_ADC_Start(&adc1);
		if (HAL_ADC_PollForConversion(&adc1, 10000) == HAL_OK && flag)
		{
			count++;;
			//	Get Raw ADC reading
			__HAL_ADC_CLEAR_FLAG(&adc1, ADC_SR_EOC);
			raw_value = HAL_ADC_GetValue(&adc1);

			//calculate the high low average
			vol = (float)raw_value * 3.3 / (pow(2, 12) - 1);
			total += vol;
			ave = total / count;
			if(vol > high)
			high = vol;
			if(vol < low)
			low = vol;

			unsigned int ui;
			memcpy(&ui, &vol, sizeof (ui));

			printf("\r\033[3:1H Current: raw_value: %x   voltage: %f %x", raw_value, vol, ui);
			printf("\r\033[4:1H Highest:  voltage: %f ", high);
			printf("\r\033[5:1H Lowest:voltage: %f ",  low);
			printf("\r\033[6:1H Average: voltage: %f ", ave);

			flag = 0;

		}
	}


	// Code goes here
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

