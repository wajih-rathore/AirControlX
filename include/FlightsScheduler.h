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
    Aircraft* getNextEmergency() const; // Const version - just peeks at emergencies without removing
    Aircraft* removeNextEmergency();    // Non-const version - finds and removes highest priority emergency
    
    // Sort queues by priority score
    void sortQueues();
    
    // Calculate estimated wait time for aircraft in queue
    int estimateWaitTime(Aircraft* aircraft);
    
    // Schedule flights based on priority and timing
    void scheduleFlight();
    
    // Get all currently active flights (both arrival and departure)
    const std::vector<Aircraft*>& getActiveFlights() const;
    
    // ======== SFML Visualization Abstraction Functions ========
    
    /**
     * Get queue statistics for visualization
     * Returns array with [total active, arrivals, departures, emergency]
     */
    int* getQueueStatistics() const;
    
    /**
     * Get all active emergency aircraft
     * Returns vector of pointers to emergency aircraft for visualization
     */
    std::vector<Aircraft*> getActiveEmergencyAircraft() const;
    
    /**
     * Get all aircraft with violations
     * Returns vector of pointers to aircraft with violations
     */
    std::vector<Aircraft*> getAircraftWithViolations() const;
    
    /**
     * Get aircraft queue by type
     * Returns vector of pointers to aircraft of specified type
     */
    std::vector<Aircraft*> getAircraftByType(AirCraftType type) const;
    
    /**
     * Get aircraft by flight state
     * Returns vector of pointers to aircraft in the specified state
     */
    std::vector<Aircraft*> getAircraftByState(FlightState state) const;
    
    /**
     * Get aircraft for visualization at specified coordinates
     * Returns aircraft located at the given display coordinates (if any)
     */
    Aircraft* getAircraftAtPosition(int x, int y, int radius) const;
    
    /**
     * Get status summary text for display
     * Returns formatted text with queue statistics
     */
    std::string getStatusSummary() const;
    
private:
    // Vector to track all active flights
    std::vector<Aircraft*> activeFlights;
};

#endif // AIRCONTROLX_FLIGHTSSCHEDULER_H