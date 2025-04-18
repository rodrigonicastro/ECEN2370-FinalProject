#ifndef Button_Driver_H_
#define Button_Driver_H_

#include <stdbool.h>
#include <stm32f4xx_hal.h>
#include "InterruptControl.h"

#define BUTTON_UNPRESSED 0
#define BUTTON_PRESSED 1

void Button_Init();
void Button_Interrupt_Init();
bool is_Button_Pressed();

#endif
