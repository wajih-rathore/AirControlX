#include "../include/ATCScontroller.h"
#include "../include/AVNGenerator.h" // Include AVNGenerator header for struct definitions
#include <iostream>
#include <ctime>
#include <unistd.h> // For write()
#include <string.h>
#include <sstream>
#include <algorithm>
using namespace std;

ATCScontroller::ATCScontroller()
{
    schedulingInterval = 1;  // Schedule every 1 second
    lastScheduleTime = time(NULL);
    runwayManager = nullptr; // Initialize to nullptr, must be set later with setRunwayManager
}

// Set the runway manager reference
void ATCScontroller::setRunwayManager(RunwayManager* rwManager)
{
    runwayManager = rwManager;
}

// Set the pipe for communication with AVN Generator
void ATCScontroller::setAVNPipe(int pipeWriteEnd)
{
    // Forward the pipe to the radar system
    radar.setAVNPipe(pipeWriteEnd);
    
    // Double check that the pipe is valid
    cout << "AVN pipe set successfully in ATCScontroller" << endl;
}

// Get count of active violations for the UI
int ATCScontroller::getActiveViolationsCount() const
{
    // Access our active flights from the scheduler
    const vector<Aircraft*>& activeFlights = scheduler.getActiveFlights();
    
    // Count violations
    int count = 0;
    for (const Aircraft* aircraft : activeFlights)
    {
        if (aircraft->hasActiveViolation)
        {
            count++;
        }
    }
    
    return count;
}

// Get list of aircraft with violations for the UI
vector<Aircraft*> ATCScontroller::getAircraftWithViolations() const
{
    // Access our active flights from the scheduler
    const vector<Aircraft*>& activeFlights = scheduler.getActiveFlights();
    
    // Filter to only those with violations
    vector<Aircraft*> violatingAircraft;
    for (Aircraft* aircraft : activeFlights)
    {
        if (aircraft->hasActiveViolation)
        {
            violatingAircraft.push_back(aircraft);
        }
    }
    
    return violatingAircraft;
}

// Monitor flights in the airspace - called periodically from main
void ATCScontroller::monitorFlight()
{
    // Check if it's time to schedule flights
    // No longer using system time, just schedule every call
    assignRunway();
}

// Assign runways to aircraft based on priority and availability
void ATCScontroller::assignRunway()
{
    // Make sure we have a valid runway manager
    if (!runwayManager)
    {
        // Oopsie! Someone forgot to set the runway manager. Let's whine about it.
        cout << "ATCScontroller: No runway manager available! Can't assign runways." << endl;
        return;
    }
    
    // Check runway availability first
    RunwayClass* rwyA = runwayManager->getRunwayByIndex(0); // RWY-A
    RunwayClass* rwyB = runwayManager->getRunwayByIndex(1); // RWY-B
    RunwayClass* rwyC = runwayManager->getRunwayByIndex(2); // RWY-C
    
    // Safety check - if any runway pointer is null, something's very wrong
    if (!rwyA || !rwyB || !rwyC)
    {
        cout << "ATCScontroller: Missing runway in manager! Can't assign runways." << endl;
        return;
    }
    
    bool rwyA_available = !rwyA->isOccupied;
    bool rwyB_available = !rwyB->isOccupied;
    bool rwyC_available = !rwyC->isOccupied;
    
    // If no runways are available, nothing to do
    if (!rwyA_available && !rwyB_available && !rwyC_available)
    {
        return;
    }
    
    // Step 1: First priority is ALWAYS emergency flights
    Aircraft* emergency = scheduler.getNextEmergency();
    if (emergency != nullptr)
    {
        // Try to assign emergency to appropriate runway based on direction
        if ((emergency->direction == Direction::North || emergency->direction == Direction::South) && rwyA_available)
        {
            // Emergency arrival - assign to RWY-A
            cout << "Emergency " << emergency->FlightNumber  << " assigned to RWY-A (emergency arrival)" <<  endl;
            rwyA->tryAssign(*emergency);
            emergency->hasRunwayAssigned = true;
            return;
        }
        else if ((emergency->direction == Direction::East || emergency->direction == Direction::West) && rwyB_available)
        {
            // Emergency departure - assign to RWY-B
             cout << "Emergency " << emergency->FlightNumber  << " assigned to RWY-B (emergency departure)" <<  endl;
            rwyB->tryAssign(*emergency);
            emergency->hasRunwayAssigned = true;
            return;
        }
        else if (rwyC_available)
        {
            // Use flexible runway for emergency
             cout << "Emergency " << emergency->FlightNumber << " assigned to RWY-C (flexible emergency)" <<  endl;
            rwyC->tryAssign(*emergency);
            emergency->hasRunwayAssigned = true;
            return;
        }
        // If we get here, all runways are occupied but appropriate for emergency
        // In a real system, we might consider clearing a runway for the emergency
    }
    
    //Special handling for cargo flights they primarily use RWY-C
    if (rwyC_available)
    {
        // Check both queues for cargo flights
        bool cargoAssigned = false;
        
        // First check arrivals
        Aircraft* arrival = nullptr;
        while ((arrival = scheduler.getNextArrival()) != nullptr)
        {
            if (arrival->type == AirCraftType::Cargo)
            {
                 cout << "Cargo arrival " << arrival->FlightNumber << " assigned to RWY-C (cargo priority)" <<  endl;
                rwyC->tryAssign(*arrival);
                arrival->hasRunwayAssigned = true;
                cargoAssigned = true;
                break;
            }
            else
            {
                // Put it back in the queue if it's not cargo
                scheduler.addArrival(arrival);
                break;  // Only check the first one to maintain priority order
            }
        }
        
        // If no cargo arrivals, check departures
        if (!cargoAssigned)
        {
            Aircraft* departure = nullptr;
            while ((departure = scheduler.getNextDeparture()) != nullptr)
            {
                if (departure->type == AirCraftType::Cargo)
                {
                     cout << "Cargo departure " << departure->FlightNumber << " assigned to RWY-C (cargo priority)" <<  endl;
                    rwyC->tryAssign(*departure);
                    departure->hasRunwayAssigned = true;
                    cargoAssigned = true;
                    break;
                }
                else
                {
                    // Put it back in the queue if it's not cargo
                    scheduler.addDeparture(departure);
                    break;  // Only check the first one to maintain priority order
                }
            }
        }
        
        // If we assigned a cargo flight, we're done for this scheduling cycle
        if (cargoAssigned)
        {
            return;
        }
    }
    
    // Step 3: Handle regular arrivals and departures based on direction
    
    // Process arrivals for RWY-A (North/South)
    if (rwyA_available)
    {
        Aircraft* arrival = scheduler.getNextArrival();
        if (arrival != nullptr)
        {
            if (arrival->direction == Direction::North || arrival->direction == Direction::South)
            {
                 cout << "Arrival " << arrival->FlightNumber << " assigned to RWY-A (direction N/S)" <<  endl;
                rwyA->tryAssign(*arrival);
                arrival->hasRunwayAssigned = true;
            }
            else
            {
                // This shouldn't happen based on your simulation setup,
                // but just in case, put it back in the queue
                scheduler.addArrival(arrival);
            }
        }
    }
    
    // Process departures for RWY-B (East/West)
    if (rwyB_available)
    {
        Aircraft* departure = scheduler.getNextDeparture();
        if (departure != nullptr)
        {
            if (departure->direction == Direction::East || departure->direction == Direction::West)
            {
                 cout << "Departure " << departure->FlightNumber 
                          << " assigned to RWY-B (direction E/W)" <<  endl;
                rwyB->tryAssign(*departure);
                departure->hasRunwayAssigned = true;
            }
            else
            {
                // This shouldn't happen based on your simulation setup,
                // but just in case, put it back in the queue
                scheduler.addDeparture(departure);
            }
        }
    }
    
    // Step 4: Use RWY-C for overflow if it's still available (FR2.3)
    if (rwyC_available)
    {
        // Check for any waiting arrivals first
        Aircraft* arrival = scheduler.getNextArrival();
        if (arrival != nullptr)
        {
             cout << "Overflow arrival " << arrival->FlightNumber  << " assigned to RWY-C (overflow)" <<  endl;
            rwyC->tryAssign(*arrival);
            arrival->hasRunwayAssigned = true;
        }
        else
        {
            // If no arrivals, check for departures
            Aircraft* departure = scheduler.getNextDeparture();
            if (departure != nullptr)
            {
                 cout << "Overflow departure " << departure->FlightNumber << " assigned to RWY-C (overflow)" <<  endl;
                rwyC->tryAssign(*departure);
                departure->hasRunwayAssigned = true;
            }
        }
    }
}

// Handle violations detected by radar monitoring
void ATCScontroller::handleViolations()
{
    // Delegate to the radar system to handle violations
    const std::vector<Aircraft*>& activeFlights = scheduler.getActiveFlights();
    //Create a new vector that stores the object rather than a reference
    std::vector<Aircraft> activeFlightsCopy;
    for (const Aircraft* aircraft : activeFlights)
    {
        activeFlightsCopy.push_back(*aircraft);
    }
    radar.handleViolations(activeFlightsCopy);
}

// Add an arrival flight to be scheduled
void ATCScontroller::scheduleArrival(Aircraft* aircraft)
{
    scheduler.addArrival(aircraft);
}

// Add a departure flight to be scheduled
void ATCScontroller::scheduleDeparture(Aircraft* aircraft)
{
    scheduler.addDeparture(aircraft);
}

FlightsScheduler* ATCScontroller::getScheduler()
{
    return &scheduler;
}

// Get a pointer to the flight scheduler (alias for getScheduler())
FlightsScheduler* ATCScontroller::getFlightScheduler()
{
    // This is an alias for getScheduler() to maintain compatibility
    return &scheduler;
}

// Test function to simulate a violation
void ATCScontroller::simulateViolation(const std::string& flightNumber, const std::string& airline, 
                                      int speed, int minAllowed, int maxAllowed)
{
    radar.reportViolation(flightNumber, airline, speed, minAllowed, maxAllowed);
}

// ======== SFML Visualization Helper Functions ========

/**
 * Get all active flights for visualization
 * Returns a thread-safe copy of all active flights
 */
std::vector<Aircraft*> ATCScontroller::getAllActiveFlights() const
{
    // Lock for thread safety
    std::lock_guard<std::mutex> lock(const_cast<ATCScontroller*>(this)->controllerMutex);
    
    // Get a copy of active flights from scheduler
    // (This assumes scheduler already has thread safety internally)
    return scheduler.getActiveFlights();
}

/**
 * Get status text for visualization
 * Returns formatted text about ATC status for the UI
 */
std::string ATCScontroller::getStatusText() const
{
    // Lock for thread safety
    std::lock_guard<std::mutex> lock(const_cast<ATCScontroller*>(this)->controllerMutex);
    
    std::stringstream statusText;
    
    // Count aircraft in different states
    int totalActive = scheduler.getActiveFlights().size();
    int holding = 0;
    int landing = 0;
    int takeoff = 0;
    int taxiing = 0;
    
    // Iterate through all active flights to count states
    for (Aircraft* aircraft : scheduler.getActiveFlights())
    {
        switch (aircraft->state)
        {
            case FlightState::Holding:
                holding++;
                break;
            case FlightState::Approach:
            case FlightState::Landing:
                landing++;
                break;
            case FlightState::TakeoffRoll:
            case FlightState::Climb:
                takeoff++;
                break;
            case FlightState::Taxi:
                taxiing++;
                break;
            default:
                break;
        }
    }
    
    // Format status text
    statusText << "ATC Status: " << totalActive << " flights active" << std::endl;
    statusText << "Holding: " << holding << " | Landing: " << landing << std::endl;
    statusText << "Takeoff: " << takeoff << " | Taxiing: " << taxiing << std::endl;
    
    // Add violation count
    int violations = getActiveViolationsCount();
    if (violations > 0)
    {
        statusText << "VIOLATIONS: " << violations << std::endl;
    }
    
    return statusText.str();
}

/**
 * Get flight with highest priority
 * Returns the flight currently with highest priority in the scheduling system
 */
Aircraft* ATCScontroller::getHighestPriorityFlight() const
{
    // Lock for thread safety
    std::lock_guard<std::mutex> lock(const_cast<ATCScontroller*>(this)->controllerMutex);
    
    // First check for any emergency flight
    Aircraft* emergencyFlight = scheduler.getNextEmergency();
    if (emergencyFlight)
    {
        return emergencyFlight;
    }
    
    // Check arrival queue - don't actually remove from queue, just peek
    if (!scheduler.arrivalQueue.empty())
    {
        // Use a copy of the queue to avoid messing with the real one
        std::vector<Aircraft*> arrivalCopy = scheduler.arrivalQueue;
        
        // Sort by priority
        std::sort(arrivalCopy.begin(), arrivalCopy.end(), 
            [](Aircraft* a, Aircraft* b) {
                return a->calculatePriorityScore() > b->calculatePriorityScore();
            });
        
        // Return highest priority arrival
        return arrivalCopy.front();
    }
    
    // Check departure queue - don't actually remove from queue, just peek
    if (!scheduler.departureQueue.empty())
    {
        // Use a copy of the queue to avoid messing with the real one
        std::vector<Aircraft*> departureCopy = scheduler.departureQueue;
        
        // Sort by priority
        std::sort(departureCopy.begin(), departureCopy.end(), 
            [](Aircraft* a, Aircraft* b) {
                return a->calculatePriorityScore() > b->calculatePriorityScore();
            });
        
        // Return highest priority departure
        return departureCopy.front();
    }
    
    // No flights in queue
    return nullptr;
}

/**
 * Get flights with specific state
 * Returns all flights currently in the specified flight state
 */
std::vector<Aircraft*> ATCScontroller::getFlightsByState(FlightState state) const
{
    // Lock for thread safety
    std::lock_guard<std::mutex> lock(const_cast<ATCScontroller*>(this)->controllerMutex);
    
    std::vector<Aircraft*> matchingFlights;
    
    // Iterate through all active flights to find matching state
    for (Aircraft* aircraft : scheduler.getActiveFlights())
    {
        if (aircraft->state == state)
        {
            matchingFlights.push_back(aircraft);
        }
    }
    
    return matchingFlights;
}

/**
 * Get flights by emergency level
 * Returns all flights with the specified emergency level
 */
std::vector<Aircraft*> ATCScontroller::getFlightsByEmergencyLevel(int emergencyLevel) const
{
    // Lock for thread safety
    std::lock_guard<std::mutex> lock(const_cast<ATCScontroller*>(this)->controllerMutex);
    
    std::vector<Aircraft*> matchingFlights;
    
    // Iterate through all active flights to find matching emergency level
    for (Aircraft* aircraft : scheduler.getActiveFlights())
    {
        if (aircraft->EmergencyNo == emergencyLevel)
        {
            matchingFlights.push_back(aircraft);
        }
    }
    
    return matchingFlights;
}

/**
 * Get flights waiting for runway
 * Returns all flights waiting for runway assignment
 */
std::vector<Aircraft*> ATCScontroller::getFlightsWaitingForRunway() const
{
    // Lock for thread safety
    std::lock_guard<std::mutex> lock(const_cast<ATCScontroller*>(this)->controllerMutex);
    
    std::vector<Aircraft*> waitingFlights;
    
    // Check arrivals queue
    for (Aircraft* aircraft : scheduler.arrivalQueue)
    {
        if (!aircraft->hasRunwayAssigned)
        {
            waitingFlights.push_back(aircraft);
        }
    }
    
    // Check departures queue
    for (Aircraft* aircraft : scheduler.departureQueue)
    {
        if (!aircraft->hasRunwayAssigned)
        {
            waitingFlights.push_back(aircraft);
        }
    }
    
    return waitingFlights;
}

/**
 * Get flight counts by type
 * Returns array with counts of each aircraft type [commercial, cargo, military, emergency, medical]
 */
int* ATCScontroller::getFlightCountsByType() const
{
    // Lock for thread safety
    std::lock_guard<std::mutex> lock(const_cast<ATCScontroller*>(this)->controllerMutex);
    
    // Static array to avoid memory leaks when returning pointer
    static int counts[5] = {0, 0, 0, 0, 0};
    
    // Reset counts
    for (int i = 0; i < 5; i++)
    {
        counts[i] = 0;
    }
    
    // Count aircraft types
    for (Aircraft* aircraft : scheduler.getActiveFlights())
    {
        switch (aircraft->type)
        {
            case AirCraftType::Commercial:
                counts[0]++;
                break;
            case AirCraftType::Cargo:
                counts[1]++;
                break;
            case AirCraftType::Military:
                counts[2]++;
                break;
            case AirCraftType::Emergency:
                counts[3]++;
                break;
            case AirCraftType::Medical:
                counts[4]++;
                break;
        }
    }
    
    return counts;
}