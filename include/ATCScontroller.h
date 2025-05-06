#ifndef AIRCONTROLX_ATCSCONTROLLER_H
#define AIRCONTROLX_ATCSCONTROLLER_H

#include "FlightsScheduler.h"
#include "Runway.h"
#include "RunwayManager.h"
#include "Radar.h"
#include <vector>

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
    Radar radar;                     // Radar system for detecting violations
    
public:
    // Constructor
    ATCScontroller();
    
    // Set the runway manager - call this after construction
    void setRunwayManager(RunwayManager* rwManager);
    
    // Set the pipe for AVN Generator communication
    void setAVNPipe(int pipeWriteEnd);
    
    // Monitor flights in the airspace
    void monitorFlight();
    
    // Assign runways to aircraft
    void assignRunway();
    
    // Handle detected violations (now delegates to Radar)
    void handleViolations();
    
    // Get count of active violations (for the UI)
    int getActiveViolationsCount() const;
    
    // Get list of aircraft with active violations (for the UI)
    std::vector<Aircraft*> getAircraftWithViolations() const;
    
    // Add a flight to be scheduled
    void scheduleArrival(Aircraft* aircraft);
    void scheduleDeparture(Aircraft* aircraft);
    
    // Get the scheduler
    FlightsScheduler* getScheduler();
    
    // Test function to simulate a violation
    void simulateViolation(const std::string& flightNumber, const std::string& airline, 
                          int speed, int minAllowed, int maxAllowed);
};

#endif // AIRCONTROLX_ATCSCONTROLLER_H