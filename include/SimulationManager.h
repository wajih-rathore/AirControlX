#ifndef AIRCONTROLX_SIMULATIONMANAGER_H
#define AIRCONTROLX_SIMULATIONMANAGER_H

#include <pthread.h>
#include <vector>
#include <iostream>
#include <unistd.h>
#include "Aircraft.h"
#include "Airline.h"
#include "ATCScontroller.h"
#include "RunwayManager.h"

/**
 * The SimulationManager class handles thread creation and management for the simulation.
 * It manages the lifecycle of aircraft threads and the ATC controller thread.
 */
class SimulationManager 
{
private:
    // Forward declarations of thread argument structures
    struct ThreadArgs;
    struct ATCArgs;
    
    // Thread storage
    std::vector<pthread_t> aircraftThreads;
    pthread_t atcControllerThread;
    
    // Thread synchronization
    pthread_mutex_t consoleMutex;
    
    // Reference to shared components
    ATCScontroller* atcController;
    RunwayManager* runwayManager;
    
    // Thread function for aircraft simulation
    static void* flightThreadFunction(void* arg);
    
    // Thread function for ATC controller
    static void* atcControllerThreadFunction(void* arg);

public:
    // Constructor and destructor
    SimulationManager(ATCScontroller* atc, RunwayManager* rwm);
    ~SimulationManager();
    
    // Launch aircraft threads for an airline
    bool launchAirlineThreads(Airline* airline);
    
    // Launch ATC controller thread
    bool launchATCControllerThread();
    
    // Create random emergency situation
    void createRandomEmergency(const std::vector<Airline*>& airlines);
    
    // Wait for all threads to complete
    void waitForCompletion();
    
    // Thread-safe console logging
    void logMessage(const std::string& message);
};

#endif // AIRCONTROLX_SIMULATIONMANAGER_H