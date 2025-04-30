#include "../include/ATCScontroller.h"
#include <iostream>
#include <ctime>
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
    //module 3 ka kaam
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