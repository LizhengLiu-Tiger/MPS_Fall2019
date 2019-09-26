/*
 * Lab2_4Depth.c
 *
 *  Created on: Sep 19, 2019
 *      Author: hyperiontiger
 *
 */
//----------------------------------

//
//
// -- Imports ---------------
//
#include "init.h"
#include "stdlib.h"

//
//
// -- Prototypes ------------
//
void blinkScreen();
void Init_GPIO();
void Init_Timer();

//
//
// -- Code Body -------------
//

int flag_1 = 0;//C8 flag
int flag_2 = 0;//timer 6 flag
int flag_3 = 0;//J0 flag
int count_tenth = 0;//0.1 sec flag
float timeReact = 0;//real reaction time
float total_time = 0;//totoal reaction time
int total_trial = 0;//total trials in one round
int run = 0;//game run flag
int pre = 0;//problem solving flag

TIM_HandleTypeDef htim7;

int main() {
	Sys_Init();
	Init_Timer();
	Init_GPIO();
    	printf("\033[2J\r\n");
	printf("TEST!\r\n");
	while (1) {
		// Main loop code goes here
		//printf("\033c\033[36m\033[2J");
		//printf("\r\033[6;1H                                 \r\n");
		run = 1;

		HAL_Delay(rand()%2000 + 1000); //Delay for a random time between 1s to 3s
		blinkScreen();
		TIM6->CR1 = 0x0001;//start the timer

		while (run)//game starts
		{
			if (pre)//clear the problem
				flag_1 = 0;
			if (!flag_1 && flag_3)//end game
			{
				if (total_trial)//more than one trial should be taken, if not, print error
					printf("\r\033[5;1HThe average reaction time is : %.1f\r\n",total_time / total_trial);
				else
					printf("\r\033[5;1HTry more times.                      \r\n");

				printf("\r\033[6;1HRestart the game!              \r\n");
				//clear variables
				flag_1 = 0;
				count_tenth = 0;
				timeReact = 0;
				run = 0;
				total_trial = 0;
				total_time = 0;
				TIM6->CR1 = 0x0002;

				//break;
			}
			else if (!flag_1 && !flag_3) // no action by the user

			{
				printf("\r\033[6;1HWaiting for user's reaction\r\n");
				pre = 0;
//				count_tenth ++;
			}

			else if (flag_1 && !flag_3)//user reacts to the flashing acreen
			{
				timeReact = (float) count_tenth / 20;//Should be divided by 10 mathematically, but the counting frequency seems to have unknown error so I just divede it by 20.
				printf("\r\033[7;1HThe reaction time is: %.1f .\r\n", timeReact);
				//do not increment if the reaction time is mistakenly 0
				if (timeReact != 0)
				{
					total_trial ++;
					total_time += timeReact;
				}
				//clear the variables
				flag_1 = 0;
				count_tenth = 0;
				timeReact = 0;
				run = 0;
				pre = 1;
				TIM6->CR1 = 0x0002;


				//break;
			}

		}
		//clear all flags again
		flag_3 = 0;
		count_tenth = 0;
		timeReact = 0;
		run = 0;
		TIM6->CR1 = 0x0002;

	}
}

//
//
// -- Utility Functions ------
//
void blinkScreen(){
	printf("\033[30;47m");
	// Clear and redraw display (flash it & sound the bell).
	printf("\a\033[s\033[2J\033[u");
	fflush(stdout);
	HAL_Delay(100);

	printf("\033[37;40m");
	// Clear and redraw display (flash it).
	printf("\033[s\033[2J\033[u");
	fflush(stdout);
}


//
//
// -- Init Functions ----------
//
void Init_Timer() {
	// Enable the TIM6 interrupt.
	// Looks like HAL hid this little gem, this register isn't mentioned in
	//   the STM32F7 ARM Reference Manual...
	
	//Rounte NVIC to EXTI
	NVIC->ISER[6 / 32] = (uint32_t) 1 << (6 % 32); //EXIT0
	NVIC->ISER[54 / 32] = (uint32_t) 1 << (54 % 32); //Timer6

	// Enable TIM6 clock
	 RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	 asm ( "nop" );
	 asm ( "nop" );

	 HAL_NVIC_SetPriority(TIM7_IRQn, 0, 0);
	// Set pre-scaler to slow down ticlks: time overflow: 0.1 s
	 TIM6->PSC = 0x00A4U;

	// Set the Auto-reload Value for 10Hz overflow
	 TIM6->ARR = 0xFFFEU;

	// Generate update events to auto reload.
	 TIM6->EGR = 0x0001;

	// Enable Update Interrupts.
	 TIM6->DIER = 0x0001;

	// Start the timer.
	 TIM6->CR1 = 0x0000;
	 //Enable Timer 7
	 HAL_Init();
	 htim7.Instance = TIM7;
	 htim7.Init.Prescaler = 24720;
	 htim7.Init.Period = 43690;
	 __HAL_RCC_TIM7_CLK_ENABLE();
	 HAL_NVIC_SetPriority(TIM7_IRQn, 0, 0);
	 HAL_NVIC_EnableIRQ(TIM7_IRQn);
	 HAL_TIM_Base_Init(&htim7);
	 HAL_TIM_Base_Start_IT(&htim7);
}

void Init_GPIO() {
	// Enable GPIO clocks?
	// Looks like GPIO reg updates are synced to a base clock.
	//  for any changes to appear the clocks need to be running.
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	// Enable clock to SYSCONFIG module to enable writing of EXTICRn registers
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	// or __HAL_RCC_SYSCFG_CLK_ENABLE();
	// Delay after an RCC peripheral clock enabling
	asm ("nop");
	asm ("nop");

	// Set C8 J0 to input mode
	GPIOJ->MODER &= 0x00000000U;
	GPIOC->MODER &= 0x00000000U;

	// GPIO Interrupt
	// By default pin PA0 will trigger the interrupt, change EXTICR1 to route proper pin
	SYSCFG->EXTICR[0] |= 0x00000009U;  // EXTICR1-4 are confusingly an array [0-3]. Route the EXTICR to C8 and J0
	SYSCFG->EXTICR[2] |= 0x00000002U;

	// Set C8 J0 as input (button) with pull-down.
	GPIOJ->PUPDR |= 0x00000002U;
	GPIOC->PUPDR |= 0x00020000U;

	// Set interrupt enable for EXTI0,8
	 NVIC->ISER[6 / 32] = (uint32_t) 1 << (6 % 32); //EXIT0
	 NVIC->ISER[23 / 32] = (uint32_t) 1 << (23 % 32); //EXIT8
	// Unmask interrupt line 0 and 8.
	 EXTI->IMR |= 0x00000001U;
	 EXTI->IMR |= 0x00000100U;

	// Register for rising edge.
//	 EXTI->RTSR |= 0x00000001U;
//	 EXTI->RTSR |= 0x00000100U;
	// And register for the falling edge.
	 EXTI->FTSR |= 0x00000001U;
	 EXTI->FTSR |= 0x00000100U;
	 //HAL
	 GPIO_InitTypeDef gpio_init;
	 gpio_init.Pin = GPIO_PIN_8;
	 gpio_init.Mode = GPIO_MODE_IT_RISING;
	 gpio_init.Pull = GPIO_PULLDOWN;
	 HAL_GPIO_Init(GPIOC, &gpio_init);
	
	 	//enable the IRQ
	 HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

//
//
// -- ISRs (IRQs) -------------
//
void TIM6_DAC_IRQHandler() {
	// Clear Interrupt Bit
	TIM6->SR &= 0x0000;
	// Other code here:
	flag_2 = 1;
	count_tenth ++;
}

// Non-HAL GPIO/EXTI Handler
void EXTI0_IRQHandler() {
	// Clear Interrupt Bit by setting it to 1
	
	EXTI->PR |= 0x00000001U;
	flag_1 = 1;


}

//HAL - GPIO/EXTI Handler
void TIM7_IRQHandler() {
	//HAL_GPIO_EXTI_IRQHandler(???);
	HAL_TIM_IRQHandler(&htim7);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin_8){
	// ISR code here.
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	if(htim->Instance == TIM7){
	}
		//counter_10++;
}

void EXTI9_5_IRQHandler(void) {
	//Enable IRQ for EXTI 8
	HAL_GPIO_EXTI_IRQHandler(8);
	if(__HAL_GPIO_EXTI_GET_FLAG(EXTI_PR_PR8))//interrupt
	{
		//trigger++;
		flag_3 = 1;
		//clear the flag
		__HAL_GPIO_EXTI_CLEAR_FLAG(EXTI_PR_PR8);
	}


}

// For the HAL timer interrupts, all of the associated Callbacks need to exist,
// otherwise during assembly, they will generate compiler errors as missing symbols
// Below are the ones that are not used.

// void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim){};
// void HAL_TIMEx_CommutationCallback(TIM_HandleTypeDef *htim){};
