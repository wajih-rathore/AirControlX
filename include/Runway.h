#ifndef AIRCONTROLX_RUNWAY_H
#define AIRCONTROLX_RUNWAY_H

#include <string>
#include "Common.h"
#include "Aircraft.h"

/**
 * RunwayClass handles runway operations at the airport.
 * Manages runway availability, assignment to aircraft, and tracking usage.
 */
class RunwayClass {
public:
    std::string id;        // Runway identifier (e.g., RWY-A)
    RunwayType type;       // Type of runway (Arrival, Departure, Flexible)
    bool isOccupied;       // Flag for runway occupancy status
    int turn;              // Variable for synchronization between aircraft

    // Constructor
    RunwayClass();
    
    // Try to assign runway to aircraft
    bool tryAssign(Aircraft &plane);
    
    // Release the runway after use
    void release();
};

#endif // AIRCONTROLX_RUNWAY_H