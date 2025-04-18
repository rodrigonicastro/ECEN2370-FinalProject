#include "LED_Driver.h"

void LED_Init(uint8_t LED){
	GPIO_InitTypeDef led;
	switch(LED){
		case 0:
			led.Pin = GPIO_PIN_14;
			led.Mode = GPIO_MODE_OUTPUT_PP;
			led.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
			led.Pull = GPIO_NOPULL;
			break;
		case 1:
			led.Pin = GPIO_PIN_13;
			led.Mode = GPIO_MODE_OUTPUT_PP;
			led.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
			led.Pull = GPIO_NOPULL;
	}

	__HAL_RCC_GPIOG_CLK_ENABLE();
	HAL_GPIO_Init(GPIOG, &led);
}

void ToggleLED(uint8_t LED){
	switch(LED){
		case 0:
			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);
			break;
		case 1:
			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
			break;
	}
}

void TurnOffLED(uint8_t LED){
	switch(LED){
		case 0:
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, 0);
			break;

		case 1:
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, 0);
			break;
	}
}

void TurnOnLED(uint8_t LED){
	switch(LED){
		case 0:
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, 1);
			break;

		case 1:
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, 1);
			break;
	}
}