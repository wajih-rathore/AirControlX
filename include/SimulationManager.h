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
#include "VisualSimulator.h"

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
    pthread_t visualizerThread;
    
    // Thread synchronization
    pthread_mutex_t consoleMutex;
    pthread_mutex_t visualDataMutex;
    
    // Reference to shared components
    ATCScontroller* atcController;
    RunwayManager* runwayManager;
    VisualSimulator* visualizer;
    
    // Flag to control visualization thread
    bool visualizationActive;
    
    // Simulation timer (in seconds)
    int simulationTimer;
    
    // Thread function for aircraft simulation
    static void* flightThreadFunction(void* arg);
    
    // Thread function for ATC controller
    static void* atcControllerThreadFunction(void* arg);
    
    // Thread function for visualization
    static void* visualizerThreadFunction(void* arg);

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
    
    // Update the simulation time
    void updateSimulationTime(int currentTime);
    
    // ======== SFML Visualization Integration Functions ========
    
    /**
     * Set the visualizer component for the simulation
     * Links the SFML visual renderer to the simulation
     */
    void setVisualizer(VisualSimulator* vis);
    
    /**
     * Launch visualization thread
     * Starts a separate thread for SFML rendering
     */
    bool launchVisualizerThread();
    
    /**
     * Get all active aircraft for visualization
     * Thread-safe method to access aircraft data for rendering
     */
    std::vector<Aircraft*> getActiveAircraftForVisualization();
    
    /**
     * Update visualization data
     * Safely transfers simulation state to visualization layer
     */
    void updateVisualizationData();
    
    /**
     * Stop visualization
     * Safely stops the visualization thread
     */
    void stopVisualization();
    
    /**
     * Check if visualization is active
     * Returns whether the visualization thread is running
     */
    bool isVisualizationActive() const;
    
    /**
     * Get simulation statistics for UI display
     * Returns formatted statistics about the simulation
     */
    std::string getSimulationStatistics() const;
    
    /**
     * Get collection of all airlines for visualization
     * Returns list of all airlines in the simulation
     */
    std::vector<Airline*> getAirlinesForVisualization() const;
};

#endif // AIRCONTROLX_SIMULATIONMANAGER_H