#ifndef AIRCONTROLX_ATCSCONTROLLER_H
#define AIRCONTROLX_ATCSCONTROLLER_H

#include "FlightsScheduler.h"
#include "Runway.h"
#include "RunwayManager.h"

/**
 * ATCScontroller class for monitoring and managing air traffic.
 * Responsible for overseeing all flights and handling violations.
 */
class ATCScontroller 
{
private:
    FlightsScheduler scheduler;      // Flight scheduler for managing flight queues
    int schedulingInterval;          // How often to run scheduling (in seconds)
    time_t lastScheduleTime;         // Last time we ran the scheduling algorithm
    RunwayManager* runwayManager;    // Pointer to runway manager (now we're properly modular!)
    
public:
    // Constructor - now takes a runway manager pointer
    ATCScontroller();
    
    // Set the runway manager - call this after construction
    void setRunwayManager(RunwayManager* rwManager);
    
    // Monitor flights in the airspace
    void monitorFlight();
    
    // Assign runways to aircraft
    void assignRunway();
    
    // Handle detected violations
    void handleViolations();
    
    // Add a flight to be scheduled
    void scheduleArrival(Aircraft* aircraft);
    void scheduleDeparture(Aircraft* aircraft);
    
    // Get the scheduler
    FlightsScheduler* getScheduler();
};

#endif // AIRCONTROLX_ATCSCONTROLLER_H