//--------------------------------
// Microprocessor Systems Lab 6 - Template - Lab06_p1_sample.c
//--------------------------------
//
//

#include "init.h"
#include "usbh_conf.h"
#include "usbh_hid.h"
#include "usbh_core.h"
#include "ff_gen_drv.h"
#include "usbh_diskio.h"

USBH_HandleTypeDef husbh;
HID_MOUSE_Info_TypeDef mouse_info;
uint8_t x;
uint8_t y;
uint8_t buttons[3];
int direction_x;
int direction_y;
int flag_pState;
int flag_conv;

void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

int main(void){
	 // System Initializations
	Sys_Init();

	// Application Initializations
	USBH_Init(&husbh, USBH_UserProcess, 0);
	// USBH Driver Initialization
	// USB Driver Class Registrations: Add device types to handle.
	USBH_RegisterClass(&husbh, &HID_Class);
	// Start USBH Driver
	USBH_Start(&husbh);
	while(1){
		USBH_Process(&husbh);
		if (flag_pState && flag_conv)
		{

			mouse_info = *USBH_HID_GetMouseInfo(&husbh);
			flag_conv = 0;
		}
		else
			continue;

		x = mouse_info.x;
		y = mouse_info.y;
		buttons[0] = mouse_info.buttons[0];
		buttons[1] = mouse_info.buttons[1];
		buttons[2] = mouse_info.buttons[2];

		if (x < 150)
			direction_x = 2;//Mouse Left
		if (x > 150)
		{
			x = 255 - x;
			direction_x = 1;//Mouse Right
		}
		if (y < 150)
			direction_y = 1;//Mouse down
		if (y > 150)
		{
			y = 255 - y;
			direction_y = 2;//Mouse up
		}

		if ((direction_x == 1) && (direction_y == 1))
			printf("The movement of the mouse: Left %d, Down %d\r\n", x, y);
		if ((direction_x == 1) && (direction_y == 2))
			printf("The movement of the mouse: Left %d, Up %d\r\n", x, y);
		if ((direction_x == 2) && (direction_y == 1))
			printf("The movement of the mouse: Right %d, Down %d\r\n", x, y);
		if ((direction_x == 2) && (direction_y == 2))
			printf("The movement of the mouse: Right %d, Up %d\r\n", x, y);

		printf("The button of the mouse: %d, %d, %d\r\n", buttons[0], buttons[1], buttons[2]);
		// Other stuff
	}
}

void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id) {
	  switch(id)
	  {
	  case HOST_USER_DISCONNECTION:
		  flag_pState = 0;
		  break;
	  case HOST_USER_CLASS_ACTIVE:
		  flag_pState = 1;
		  break;
	  default:
		  break;
	}
}

void USBH_HID_EventCallback(USBH_HandleTypeDef *phost)
{
	flag_conv = 1;
}
// Interrupts and Callbacks...

