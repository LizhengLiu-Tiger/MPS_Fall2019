//------------------------------------------------------------------------------------
// Hello.c
//------------------------------------------------------------------------------------
//
// Test program to demonstrate serial port I/O.  This program writes a message on
// the console using the printf() function, and reads characters using the getchar()
// function.  An ANSI escape sequence is used to clear the screen if a '2' is typed.
// A '1' repeats the message and the program responds to other input characters with
// an appropriate message.
//
// Any valid keystroke turns on the green LED on the board; invalid entries turn it off
//


//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "stm32f769xx.h"
#include "hello.h"
#include<stdint.h>
//#include<stdbool.h>

#define esc 27
#define KRED "\x1B[31m"
#define KYEL "\x1B[33m"


//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void)
{
    Sys_Init(); // This always goes at the top of main (defined in init.c)

//    char choice;
//	char in[3];
	char char1;
//    int size = 3;
    int init_1 = 1;
    int NP_row = 9;
    int D0 = 0;
    int D1 = 0;
    int D2 = 0;
    int D3 = 0;

    printf("\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n

    __HAL_RCC_GPIOJ_CLK_ENABLE(); // Need to enable clock for peripheral bus on GPIO Port J, A, D
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();

    GPIO_InitTypeDef gpio_init;
    gpio_init.Pin = GPIO_PIN_13 | GPIO_PIN_5;//GPIO outputs
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_PULLUP;
    gpio_init.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOJ, &gpio_init);

    gpio_init.Pin = GPIO_PIN_12;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_PULLUP;
    gpio_init.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOA, &gpio_init);

    gpio_init.Pin = GPIO_PIN_4;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_PULLUP;
    gpio_init.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOD, &gpio_init);

    gpio_init.Pin = GPIO_PIN_6 | GPIO_PIN_7;//GPIO inputs
    gpio_init.Mode = GPIO_MODE_INPUT;
    gpio_init.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &gpio_init);

    gpio_init.Pin = GPIO_PIN_1;
    gpio_init.Mode = GPIO_MODE_INPUT;
    gpio_init.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOJ, &gpio_init);

    gpio_init.Pin = GPIO_PIN_6;
    gpio_init.Mode = GPIO_MODE_INPUT;
    gpio_init.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOF, &gpio_init);

    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13 | GPIO_PIN_5, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);

    D0 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);
    D1 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6);
    D2 = HAL_GPIO_ReadPin(GPIOJ, GPIO_PIN_1);
    D3 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6);
//
//    volatile uint32_t * RCC_AHB1_CLOCK_ENABLE = (uint32_t*) 0x40023830U; // Enable clock for peripheral bus on GPIO Port J
//    *RCC_AHB1_CLOCK_ENABLE |= 512U; // Bitmask for RCC AHB1 initialization: 0x00000200U or 512U in decimal
//
//    volatile uint32_t * RCC_AHB1_CLOCK_ENABLE_1 = (uint32_t*) 0x40023830U; // Enable clock for peripheral bus on GPIO Port A
//    *RCC_AHB1_CLOCK_ENABLE_1 |= 1U; // Bitmask for RCC AHB1 initialization: 0x00000200U or 512U in decimal
//
//    volatile uint32_t * RCC_AHB1_CLOCK_ENABLE_2 = (uint32_t*) 0x40023830U; // Enable clock for peripheral bus on GPIO Port D
//    *RCC_AHB1_CLOCK_ENABLE_2 |= 8U; // Bitmask for RCC AHB1 initialization: 0x00000200U or 512U in decimal
//
//    GPIOJ->MODER |= 1024U; //Bitmask for GPIO J Pin 5 initialization (set it to Output mode): 0x00000400U or 1024U in decimal
//    GPIOJ->BSRR = (uint16_t)GPIO_PIN_5; // Turn on Green LED (LED2)
//
//    GPIOJ->MODER |= 67108864U; //Bitmask for GPIO J Pin 13 initialization (set it to Output mode): 0x00000400U or 1024U in decimal
//    GPIOJ->BSRR = (uint16_t)GPIO_PIN_13; // Turn on (LED1)
//
//    GPIOA->MODER |= 16777216U; //Bitmask for GPIO A Pin 12 initialization (set it to Output mode): 0x00000400U or 1024U in decimal
//    GPIOA->BSRR = (uint16_t)GPIO_PIN_12; // Turn on (LED3)
//
//    GPIOD->MODER |= 256U; //Bitmask for GPIO D Pin 4 initialization (set it to Output mode): 0x00000400U or 1024U in decimal
//    GPIOD->BSRR = (uint32_t)GPIO_PIN_4 << 16; // Turn on  (LED4)
    //GPIOJ->BSRR = (uint32_t)GPIO_PIN_5 << 16; // Turn off Green LED (LED2)
    //GPIOJ->ODR ^= (uint16_t)GPIO_PIN_5; // Toggle LED2



// It doesn't get lower level than this!
//    volatile uint32_t * GREENLEDMODER = (uint32_t*) 0x40022400U; // Init GPIO J Pin 5 (LED2 with no Alt. func.) to Output
//    *GREENLEDMODER |= 1024U; // Bitmask for GPIO J Pin 5 initialization: 0x00000400U or 1024U in decimal

    //volatile uint32_t * GREENLEDBSRR = (uint32_t*) 0x40022418U; // Address of GPIO J Bit Set/Reset Register
    //*GREENLEDBSRR = (uint16_t)0x0020U; // Turn on Green LED (LED2)

    HAL_Delay(1000); // Pause for a second

//    volatile uint32_t * GREENLEDODR = (uint32_t*) 0x40022414U; // Address of GPIO J Output Data Register
//    *GREENLEDODR ^= (uint16_t)0x0020U; // Toggle Green LED (LED2)
    printf("                  TEST2:  enter < esc > or < ctrl[ > to quit!\r\n");
    printf ( "\033[9;1H");
    printf("\033[s");

    while(init_1)
    {

		    D0 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);
		    D1 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6);
		    D2 = HAL_GPIO_ReadPin(GPIOJ, GPIO_PIN_1);
		    D3 = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6);
		    
    	if (D0)
    		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_SET);
    	else if (!D0)
    		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_RESET);
    		
    	if (D1)
    		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, GPIO_PIN_SET);
    	else if (!D1)
    		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, GPIO_PIN_RESET);	
    		
    	if (D2)
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
    	else if (!D2)
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);	

    	if (D3)
    		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
    	else if (!D3)
    		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);	
    		

		    printf("Value of D0: %d\n\r", D0);
		    printf("Value of D1: %d\n\r", D1);
		    printf("Value of D2: %d\n\r", D2);
		    printf("Value of D3: %d\n\r", D3);
		    printf("End of this turn\n\n\r");

    }

//        printf("Hello World!\r\n\n");
//        printf("( Welcome to Microprocessor Systems )\r\n\n\n");
//        printf("1=repeat, 2=clear, 0=quit.\r\n\n"); // Menu of choices
				// Don't forget to end printf with newline or run fflush(stdout) after it!

//        choice = uart_getchar(&USB_UART, 1);
//				choice = getchar();
//				putchar(choice);

// Messing around with stuff:
//        putchar('9'); // Putchar is weird, man.
//				choice = uart_getchar(&USB_UART, 0);
//				uart_putchar(&USB_UART, &choice);
//				puts("stuff\r\n");

        // select which option to run
//        HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, GPIO_PIN_SET);         // Turn green LED on (GPIO_PIN_SET == 1)
//        *GREENLEDBSRR = (uint16_t)0x0020U; // Turn on Green LED (LED2)
//        if (choice == '0')
//            return 1;
//        else if(choice == '1')
//            printf("\r\n\nHere we go again.\r\n\n");
//        else if(choice == '2')          // clear the screen with <ESC>[2J
//        {
//            printf("\033[2J\033[;H");
//            fflush(stdout);
//        }
//        else
//        {
            // inform the user how bright he is
//            *GREENLEDBSRR = (uint32_t)0x0020U << 16; // Turn off Green LED (LED2)
//            printf("\r\nA \"");
//						putchar(choice);
//						uart_print(&USB_UART, choice);
//            printf("\" is not a valid choice.\r\n\n");
//        }
//
//// Messing around with more stuff
//				printf("Enter an alphanumeric sequence (press <enter> if you want to send less than %d characters): \r\n", size + 1);
//				int a = uart_getline(&USB_UART, in, size);
//				printf("\r\nuart_getline result: %d\r\n", a);
//		}
}

//------------------------------------------------------------------------------------
//Extra thing to consider...
//------------------------------------------------------------------------------------
void serial_print_things(void) {
	//Input Buffer
	char input[2];
	input[0]=0;
	input[1]=0;

	//Initialize the system
	Sys_Init();
	initUart(&Second_UART, 9600, USART6); // Allow printing over USART6 (Arduino pins D0 - Rx and D1 - TX)
	uart_print(&USB_UART, "\033[2J\033[;H");
	uart_print(&Second_UART, "\033[2J\033[;H");
	uart_print(&USB_UART, "Hello World: This is the USB Serial Connection\r\n");
	uart_print(&Second_UART, "Hello World: This is the other UART Connection\r\n");
	uart_print(&USB_UART, "In order to send newlines to this terminal,\r\n");
	uart_print(&USB_UART, "Press <ESC> and type \"[20h\" (without quotes)\r\n");
	uart_print(&USB_UART, "To get out of newline mode and back to line feed mode,\r\n");
	uart_print(&USB_UART, "Press <ESC> and type \"[20l\" (without quotes)\r\n");
	uart_print(&USB_UART, "\"Thanks for Playing!\"\r\n");

	printf("HIS SENTENCE USES PRINTF!!!\r\n");
  // Don't forget to end printf with newline or run fflush(stdout) after it!

	while(1) {
		input[0]=uart_getchar(&USB_UART, 0);
		uart_print(&USB_UART, input);
		uart_print(&Second_UART, input);
	}

	while(1);// HALT AND CATCH FIRE
}
