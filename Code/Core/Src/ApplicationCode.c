/*
 * ApplicationCode.c
 *
 *  Created on: Dec 30, 2023 (updated 11/12/2024) Thanks Donavon! 
 *      Author: Xavion
 */

#include "ApplicationCode.h"

/* Static variables */

#define GPIOA_PORT_NUMBER 0

extern void initialise_monitor_handles(void); 

void ApplicationInit(void)
{
	initialise_monitor_handles(); // Allows printf functionality
    LTCD__Init();
    LTCD_Layer_Init(0);
    LCD_Clear(0,LCD_COLOR_WHITE);

    #if COMPILE_TOUCH_FUNCTIONS == 1
	InitializeLCDTouch();

	// This is the orientation for the board to be direclty up where the buttons are vertically above the screen
	// Top left would be low x value, high y value. Bottom right would be low x value, low y value.
	StaticTouchData.orientation = STMPE811_Orientation_Portrait_2;

	#endif // COMPILE_TOUCH_FUNCTIONS
	
	greenLEDInit();
	turnOffGreenLED();
	redLEDInit();
	turnOffRedLED();
	ButtonInit();
	addSchedulerEvent(DISPLAY_MENU_SCREEN_EVENT);
}

void LCD_Visual_Demo(void)
{
	visualDemo();
}

#if COMPILE_TOUCH_FUNCTIONS == 1
void LCD_Touch_Polling_Demo(void)
{
	LCD_Clear(0,LCD_COLOR_GREEN);
	while (1) {
		/* If touch pressed */
		if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed) {
			/* Touch valid */
			printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
			LCD_Clear(0, LCD_COLOR_RED);
		} else {
			/* Touch not pressed */
			printf("Not Pressed\n\n");
			LCD_Clear(0, LCD_COLOR_GREEN);
		}
	}
}
#endif // COMPILE_TOUCH_FUNCTIONS

void ButtonInit(){
	Button_Init();
}

void greenLEDInit(){
	LED_Init(GREEN_LED);
}

void redLEDInit(){
	LED_Init(RED_LED);
}

void toggleGreenLED(){
	ToggleLED(GREEN_LED);
}

void toggleRedLED(){
	ToggleLED(RED_LED);
}

void turnOnGreenLED(){
	TurnOnLED(GREEN_LED);
}

void turnOnRedLED(){
	TurnOnLED(RED_LED);
}

void turnOffGreenLED(){
	TurnOffLED(GREEN_LED);
}

void turnOffRedLED(){
	TurnOffLED(RED_LED);
}

void Display_Menu_Screen(){
	LCD_Clear(0, LCD_COLOR_WHITE);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	LCD_DisplayChar(80,10,"W");
	LCD_DisplayChar(92,10,"e");
	LCD_DisplayChar(100,10,"l");
	LCD_DisplayChar(107,10,"c");
	LCD_DisplayChar(117,10,"o");
	LCD_DisplayChar(133,10,"m");
	LCD_DisplayChar(148,10,"e");
	// LCD_DisplayChar(156,10,'!');

	LCD_DisplayChar(45,30,"t");
	LCD_DisplayChar(55,30,"o");
	
	LCD_DisplayChar(85,30,"C");
	LCD_DisplayChar(100,30,"o");
	LCD_DisplayChar(113,30,"n");
	LCD_DisplayChar(126,30,"n");
	LCD_DisplayChar(139,30,"e");
	LCD_DisplayChar(151,30,"c");
	LCD_DisplayChar(161,30,"t");
	LCD_DisplayChar(181,30,"4");
	LCD_DisplayChar(190,30,"!");

	LCD_DisplayChar(60,70,"Select Mode:");

	LCD_DisplayChar(15,180,"1-player");
	LCD_DisplayChar(150,180,"2-player");
}

void EXTI0_IRQHandler(){ //Button Interrupt Handler
	DisableInterrupt(EXTI0_IRQ_NUMBER);
	
	//add handler code

	ClearPendingEXTIBit(1 << GPIOA_PORT_NUMBER);
	EnableInterrupt(EXTI0_IRQ_NUMBER);
}
