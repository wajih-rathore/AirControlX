#include "../include/Simulation.h"

/**
 * Constructor initializes simulation components and default parameters
 */
Simulation::Simulation()
{
    // Create the component managers
    airlineManager = new AirlineManager();
    runwayManager = new RunwayManager();
    atcController = new ATCScontroller();
    simulationManager = new SimulationManager(atcController, runwayManager);
    
    // Set default simulation parameters
    simulationDuration = 300; // 5 minutes
    isRunning = false;
    
    // Seed random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

/**
 * Destructor cleans up dynamically allocated resources
 */
Simulation::~Simulation()
{
    delete airlineManager;
    delete runwayManager;
    delete atcController;
    delete simulationManager;
}

/**
 * Initialize all simulation components
 */
bool Simulation::initialize()
{
    std::cout << "AirControlX - Automated Air Traffic Control System" << std::endl;
    std::cout << "Module 2: Flight Scheduling Implementation" << std::endl << std::endl;
    
    try {
        // Initialize all the components
        std::cout << "Initializing airlines and runways..." << std::endl;
        
        // Initialize airlines
        airlineManager->initialize();
        
        // Initialize runways
        runwayManager->initialize();
        
        // Set the runway manager in the ATC controller
        // This is crucial! Without this, ATC can't manage runways
        atcController->setRunwayManager(runwayManager);
        
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Initialization error: " << e.what() << std::endl;
        return false;
    }
}

/**
 * Run the simulation
 */
bool Simulation::run()
{
    if (isRunning) {
        std::cerr << "Simulation is already running!" << std::endl;
        return false;
    }
    
    isRunning = true;
    
    try {
        // Launch ATC controller thread
        if (!simulationManager->launchATCControllerThread()) {
            std::cerr << "Failed to launch ATC controller thread!" << std::endl;
            return false;
        }
        
        // Create random emergency for testing
        simulationManager->createRandomEmergency(airlineManager->getAllAirlines());
        
        std::cout << "Launching aircraft threads..." << std::endl;
        sleep(1);  // Short pause for readability
        
        // Launch aircraft threads for each airline with a short delay between
        for (Airline* airline : airlineManager->getAllAirlines()) {
            simulationManager->launchAirlineThreads(airline);
            sleep(1);  // Stagger launches for readability
        }
        
        std::cout << "All aircraft launched, simulation running..." << std::endl;
        
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Simulation error: " << e.what() << std::endl;
        isRunning = false;
        return false;
    }
}

/**
 * Wait for the simulation to complete
 */
bool Simulation::waitForCompletion()
{
    if (isRunning) {
        // Wait for all threads to complete
        simulationManager->waitForCompletion();
        isRunning = false;
        std::cout << "\nSimulation complete!" << std::endl;
    }
    return true; // Return success
}