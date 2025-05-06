#include "../include/ATCScontroller.h"
#include "../include/AVNGenerator.h" // Include AVNGenerator header for struct definitions
#include <iostream>
#include <ctime>
#include <unistd.h> // For write()
#include <string.h>
using namespace std;

ATCScontroller::ATCScontroller()
{
    schedulingInterval = 1;  // Schedule every 1 second
    lastScheduleTime = time(NULL);
    runwayManager = nullptr; // Initialize to nullptr, must be set later with setRunwayManager
    avnPipeWriteEnd = -1;    // Initialize pipe file descriptor to invalid value
}

// Set the runway manager reference
void ATCScontroller::setRunwayManager(RunwayManager* rwManager)
{
    runwayManager = rwManager;
}

// Set the pipe for communication with AVN Generator
void ATCScontroller::setAVNPipe(int pipeWriteEnd)
{
    avnPipeWriteEnd = pipeWriteEnd;
    
    // Double check that the pipe is valid
    if (avnPipeWriteEnd <= 0)
    {
        cerr << "WARNING: Invalid AVN pipe file descriptor provided!" << endl;
    }
    else 
    {
        cout << "AVN pipe set successfully in ATCScontroller" << endl;
    }
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
    time_t currentTime = time(NULL);
    if (currentTime - lastScheduleTime >= schedulingInterval)
    {
        // It's time to schedule flights
        assignRunway();
        lastScheduleTime = currentTime;
    }
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
    // This function detects aircraft speed violations and sends them to the AVN Generator process
    // It's called periodically from the radar monitoring thread
    
    // Access our active flights from the scheduler
    // Using const reference since we only need to read from the vector, not modify it
    const std::vector<Aircraft*>& activeFlights = scheduler.getActiveFlights();
    
    // Check if we have a pipe to communicate with AVN Generator
    if (avnPipeWriteEnd <= 0)
    {
        // No pipe set up - can't send violations
        std::cout << "ATCScontroller: No AVN pipe set up - can't send violations" << std::endl;
        return;
    }
    
    // Loop through all active flights and check for speed violations
    for (Aircraft* aircraft : activeFlights)
    {
        // Skip aircraft that already have active violations
        if (aircraft->hasActiveViolation)
        {
            continue;
        }
        
        // Check for speed violations based on current state
        bool speedViolation = false;
        int minAllowed = 0;
        int maxAllowed = 0;
        
        // Set allowed speed range based on aircraft state
        // Converting from FlightState to our internal state handling
        switch (aircraft->state)
        {
            case FlightState::Holding:
                minAllowed = 400;
                maxAllowed = 600;
                speedViolation = (aircraft->speed > maxAllowed);
                break;
                
            case FlightState::Approach:
                minAllowed = 240;
                maxAllowed = 290;
                speedViolation = (aircraft->speed < minAllowed || aircraft->speed > maxAllowed);
                break;
                
            case FlightState::Landing:
                minAllowed = 30; // Landing should end below this
                maxAllowed = 240; // Start of landing
                speedViolation = (aircraft->speed > maxAllowed);
                break;
                
            case FlightState::Taxi:
                minAllowed = 15;
                maxAllowed = 30;
                speedViolation = (aircraft->speed > maxAllowed);
                break;
                
            case FlightState::AtGate:
                minAllowed = 0;
                maxAllowed = 5;
                speedViolation = (aircraft->speed > 10); // Violation if > 10
                break;
                
            case FlightState::TakeoffRoll:
                minAllowed = 0;
                maxAllowed = 290;
                speedViolation = (aircraft->speed > maxAllowed);
                break;
                
            case FlightState::Climb:
                minAllowed = 250;
                maxAllowed = 463;
                speedViolation = (aircraft->speed > maxAllowed);
                break;
                
            case FlightState::Cruise: // This was previously called "Departure" in the code
                minAllowed = 800;
                maxAllowed = 900;
                speedViolation = (aircraft->speed < minAllowed || aircraft->speed > maxAllowed);
                break;
                
            default:
                // Unknown state - no violation check
                break;
        }
        
        // If we found a speed violation, send it to AVN Generator
        if (speedViolation)
        {
            std::cout << "VIOLATION DETECTED: " << aircraft->FlightNumber 
                 << " (" << aircraft->Airline << ") - Speed: " << aircraft->speed 
                 << " km/h (Allowed: " << minAllowed << "-" << maxAllowed << " km/h)" << std::endl;
                 
            // Mark aircraft as having active violation
            aircraft->hasActiveViolation = true;
            
            // Create violation data to send through pipe
            ViolationData violation;
            
            // Copy aircraft data to the struct with proper string handling
            // Using strncpy to avoid buffer overflows (a common C/C++ security issue)
            strncpy(violation.flightNumber, aircraft->FlightNumber.c_str(), sizeof(violation.flightNumber) - 1);
            violation.flightNumber[sizeof(violation.flightNumber) - 1] = '\0';  // Ensure null-termination
            
            strncpy(violation.airLine, aircraft->Airline.c_str(), sizeof(violation.airLine) - 1);
            violation.airLine[sizeof(violation.airLine) - 1] = '\0';  // Ensure null-termination
            
            // Set speed and allowed range
            violation.speed = aircraft->speed;
            violation.minAllowed = minAllowed;
            violation.maxAllowed = maxAllowed;
            
            // Send through pipe to AVN Generator process
            // The write() might fail if the pipe is full or broken - we should handle that
            ssize_t bytesWritten = write(avnPipeWriteEnd, &violation, sizeof(ViolationData));
            
            if (bytesWritten != sizeof(ViolationData))
            {
                std::cerr << "ERROR: Failed to send violation data to AVN Generator" << std::endl;
                // In a real system, we might want to retry or queue this for later
            }
            else 
            {
                std::cout << "Violation data sent to AVN Generator process" << std::endl;
            }
        }
    }
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