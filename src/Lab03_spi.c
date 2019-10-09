//----------------------------------
// Lab 3 - Part 3: SPI - Lab03_spi.c
//----------------------------------
//

#include "init.h"

// If needed:
//#include <stdio.h>
//#include <stdlib.h>

/*
 * For convenience, configure the SPI handler here
 */
// See 769 Description of HAL drivers.pdf, Ch. 58.1 or stm32f7xx_hal_spi.c
char input_spi2[1];
char input_spi2s[1];

void configureSPI()
{
	hspi_2.Instance = SPI2; // Please use SPI2!
	hspi_2.Init.Mode = SPI_MODE_MASTER; // Set master mode
	hspi_2.Init.TIMode = SPI_TIMODE_DISABLE; // Use Motorola mode, not TI mode
	hspi_2.Init.Direction = SPI_DIRECTION_2LINES;
	hspi_2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi_2.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi_2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	hspi_2.Init.NSS = SPI_NSS_SOFT;

//	hspi_2s.Instance = SPI2; // Please use SPI2!
//	hspi_2s.Init.Mode = SPI_MODE_SLAVE; // Set master mode
//	hspi_2s.Init.TIMode = SPI_TIMODE_DISABLE; // Use Motorola mode, not TI mode
//	hspi_2s.Init.Direction = SPI_DIRECTION_2LINES;
//	hspi_2s.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
//	hspi_2s.Init.DataSize = SPI_DATASIZE_8BIT;
//	hspi_2s.Init.NSS = SPI_NSS_SOFT;
/*
 * ... You get the idea.
 */

	HAL_SPI_Init(&hspi_2);
	//HAL_SPI_Init(&hspi_2s);
	//
	//HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi);
	//
	// HAL_SPI_Init() will call HAL_SPI_MspInit() after reading the properities of
	// the passed SPI_HandleTypeDef. After finishing the MspInit call, it will set
	// the SPI property bits. This is how all HAL_[peripheral]_Init() functions work.
}

/*
 * This is called upon SPI initialization. It handles the configuration
 * of the GPIO pins for SPI.
 */
 // Do NOT change the name of an MspInit function; it needs to override a
 // __weak function of the same name. It does not need a prototype in the header.
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	// SPI GPIO initialization structure here

	if (hspi->Instance == SPI2)
	{
		GPIO_InitTypeDef  GPIO_InitStruct;
		__GPIOA_CLK_ENABLE();
		__GPIOB_CLK_ENABLE();


		// Initialize TXRX Pin
		GPIO_InitStruct.Pin       = GPIO_PIN_14 | GPIO_PIN_15; //MISO ; MOSI
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		// Initialize CLK Pin
		GPIO_InitStruct.Pin       = GPIO_PIN_12; //CLK
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		//Init NSS
		GPIO_InitStruct.Pin       = GPIO_PIN_11; //NSS
		GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		// Enable UART Clocking
		__SPI2_CLK_ENABLE();
		// Enable SPI GPIO port clocks, set HAL GPIO init structure's values for each
		// SPI-related port pin (SPI port pin configuration), enable SPI IRQs (if applicable), etc.
	}
}

int main(void)
{
	Sys_Init();
	HAL_UART_MspInit(&USB_UART);
	configureSPI();
	printf("\r\033[2J\033[1;1HYour code here!\r\n");

	int n = 1;
	int m = 0;
	while(1)
	{

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);

		input_spi2s[0] = uart_getchar(&USB_UART, 0);
		//printf("\r");

		HAL_SPI_TransmitReceive(&hspi_2, input_spi2s, input_spi2, 1, HAL_MAX_DELAY);
		printf("\033[%d;%dH",m+2, n);
		fflush(stdout);
		printf("%c\n", input_spi2s[0]);
		printf("\033[%d;%dH",m+12, n);
		fflush(stdout);
		printf("%c\n", input_spi2[0]);
		//uart_putchar(&USB_UART, input_spi2);

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

		n ++;

		if(n>79)
		{
			m++;
			n=1;
		}
	}
// See 769 Description of HAL drivers.pdf, Ch. 58.2.3 or stm32f7xx_hal_spi.c
//
//	HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
//
}
