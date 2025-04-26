#ifndef AIRCONTROLX_RADAR_H
#define AIRCONTROLX_RADAR_H

#include "Aircraft.h"

/**
 * Radar class for monitoring aircraft speed and detecting violations.
 * Used to track aircraft and report violations to the ATC system.
 */
class Radar {
public:
    int range;  // Radar range in kilometers
    
    // Constructor
    Radar();
    
    // Monitor aircraft and detect violations
    int monitorAirCraft(Aircraft& plane);
};

#endif // AIRCONTROLX_RADAR_H