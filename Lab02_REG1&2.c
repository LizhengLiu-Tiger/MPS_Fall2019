//----------------------------------
// Lab 2 - Timer Interrupts - Lab02.c
//----------------------------------
// Objective:
//   Build a small game that records user's reaction time.
//

//
//
// -- Imports ---------------
//
#include "init.h"

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
volatile uint8_t timeUpdated = 0;
volatile uint8_t buttonPressed = 0;
volatile uint8_t buttonReleased = 0;
volatile uint32_t elapsed = 0;

int32_t randomNumber = 0;
uint32_t startTime = 0;
float averageScore = 0;
unsigned int iterations = 0;
int flag_1 = 0;
int flag_2 = 0;
int count_ten = 0;
int trigger = 0;

int main() {
	Sys_Init();
	Init_Timer();
	Init_GPIO();
    printf("\033[2J\r\n");
	printf("TEST!\r\n");
	while (1) {
		// Main loop code goes here
		//printf("\033c\033[36m\033[2J");
		if (flag_2)
		{
			printf("\r\033[8;1H Tenth sec have passed: %d.\r\n", count_ten);
			count_ten ++;
			flag_2 = 0;
		}

		if (trigger)
		{
			printf("\r\033[6;1HPushbutton 2 is pressed   \r\n");
			trigger = 0;
		}
		else
			printf("\r\033[6;1HPushbutton 2 is not pressed\r\n");

		if (flag_1) //****Question about how to test the last bit only*********
		{
				printf("\r\033[5;1HPushbutton 1 is pressed   \r\n");
				flag_1 = 0;
		}
		else
			printf("\r\033[5;1HPushbutton 1 is not pressed\r\n");
		//blinkScreen(); // Alternatively: in some terminals, the BELL can be configured to produce
					   // 			a "visual bell" ... a blink.
		HAL_Delay(1000);
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
	//   the STM32F7 ARM Reference Manual....
	//NVIC->ISER[IRQ6 / 32] = (uint32_t) 1 << (IRQ6 % 32); //EXIT0
	NVIC->ISER[54 / 32] = (uint32_t) 1 << (54 % 32); //Timer6

	// Enable TIM6 clock
	 RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	 asm ( "nop" );
	 asm ( "nop" );

	// Set pre-scaler to slow down ticlks
	 TIM6->PSC = 0x405FU;

	// Set the Auto-reload Value for 10Hz overflow
	 TIM6->ARR = 0xFFFEU;

	// Generate update events to auto reload.
	 TIM6->EGR = 0x0001;

	// Enable Update Interrupts.
	 TIM6->DIER = 0x0001;

	// Start the timer.
	 TIM6->CR1 = 0x0001;

//	 HAL_Init();
//	 	htim7.Instance = TIM7;
//	 	htim7.Init.Prescaler = 24720;
//	 	htim7.Init.Period = 43690;
//
//
//
//
//	 	__HAL_RCC_TIM7_CLK_ENABLE();
//
//
//	 	HAL_NVIC_SetPriority(TIM7_IRQn, 0, 0);
//	 	HAL_NVIC_EnableIRQ(TIM7_IRQn);
//
//	 	HAL_TIM_Base_Init(&htim7);
//	 	HAL_TIM_Base_Start_IT(&htim7);

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

	 GPIO_InitTypeDef gpio_init;
	 	gpio_init.Pin = GPIO_PIN_8;
	 	gpio_init.Mode = GPIO_MODE_IT_RISING;
	 	gpio_init.Pull = GPIO_PULLDOWN;
	 	HAL_GPIO_Init(GPIOC, &gpio_init);

	 	//enable the I
	 	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

//
//
// -- ISRs (IRQs) -------------
//
void TIM6_DAC_IRQHandler() {
	// Clear Interrupt Bit
	TIM6->SR &= 0x0000;
	//printf("\rtest1111111111\r\n");
	// Other code here:
	flag_2 = 1;
}

// Non-HAL GPIO/EXTI Handler
void EXTI0_IRQHandler() {
	// Clear Interrupt Bit by setting it to 1. ******Q:why clear PR firstly?**********

	if ((EXTI->PR | 0x11111110U) == 0x11111111U)
	{
		flag_1 = 1;
		EXTI->PR |= 0x00000001U;
	}
	// Other code here:


}

//HAL - GPIO/EXTI Handler
void EXTI9_5_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(8);

	if(__HAL_GPIO_EXTI_GET_FLAG(EXTI_PR_PR8)	)
	{
		trigger = 1;
		__HAL_GPIO_EXTI_CLEAR_FLAG(EXTI_PR_PR8);
	}


}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin_8){
	// ISR code here.
}


// For the HAL timer interrupts, all of the associated Callbacks need to exist,
// otherwise during assembly, they will generate compiler errors as missing symbols
// Below are the ones that are not used.

// void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim){};
// void HAL_TIMEx_CommutationCallback(TIM_HandleTypeDef *htim){};
