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
#include "VisualSimulator.h"
#include "Timer.h"

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
    VisualSimulator* visualizer;  // SFML visualization component
    Timer timer;             // Timer for simulation duration
    
    // Simulation parameters
    int simulationDuration; // in seconds
    bool isRunning;
    bool isPaused;         // Flag to track pause state

    // Update simulation state (called in render loop)
    void update();

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

    // Pause/resume simulation
    void togglePause();
    bool isPausedState() const;

    // Getters for visualizer access to data
    AirlineManager* getAirlineManager() const { return airlineManager; }
    RunwayManager* getRunwayManager() const { return runwayManager; }
    ATCScontroller* getATCController() const { return atcController; }
    int getElapsedTime() const { return timer.getElapsedSeconds(); }
    int getRemainingTime() const { return simulationDuration - timer.getElapsedSeconds(); }
};

#endif // AIRCONTROLX_SIMULATION_H