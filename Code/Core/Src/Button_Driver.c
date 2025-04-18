#include "Button_Driver.h"

void Button_Init(){
	GPIO_InitTypeDef button;

	button.Pin = GPIO_PIN_0;
	button.Mode = GPIO_MODE_IT_RISING;
	button.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	button.Pull = GPIO_NOPULL;

	__HAL_RCC_GPIOA_CLK_ENABLE();
	HAL_GPIO_Init(GPIOA, &button);
	EnableInterrupt(EXTI0_IRQ_NUMBER);
}