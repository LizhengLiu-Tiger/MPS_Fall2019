//------------------------------------
// Lab 3 - Part 1: UART - Lab03_uart.c
//------------------------------------
//

#include "init.h"

char input_1[1];
int input_c;

// Main Execution Loop
int main(void) {
	//Initialize the system:UART6, TX: C6, RX: C7
	Sys_Init();
	HAL_UART_MspInit(&USB_UART);
	HAL_UART_MspInit(&USB_UART_6);


	printf("\033[0;0HYour code here!\r\n");
	while(input_1[0] != 27)
	{
		input_1[0] = uart_getchar(&USB_UART, 1);
		if (input_1[0] != 27)
			uart_putchar(&USB_UART_6, input_1);
		else
		{
			uart_print(&USB_UART, "\r\nExit\r");
			uart_print(&USB_UART_6, "\r\nExit\r");
			break;
		}
	}
	// Code

}
