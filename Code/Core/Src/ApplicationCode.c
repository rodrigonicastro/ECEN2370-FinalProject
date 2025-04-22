/*
 * ApplicationCode.c
 *
 *  Created on: Dec 30, 2023 (updated 11/12/2024) Thanks Donavon! 
 *      Author: Xavion
 */

#include "ApplicationCode.h"

/* Static variables */

#define GPIOA_PORT_NUMBER 0

#define FIRST_PLAYER 1
#define SECOND_PLAYER 2

extern void initialise_monitor_handles(void); 

static bool Drop_Coin = 0;

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

void Display_Board(){
	for(int i = 0; i <= NUM_COLS; i++){
		LCD_Draw_Vertical_Line(15+30*i, 80, 181, LCD_COLOR_BLACK);
	}

	for(int i = 0; i <= NUM_ROWS; i++){
		LCD_Draw_Horizontal_Line(15, 80+30*i, 210, LCD_COLOR_BLACK);
	}
}

void Update_Board(int row, int col, int value){
	board[row][col] = value;
}

void Display_Coins(){
	for(int i = 0; i < NUM_ROWS; i++){
		for(int j = 0; j < NUM_COLS; j++){
			if(board[i][j] == 1){
				LCD_Draw_Circle_Fill(30+30*j, 95+30*i, 8, LCD_COLOR_BLUE);
			}

			else if(board[i][j] == 2){
				LCD_Draw_Circle_Fill(30+30*j, 95+30*i, 8, LCD_COLOR_RED);
			}
		}
	}
}

void Single_Player(RNG_HandleTypeDef* hrng){
	turns = 0;

	while(Game_Status() == ONGOING){
		//User Turn
		if(turns % 2 == 0){
			int position = 0;
			LCD_Clear(0, LCD_COLOR_WHITE);
			Display_Board();
			Display_Coins();
			LCD_Draw_Circle_Fill(30, 65, 8, LCD_COLOR_BLUE);

			//Choosing Coin Placement Routine
			while(1){
				bool touched = 0;
				if(returnTouchStateAndLocation(&StaticTouchData)  == STMPE811_State_Pressed){
					LCD_Quadrant touchedQuadrant = returnTouchQuadrant(StaticTouchData);
					if(touchedQuadrant == BOTTOM_RIGHT){
						if(position <= 0) position = 6;
						else position--;

						touched = 1;
					}
	
					else if(touchedQuadrant == BOTTOM_LEFT){
						if(position >= 6) position = 0;
						else position++;

						touched = 1;
					}
				}
				
				//If user touched left or right, update coin position accordingly
				if(touched){
					LCD_Clear(0, LCD_COLOR_WHITE);
					Display_Board();
					Display_Coins();
					LCD_Draw_Circle_Fill(30+30*position, 65, 8, LCD_COLOR_BLUE2);

					touched = 0;
				}

				if(Drop_Coin){
					Drop_Coin = 0;
					break;
				}
			}
			
			//Place Coin Routine
			bool valid = Place_Coin(position, FIRST_PLAYER);
			
			//If move is not valid, continue on user's turn
			if(!valid) continue;
		}

		//ai turn
		else{
			LCD_DisplayChar(30, 35, "Generating Move.");
			HAL_Delay(750);
			LCD_DisplayChar(30, 35, "Generating Move..");
			HAL_Delay(750);
			LCD_DisplayChar(30, 35, "Generating Move...");

			#if USE_AI_FOR_SINGLE_PLAYER == 0
			while(!Place_Coin(Generate_Random_Move(hrng), SECOND_PLAYER)); //generate random moves until one is valid
			#else
			//Intelligent AI move generator
			#endif

		}
		turns++;
	}
	addSchedulerEvent(DISPLAY_RESULTS_EVENT);
}

int Generate_Random_Move(RNG_HandleTypeDef* hrng){
	uint32_t randNum = 0;
	HAL_RNG_GenerateRandomNumber(hrng, &randNum);

	return randNum % 7;
}

bool Place_Coin(int position, int player){
	if(position > 6 || position < 0) return false;

	for(int i = NUM_ROWS-1; i >= 0; i--){
		if(board[i][position] == 0){
			Update_Board(i, position, player);
			LCD_Clear(0, LCD_COLOR_WHITE);
			Display_Board();
			Display_Coins();
			return true;
		}
	}

	return false;
}

void Two_Player(){
	//two-player mode code
}

GAME_RESULT Game_Status(){
	//algo idea (brute force): first check for all vertical solutions, then horizontal, then diagonal
	
	//Check for wins in each position
	for(int i = 0; i < NUM_ROWS; i++){
		for(int j = 0; j < NUM_COLS; j++){
			//If curr position is 0, don't check it
			if(board[i][j] != 0){
				//Vertical win (only check if it won't exceed dimensions)
				if(j+3 < NUM_ROWS){
					if(board[i][j] == board[i][j+1] && board[i][j+1] == board[i][j+2] && board[i][j+2] == board[j+3]){
						if(board[i][j] == 1){
							num_blue_victories++;
							return BLUE_WINS;
						} 
						else{
							num_red_victories++;
							return RED_WINS;
						}
					}
				}

				//Horizontal win (only check if it won't exceed dimensions)
				if(i+3 < NUM_COLS){
					if(board[i][j] == board[i+1][j] && board[i+1][j] == board[i+2][j] && board[i+2][j] == board[i+3][j]){
						if(board[i][j] == 1){
							num_blue_victories++;
							return BLUE_WINS;
						}
						else{
							num_red_victories++;
							return RED_WINS;
						}
					}
				}

				//Diagonal win (only check if it won't exceed dimensions)
				if(i+3 < NUM_COLS && j+3 < NUM_ROWS){
					if(board[i][j] == board[i+1][j+1] && board[i+1][j+1] == board[i+2][j+2] && board[i+2][j+2] == board[i+3][j+3]){
						if(board[i][j] == 1){
							num_blue_victories++;
							return BLUE_WINS;
						}
						else{
							num_red_victories++;
							return RED_WINS;
						}
					}
				}
			}
			
		}
	}

	//Check for tie (board is full)
	for(int i = 0; i < NUM_ROWS; i++){
		for(int j = 0; j < NUM_COLS; j++){
			if(board[i][j] == 0){
				return ONGOING;
			}
		}
	}

	num_ties++;
	return TIE;
}

void Display_Results(){
	GAME_RESULT result = Game_Status();
	switch(result){
		case BLUE_WINS:
			LCD_SetTextColor(LCD_COLOR_BLUE);
			LCD_DisplayChar(65, 10, "Blue Wins!");
			break;
		case RED_WINS:
			LCD_SetTextColor(LCD_COLOR_RED);
			LCD_DisplayChar(70, 10, "Red Wins!");
			break;
		case TIE:
			LCD_SetTextColor(LCD_COLOR_BLACK);
			LCD_DisplayChar(60, 10, "It's a tie!");
			break;
		default:
			LCD_SetTextColor(LCD_COLOR_BLACK);
			LCD_DisplayChar(80, 10, "Unknown erorr :(");
			break;
	}
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_DisplayChar(65, 45, "Show stats");

	while(1){
		if(returnTouchStateAndLocation(&StaticTouchData)  == STMPE811_State_Pressed){
			LCD_Quadrant touchedQuadrant = returnTouchQuadrant(StaticTouchData);
			if(touchedQuadrant == BOTTOM_LEFT || touchedQuadrant == BOTTOM_RIGHT) break;
		}
	}

	//show stats
}

void EXTI0_IRQHandler(){ //Button Interrupt Handler
	DisableInterrupt(EXTI0_IRQ_NUMBER);
	
	Drop_Coin = 1;

	ClearPendingEXTIBit(1 << GPIOA_PORT_NUMBER);
	EnableInterrupt(EXTI0_IRQ_NUMBER);
}
