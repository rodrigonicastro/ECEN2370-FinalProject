#ifndef LED_Driver_H_
#define LED_Driver_H_

#include <stm32f4xx_hal.h>
#define GREEN_LED 1
#define RED_LED 0

void LED_Init(uint8_t LED);

void ToggleLED(uint8_t LED);

void TurnOffLED(uint8_t LED);

void TurnOnLED(uint8_t LED);

#endif