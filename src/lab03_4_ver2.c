/*
 * lab03_4_ver2.c
 *
 *  Created on: Oct 7, 2019
 *      Author: hyperiontiger
 */
/*
 * Lab03_task4.c
 *
 *  Created on: Oct 1, 2019
 *      Author: hyperiontiger
 */

#include "init.h"


int input_T;
int input_V;
int len;
int char_count;
int temp;

int new_ID;
char input_1[1];
char read_att[10];
uint8_t read_ID[2];
uint8_t input_att[1];
uint8_t input_T_H;
uint8_t input_T_L;
uint8_t input_T_A;
uint8_t input_ID;
uint8_t input_state;
uint8_t input_uart;
uint8_t input_Maj;
uint8_t input_Minor;
uint8_t input_char;
uint8_t input_unlock = 128;

uint8_t reset = 1;
uint8_t addr_ID = 0;
uint8_t addr_att = 8;
uint8_t addr_Maj = 0;
uint8_t addr_Minor = 0;
uint8_t addr_state = 0;
uint8_t addr_char = 0;
uint8_t addr_tH = 0;
uint8_t addr_tL = 0;
uint8_t addr_CTRL = 0;
char user;
int check;

void configureSPI()
{
	hspi_2.Instance = SPI2; // Please use SPI2!
	hspi_2.Init.Mode = SPI_MODE_MASTER; // Set master mode
	hspi_2.Init.TIMode = SPI_TIMODE_DISABLE; // Use Motorola mode, not TI mode
	hspi_2.Init.Direction = SPI_DIRECTION_2LINES;
	hspi_2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi_2.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi_2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	hspi_2.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi_2.Init.CLKPolarity = SPI_POLARITY_HIGH;
	hspi_2.Init.NSS = SPI_NSS_HARD_OUTPUT;
	hspi_2.Init.FirstBit = SPI_FIRSTBIT_LSB;

	HAL_SPI_Init(&hspi_2);
}

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

void version(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi_2, &addr_Maj, 1, HAL_MAX_DELAY);//Read Maj
	for(int i_3 = 0;i_3<2160;i_3++){
	}
	HAL_SPI_Receive(&hspi_2, &input_Maj, 1, HAL_MAX_DELAY);//Rev Maj
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi_2, &addr_Minor, 1, HAL_MAX_DELAY);//Read Minor
	for(int i_3 = 0;i_3<2160;i_3++){
	}
	HAL_SPI_Receive(&hspi_2, &input_Minor, 1, HAL_MAX_DELAY);//Rev Minor
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	input_V = (((unsigned int)(input_Maj) << 8) | input_Minor);
	printf("\033[7;5HVersion: %x\r\n", input_V);
}

void get_temp(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi_2, &addr_CTRL, 1, HAL_MAX_DELAY);//Read
	for(int i_3 = 0;i_3<2160;i_3++){
	}
	HAL_SPI_Transmit(&hspi_2, &addr_CTRL, 1, HAL_MAX_DELAY);//Request temp
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

	while(1)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi_2, &addr_state, 1, HAL_MAX_DELAY);//Read
		for(int i_3 = 0;i_3<2160;i_3++){
		}
		HAL_SPI_Receive(&hspi_2, &input_state, 1, HAL_MAX_DELAY);//check if ready
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

		check = (input_state & 8);

		if(check)
			break;

	}

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi_2, &addr_tH, 1, HAL_MAX_DELAY);//Read
	for(int i_3 = 0;i_3<2160;i_3++){
	}
	HAL_SPI_Receive(&hspi_2, &input_T_H, 1, HAL_MAX_DELAY);//Read temp H
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi_2, &addr_tL, 1, HAL_MAX_DELAY);//Read
	for(int i_3 = 0;i_3<2160;i_3++){
	}
	HAL_SPI_Receive(&hspi_2, &input_T_L, 1, HAL_MAX_DELAY);//Read tempL
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

	input_T = (((unsigned int)(input_T_H) << 8) | input_T_L);
	printf("\033[5;5HTemp: %d\r\n", input_T);
}

void get_ID(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi_2, &addr_ID, 1, HAL_MAX_DELAY);//Read ID reg
	for(int i_3 = 0;i_3<2160;i_3++){
	}
	HAL_SPI_Receive(&hspi_2, &input_ID, 1, HAL_MAX_DELAY);//Rev ID
	printf("\033[4;5HCurrent ID: %x\r\n", input_ID);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

}

void reset_device(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi_2, &addr_CTRL, 1, HAL_MAX_DELAY);//Read ID reg
	for(int i_3 = 0;i_3<2160;i_3++){
	}
	HAL_SPI_Transmit(&hspi_2, &reset, 1, HAL_MAX_DELAY);//Rev ID
	printf("\033[17;1H The device has been reset\r\n");
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

}

void change_ID(void)
{

	//change ID
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi_2, &addr_CTRL, 1, HAL_MAX_DELAY);//Read CTRL
	for(int i_3 = 0;i_3<2160;i_3++){
	}
	HAL_SPI_Transmit(&hspi_2, &input_unlock, 1, HAL_MAX_DELAY);//Unlock ID
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

	HAL_UART_Receive(&USB_UART, &read_ID[0], 1, HAL_MAX_DELAY);
	//HAL_Delay(500);
//	HAL_UART_Receive(&USB_UART, &read_ID[1], 1, HAL_MAX_DELAY);
	new_ID = (((unsigned int)(read_ID[0]) << 4) | read_ID[1]);
	//rev change
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi_2, &addr_ID, 1, HAL_MAX_DELAY);//Read ID reg
	for(int i_3 = 0;i_3<2160;i_3++){
	}
	HAL_SPI_Transmit(&hspi_2, &read_ID[0], 1, HAL_MAX_DELAY);//Rev ID
	printf("\033[4;40Hnew ID: %x\r\n", input_ID);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

}

void change_att(void)
{
	input_att[0] = 1;
	len = 0;
	while (input_att[0] != 64 && len < 10)//type @ to end
	{
	HAL_UART_Receive(&USB_UART, input_att, 1, HAL_MAX_DELAY);
	if(input_att[0] != 64){
	read_att[len]= input_att[0];
	len++;
	}
	}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi_2, &addr_att, 1, HAL_MAX_DELAY);//req att
	for(int i_3 = 0;i_3<2160;i_3++){
		}
	HAL_SPI_Transmit(&hspi_2, read_att, len, HAL_MAX_DELAY);//set att
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
}

int main(void)
{
	/*main code
	 *
	 */
	Sys_Init();
	HAL_UART_MspInit(&USB_UART);
	configureSPI();
	printf("\r\033[2J\033[1;1HYour code here!\r\n");

	addr_ID = 9;
	addr_state = 3;
	addr_char = 7;
	addr_tH = 6;
	addr_tL = 5;
	addr_CTRL = 2;
	addr_Minor = 1;

	version();

	printf("\033[13:1H Now in data transfering mode");
	while(1)
	{	printf("\033[12;1H Press esc to update info and ID");
		if(HAL_UART_Receive(&USB_UART, input_1, 1, 10) == HAL_OK)
		{
		/*user menu
		 *
		 */
			while(input_1[0] == 27)
			{
				printf("\033[2J");
				version();
				get_temp();
				get_ID();
				printf("\033[11:1H Press u to Reset the terminal\n");
				printf("\033[12:1H Press o to change the ID\n");
				printf("\033[13:1H Press i to change the attribute\n");
				printf("\033[15:1H Press other button to go back\n");
				HAL_UART_Receive(&USB_UART, input_1, 1, HAL_MAX_DELAY);
				if(input_1[0] == 111)
				{
					change_ID();
				}
				if(input_1[0] == 105)
				{
					change_att();

				}
				if(input_1[0] == 117)
				{
					reset_device();
				}

				if(input_1[0] == 27) break;
			}

			/* Char input
			 *
			 */
			input_char = input_1[0];
			//printf("\033[2J");
			printf("\033[11:1H\033[2K");
			printf("\033[12:1H\033[2K");
			printf("\033[13:1H\033[2K");
			printf("\033[14:1H\033[2K");
			printf("\033[13:1H Now in data transfering mode");

			version();

			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
			HAL_SPI_Transmit(&hspi_2, &addr_char, 1, HAL_MAX_DELAY);//Req char
			for(int i_3 = 0;i_3<2160;i_3++){
			}
			HAL_SPI_Transmit(&hspi_2, &input_char, 1, HAL_MAX_DELAY);//transmit char
			//printf("\033[4;5HCurrent ID: %x\r\n", input_ID);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);


		}

		else
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
			HAL_SPI_Transmit(&hspi_2, &addr_state, 1, HAL_MAX_DELAY);//read the 6,7, # of things in buffer
			for(int i_3 = 0;i_3<2160;i_3++){
			}
			HAL_SPI_Receive(&hspi_2, &input_char, 1, HAL_MAX_DELAY);//transmit char
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

			temp = (input_char & 96);   // &= 0110 0000 to get 7,6 bit
			if (temp == 96)
				char_count=3;
			if(temp == 0)
				char_count = 0;
			if(temp == 32)
				char_count = 1;
			if(temp == 64)
				char_count = 2;

			if(char_count != 0)
			{
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
				HAL_SPI_Transmit(&hspi_2, &addr_char, 1, HAL_MAX_DELAY);//read the 6,7, # of things in buffer
				for(int i_3 = 0;i_3<2160;i_3++){
				}
				HAL_SPI_Receive(&hspi_2, &input_char, 1, HAL_MAX_DELAY);//transmit char
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

				printf("\033[16;1H The input from the slave is %c", input_char);
			}

		}




	}

}
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
//		HAL_SPI_Transmit(&hspi_2, &addr_state, 1, HAL_MAX_DELAY);//Set reg 3
//		for(int i_1 = 0;i_1<2160;i_1++){
//		}
//		HAL_SPI_Receive(&hspi_2, &input_state, 1, HAL_MAX_DELAY);//req state
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
//		printf("\033[3;5HState: %x\r\n", input_state);
//
