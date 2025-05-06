#ifndef AIRCONTROLX_ATCSCONTROLLER_H
#define AIRCONTROLX_ATCSCONTROLLER_H

#include "FlightsScheduler.h"
#include "Runway.h"
#include "RunwayManager.h"
#include "Radar.h"
#include <vector>
#include <mutex>
#include <string>

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
    
    // Mutex for thread-safe access to controller data
    std::mutex controllerMutex;
    
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
    
    // Alias for getScheduler() for compatibility
    FlightsScheduler* getFlightScheduler();
    
    // Test function to simulate a violation
    void simulateViolation(const std::string& flightNumber, const std::string& airline, 
                          int speed, int minAllowed, int maxAllowed);
    
    // ======== SFML Visualization Helper Functions ========
    
    /**
     * Get all active flights for visualization
     * Returns a thread-safe copy of all active flights
     */
    std::vector<Aircraft*> getAllActiveFlights() const;
    
    /**
     * Get status text for visualization
     * Returns formatted text about ATC status for the UI
     */
    std::string getStatusText() const;
    
    /**
     * Get flight with highest priority
     * Returns the flight currently with highest priority in the scheduling system
     */
    Aircraft* getHighestPriorityFlight() const;
    
    /**
     * Get flights with specific state
     * Returns all flights currently in the specified flight state
     */
    std::vector<Aircraft*> getFlightsByState(FlightState state) const;
    
    /**
     * Get flights by emergency level
     * Returns all flights with the specified emergency level
     */
    std::vector<Aircraft*> getFlightsByEmergencyLevel(int emergencyLevel) const;
    
    /**
     * Get flights waiting for runway
     * Returns all flights waiting for runway assignment
     */
    std::vector<Aircraft*> getFlightsWaitingForRunway() const;
    
    /**
     * Get flight counts by type
     * Returns array with counts of each aircraft type [commercial, cargo, military, emergency, medical]
     */
    int* getFlightCountsByType() const;
};

#endif // AIRCONTROLX_ATCSCONTROLLER_H