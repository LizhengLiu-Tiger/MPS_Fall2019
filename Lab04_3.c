/*
 * Lab04_3.c
 *
 *  Created on: Oct 17, 2019
 *      Author: hyperiontiger
1. In basic inline assembly only: loads (into registers) and adds two integer numbers (hard-coded).
Use extended assembly to pass the resultant value to a C variable and print it.
2. In extended assembly: multiply 2 single precision floats.
3. In extended assembly: evaluate the equation 2x + 5 through floating point addition (VADD), multipli-
cation (VMUL), and division (VDIV).
4. In extended assembly: evaluate the previous equation using floating point MAC commands where
applicable.
 */

#include "init.h"

uint8_t var;
double var3;
double var1;
double var2;
double x;
double in_1;
double in_2;
volatile double in_3;

int main()
{
	Sys_Init();

	printf("\r\033[2J\033[1;1HLab4-3!\r\n");

	//Set 1: define the integer: r2 = 6, r3 = 7
	asm("LDR r2, =6");
	asm("LDR r3, =7");
	asm("ADD r4, r2, r3");

	asm("STR r4,%0" : "=m" (var));

	printf("Set1:%d\r\n", var);


	//Set 2: define the floating numbers: var1 = 97.33, var2 = 22.66
	var1 = 97.33;
	var2 = 22.66;

    asm("VMUL.F64 %P[out],%P[in1],%P[in2]"
           :[out] "=w" (var3)
           :[in1] "w" (var1),[in2] "w" (var2));

    printf("Set2: %f\r\n", var3);

    //Set 3: 2x/3+5
    x = 6;
    in_1 = 2;
    in_2 = 3;
    in_3 = 5;

    asm("VMUL.F64 %P[out],%P[in1],%P[in2]"
           :[out] "=w" (var3)
           :[in1] "w" (x),[in2] "w" (in_1));
    asm("VDIV.F64 %P[out],%P[in1],%P[in2]"
           :[out] "=w" (var3)
           :[in1] "w" (var3),[in2] "w" (in_2));
    asm("VADD.F64 %P[out],%P[in1],%P[in2]"
           :[out] "=w" (var3)
           :[in1] "w" (var3),[in2] "w" (in_3));

    printf("Set3: %f\r\n", var3);

    //Set 4: MAC
    x = 6;
    in_1 = 2;
    in_2 = 3;
    in_3 = 5;
    asm("VDIV.F64 %P[out],%P[in1],%P[in2]"
           :[out] "=w" (var3)
           :[in1] "w" (x),[in2] "w" (in_2));
    asm volatile ("VMLA.F64 %P[out],%P[in1],%P[in2]"
           :[out] "+&w" (in_3)
           :[in1] "w" (var3),[in2] "w" (in_1));

    printf("Set4: %f\r\n", in_3);



}
