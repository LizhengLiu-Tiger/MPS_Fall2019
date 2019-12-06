/**
 *	Keil project for matrix keypad 4x4
 *
 *	Before you start, select your target, on the right of the "Load" button
 *
 *	@author		Tilen Majerle
 *	@email		tilen@majerle.eu
 *	@website	http://stm32f4-discovery.net
 *	@ide		Keil uVision 5
 *	@packs		STM32F4xx Keil packs version 2.2.0 or greater required
 *	@stdperiph	STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 */
/* Include core modules */
/* Include my libraries here */
#include "init.h"
#include "keypad.h"
#include <stdio.h>

int main(void) {
	/* Create keypad instance */
	TM_KEYPAD_Button_t Keypad_Button;

	/* Initialize system */
	Sys_Init();
	printf("\r\033[2J\033[1;1HKeyboard Test2\r\n");


	/* Initialize matrix keyboard */
	TM_KEYPAD_Init(TM_KEYPAD_Type_Large);

	while (1) {
		/* Read keyboard data */
		Keypad_Button = TM_KEYPAD_Read();

		/* Keypad was pressed */
		if (Keypad_Button != TM_KEYPAD_Button_NOPRESSED) {/* Keypad is pressed */
			switch (Keypad_Button) {
				case TM_KEYPAD_Button_0:		/* Button 0 pressed */
					printf("0");
					break;
				case TM_KEYPAD_Button_1:		/* Button 1 pressed */
					printf("1");
					break;
				case TM_KEYPAD_Button_2:		/* Button 2 pressed */
					printf("2");
					break;
				case TM_KEYPAD_Button_3:		/* Button 3 pressed */
					printf("3");
					break;
				case TM_KEYPAD_Button_4:		/* Button 4 pressed */
					printf("4");
					break;
				case TM_KEYPAD_Button_5:		/* Button 5 pressed */
					printf("5");
					break;
				case TM_KEYPAD_Button_6:		/* Button 6 pressed */
					printf("6");
					break;
				case TM_KEYPAD_Button_7:		/* Button 7 pressed */
					printf("7");
					break;
				case TM_KEYPAD_Button_8:		/* Button 8 pressed */
					printf("8");
					break;
				case TM_KEYPAD_Button_9:		/* Button 9 pressed */
					printf("9");
					break;
				case TM_KEYPAD_Button_STAR:		/* Button STAR pressed */
					printf("*");
					break;
				case TM_KEYPAD_Button_HASH:		/* Button HASH pressed */
					printf("#");
					break;
				case TM_KEYPAD_Button_A:		/* Button A pressed, only on large keyboard */
					printf("A");
					break;
				case TM_KEYPAD_Button_B:		/* Button B pressed, only on large keyboard */
					printf("B");
					break;
				case TM_KEYPAD_Button_C:		/* Button C pressed, only on large keyboard */
					printf("C");
					break;
				case TM_KEYPAD_Button_D:		/* Button D pressed, only on large keyboard */
					printf("D");
					break;
				default:
					break;
			}

		}
	}
}

/* 1ms handler */
//void TM_DELAY_1msHandler(void) {
//	/* Process keypad */
//	TM_KEYPAD_Update();
//}
