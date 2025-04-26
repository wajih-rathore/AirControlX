#ifndef AIRCONTROLX_TIMER_H
#define AIRCONTROLX_TIMER_H

#include <ctime>

/**
 * Timer class for managing time-related operations in the AirControlX system.
 * Used for tracking elapsed time, checking timeouts, and scheduling events.
 */
class Timer {
private:
    time_t startTime;     // Time when the timer started
    int durationSeconds;  // Duration of the timer in seconds
    bool running;         // Flag to track if timer is currently running

public:
    // Constructor
    Timer();
    
    // Start the timer
    void start();
    
    // Stop the timer
    void stop();
    
    // Check if timer duration has elapsed
    bool isTimeUp();
    
    // Get elapsed time in seconds
    int getElapsedSeconds();
    
    // Set timer duration
    void setDuration(int seconds);
};

#endif // AIRCONTROLX_TIMER_H