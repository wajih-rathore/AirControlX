#ifndef AIRCONTROLX_AIRLINEMANAGER_H
#define AIRCONTROLX_AIRLINEMANAGER_H

#include <vector>
#include <string>
#include "Airline.h"
#include "Common.h"

/**
 * The AirlineManager class manages all airline operations.
 * It handles airline creation, aircraft initialization, and airline lookups.
 */
class AirlineManager 
{
private:
    // Collection of airlines in the system
    std::vector<Airline*> airlines;

public:
    // Constructor and destructor
    AirlineManager();
    ~AirlineManager();
    
    // Initialize all airlines with their configurations
    void initialize();
    
    // Get all airlines
    std::vector<Airline*>& getAllAirlines();
    
    // Get an airline by name
    Airline* getAirlineByName(const std::string& name);
    
    // Get a random airline (useful for emergency generation)
    Airline* getRandomAirline();
};

#endif // AIRCONTROLX_AIRLINEMANAGER_H