/*
 * rfid.c
 *
 *  Created on: Dec 4, 2019
 *      Author: hyperiontiger
 */

#include "rfid.h"
#include "rfid_functions.h"
#include "init.h"

SPI_HandleTypeDef SpiHandle;

void SystemClock_Config(void);
static void Error_Handler(void);
//static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);
//static void CPU_CACHE_Enable(void);
void SPI_Init(void);
void LCD_LOG_Setup(void);

int main(void)
{

	/* Recognized card ID */
  uint8_t CardID[4];
  uint8_t type;
  char *result;

  /* Enable the CPU Cache */
//  CPU_CACHE_Enable();

  /* STM32F7xx HAL library initialization:
       - Configure the Flash ART accelerator
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
 Sys_Init();

//  BSP_LED_Init(LED_GREEN);
//  BSP_LED_Init(LED_RED);


  /* Configure the system clock to 216 MHz */
//  SystemClock_Config();

  // init the pins before you init spi and delay for 5ms
  SPI_Init();
  printf("\r\033[2J\033[1;1HRFID Test2\r\n");
//  LCD_LOG_Setup();

  RFID_RC522_Init();

  /* Infinite loop */
  while (1)
  {
    int status = TM_MFRC522_Check(CardID, &type);
  	if (status == MI_OK)
  	{
//      LCD_UsrLog ((char *)"Found tag: ");
//      bin_to_strhex((unsigned char *)CardID, sizeof(CardID), &result);
//      LCD_UsrLog ((char *)result);
//      LCD_UsrLog ((char *)"\n");
//      LCD_UsrLog ((char *)"Type is: ");
//      bin_to_strhex((unsigned char *)&type, 1, &result);
//      LCD_UsrLog ((char *)result);
//      LCD_UsrLog ((char *)"\n");
  		printf("The ID is %d%d%d%d.\r\n", CardID[0], CardID[1], CardID[2], CardID[3]);

  	}
  	else
  	{

    }
  }
}

//void LCD_LOG_Setup(void)
//{
//  /* Initialize the LCD */
////  BSP_LCD_Init();
////
////  /* Initialize the LCD Layers */
////  BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS);
//
//  /* Set LCD Foreground Layer  */
////  BSP_LCD_SelectLayer(1);
////
////  BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
//
//  /* Initialize LCD Log module */
//  LCD_LOG_Init();
//
//  /* Show Header and Footer texts */
//  LCD_LOG_SetHeader((uint8_t *)"RFID_RC522 Demo");
//  LCD_LOG_SetFooter((uint8_t *)"STM32F769I-DISCO board");
//
//  LCD_UsrLog ((char *)"State: LCD Initialized.\n");
//  LCD_LOG_UpdateDisplay();
//}


void SPI_Init() {
	/*##-1- Configure the SPI peripheral #######################################*/
    /* Set the SPI parameters */
	SPIx_FORCE_RESET();
	SPIx_RELEASE_RESET();
	SPIx_CLK_ENABLE();

  SpiHandle.Instance               = SPIx;
  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE; // data stable on first clock edge, if clock is idle low, that is low->high transition
  SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW; // clock idle state low
  SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
  SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
  SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  //SpiHandle.Init.CRCPolynomial     = 7;
  SpiHandle.Init.NSS               = SPI_NSS_SOFT;
  SpiHandle.Init.Mode 			       = SPI_MODE_MASTER;
  /* Slave board must wait until Master Board is ready. This to guarantee the
     correctness of transmitted/received data */
  //HAL_Delay(5);

  if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi) {
  GPIO_InitTypeDef  GPIO_InitStruct;
  GPIO_InitTypeDef  GPIO_InitStruct1;

  //SPI_InitTypeDef SPI_InitStruct;

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
  SPIx_SCK_GPIO_CLK_ENABLE();
  SPIx_MISO_GPIO_CLK_ENABLE();
  SPIx_MOSI_GPIO_CLK_ENABLE();
  SPIx_CS_GPIO_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* SPI SCK GPIO pin configuration  */
  GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = SPIx_SCK_AF;
  HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);

  /* SPI MISO GPIO pin configuration; MISO line should be floating */
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Pin = SPIx_MISO_PIN;
  GPIO_InitStruct.Alternate = SPIx_MISO_AF;
  HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);

  /* SPI MOSI GPIO pin configuration  */
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
  GPIO_InitStruct.Alternate = SPIx_MOSI_AF;
  HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);

  /* SPI CS GPIO pin configuration  */
  GPIO_InitStruct1.Pin = SPIx_CS_PIN;
  GPIO_InitStruct1.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct1.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct1.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SPIx_CS_GPIO_PORT, &GPIO_InitStruct1);
}

void SPIx_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&SpiHandle);
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            PLL_R                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  None
  * @retval None
  */
//  void SystemClock_Config(void)
//  {
//    RCC_ClkInitTypeDef RCC_ClkInitStruct;
//    RCC_OscInitTypeDef RCC_OscInitStruct;
//    HAL_StatusTypeDef ret = HAL_OK;
//
//    /* Enable HSE Oscillator and activate PLL with HSE as source */
//    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
//    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
//    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
//    RCC_OscInitStruct.PLL.PLLM = 25;
//    RCC_OscInitStruct.PLL.PLLN = 400;
//    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
//    RCC_OscInitStruct.PLL.PLLQ = 9;
//    RCC_OscInitStruct.PLL.PLLR = 7;
//
//    ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
//    if(ret != HAL_OK)
//    {
//      while(1) { ; }
//    }
//
//    /* Activate the OverDrive to reach the 216 MHz Frequency */
//    ret = HAL_PWREx_EnableOverDrive();
//    if(ret != HAL_OK)
//    {
//      while(1) { ; }
//    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
//    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
//    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
//    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
//
//    ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
//    if(ret != HAL_OK)
//    {
//      while(1) { ; }
//    }
//  }

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
//  /* Configure LED1 which is shared with SPI2_SCK signal */
//  BSP_LED_Init(LED1);
//  BSP_LED_Off(LED1);
//  while(1)
//  {
//    /* Toogle LED1 for error */
//    BSP_LED_Toggle(LED1);
//    HAL_Delay(1000);
//  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
//static void CPU_CACHE_Enable(void)
//{
//  /* Enable I-Cache */
//  SCB_EnableICache();
//
//  /* Enable D-Cache */
//  SCB_EnableDCache();
//}
