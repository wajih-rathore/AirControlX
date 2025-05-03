#include "../include/SimulationManager.h"
#include <cstdlib>
#include <ctime>

// Struct that holds thread arguments for flight simulation
struct SimulationManager::ThreadArgs 
{
    Aircraft* aircraft;
    SimulationManager* manager;
    ATCScontroller* atc;
    RunwayManager* runways;
};

// Struct that holds thread arguments for ATC controller
struct SimulationManager::ATCArgs 
{
    SimulationManager* manager;
    ATCScontroller* atc;
    RunwayManager* runways;
    int duration;
};

/**
 * Constructor initializes threading resources and stores references to shared components
 */
SimulationManager::SimulationManager(ATCScontroller* atc, RunwayManager* rwm) 
    : atcController(atc), runwayManager(rwm)
{
    // Initialize the mutex for thread-safe console output
    pthread_mutex_init(&consoleMutex, nullptr);
}

/**
 * Destructor cleans up threading resources
 */
SimulationManager::~SimulationManager() 
{
    // Destroy the mutex when done
    pthread_mutex_destroy(&consoleMutex);
}

/**
 * Thread function that simulates a plane flight
 * This is where all the flight logic happens - previously in Flight() function in main.cpp
 */
void* SimulationManager::flightThreadFunction(void* arg) 
{
    // Cast the argument back to ThreadArgs
    ThreadArgs* args = static_cast<ThreadArgs*>(arg);
    Aircraft* plane = args->aircraft;
    SimulationManager* manager = args->manager;
    ATCScontroller* atcController = args->atc;
    RunwayManager* runwayManager = args->runways;
    
    // Mark the plane as active
    plane->isActive = true;
    
    // Log that the flight is active
    manager->logMessage("Flight " + plane->FlightNumber + " is now active");

    // Set flight direction based on type (for simulation purposes)
    // North/South for arrivals, East/West for departures
    // We'll use a simple rule: even index = arrival, odd index = departure
    if (plane->aircraftIndex % 2 == 0) 
    {
        // Arrival flow - either North or South
        plane->direction = (plane->aircraftIndex % 4 == 0) ? Direction::North : Direction::South;
        
        //NOTE : WE CAN DO THE POSITION WALA PART OR NOT, TO BE DECIDEDDDDDDDD ????
        /* Commenting this for now, same for the departures
        // Randomly generate x and y positions based on direction
        Was also thinking of adding z(height) and making directions to 360
        //Getting a bit complicated, but we can add this later
        //Have to align it with the sprites too
        // Have to do some math with the speed as well as distance
        //Monday ko together we do this after classes
        //Update Position Function has to be added somewhere
        if(plane->direction == Direction::North) 
        {
            plane->x_position = rand() % 1000000; // Random x position for North
            plane->y_position = 0; // Fixed y position for North
        }
        else if(plane->direction == Direction::South) 
        {
            plane->x_position = rand() % 100; // Random x position for South
            plane->y_position = 100; // Fixed y position for South
        }
        */
        
        // For arrivals, start at Holding state
        plane->state = FlightState::Holding;
        
        // Add to arrival queue
        atcController->scheduleArrival(plane);
        
        manager->logMessage("Flight " + plane->FlightNumber + 
                           " entering from " + 
                           (plane->direction == Direction::North ? "North" : "South") + 
                           " has entered the arrival queue");
        
        // Simulate the arrival sequence
        int waitTime = 0;
        while (!plane->hasRunwayAssigned && waitTime < 30) 
        {
            sleep(1);  // Check every second if runway assigned
            waitTime++;
            
            // Every 5 seconds, print status update with estimated wait time
            if (waitTime % 5 == 0) 
            {
                int estimatedWait = atcController->getScheduler()->estimateWaitTime(plane);
                manager->logMessage("Flight " + plane->FlightNumber + 
                                   " holding, estimated wait: " + 
                                   std::to_string(estimatedWait) + " minutes");
            }
        }
        
        // If runway assigned, proceed with landing sequence
        if (plane->hasRunwayAssigned) 
        {
            manager->logMessage("Flight " + plane->FlightNumber + 
                               " has been assigned a runway!");
            
            // Approach phase
            plane->state = FlightState::Approach;
            plane->UpdateSpeed();
            sleep(3);
            
            // Landing phase
            plane->state = FlightState::Landing;
            plane->UpdateSpeed();
            sleep(2);
            
            // Taxi phase
            plane->state = FlightState::Taxi;
            plane->UpdateSpeed();
            manager->logMessage("Flight " + plane->FlightNumber + 
                               " is now taxiing at " + std::to_string(plane->speed) + " km/h");
            sleep(2);
            
            // At gate
            plane->state = FlightState::AtGate;
            plane->UpdateSpeed();
            
            manager->logMessage("Flight " + plane->FlightNumber + 
                               " has arrived at gate");
            
            // Release the runway now that we're at the gate
            for (int i = 0; i < runwayManager->getRunwayCount(); i++) 
            {
                RunwayClass* runway = runwayManager->getRunwayByIndex(i);
                if (runway && runway->isOccupied) 
                {
                    runway->release();
                    break;
                }
            }
        } 
        else 
        {
            manager->logMessage("Flight " + plane->FlightNumber + 
                               " timed out waiting for runway!");
        }
    } 
    else 
    {
        // Departure flow - either East or West
        plane->direction = (plane->aircraftIndex % 4 == 1) ? Direction::East : Direction::West;
        
        // For departures, start at the gate
        plane->state = FlightState::AtGate;
        
        // Add to departure queue
        atcController->scheduleDeparture(plane);
        
        manager->logMessage("Flight " + plane->FlightNumber + 
                           " departing to " + 
                           (plane->direction == Direction::East ? "East" : "West") + 
                           " has entered the departure queue");
        
        // Simulate the departure sequence
        int waitTime = 0;
        while (!plane->hasRunwayAssigned && waitTime < 30) 
        {
            sleep(1);  // Check every second if runway assigned
            waitTime++;
            
            // Every 5 seconds, print status update with estimated wait time
            if (waitTime % 5 == 0) 
            {
                int estimatedWait = atcController->getScheduler()->estimateWaitTime(plane);
                manager->logMessage("Flight " + plane->FlightNumber + 
                                   " at gate, estimated wait: " + 
                                   std::to_string(estimatedWait) + " minutes");
            }
        }
        
        // If runway assigned, proceed with takeoff sequence
        if (plane->hasRunwayAssigned) 
        {
            manager->logMessage("Flight " + plane->FlightNumber + 
                               " has been assigned a runway!");
            
            // Taxi phase
            plane->state = FlightState::Taxi;
            plane->UpdateSpeed();
            manager->logMessage("Flight " + plane->FlightNumber + 
                               " is now taxiing at " + std::to_string(plane->speed) + " km/h");
            sleep(2);
            
            // Takeoff phase
            plane->state = FlightState::TakeoffRoll;
            plane->UpdateSpeed();
            manager->logMessage("Flight " + plane->FlightNumber + 
                               " is taking off at " + std::to_string(plane->speed) + " km/h");
            sleep(2);
            
            // Climb phase
            plane->state = FlightState::Climb;
            plane->UpdateSpeed();
            manager->logMessage("Flight " + plane->FlightNumber + 
                               " is climbing at " + std::to_string(plane->speed) + " km/h");
            sleep(2);
            
            // Cruise phase
            plane->state = FlightState::Cruise;
            plane->UpdateSpeed();
            
            manager->logMessage("Flight " + plane->FlightNumber + 
                               " has reached cruising altitude at " + std::to_string(plane->speed) + " km/h");
            
            // Release the runway now that we're airborne
            for (int i = 0; i < runwayManager->getRunwayCount(); i++) 
            {
                RunwayClass* runway = runwayManager->getRunwayByIndex(i);
                if (runway && runway->isOccupied) 
                {
                    runway->release();
                    break;
                }
            }
        } 
        else 
        {
            manager->logMessage("Flight " + plane->FlightNumber + 
                               " timed out waiting for runway!");
        }
    }
    
    // Flight is now complete or timed out
    plane->isActive = false;
    
    manager->logMessage("Flight " + plane->FlightNumber + 
                       " has completed its journey");
    
    // Clean up the args we allocated
    delete args;
    
    // Exit thread
    pthread_exit(nullptr);
}

/**
 * Thread function that monitors air traffic control
 * This was previously a lambda in main.cpp
 */
void* SimulationManager::atcControllerThreadFunction(void* arg) 
{
    // Extract the arguments
    ATCArgs* args = static_cast<ATCArgs*>(arg);
    SimulationManager* manager = args->manager;
    ATCScontroller* atcController = args->atc;
    RunwayManager* runwayManager = args->runways;
    int duration = args->duration;
    
    manager->logMessage("ATC controller active - monitoring flights");
    
    // Run the controller for the specified duration
    for (int i = 0; i < duration; i++) 
    {
        atcController->monitorFlight();
        sleep(1);  // Check every second
        
        // Every 20 seconds, print runway status
        if (i % 20 == 0) 
        {
            manager->logMessage("\n" + runwayManager->getStatusReport() + "\n");
        }
    }
    
    // Clean up the args we allocated
    delete args;
    
    return nullptr;
}

/**
 * Launch threads for all aircraft in an airline
 */
bool SimulationManager::launchAirlineThreads(Airline* airline) 
{
    // Create a thread for each aircraft in the airline
    for (size_t i = 0; i < airline->aircrafts.size(); ++i) 
    {
        // Create arguments struct to pass to thread
        ThreadArgs* args = new ThreadArgs;
        args->aircraft = &airline->aircrafts[i];
        args->manager = this;
        args->atc = atcController;
        args->runways = runwayManager;
        
        // Create thread ID
        pthread_t threadId;
        aircraftThreads.push_back(threadId);
        
        // Launch thread
        int result = pthread_create(&aircraftThreads.back(), NULL, 
                                   flightThreadFunction, 
                                   static_cast<void*>(args));
        
        if (result != 0) {
            std::cerr << "Error creating thread for Aircraft " << 
                     airline->aircrafts[i].FlightNumber << std::endl;
            return false;
        }
    }

    return true;
}

/**
 * Launch ATC controller thread
 */
bool SimulationManager::launchATCControllerThread() 
{
    // Create arguments
    ATCArgs* args = new ATCArgs;
    args->manager = this;
    args->atc = atcController;
    args->runways = runwayManager;
    args->duration = 300; // 5 minutes = 300 seconds
    
    // Launch thread
    int result = pthread_create(&atcControllerThread, NULL, 
                               atcControllerThreadFunction, 
                               static_cast<void*>(args));
                               
    return (result == 0);
}

/**
 * Create random emergency situation
 */
void SimulationManager::createRandomEmergency(const std::vector<Airline*>& airlines) 
{
    // 1 in 3 chance of emergency
    if (rand() % 3 == 0) {
        // Pick a random airline
        if (!airlines.empty()) {
            int randomAirlineIndex = rand() % airlines.size();
            Airline* randomAirline = airlines[randomAirlineIndex];
            
            if (!randomAirline->aircrafts.empty()) {
                // Pick a random aircraft
                int randomAircraftIndex = rand() % randomAirline->aircrafts.size();
                
                // Set emergency level (1-3)
                randomAirline->aircrafts[randomAircraftIndex].EmergencyNo = 1 + rand() % 3;
                
                logMessage("EMERGENCY ALERT: " + 
                          randomAirline->aircrafts[randomAircraftIndex].FlightNumber + 
                          " has declared emergency level " + 
                          std::to_string(randomAirline->aircrafts[randomAircraftIndex].EmergencyNo));
            }
        }
    }
}

/**
 * Wait for all threads to complete
 */
void SimulationManager::waitForCompletion() 
{
    // Wait for all aircraft threads to finish
    for (auto& threadId : aircraftThreads) {
        pthread_join(threadId, nullptr);
    }
    
    // Join the ATC controller thread
    pthread_join(atcControllerThread, nullptr);
}

/**
 * Thread-safe console logging with improved formatting
 */
void SimulationManager::logMessage(const std::string& message) 
{
    pthread_mutex_lock(&consoleMutex);
    
    // Get the current time for timestamping
    time_t now = time(nullptr);
    struct tm *timeinfo = localtime(&now);
    char timestamp[10];
    strftime(timestamp, sizeof(timestamp), "[%H:%M:%S]", timeinfo);
    
    // Detect message type and format accordingly
    if (message.find("Flight") != std::string::npos && message.find("runway") != std::string::npos) {
        // Runway assignment messages
        std::cout << "\033[1;33m" << timestamp << " [RUNWAY] " << message << "\033[0m" << std::endl;
    }
    else if (message.find("Flight") != std::string::npos && message.find("cruising") != std::string::npos) {
        // Flight status updates
        std::cout << "\033[1;32m" << timestamp << " [STATUS] " << message << "\033[0m" << std::endl;
    }
    else if (message.find("EMERGENCY") != std::string::npos) {
        // Emergency messages
        std::cout << "\033[1;31m" << timestamp << " [EMERGENCY] " << message << "\033[0m" << std::endl;
    }
    else if (message.find("VIOLATION") != std::string::npos) {
        // Violation messages
        std::cout << "\033[1;35m" << timestamp << " [VIOLATION] " << message << "\033[0m" << std::endl;
    }
    else {
        // Regular messages
        std::cout << "\033[1;36m" << timestamp << " [INFO] " << message << "\033[0m" << std::endl;
    }
    
    pthread_mutex_unlock(&consoleMutex);
}