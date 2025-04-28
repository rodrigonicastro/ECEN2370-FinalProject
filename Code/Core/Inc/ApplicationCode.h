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
#include <stdbool.h>
#include <stdlib.h>

#ifndef INC_APPLICATIONCODE_H_
#define INC_APPLICATIONCODE_H_

#define USE_AI_FOR_SINGLE_PLAYER 1

void ApplicationInit(void);
void LCD_Visual_Demo(void);

#if (COMPILE_TOUCH_FUNCTIONS == 1)
void LCD_Touch_Polling_Demo(void);
#endif // (COMPILE_TOUCH_FUNCTIONS == 1)

#if COMPILE_TOUCH_FUNCTIONS == 1
static STMPE811_TouchData StaticTouchData;
#endif // COMPILE_TOUCH_FUNCTIONS


#define NUM_COLS 7
#define NUM_ROWS 6

static int board[NUM_ROWS][NUM_COLS];

static int num_blue_victories;
static int num_red_victories;
static int num_ties;
static int turns;

typedef enum{
    ONGOING = 0,
    BLUE_WINS = 1,
    RED_WINS = 2,
    TIE = 3
} GAME_RESULT;

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

void Display_Board();

void Update_Board(int row, int col, int value);

void Display_Coins();

void Single_Player(RNG_HandleTypeDef* hrng);

int Generate_Random_Move(RNG_HandleTypeDef* hrng);

int Generate_AI_Move(RNG_HandleTypeDef* hrng);

bool Place_Coin(int position, int player);

void Two_Player();

GAME_RESULT Game_Status();

void Display_Results(uint32_t seconds);

void Display_Quit_Screen();

void Reset_Board();

void Reset_Stats();

void Start_Timer();

void Stop_Timer();

void Reset_Timer();

uint32_t Get_Timer_Value();

#endif /* INC_APPLICATIONCODE_H_ */
