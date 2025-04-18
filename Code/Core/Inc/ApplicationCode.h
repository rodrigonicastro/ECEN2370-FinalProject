/*
 * ApplicationCode.h
 *
 *  Created on: Dec 30, 2023
 *      Author: Xavion
 */

#include "LCD_Driver.h"
#include "stm32f4xx_hal.h"
#include "Button_Driver.h"
#include "LED_Driver.h"
#include "Scheduler.h"
#include <stdio.h>


#ifndef INC_APPLICATIONCODE_H_
#define INC_APPLICATIONCODE_H_

void ApplicationInit(void);
void LCD_Visual_Demo(void);

#if (COMPILE_TOUCH_FUNCTIONS == 1)
void LCD_Touch_Polling_Demo(void);
#endif // (COMPILE_TOUCH_FUNCTIONS == 1)

#if COMPILE_TOUCH_FUNCTIONS == 1
static STMPE811_TouchData StaticTouchData;
#endif // COMPILE_TOUCH_FUNCTIONS

void ButtonInit();

void greenLEDInit();

void redLEDInit();

void toggleGreenLED();

void toggleRedLED();

void turnOnGreenLED();

void turnOnRedLED();

void turnOffGreenLED();

void turnOffRedLED();

void Display_Menu_Screen();

#endif /* INC_APPLICATIONCODE_H_ */
