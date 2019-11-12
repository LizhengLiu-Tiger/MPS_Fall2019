/*
 * Lab04_4 C filter
 * Created on Oct 21, 2019
 * Author: Tiger Liu and Frank Ji
 */

#include "init.h"
#include "math.h"

DMA_HandleTypeDef hdma_adc; //DMA handle for the ADC1

void configureADC();

//define global variables
int flag =0;
volatile uint32_t raw_value;
int prev1 = 0;
int prev2 = 0;
float prev_result = 0;
int temp;
int output_value;
float result;

int adc_flag = 0;

float vol;

//define adc and dac
ADC_HandleTypeDef adc1;
DAC_HandleTypeDef hdac;

// Main Execution Loop

int main(void)
{
	//Initialize the system
	Sys_Init();
	configureADC();
	configureDAC();

	printf("\r\033[2J\033[1;1HLab5-3!\r\n");
    	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
	HAL_ADC_Start_DMA(&adc1, &raw_value, 1);//Start ADC conversion in DMA mode

	  while(1)
	  {
		  		//store previous result
			  	prev_result = result;
			  	temp = prev1;
		  		prev1 = raw_value;
		  		prev2 = temp;
			  	//clear  flag
				while(!adc_flag);
				adc_flag = 0;

				//Implement the filter value and output through DAC
			  	result = 0.3125*(float)raw_value+0.240385*(float)prev1+0.3125*(float)prev2+0.296875*(float)prev_result;
			  	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (int)result);

	  }

}

void configureADC()
{
	// Enable the ADC Clock.ExternalTrigConv
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	//ADC_HandleTypeDef adc1;//movetop
	adc1.Instance = ADC1;
	adc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	adc1.Init.Resolution = ADC_RESOLUTION_12B;
	adc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	adc1.Init.DMAContinuousRequests = ENABLE;
	adc1.Init.ContinuousConvMode = ENABLE;
	adc1.Init.DiscontinuousConvMode = DISABLE;
	adc1.Init.NbrOfConversion = 1;
	adc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	HAL_ADC_Init(&adc1); // Initialize the ADC
	
	// Configure the ADC channel

	ADC_ChannelConfTypeDef _ADC1_IN6;
	_ADC1_IN6.Channel = ADC_CHANNEL_6;
	_ADC1_IN6.Offset = 0;
	_ADC1_IN6.Rank = ADC_REGULAR_RANK_1;
	_ADC1_IN6.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	HAL_ADC_ConfigChannel(&adc1, &_ADC1_IN6);

	__HAL_RCC_DMA2_CLK_ENABLE();

	hdma_adc.Init.Channel = DMA_CHANNEL_0;
	hdma_adc.Instance = DMA2_Stream0 ;
	hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_adc.Init.MemInc = DMA_MINC_DISABLE;
	hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdma_adc.Init.Mode = DMA_CIRCULAR;
	hdma_adc.Init.Priority = DMA_PRIORITY_HIGH;
	// hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	__HAL_LINKDMA(&adc1,DMA_Handle,hdma_adc);
	HAL_DMA_Init(&hdma_adc);
	
	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn,0,0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

	HAL_NVIC_SetPriority(ADC_IRQn,1,1);
	HAL_NVIC_EnableIRQ(ADC_IRQn);

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


void configureDAC()
{
	  DAC_ChannelConfTypeDef sConfig;
	  GPIO_InitTypeDef GPIO_InitStruct;

	  __HAL_RCC_DAC_CLK_ENABLE();

	  /* DAC Initialization  */
	  hdac.Instance = DAC1;
	  HAL_DAC_Init(&hdac);

	  /**DAC channel OUT1 config */
	  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	  HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);

	  /* DAC GPIO Configuration
	     PA4     ------> DAC_OUT1
	  */
	  GPIO_InitStruct.Pin = GPIO_PIN_4;
	  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


void DMA2_Stream0_IRQHandler(void){
	HAL_DMA_IRQHandler(&hdma_adc);
}

void ADC1_IRQHandler(void){
	HAL_ADC_IRQHandler(&adc1);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	adc_flag = 1;
}


