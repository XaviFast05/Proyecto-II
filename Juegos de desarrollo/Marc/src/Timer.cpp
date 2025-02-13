// ----------------------------------------------------
// Fast timer with milisecons precision
// ----------------------------------------------------

#include "Timer.h"
#include "SDL2\SDL_timer.h"
	
Timer::Timer()
{
	Start();
}

void Timer::Start()
{
	startTime = SDL_GetTicks();
}

float Timer::ReadSec() const
{
	return (float)(SDL_GetTicks() - startTime) / 1000.0f;
}

float Timer::ReadMSec() const
{
	return (float)(SDL_GetTicks() - startTime);
}