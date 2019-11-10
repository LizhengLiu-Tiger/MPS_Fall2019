#include "uart.h"
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef dma1_SPItx;
DMA_HandleTypeDef dma1_SPIrx;
DMA_HandleTypeDef dma2_ADC1;
ADC_HandleTypeDef adc1;
SPI_HandleTypeDef hspi_2;
char input_spi2[1];
char input_spi2s[1];
char* ptr;
char input[1];
uint8_t *pData;
int flag_coml;

int rx_flag = 0;
int tx_flag = 0;
// Initialize Hardware Resources
// Peripheral's clock enable
// Peripheral's GPIO Configuration
void HAL_UART_MspInit(UART_HandleTypeDef *huart){
//	char *msg = "Hello STM32 Lovers! This message is transferred in DMA Mode.\r\n";
	GPIO_InitTypeDef  GPIO_InitStruct;

	if (huart->Instance == USART1) {
		// Enable GPIO Clocks
		__GPIOA_CLK_ENABLE();

		// Initialize TX Pin
		GPIO_InitStruct.Pin       = GPIO_PIN_9;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); //TX Config

		// Initialize RX Pin
		GPIO_InitStruct.Pin = GPIO_PIN_10;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); //RX Config

		// Enable UART Clocking
		__USART1_CLK_ENABLE();

		///////////////////////////////////////////////
		// Add DMA initializations here
		///////////////////////////////////////////////
		__HAL_RCC_DMA2_CLK_ENABLE();
		__HAL_RCC_DMA1_CLK_ENABLE();

		  hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
		  hdma_usart1_tx.Instance = DMA2_Stream7 ;
		  hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		  hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		  hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
		  hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		  hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		  hdma_usart1_tx.Init.Mode = DMA_NORMAL;
		  hdma_usart1_tx.Init.Priority = DMA_PRIORITY_HIGH;
		 // hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		  HAL_DMA_Init(&hdma_usart1_tx);

		  __HAL_LINKDMA(&USB_UART,hdmatx,hdma_usart1_tx);

			HAL_NVIC_SetPriority(DMA2_Stream7_IRQn,1,1);
			HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
		 // HAL_DMA_Start(&hdma_usart1_tx,  (uint32_t)msg,  (uint32_t)&huart->Instance->TDR, strlen(msg));
		  //Enable UART in DMA mode
		  //huart->Instance->CR3 |= USART_CR3_DMAT;


			  hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
			  hdma_usart1_rx.Instance = DMA2_Stream2 ;
			  hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
			  hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
			  hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
			  hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
			  hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
			  hdma_usart1_rx.Init.Mode = DMA_NORMAL;
			  hdma_usart1_rx.Init.Priority = DMA_PRIORITY_HIGH;
			//  hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
			  HAL_DMA_Init(&hdma_usart1_rx);

			  __HAL_LINKDMA(&USB_UART,hdmarx,hdma_usart1_rx);

				HAL_NVIC_SetPriority(DMA2_Stream2_IRQn,0,0);
				HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

				HAL_NVIC_SetPriority(USART1_IRQn,2,2);
				HAL_NVIC_EnableIRQ(USART1_IRQn);

			dma1_SPItx.Instance = DMA1_Stream4;
			dma1_SPItx.Init.Channel = DMA_CHANNEL_0;//0 for SPI TX
			dma1_SPItx.Init.Direction = DMA_MEMORY_TO_PERIPH;
			dma1_SPItx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
			dma1_SPItx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; // ??Enable both sizes?
			dma1_SPItx.Init.PeriphInc = DMA_PINC_DISABLE;
			dma1_SPItx.Init.MemInc = DMA_MINC_ENABLE; // ??Enable or Dis? Both?
			dma1_SPItx.Init.Mode = DMA_NORMAL; //Any more configure?? P
			dma1_SPItx.Init.Priority = DMA_PRIORITY_LOW;
			HAL_DMA_Init(&dma1_SPItx);
			__HAL_LINKDMA(&hspi_2, hdmatx, dma1_SPItx); // DMA to SPI tx
			HAL_NVIC_SetPriority(DMA1_Stream4_IRQn,3,3);
			HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);



			dma1_SPIrx.Instance = DMA1_Stream3;
			dma1_SPIrx.Init.Channel = DMA_CHANNEL_0;//0 for SPI RX
			dma1_SPIrx.Init.Direction = DMA_PERIPH_TO_MEMORY;
			dma1_SPIrx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
			dma1_SPIrx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; // ??Enable both sizes?
			dma1_SPIrx.Init.PeriphInc = DMA_PINC_DISABLE;
			dma1_SPIrx.Init.MemInc = DMA_MINC_ENABLE; // ??Enable or Dis? Both?
			dma1_SPIrx.Init.Mode = DMA_NORMAL; //Any more configure?? P
			dma1_SPIrx.Init.Priority = DMA_PRIORITY_LOW;
			HAL_DMA_Init(&dma1_SPIrx);
			__HAL_LINKDMA(&hspi_2, hdmarx, dma1_SPIrx); // DMA to SPI rx
			HAL_NVIC_SetPriority(DMA1_Stream3_IRQn,4,4);
			HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

			HAL_NVIC_SetPriority(SPI2_IRQn,5,5);
			HAL_NVIC_EnableIRQ(SPI2_IRQn);

			dma2_ADC1.Instance = DMA1_Stream0;
			dma2_ADC1.Init.Channel = DMA_CHANNEL_0;//0 for SPI RX
			dma2_ADC1.Init.Direction = DMA_PERIPH_TO_MEMORY;
			dma2_ADC1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
			dma2_ADC1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // ??Enable both sizes?
			dma2_ADC1.Init.PeriphInc = DMA_PINC_DISABLE;
			dma2_ADC1.Init.MemInc = DMA_MINC_ENABLE; // ??Enable or Dis? Both?
			dma2_ADC1.Init.Mode = DMA_CIRCULAR; //Any more configure?? P
			dma2_ADC1.Init.Priority = DMA_PRIORITY_HIGH;
			dma2_ADC1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
			HAL_DMA_Init(&dma2_ADC1);
			__HAL_LINKDMA(&adc1, DMA_Handle, dma2_ADC1); // DMA to SPI rx
			HAL_NVIC_SetPriority(DMA2_Stream0_IRQn,0,0);
			HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

			HAL_NVIC_SetPriority(ADC_IRQn,7,7);
			HAL_NVIC_EnableIRQ(ADC_IRQn);

	} else if (huart->Instance == USART6) {
		// Enable GPIO Clocks
		__GPIOC_CLK_ENABLE();

		// Initialize TX Pin
		GPIO_InitStruct.Pin       = GPIO_PIN_6;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); //TX Config

		// Initialize RX Pin
		GPIO_InitStruct.Pin = GPIO_PIN_7;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); //RX Config

		// Enable UART Clocking
		__USART6_CLK_ENABLE();

	}
}

//UART Initialization
void initUart(UART_HandleTypeDef* Uhand, uint32_t Baud, USART_TypeDef* Tgt) {
	Uhand->Instance        = Tgt;

	Uhand->Init.BaudRate   = Baud;
	Uhand->Init.WordLength = UART_WORDLENGTH_8B;
	Uhand->Init.StopBits   = UART_STOPBITS_1;
	Uhand->Init.Parity     = UART_PARITY_NONE;
	Uhand->Init.Mode       = UART_MODE_TX_RX;
	Uhand->Init.HwFlowCtl  = UART_HWCONTROL_NONE;

	HAL_UART_Init(Uhand);
}

/* ============================================================================

Reassigning _write and _read to USB_UART by default.

Note the following for an explanation of syntax:

HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)

HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)

============================================================================= */

///////////////////////////////////////////////
// Change _write() and _read() to use DMAs
///////////////////////////////////////////////

// Make printf(), putchar(), etc. default to work over USB UART
int _write(int file, char *ptr, int len) {

	HAL_UART_Transmit_DMA(&USB_UART, (uint8_t*) ptr, len);
	while(tx_flag != 1);
	tx_flag = 0;

	return len;
}

// Make scanf(), getchar(), etc. default to work over USB UART
int _read(int file, char *ptr, int len) {
	*ptr = 0x00; // Clear the character buffer because scanf() is finicky
	len = 1; // Again because of scanf's finickiness, len must = 1
	HAL_UART_Receive_DMA(&USB_UART, (uint8_t*) ptr, len);
	while(rx_flag != 1);
	rx_flag = 0;

	return len;
}

/* ============================================================================

Custom UART functions that can be mapped to any UART of your choice when called

Note the following for an explanation of syntax:

HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)

HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)

============================================================================= */

// Print a string on the specified UART. (Basically a redirectable puts)
void uart_print(UART_HandleTypeDef *huart, char* string) {
    HAL_UART_Transmit_DMA(huart, (uint8_t*) string, strlen((const char*)string));
}

// Get one character
// 'echo' means enable (1) or disable (0) echoing of characters
char uart_getchar(UART_HandleTypeDef *huart, uint8_t echo) {
	//char input[1];
	while(HAL_UART_Receive_DMA(huart, (uint8_t *)input, 1) != HAL_OK);
	if (echo) while(HAL_UART_Transmit_DMA(huart, (uint8_t*) input, 1) != HAL_OK);
	return (char)input[0];
}

// Send one character
void uart_putchar(UART_HandleTypeDef *huart, char * input) {
	HAL_UART_Transmit_DMA(huart, (uint8_t*) input, 1);
}

// Collects characters until size limit or an endline is recieved
// Returns number of characters collected
// 'max size' should match the size of the array or be smaller
int uart_getline(UART_HandleTypeDef *huart, char * input, int max_size)
{
  char single;
  int i = 0;

	while(1)
	{
		if (i > max_size)
      {
				break;
			}

		single = uart_getchar(huart, 1); // Get one character

  	if (single == '\n' || single == '\r') // Unix & co. use \n, Windows uses \r\n
			break;
  	else
			input[i] = single;
  	i++;
	}
  return i; // The way this is set up, it will always report the wrong size
}


void USART1_IRQHandler(void){
	HAL_UART_IRQHandler(&USB_UART);
}


void SPI2_IRQHandler(void){
	HAL_SPI_IRQHandler(&hspi_2);
}


void DMA2_Stream2_IRQHandler(void){
	HAL_DMA_IRQHandler(&hdma_usart1_rx);
}

void DMA2_Stream0_IRQHandler(void){
	HAL_DMA_IRQHandler(&dma2_ADC1);
}


void DMA2_Stream7_IRQHandler(void){
	HAL_DMA_IRQHandler(&hdma_usart1_tx);
}

void DMA1_Stream3_IRQHandler(void){
	HAL_DMA_IRQHandler(&dma1_SPIrx);
}


void DMA1_Stream4_IRQHandler(void){
	HAL_DMA_IRQHandler(&dma1_SPItx);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *haurt){
	rx_flag = 1;
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *haurt){
	tx_flag = 1;
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
//	rx_flag = 1;
}

//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc){
//	flag_coml = 1;
//}


void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi){
//	tx_flag = 1;
}

void ADC_IRQHandler(void){
	HAL_ADC_IRQHandler(&adc1);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc){
	flag_coml = 1;
}













