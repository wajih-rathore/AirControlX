#ifndef AIRCONTROLX_FLIGHTSSCHEDULER_H
#define AIRCONTROLX_FLIGHTSSCHEDULER_H

#include <queue>
#include "Aircraft.h"

/**
 * FlightsScheduler class for managing flight schedules.
 * Handles queues of arrivals and departures based on priority.
 */
class FlightsScheduler {
public:
    std::queue<Aircraft> arrivalQueue;    // Queue for arriving aircraft
    std::queue<Aircraft> departureQueue;  // Queue for departing aircraft
    
    // Schedule flights based on priority and timing
    void scheduleFlight();
};

#endif // AIRCONTROLX_FLIGHTSSCHEDULER_H