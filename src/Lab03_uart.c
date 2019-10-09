//------------------------------------
// Lab 3 - Part 1: UART - Lab03_uart.c
//------------------------------------
//
#include "init.h"

char input_1[1];
char input_6[1];
int input_c;


// Main Execution Loop
int main(void) {
	//Initialize the system:UART6, TX: C6, RX: C7 rx_Iv
	Sys_Init();
	HAL_UART_MspInit(&USB_UART);
	HAL_UART_MspInit(&USB_UART_6);

	printf("\r\033[2J\033[1;1HYour code here!\r\n");
	while(input_1[0] != 27)
	{
		if(HAL_UART_Receive(&USB_UART, input_1, 1, 10) == HAL_OK)
		{
			HAL_UART_Transmit(&USB_UART, input_1, 1, 10);
			HAL_UART_Transmit(&USB_UART_6, input_1, 1, 10);
		}
		if(HAL_UART_Receive(&USB_UART_6, input_6, 1, 10) == HAL_OK)
		{
			HAL_UART_Transmit(&USB_UART_6, input_6, 1, 10);
			HAL_UART_Transmit(&USB_UART, input_6, 1, 10);
		}
//		if (input_1[0] != 27)
//			uart_putchar(&USB_UART_6, input_1);
//		else
//		{
//			uart_print(&USB_UART, "\r\nExit\r");
//			uart_print(&USB_UART_6, "\r\nExit\r");
//			break;
//		}
//
//		if (input_6[0] != 27)
//			uart_putchar(&USB_UART_6, input_6);
//		else
		if((input_1[0] == 27) || (input_6[0] == 27))
		{
			uart_print(&USB_UART, "\r\nExit\r");
			uart_print(&USB_UART_6, "\r\nExit\r");
			break;
		}

	}
	// Code

}
