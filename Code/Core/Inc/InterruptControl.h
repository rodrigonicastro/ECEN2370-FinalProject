#ifndef INTERRUPTCONTROL_H_
#define INTERRUPTCONTROL_H_

//#include "STM32F429i.h"
#include <stm32f4xx_hal.h>

#define EXTI0_IRQ_NUMBER 6

void EnableInterrupt(IRQn_Type IRQ_Number);

void DisableInterrupt(IRQn_Type IRQ_Number);

void ClearPendingInterrupt(IRQn_Type IRQ_Number);

void SetPendingInterrupt(IRQn_Type IRQ_Number);

void ClearPendingEXTIBit(uint8_t PinNumber);
#endif