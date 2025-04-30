#ifndef AIRCONTROLX_SIMULATION_H
#define AIRCONTROLX_SIMULATION_H

#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

#include "Common.h"
#include "AirlineManager.h"
#include "SimulationManager.h"
#include "RunwayManager.h"
#include "ATCScontroller.h"

/**
 * The Simulation class acts as the main coordinator for the AirControlX system.
 * It initializes all components and manages the simulation lifecycle.
 */
class Simulation 
{
private:
    // Core components of our simulation
    AirlineManager* airlineManager;
    RunwayManager* runwayManager;
    SimulationManager* simulationManager;
    ATCScontroller* atcController;
    
    // Simulation parameters
    int simulationDuration; // in seconds
    bool isRunning;

public:
    // Constructor and destructor
    Simulation();
    ~Simulation();
    
    // Initialize the simulation components
    bool initialize();
    
    // Run the simulation
    bool run();
    
    // Wait for the simulation to complete
    bool waitForCompletion();
};

#endif // AIRCONTROLX_SIMULATION_H