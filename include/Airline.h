#ifndef AIRCONTROLX_AIRLINE_H
#define AIRCONTROLX_AIRLINE_H

#include <string>
#include <vector>
#include "Common.h"
#include "Aircraft.h"

/**
 * Airline class for managing a fleet of aircraft.
 * Handles scheduling and monitoring of flights for a specific airline.
 */
class Airline {
public:
    std::string name;              // Name of the airline
    AirCraftType type;             // Primary aircraft type operated by the airline
    std::vector<Aircraft> aircrafts; // Fleet of aircraft
    int activeFlights;             // Number of currently active flights
    int maxFlights;                // Maximum allowed simultaneous flights
    
    // Constructor
    Airline();
    
    // Check if airline can schedule more flights
    bool canScheduleFlight();
    
    // Get an available aircraft for scheduling
    Aircraft* getAvailableAircraft();
};

#endif // AIRCONTROLX_AIRLINE_H