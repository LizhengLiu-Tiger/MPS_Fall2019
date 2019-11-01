/*
 * Lab04 task3 part1 (integers)
 * By Jiahong Ji (Frank)
 */


#include "init.h"
#include "math.h"

int main(void)
{
	//Initialize the system
	Sys_Init();
	printf("\r\033[2J\033[1;1HLab4-3(2)!\r\n");

	//Step 1: define integers, do summation
	asm("LDR r2, =4");
	asm("LDR r3, =2");
	asm("ADD r4, r2 , r3");
	int32_t var;
	asm("STR r4, %0" : "=m"(var));
	printf("First adding %d\r\n",var);fflush(stdout);

	//Step 2: define the integers input1 and input2, do multiplication
	int32_t input1 = 12;
	int32_t input2 = 2;
	asm("MUL %[out], %[in], %[in2]"
			:[out] "=r" (var)
			:[in] "r" (input1), [in2] "r" (input2));
	printf("Second Muti: %d\r\n",var);fflush(stdout);

	//Step 3: define the integers, result = 2x/3+5
	int32_t x =3;
	int32_t input3 = 3;
	int32_t input5 = 5;
	int32_t result =0;
	asm("MUL %[OUT], %[in2], %[in]"
			:[OUT] "=r"(result)
			 :[in2] "r" (input2), [in] "r" (x));
	asm("UDIV %[OUT], %[in2], %[in]"
			:[OUT] "=r"(result)
			 :[in2] "r" (result), [in] "r" (input3));
	asm("ADD %[OUT], %[in2], %[in]"
			:[OUT] "=r"(result)
			 :[in2] "r" (result), [in] "r" (input5));
	printf("Third Muti: %d\r\n",result);fflush(stdout);

	//Step4: MAC
	asm("UDIV %[OUT], %[in2], %[in]"
			:[OUT] "=r"(result)
			 :[in2] "r" (x), [in] "r" (input3));
	asm("MLA %[OUT], %[in2], %[in], %[add]"
			:[OUT] "=r"(result)
			 :[in2] "r" (result), [in] "r" (input2),[add] "r" (input5));
	printf("Forth MLA accum: %d\r\n",result);fflush(stdout);



}



