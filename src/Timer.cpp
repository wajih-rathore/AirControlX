#include "../include/Timer.h"

// Constructor initializes timer values
Timer::Timer() 
{
    startTime = 0;
    durationSeconds = 0;
    running = false;
}

// Start the timer by recording current time and setting running flag
void Timer::start() 
{
    startTime = time(NULL);
    running = true;
}

// Stop the timer by clearing the running flag
void Timer::stop() 
{
    running = false;
}

// Check if the timer has exceeded its set duration
bool Timer::isTimeUp() const 
{
    if (!running) return false;
    return (getElapsedSeconds() >= durationSeconds);
}

// Calculate and return the elapsed time in seconds since the timer started
int Timer::getElapsedSeconds() const 
{
    if (!running) return 0;
    return static_cast<int>(time(NULL) - startTime);
}

// Set the duration of the timer in seconds
void Timer::setDuration(int seconds) 
{
    durationSeconds = seconds;
}