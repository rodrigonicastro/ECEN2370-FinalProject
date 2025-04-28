/*
 * ApplicationCode.c
 *
 *  Created on: Dec 30, 2023 (updated 11/12/2024) Thanks Donavon! 
 *      Author: Xavion
 */

#include "ApplicationCode.h"

/* Static variables */

#define GPIOA_PORT_NUMBER 0

#define BLUE_PLAYER 1
#define RED_PLAYER 2

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

void TimerInit(){

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
	//start timer
	
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
			bool valid = Place_Coin(position, BLUE_PLAYER);
			
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
			while(!Place_Coin(Generate_Random_Move(hrng), RED_PLAYER)); //generate random moves until one is valid
			#else
			while(!Place_Coin(Generate_AI_Move(hrng), RED_PLAYER));
			#endif

		}
		turns++;
	}
	addSchedulerEvent(DISPLAY_RESULTS_EVENT);
	//stop timer
}

int Generate_Random_Move(RNG_HandleTypeDef* hrng){
	uint32_t randNum = 0;
	HAL_RNG_GenerateRandomNumber(hrng, &randNum);

	return randNum % 7;
}

int Generate_AI_Move(RNG_HandleTypeDef* hrng){
	//Defense: if blue has 3 in a row and red can block it, play this move
		//Could either get a list of sequences and sort by the number of coins in a row (descending)
			//This would make the bot very defensive
			//Could store this as a 2d array where the inner arrays would have size 2 with each element being the x,y values
		//Or only store sequences with 3 blue coins in a row
			//This would make the bot less defensive

		//New idea: sort placement positions (0-5) by the length of blue sequence it would interrupt
			//How to do this? no idea yet
				//Brute force is obviously a potential method, but I don't like it
				//If I place a red coin in position 0, what is the length of blue coins I would be interrupting? etc.
					//Play the move that interrupts the biggest sequence if this sequence >(=?) 2, else: play attack
		
		//Also needs to account for non-continous sequences, like (0,0), (0,1), and (0,3)
			//(if we placed a red coin in (0,2) it would stop blue from winning)

	//Attack: play a move where # of red coins in a row increases (prioritize higher nums)
		//Could get a list of sequences and sort by the number of coins in a row (descending)
		//Could apply the same logic for defense:
			//Sort the positions by length of red coins it would create.

	//Do I want the bot to play defensively only when user has 3 in a row, or do I want the bot to try and predict moves?

	//Else: play random?

//Decided on sort placement positions idea:
	//Defense: sort placement position by biggest length of blue coins sequence it would interrupt
	//Don't really need to sort every placement, just the biggest length
		//Actually have to, bc then I would have no way to determine which is greater or which position it is.
		//With the array, the index is the board position and the value is the length
			//So we can't actually sort this array, or else the positions would make no sense
	int defense[7] = {0, 0, 0, 0, 0, 0, 0};

	for(int j = 0; j < NUM_COLS; j++){
		for(int i = NUM_ROWS-1; i >= 0; i--){
			if(board[i][j] == 0){
				//check this position for how many blue coins it would interrupt
				//check below
				int sequence_length = 0; //var to track length of current sequence
				for(int a = i; a < NUM_ROWS; a++){
					if(board[a][j] == 1) sequence_length++; //if a blue coin is detected below, increment the sequence
				}

				//if the current sequence being checked is greater than the biggest sequence stored, update it
				if(sequence_length > defense[j]) defense[j] = sequence_length;

				sequence_length = 0; //reset length to use for next checks
				
				//check right+left
				//Have to consider sequences that are not continuous (e.g., (5,0), (5,1), and (5,3) -> placing a coin at (5,2) would avoid a win )
				//Is there a blue coin immediately to the left? 
					//If not, no need to check left
					//If yes, how many?
				int left = 0;
				for(int b = j-1; b >= 0; b--){
					if(board[i][b] == 1) left++;
					else break;
				}

				//Is there a blue coin immediately to the right? 
					//If not, no need to check right
					//If yes, how many?
				int right = 0;
				for(int b = j+1; b < NUM_COLS; b++){
					if(board[i][b] == 1) right++;
					else break;
				}

				//Sum left and right sequences
				sequence_length = left+right;

				if(sequence_length > defense[j]) defense[j] = sequence_length;

				//check diagonals (all diagonals+both directions)
				//Apply same logic used for left+right

				//Check top-left/bottom-right diagonal
				//Check top-left
				int a = i-1;
				int b = j-1;
				int top_left = 0;
				while(a >= 0 && b >= 0){
					if(board[a][b] == 1) top_left++;
					a--;
					b--;
				}

				//Check bottom-right
				a = i+1;
				b = j+1;
				int bottom_right = 0;
				while(a < NUM_ROWS && b < NUM_COLS){
					if(board[a][b] == 1) bottom_right++;
					else break;
					a++;
					b++;
				}

				//Sum top-left and bottom-right sequences
				sequence_length = top_left+bottom_right;

				if(sequence_length > defense[j]) defense[j] = sequence_length;
				
				//Check top-right/bottom-left diagonal
				//Check top-right
				a = i-1;
				b = j+1;
				int top_right = 0;
				while(a >= 0 && b < NUM_COLS){
					if(board[a][b] == 1) top_right++;
					else break;
					a--;
					b++;
				}

				//Check bottom-left
				a = i+1;
				b = j-1;
				int bottom_left = 0;
				while(a < NUM_ROWS && b >= 0){
					if(board[a][b] == 1) bottom_left++;
					else break;
					a++;
					b--;
				}

				//Sum top-right and bottom-left sequences
				sequence_length = top_right+bottom_left;

				if(sequence_length > defense[j]) defense[j] = sequence_length;
				break;
			}
		}
	}

	//Determine best position
	int best_defense = 0;
	for(int j = 0; j < NUM_COLS; j++){
		if(defense[j] > defense[best_defense]) best_defense = j;
	}

	//Apply exact same logic to determine best attack, only switching blue coins for red coins
	int attack[7] = {0, 0, 0, 0, 0, 0, 0};

	for(int j = 0; j < NUM_COLS; j++){
		for(int i = NUM_ROWS-1; i >= 0; i--){
			if(board[i][j] == 0){
				//check this position for how many blue coins it would interrupt
				//check below
				int sequence_length = 0; //var to track length of current sequence
				for(int a = i; a < NUM_ROWS; a++){
					if(board[a][j] == 2) sequence_length++; //if a red coin is detected below, increment the sequence
				}

				//if the current sequence being checked is greater than the biggest sequence stored, update it
				if(sequence_length > attack[j]) attack[j] = sequence_length;

				sequence_length = 0; //reset length to use for next checks
				
				//check right+left
				//Have to consider sequences that are not continuous (e.g., (5,0), (5,1), and (5,3) -> placing a coin at (5,2) would yield a win )
				//Is there a red coin immediately to the left? 
					//If not, no need to check left
					//If yes, how many?
				int left = 0;
				for(int b = j-1; b >= 0; b--){
					if(board[i][b] == 2) left++;
					else break;
				}

				//Is there a red coin immediately to the right? 
					//If not, no need to check right
					//If yes, how many?
				int right = 0;
				for(int b = j+1; b < NUM_COLS; b++){
					if(board[i][b] == 2) right++;
					else break;
				}

				//Sum left and right sequences
				sequence_length = left+right;

				if(sequence_length > attack[j]) attack[j] = sequence_length;

				//check diagonals (all diagonals+both directions)
				//Apply same logic used for left+right

				//Check top-left/bottom-right diagonal
				//Check top-left
				int a = i-1;
				int b = j-1;
				int top_left = 0;
				while(a >= 0 && b >= 0){
					if(board[a][b] == 2) top_left++;
					a--;
					b--;
				}

				//Check bottom-right
				a = i+1;
				b = j+1;
				int bottom_right = 0;
				while(a < NUM_ROWS && b < NUM_COLS){
					if(board[a][b] == 2) bottom_right++;
					else break;
					a++;
					b++;
				}

				//Sum top-left and bottom-right sequences
				sequence_length = top_left+bottom_right;

				if(sequence_length > attack[j]) attack[j] = sequence_length;
				
				//Check top-right/bottom-left diagonal
				//Check top-right
				a = i-1;
				b = j+1;
				int top_right = 0;
				while(a >= 0 && b < NUM_COLS){
					if(board[a][b] == 2) top_right++;
					else break;
					a--;
					b++;
				}

				//Check bottom-left
				a = i+1;
				b = j-1;
				int bottom_left = 0;
				while(a < NUM_ROWS && b >= 0){
					if(board[a][b] == 2) bottom_left++;
					else break;
					a++;
					b--;
				}

				//Sum top-right and bottom-left sequences
				sequence_length = top_right+bottom_left;

				if(sequence_length > attack[j]) attack[j] = sequence_length;
				
				break;
			}
		}
	}

	//Determine best position
	int best_attack = 0;
	for(int j = 0; j < NUM_COLS; j++){
		if(attack[j] > attack[best_attack]) best_attack = j;
	}

	if(defense[best_defense] > attack[best_attack] && defense[best_defense] > 2) return best_defense;

	if(attack[best_attack] == 0) return Generate_Random_Move(hrng);

	return best_attack;

	//Improvement ideas:
		//Don't take into consideration an attack that, although creates a bigger sequence, can't result in a win (e.g., col/row almost full)
		//Don't take into consideration a defense that, although interrupts the biggest sequence, it won't stop a win (e.g., column almost full)
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
	turns = 0;

	while(Game_Status() == ONGOING){
		//Blue's Turn
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
					LCD_Draw_Circle_Fill(30+30*position, 65, 8, LCD_COLOR_BLUE);

					touched = 0;
				}

				if(Drop_Coin){
					Drop_Coin = 0;
					break;
				}
			}
			
			//Place Coin Routine
			bool valid = Place_Coin(position, BLUE_PLAYER);
			
			//If move is not valid, continue on user's turn
			if(!valid) continue;
		}

		//Red's Turn
		else{
			int position = 0;
			LCD_Clear(0, LCD_COLOR_WHITE);
			Display_Board();
			Display_Coins();
			LCD_Draw_Circle_Fill(30, 65, 8, LCD_COLOR_RED);

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
					LCD_Draw_Circle_Fill(30+30*position, 65, 8, LCD_COLOR_RED);

					touched = 0;
				}

				if(Drop_Coin){
					Drop_Coin = 0;
					break;
				}
			}
			
			//Place Coin Routine
			bool valid = Place_Coin(position, RED_PLAYER);
			
			//If move is not valid, continue on user's turn
			if(!valid) continue;
		}
		turns++;
	}
	addSchedulerEvent(DISPLAY_RESULTS_EVENT);
}

GAME_RESULT Game_Status(){
	//algo idea (brute force): first check for all vertical solutions, then horizontal, then diagonal
	
	//Check for wins in each position
	for(int i = 0; i < NUM_ROWS; i++){
		for(int j = 0; j < NUM_COLS; j++){
			//If curr position is 0, don't check it
			if(board[i][j] != 0){
				//Horizomtal win (only check if it won't exceed dimensions)
				if(j+3 < NUM_COLS){
					if(board[i][j] == board[i][j+1] && board[i][j+1] == board[i][j+2] && board[i][j+2] == board[i][j+3]){
						if(board[i][j] == 1) return BLUE_WINS;
						else return RED_WINS;
					}
				}

				//Vertical win (only check if it won't exceed dimensions)
				if(i+3 < NUM_ROWS){
					if(board[i][j] == board[i+1][j] && board[i+1][j] == board[i+2][j] && board[i+2][j] == board[i+3][j]){
						if(board[i][j] == 1) return BLUE_WINS;
						else return RED_WINS;
					}
				}

				//Diagonal win (only check if it won't exceed dimensions)
				//4 different diagonals to check
				//Bottom-right diagonal
				if(i+3 < NUM_ROWS && j+3 < NUM_COLS){////////////////
					if(board[i][j] == board[i+1][j+1] && board[i+1][j+1] == board[i+2][j+2] && board[i+2][j+2] == board[i+3][j+3]){
						if(board[i][j] == 1) return BLUE_WINS;
						else return RED_WINS;
					}
				}

				//Top-right diagonal
				if(i+3 < NUM_ROWS && j-3 >= 0){
					if(board[i][j] == board[i+1][j-1] && board[i+1][j-1] == board[i+2][j-2] && board[i+2][j-2] == board[i+3][j-3]){
						if(board[i][j] == 1) return BLUE_WINS;
						else return RED_WINS;
					}
				}

				//Bottom-left diagonal
				if(i-3 >= 0 && j+3 < NUM_COLS){
					if(board[i][j] == board[i-1][j+1] && board[i-1][j+1] == board[i-2][j+2] && board[i-2][j+2] == board[i-3][j+3]){
						if(board[i][j] == 1) return BLUE_WINS;
						else return RED_WINS;
					}
				}

				//Top-left diagonal
				if(i-3 >= 0 && j-3 >= 0){
					if(board[i][j] == board[i-1][j-1] && board[i-1][j-1] == board[i-2][j-2] && board[i-2][j-2] == board[i-3][j-3]){
						if(board[i][j] == 1) return BLUE_WINS;
						else return RED_WINS;
					}
				}
			}
			
		}
	}

	//Check for tie (board is full)
	for(int i = 0; i < NUM_ROWS; i++){
		for(int j = 0; j < NUM_COLS; j++){
			if(board[i][j] == 0) return ONGOING;
		}
	}
	return TIE;
}

void Display_Results(uint32_t seconds){
	GAME_RESULT result = Game_Status();

	if(result == BLUE_WINS) num_blue_victories++;
	else if (result == RED_WINS) num_red_victories++;
	else if (result == TIE) num_ties++;

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

	//Wait for user to click show stats
	while(1){
		if(returnTouchStateAndLocation(&StaticTouchData)  == STMPE811_State_Pressed){
			LCD_Quadrant touchedQuadrant = returnTouchQuadrant(StaticTouchData);
			if(touchedQuadrant == BOTTOM_LEFT || touchedQuadrant == BOTTOM_RIGHT) break;
		}
	}

	LCD_Clear(0, LCD_COLOR_WHITE);

	char str_blue_wins[20];
	sprintf(str_blue_wins, "%d", num_blue_victories);

	char str_red_wins[20];
	sprintf(str_red_wins, "%d", num_red_victories);

	char str_ties[20];
	sprintf(str_ties, "%d", num_ties);

	int num_games = num_blue_victories + num_red_victories + num_ties;
	char str_games[20];
	sprintf(str_games, "%d", num_games);

	char str_seconds[20];
	sprintf(str_seconds, "%d", seconds);

	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_DisplayChar(5, 10, "Total games played: ");
	LCD_DisplayChar(200, 10, str_games);

	LCD_SetTextColor(LCD_COLOR_BLUE);
	LCD_DisplayChar(5, 40, "Blue won ");
	LCD_DisplayChar(93, 40, str_blue_wins);
	LCD_DisplayChar(110, 40, "games");

	LCD_SetTextColor(LCD_COLOR_RED);
	LCD_DisplayChar(5, 70, "Red won ");
	LCD_DisplayChar(93, 70, str_red_wins);
	LCD_DisplayChar(110, 70, "games");

	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_DisplayChar(5, 100, str_ties);
	LCD_DisplayChar(20, 100, "games ended in a tie");

	LCD_DisplayChar(5, 130, "This game took ");
	LCD_DisplayChar(160, 130, str_seconds);
	LCD_DisplayChar(155, 150, "seconds");

	LCD_DisplayChar(10, 200, "Play another game?");

	LCD_SetTextColor(LCD_COLOR_RED);
	LCD_DisplayChar(20, 250, "No");

	LCD_SetTextColor(LCD_COLOR_GREEN);
	LCD_DisplayChar(190, 250, "Yes");
}

void Display_Quit_Screen(){
	LCD_Clear(0, LCD_COLOR_WHITE);
	LCD_SetTextColor(LCD_COLOR_BLACK);

	LCD_DisplayChar(20, 80, "See you next time!");
	
	LCD_DisplayChar(0, 170, "Terminating program");
	HAL_Delay(750);

	LCD_DisplayChar(0, 170, "Terminating program.");
	HAL_Delay(750);

	LCD_DisplayChar(0, 170, "Terminating program..");
	HAL_Delay(750);

	LCD_DisplayChar(0, 170, "Terminating program...");
	HAL_Delay(750);

	LCD_DisplayChar(50, 200, "Goodbye!");
	HAL_Delay(750);

	LCD_Clear(0, LCD_COLOR_BLACK);
}

void Reset_Board(){
	for(int i = 0; i < NUM_ROWS; i++){
		for(int j = 0; j < NUM_COLS; j++){
			board[i][j] = 0;
		}
	}
	//reset timer
}

void Reset_Stats(){
	num_blue_victories = 0;
	num_red_victories = 0;
	num_ties = 0;
}

void Start_Timer(){

}

void Stop_Timer(){

}

void Reset_Timer(){

}

uint32_t Get_Timer_Value(){
	return 0;
}

void EXTI0_IRQHandler(){ //Button Interrupt Handler
	DisableInterrupt(EXTI0_IRQ_NUMBER);
	
	Drop_Coin = 1;

	ClearPendingEXTIBit(1 << GPIOA_PORT_NUMBER);
	EnableInterrupt(EXTI0_IRQ_NUMBER);
}