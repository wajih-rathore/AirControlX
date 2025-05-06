#ifndef AIRCONTROLX_FLIGHTSSCHEDULER_H
#define AIRCONTROLX_FLIGHTSSCHEDULER_H

#include <vector>
#include <mutex>
#include "Aircraft.h"

/**
 * FlightsScheduler class for managing flight schedules.
 * Handles queues of arrivals and departures based on priority and wait time.
 */
class FlightsScheduler 
{
public:
    // Using vectors instead of queues for more flexible management
    std::vector<Aircraft*> arrivalQueue;    // Queue for arriving aircraft
    std::vector<Aircraft*> departureQueue;  // Queue for departing aircraft
    
    // Mutexes to protect our queues during concurrent access
    // These prevent data corruption when multiple threads try to modify the queues
    std::mutex arrivalMutex;
    std::mutex departureMutex;
    
    // Constructor initializes empty queues
    FlightsScheduler();
    
    // Add flights to queues
    void addArrival(Aircraft* aircraft);
    void addDeparture(Aircraft* aircraft);
    
    // Get next flights to be scheduled (highest priority first)
    Aircraft* getNextArrival();
    Aircraft* getNextDeparture();
    Aircraft* getNextEmergency(); // Checks both queues for emergency flights
    
    // Sort queues by priority score
    void sortQueues();
    
    // Calculate estimated wait time for aircraft in queue
    int estimateWaitTime(Aircraft* aircraft);
    
    // Schedule flights based on priority and timing
    void scheduleFlight();
    
    // Get all currently active flights (both arrival and departure)
    const std::vector<Aircraft*>& getActiveFlights() const;
    
private:
    // Vector to track all active flights
    std::vector<Aircraft*> activeFlights;
};

#endif // AIRCONTROLX_FLIGHTSSCHEDULER_H