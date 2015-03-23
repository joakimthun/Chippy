#ifndef TIMER_H
#define TIMER_H

#include <SDL.h>

typedef struct {
	Uint32 ticks;
} Timer;


Timer* create_timer();
void destroy_timer(Timer* timer);
void set_ticks(Timer* timer);
Uint32 get_ticks(Timer* timer);

#endif