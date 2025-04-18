#ifndef Scheduler_H_
#define Scheduler_H_

#include <stdint.h>
#define DISPLAY_MENU_SCREEN_EVENT (1 << 0)

uint32_t getScheduledEvents();

void addSchedulerEvent(uint32_t event);

void removeSchedulerEvent(uint32_t event);

#endif