#include <SDL/SDL.h>

#include "Time.h"

Uint32 Time::_currentTimeMS (0);
Uint32 Time::_deltaTimeMS (0);

float Time::GetDeltaTime()
{
	return _deltaTimeMS / 1000.0f;
}

float Time::GetTime()
{
	return _currentTimeMS;
}

void Time::Init() 
{
	_currentTimeMS = SDL_GetTicks();
}

void Time::UpdateFrame()
{
	Uint32 lastTimeMS = _currentTimeMS;
	_currentTimeMS = SDL_GetTicks();

	_deltaTimeMS = _currentTimeMS - lastTimeMS;
}

unsigned int Time::GetDeltaTimeMS ()
{
	return _deltaTimeMS;
}

/*
 * Get total milliseconds passed since the start of game until the start of
 * current frame. 
 *
 * It is possible that more milliseconds already pass since the starting of
 * frame.
*/

unsigned int Time::GetTimeMS ()
{
	return _currentTimeMS;
}

/* 
 * Get all miliseconds passed since the start of game.
*/

unsigned int Time::GetElapsedTimeMS ()
{
	return SDL_GetTicks ();
}