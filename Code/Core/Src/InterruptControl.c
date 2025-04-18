#include "InterruptControl.h"

void EnableInterrupt(IRQn_Type IRQ_Number){
	HAL_NVIC_EnableIRQ(IRQ_Number);
}

void DisableInterrupt(IRQn_Type IRQ_Number){
	HAL_NVIC_DisableIRQ(IRQ_Number);
}

void SetPendingInterrupt(IRQn_Type IRQ_Number){
	HAL_NVIC_SetPendingIRQ(IRQ_Number);
}

void ClearPendingInterrupt(IRQn_Type IRQ_Number){
	HAL_NVIC_ClearPendingIRQ(IRQ_Number);
}

void ClearPendingEXTIBit(uint8_t PinNumber){
	__HAL_GPIO_EXTI_CLEAR_FLAG(PinNumber);
}