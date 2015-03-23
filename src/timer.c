#include "timer.h"

Timer* create_timer()
{
	Timer* timer = (Timer*)malloc(sizeof(Timer));
	timer->ticks = 0;

	return timer;
}

void destroy_timer(Timer* timer)
{
	free(timer);
}

void set_ticks(Timer* timer)
{
	timer->ticks = SDL_GetTicks();
}

Uint32 get_ticks(Timer* timer)
{
	Uint32 current = SDL_GetTicks();
	return current - timer->ticks;
}