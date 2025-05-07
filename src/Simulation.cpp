#include "../include/Simulation.h"
#include "../include/visual/Visualizer.h"

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
    visualizer = new VisualSimulator();  // Create the visualizer component
    
    // Set default simulation parameters
    simulationDuration = 300; // 5 minutes
    isRunning = false;
    isPaused = false;
    
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
    delete visualizer;  // Clean up visualizer
}

/**
 * Initialize all simulation components
 */
bool Simulation::initialize()
{
    std::cout << "AirControlX - Automated Air Traffic Control System" << std::endl;
    std::cout << "Module 3: SFML Visualization and Subsystems Integration" << std::endl << std::endl;
    
    try {
        // Initialize all the components
        std::cout << "Initializing airlines and runways..." << std::endl;
        
        // Initialize airlines
        airlineManager->initialize();
        
        // Initialize runways
        runwayManager->initialize();
        
        // Set the runway manager in the ATC controller
        atcController->setRunwayManager(runwayManager);
        
        visualizer->loadGraphics();  // Load graphics for visualization
        visualizer->setRunwayManager(runwayManager);  // Set runway manager for visualization
        visualizer->airlines = airlineManager->getAllAirlines();  // Set airlines for visualization


        // Initialize and start the timer
        timer.setDuration(simulationDuration);
        timer.start();
        
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
    
    sf::Clock visualclock;

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
        
        // Launch aircraft threads for each airline with a short delay between
        for (Airline* airline : airlineManager->getAllAirlines()) {
            simulationManager->launchAirlineThreads(airline);
            // No sleep here - faster startup for visual version
        }
        
        std::cout << "All aircraft launched, simulation running..." << std::endl;
        
        sf::Clock radarclock;

        // Main SFML rendering loop
        while (visualizer->running() && !timer.isTimeUp()) {
            // Update simulation time in the manager
            simulationManager->updateSimulationTime(timer.getElapsedSeconds());
            
            // Handle window events (close, keyboard input)
            visualizer->handleEvents();

            if(radarclock.getElapsedTime().asSeconds() > 0.5f) {
                radarclock.restart();
                // Update radar data
                for(auto airline : airlineManager->getAllAirlines()) {
                    radar.detectViolations(airline->aircrafts);
                  
            }
        }
            // Update simulation state if not paused
            if (!isPaused) {
                update();
            }
            if(visualclock.getElapsedTime().asSeconds() > 0.04f) {
                visualclock.restart();
                // Update aircraft positions and check for arrivals/departures
                visualizer->checkForArrivalsOrDepartures();
                visualizer->Update();  // Update visualizer state
            }
            // Render current state
            visualizer->display();
        }
        
        isRunning = false;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Simulation error: " << e.what() << std::endl;
        isRunning = false;
        return false;
    }
}

/**
 * Update simulation state
 * This is called in the render loop to update simulation state
 */
void Simulation::update()
{
    // Currently, most of our updates happen in threads
    // In future modules, we'll add more update logic here
    
    // This is where we'll update aircraft positions, check for violations, etc.
}

/**
 * Wait for the simulation to complete
 */
bool Simulation::waitForCompletion()
{
    if (isRunning) {
        // This is now handled by the SFML render loop in run()
        // So we just return success
        std::cout << "\nSimulation complete!" << std::endl;
    }
    return true;
}

/**
 * Toggle pause state
 */
void Simulation::togglePause()
{
    isPaused = !isPaused;
    std::cout << (isPaused ? "Simulation paused." : "Simulation resumed.") << std::endl;
}

/**
 * Check if simulation is paused
 */
bool Simulation::isPausedState() const
{
    return isPaused;
}