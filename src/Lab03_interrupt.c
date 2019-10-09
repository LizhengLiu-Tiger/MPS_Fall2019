/*
 * Lab03_interrupt.c
 *
 *  Created on: Sep 26, 2019
 *      Author: hyperiontiger
 */
#include "init.h"

void Init_UART();

int flag_uart1 = 0;
int flag_uart6 = 0;
char input_Mac[1];
char input_Win[1];
__IO ITStatus Uartstatus = SET;
__IO ITStatus Uartstatus_1 = SET;

int main(void) {
	//Initialize the system:UART6, TX: C6, RX: C7; HAL_NVIC_SetPriority(USART1_IRQn, 1, 1)
	Sys_Init();
	printf("\033[2J\r\n");
	printf("\rLab03_test!\r\n");
	Init_UART(&USB_UART, &USB_UART_6);


	while(1){
		if (input_Mac[0] == 27 || input_Win[0] == 27)
		{
			break;
		}
		//input_Mac[0] = uart_getchar_IT(&USB_UART, 1);
		//flag_uart6++;
		//HAL_Delay(1000);



	}

}

void Init_UART(UART_HandleTypeDef *huart_1, UART_HandleTypeDef *huart_6)
{
	HAL_UART_MspInit(&USB_UART);
	HAL_UART_MspInit(&USB_UART_6);

	NVIC->ISER[37 / 32] = (uint32_t) 1 << (37 % 32); //USART1, priority 44
	NVIC->ISER[71 / 32] = (uint32_t) 1 << (71 % 32); //USART6, priority 78

	HAL_NVIC_EnableIRQ(USART6_IRQn);
	HAL_NVIC_EnableIRQ(USART1_IRQn);

	 //__HAL_UNLOCK(huart_1);
	 //__HAL_UNLOCK(huart_6);
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
	//HAL_NVIC_SetPriority(USART6_IRQn, 1, 2);

	SET_BIT(huart_1->Instance->CR1, USART_CR1_RXNEIE);
	SET_BIT(huart_6->Instance->CR1, USART_CR1_RXNEIE);
}

void USART1_IRQHandler()
{
	HAL_UART_IRQHandler(&USB_UART);
	if (Uartstatus)
	{
	//already clears all flags
		Uartstatus = RESET;
		HAL_UART_Receive_IT(&USB_UART, input_Mac, 1);
		HAL_UART_Transmit_IT(&USB_UART, input_Mac, 1);
		//input_Mac[0] = uart_getchar_IT(&USB_UART, 1);
		//printf("\rget IRQ\r\n");
	}


}

void USART6_IRQHandler()
{
	HAL_UART_IRQHandler(&USB_UART_6);
	if (Uartstatus_1)
	{
		Uartstatus_1 = RESET;
		HAL_UART_Receive_IT(&USB_UART_6, input_Win, 1);
		//HAL_UART_Transmit_IT(&USB_UART_6, input_Win, 1);
		//input_Win[0] = uart_getchar_IT(&USB_UART_6, 1);
	}
	//uart_putchar(&USB_UART, input_Win);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		flag_uart1 = 1;
		Uartstatus = SET;
		//printf("\rget IRQCALLB\r\n");
		HAL_UART_Transmit_IT(&USB_UART_6, input_Mac, 1);
//		HAL_UART_Transmit_IT(&USB_UART, input_Mac, 1);

		//uart_putchar_IT(&USB_UART, input_Mac);
	}

	if (huart->Instance == USART6)
	{
		flag_uart6 = 1;
		Uartstatus_1 = SET;
		//input_Win[0] = uart_getchar_IT(&USB_UART_6, 0);
		HAL_UART_Transmit_IT(&USB_UART, input_Win, 1);
//		HAL_UART_Transmit_IT(&USB_UART_6, input_Win, 1);
	}
}



