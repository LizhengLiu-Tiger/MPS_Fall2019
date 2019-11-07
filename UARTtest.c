#include "uart.h"

// Initialize Hardware Resources
// Peripheral's clock enable
// Peripheral's GPIO Configuration
char *ptr;
int flag_tx;
int flag_rx;
//char *ptr_r;
//char *ptr_spit;
//char *ptr_spir;

void HAL_UART_MspInit(UART_HandleTypeDef *huart){
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
		configureDMA();


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
//	HAL_DMA_Start_IT(&dma2_UART1tx, (uint32_t)ptr, (uint32_t)&USB_UART.Instance->TDR, strlen(ptr)); //For usart1 tx
//	USB_UART.Instance->CR3 |= USART_CR3_DMAT;
	return len;
}

// Make scanf(), getchar(), etc. default to work over USB UART
int _read(int file, char *ptr, int len) {
	*ptr = 0x00; // Clear the character buffer because scanf() is finicky
	len = 1; // Again because of scanf's finickiness, len must = 1
	HAL_UART_Receive_DMA(&USB_UART, (uint8_t*) ptr, len);
//	HAL_DMA_Start_IT(&dma2_UART1rx, (uint32_t)ptrr, (uint32_t)&USB_UART.Instance->RDR, strlen(ptrr)); //For usart1 rx
//	USB_UART.Instance->CR3 |= USART_CR3_DMAT;
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
	char input[1];
	HAL_UART_Receive_DMA(huart, (uint8_t *)input, 1);
	if (echo) HAL_UART_Transmit_DMA(huart, (uint8_t*) input, 1);
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

void configureDMA()
{
//	__HAL_RCC_DMA1_CLK_ENABLE();
	__DMA2_CLK_ENABLE();

//	HAL_DMA_DeInit(&dma2_UART1tx);
//	HAL_DMA_DeInit(&dma2_UART1rx);
//	HAL_DMA_DeInit(&dma1_SPItx);
//	HAL_DMA_DeInit(&dma1_SPIrx);
//
//	NVIC->ISER[12 / 32] = (uint32_t) 1 << (12 % 32); //DMA1 Stream 1
//	NVIC->ISER[13 / 32] = (uint32_t) 1 << (13 % 32); //DMA1 Stream 2
//
//	HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
//	HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
//
//	HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
//	HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 1, 1);
//	NVIC->ISER[58 / 32] = (uint32_t) 1 << (58 % 32); //DMA1 Stream 2
//	NVIC->ISER[70 / 32] = (uint32_t) 1 << (70 % 32); //DMA1 Stream 7

	dma2_UART1tx.Instance = DMA2_Stream7;
	dma2_UART1tx.Init.Channel = DMA_CHANNEL_4;//4 for UART1tx
	dma2_UART1tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
	dma2_UART1tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	dma2_UART1tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; // ??Enable both sizes? Yes
	dma2_UART1tx.Init.PeriphInc = DMA_PINC_DISABLE;
	dma2_UART1tx.Init.MemInc = DMA_MINC_ENABLE; // ??Enable or Dis????? Both? Yes
	dma2_UART1tx.Init.Mode = DMA_NORMAL; //Any more configure?? P
	dma2_UART1tx.Init.Priority = DMA_PRIORITY_LOW;

	HAL_DMA_Init(&dma2_UART1tx);

	//USB_UART.Instance->CR3 |= USART_CR3_DMAT;

	__HAL_LINKDMA(&USB_UART, hdmatx, dma2_UART1tx); // Associate the DMA to the peripheral UART1
	HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

	dma2_UART1rx.Instance = DMA2_Stream2;
	dma2_UART1rx.Init.Channel = DMA_CHANNEL_4;//4 for UART1rx
	dma2_UART1rx.Init.Direction = DMA_MEMORY_TO_PERIPH;
	dma2_UART1rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	dma2_UART1rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; // ??Enable both sizes?
	dma2_UART1rx.Init.PeriphInc = DMA_PINC_DISABLE;
	dma2_UART1rx.Init.MemInc = DMA_MINC_ENABLE; // ??Enable or Dis? Both?
	dma2_UART1rx.Init.Mode = DMA_NORMAL; //Any more configure?? P
	dma2_UART1rx.Init.Priority = DMA_PRIORITY_LOW;

	HAL_DMA_Init(&dma2_UART1rx);

    //USB_UART.Instance->CR3 |= USART_CR3_DMAT;

    __HAL_LINKDMA(&USB_UART, hdmarx, dma2_UART1rx); // Associate the DMA to the peripheral UART1
	HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
	HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 1, 1);
//	dma1_SPItx.Instance = DMA1_Stream4;
//	dma1_SPItx.Init.Channel = DMA_CHANNEL_0;//0 for SPI TX
//	dma1_SPItx.Init.Direction = DMA_MEMORY_TO_PERIPH;
//	dma1_SPItx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//	dma1_SPItx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; // ??Enable both sizes?
//	dma1_SPItx.Init.PeriphInc = DMA_PINC_ENABLE;
//	dma1_SPItx.Init.MemInc = DMA_MINC_ENABLE; // ??Enable or Dis? Both?
//	dma1_SPItx.Init.Mode = DMA_NORMAL; //Any more configure?? P
//	dma1_SPItx.Init.Priority = DMA_PRIORITY_HIGH;
//
//	dma1_SPIrx.Instance = DMA1_Stream3;
//	dma1_SPIrx.Init.Channel = DMA_CHANNEL_0;//0 for SPI RX
//	dma1_SPIrx.Init.Direction = DMA_PERIPH_TO_MEMORY;
//	dma1_SPIrx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//	dma1_SPIrx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; // ??Enable both sizes?
//	dma1_SPIrx.Init.PeriphInc = DMA_PINC_ENABLE;
//	dma1_SPIrx.Init.MemInc = DMA_MINC_ENABLE; // ??Enable or Dis? Both?
//	dma1_SPIrx.Init.Mode = DMA_NORMAL; //Any more configure?? P
//	dma1_SPIrx.Init.Priority = DMA_PRIORITY_HIGH;

//	HAL_DMA_Init(&dma1_SPItx);
//	HAL_DMA_Init(&dma1_SPIrx);

//	__HAL_LINKDMA(&hspi_2, hdmatx, dma1_SPItx); // DMA to SPI tx
//	__HAL_LINKDMA(&hspi_2, hdmatx, dma1_SPIrx); // DMA to SPI rx

//	HAL_DMA_Start(&dma2_UART1tx, (uint32_t)ptr_w, (uint32_t)&USB_UART.Instance->TDR, strlen(ptr_w)); //For usart1 tx
//	HAL_DMA_Start(&dma2_UART1rx, (uint32_t)ptr_r, (uint32_t)&USB_UART.Instance->RDR, strlen(ptr_r)); //For usart1 rx
//	HAL_DMA_Start_IT(&dma1_SPItx, (uint32_t)ptr_spit, (uint32_t)&hspi_2.Instance->DR, strlen(ptr_spit)); // Where to put IT functions?
//	HAL_DMA_Start_IT(&dma1_SPIrx, (uint32_t)ptr_spir, (uint32_t)&hspi_2.Instance->DR, strlen(ptr_spir));

//	SPI2->CR2 |= 0x0002;
//	SPI2->CR2 |= 0x0001;
//	SPI2->CR1 |= SPI_CR1_SPE;
}

void DMA2_Stream7_IRQHandler()
{
//	HAL_DMA_IRQHandler(&dma1_SPItx);
//	HAL_DMA_IRQHandler(&dma1_SPIrx);
	HAL_DMA_IRQHandler(&dma2_UART1tx);

}

void DMA2_Stream2_IRQHansdler()
{
//	HAL_DMA_IRQHandler(&dma1_SPItx);
//	HAL_DMA_IRQHandler(&dma1_SPIrx);
	HAL_DMA_IRQHandler(&dma2_UART1rx);

}

void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&USB_UART);
}


