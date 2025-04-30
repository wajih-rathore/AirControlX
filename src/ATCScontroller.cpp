#include "../include/ATCScontroller.h"
#include <iostream>
#include <ctime>
#include <iomanip>
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
    // SAFETY FIRST! Check if runwayManager is properly initialized
    if (runwayManager == nullptr)
    {
        std::cerr << "\n┌────────────────── ERROR ──────────────────┐" << std::endl;
        std::cerr << "│ Runway manager not initialized!           │" << std::endl;
        std::cerr << "│ Please call setRunwayManager() first      │" << std::endl;
        std::cerr << "└──────────────────────────────────────────┘" << std::endl;
        return;
    }
    
    // Get runway availability - with proper error checking
    bool rwyA_available = false;
    bool rwyB_available = false;
    bool rwyC_available = false;
    
    RunwayClass* rwyA = nullptr;
    RunwayClass* rwyB = nullptr;
    RunwayClass* rwyC = nullptr;
    
    try
    {
        // Check availability of runways
        rwyA_available = runwayManager->isRunwayAvailable("RWY-A");
        rwyB_available = runwayManager->isRunwayAvailable("RWY-B");
        rwyC_available = runwayManager->isRunwayAvailable("RWY-C");
        
        // Get runway pointers (only if they're available - saves unnecessary calls)
        if (rwyA_available) rwyA = runwayManager->getRunway("RWY-A");
        if (rwyB_available) rwyB = runwayManager->getRunway("RWY-B");
        if (rwyC_available) rwyC = runwayManager->getRunway("RWY-C");
    }
    catch (const std::exception& e)
    {
        // Catching any exceptions from runway manager operations
        std::cerr << "\n┌────────────────── ERROR ──────────────────┐" << std::endl;
        std::cerr << "│ Error accessing runway information:       │" << std::endl;
        std::cerr << "│ " << std::left << std::setw(40) << e.what() << "│" << std::endl;
        std::cerr << "└──────────────────────────────────────────┘" << std::endl;
        return;
    }
    
    // North/South arrivals to RWY-A
    if (rwyA_available && rwyA != nullptr)
    {
        Aircraft* arrival = scheduler.getNextArrival();
        if (arrival != nullptr && 
            (arrival->direction == Direction::North || arrival->direction == Direction::South))
        {
            std::string directionStr = (arrival->direction == Direction::North) ? "North" : "South";
            std::cout << "\n┌────────────────── RUNWAY ASSIGNMENT ────────────────────┐" << std::endl;
            std::cout << "│ Flight: " << std::left << std::setw(12) << arrival->FlightNumber;
            std::cout << " Direction: " << std::left << std::setw(6) << directionStr;
            std::cout << " Assigned: RWY-A │" << std::endl;
            std::cout << "└────────────────────────────────────────────────────────┘" << std::endl;
            
            rwyA->tryAssign(*arrival);
            arrival->hasRunwayAssigned = true;
        }
    }
    
    // East/West departures to RWY-B
    if (rwyB_available && rwyB != nullptr)
    {
        Aircraft* departure = scheduler.getNextDeparture();
        if (departure != nullptr && 
            (departure->direction == Direction::East || departure->direction == Direction::West))
        {
            std::string directionStr = (departure->direction == Direction::East) ? "East" : "West";
            std::cout << "\n┌────────────────── RUNWAY ASSIGNMENT ──────────────────┐" << std::endl;
            std::cout << "│ Flight: " << std::left << std::setw(12) << departure->FlightNumber;
            std::cout << " Direction: " << std::left << std::setw(6) << directionStr;
            std::cout << " Assigned: RWY-B │" << std::endl;
            std::cout << "└──────────────────────────────────────────────────────┘" << std::endl;
            
            rwyB->tryAssign(*departure);
            departure->hasRunwayAssigned = true;
        }
    }
    
    // Cargo/emergency/overflow to RWY-C
    if (rwyC_available && rwyC != nullptr)
    {
        // Check for any waiting arrivals first
        Aircraft* arrival = scheduler.getNextArrival();
        if (arrival != nullptr)
        {
            std::string typeStr;
            switch(arrival->type) {
                case AirCraftType::Cargo: typeStr = "Cargo"; break;
                case AirCraftType::Medical: typeStr = "Medical"; break;
                case AirCraftType::Military: typeStr = "Military"; break;
                default: typeStr = "Overflow"; break;
            }
            
            std::cout << "\n┌────────────────── RUNWAY ASSIGNMENT ──────────────────┐" << std::endl;
            std::cout << "│ Flight: " << std::left << std::setw(12) << arrival->FlightNumber;
            std::cout << " Type: " << std::left << std::setw(8) << typeStr;
            std::cout << " Assigned: RWY-C │" << std::endl;
            std::cout << "└──────────────────────────────────────────────────────┘" << std::endl;
            
            rwyC->tryAssign(*arrival);
            arrival->hasRunwayAssigned = true;
        }
        else
        {
            // If no arrivals, check for departures
            Aircraft* departure = scheduler.getNextDeparture();
            if (departure != nullptr)
            {
                std::string typeStr;
                switch(departure->type) {
                    case AirCraftType::Cargo: typeStr = "Cargo"; break;
                    case AirCraftType::Medical: typeStr = "Medical"; break;
                    case AirCraftType::Military: typeStr = "Military"; break;
                    default: typeStr = "Overflow"; break;
                }
                
                std::cout << "\n┌────────────────── RUNWAY ASSIGNMENT ──────────────────┐" << std::endl;
                std::cout << "│ Flight: " << std::left << std::setw(12) << departure->FlightNumber;
                std::cout << " Type: " << std::left << std::setw(8) << typeStr;
                std::cout << " Assigned: RWY-C │" << std::endl;
                std::cout << "└──────────────────────────────────────────────────────┘" << std::endl;
                
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