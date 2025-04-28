#ifndef Scheduler_H_
#define Scheduler_H_

#include <stdint.h>

#define DISPLAY_MENU_SCREEN_EVENT (1 << 0)
#define DISPLAY_BOARD_EVENT (1 << 1)
#define SINGLE_PLAYER_EVENT (1 << 2)
#define TWO_PLAYER_EVENT (1 << 3)
#define PLACE_COIN_EVENT (1 << 4)
#define DISPLAY_RESULTS_EVENT (1 << 5)
#define DISPLAY_QUIT_SCREEN_EVENT (1 << 6)

uint32_t getScheduledEvents();

void addSchedulerEvent(uint32_t event);

void removeSchedulerEvent(uint32_t event);

#endif