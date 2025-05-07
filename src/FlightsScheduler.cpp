#include "../include/FlightsScheduler.h"
#include <algorithm>
#include <ctime>
#include <sstream>
#include <cmath>

// Constructor initializes empty queues
FlightsScheduler::FlightsScheduler()
{
    //vectors use karrahe hein so no need to initialize anything ig
}

void FlightsScheduler::addArrival(Aircraft* aircraft)
{
    // Set the current time as queue entry time (for FCFS tracking)
    aircraft->queueEntryTime = time(NULL);
    

    arrivalMutex.lock();

    //critical section starts from here RAAAA
    arrivalQueue.push_back(aircraft);
    
    // Sort the queue by priority (now that we've added a new aircraft)
    sortQueues();
    
    // Add to active flights list if it's not already there
    if (std::find(activeFlights.begin(), activeFlights.end(), aircraft) == activeFlights.end())
    {
        activeFlights.push_back(aircraft);
    }
    
    // Unlock the mutex when we're done
    arrivalMutex.unlock();
}

// Add a departure flight to the queue
void FlightsScheduler::addDeparture(Aircraft* aircraft)
{
    // Set the current time as queue entry time (for FCFS tracking)
    aircraft->queueEntryTime = time(NULL);
    
    departureMutex.lock();
    
    // Add aircraft to the queue
    departureQueue.push_back(aircraft);
    
    // Sort the queue by priority (now that we've added a new aircraft)
    sortQueues();
    
    // Add to active flights list if it's not already there
    if (std::find(activeFlights.begin(), activeFlights.end(), aircraft) == activeFlights.end())
    {
        activeFlights.push_back(aircraft);
    }
    
    // Unlock the mutex when we're done
    departureMutex.unlock();
}

// Get the highest priority arrival flight
Aircraft* FlightsScheduler::getNextArrival()
{
    // Lock the queue while we're reading/modifying it
    arrivalMutex.lock();
    
    // Check if there are any arrivals waiting
    if (arrivalQueue.empty())
    {
        arrivalMutex.unlock(); // Don't forget to unlock before returning!
        return nullptr;        // No aircraft in queue
    }
    
    // Get the first aircraft (highest priority after sorting)
    Aircraft* nextAircraft = arrivalQueue.front();
    
    // Remove it from the queue
    arrivalQueue.erase(arrivalQueue.begin());
    
    // Unlock the mutex when we're done
    arrivalMutex.unlock();
    
    // Return the highest priority aircraft
    return nextAircraft;
}

// Get the highest priority departure flight
Aircraft* FlightsScheduler::getNextDeparture()
{
    // Lock the queue while we're reading/modifying it
    departureMutex.lock();
    
    // Check if there are any departures waiting
    if (departureQueue.empty())
    {
        departureMutex.unlock(); // Don't forget to unlock before returning!
        return nullptr;          // No aircraft in queue
    }
    
    // Get the first aircraft (highest priority after sorting)
    Aircraft* nextAircraft = departureQueue.front();
    
    // Remove it from the queue
    departureQueue.erase(departureQueue.begin());
    
    // Unlock the mutex when we're done
    departureMutex.unlock();
    
    // Return the highest priority aircraft
    return nextAircraft;
}

// Find any emergency flight in either queue (highest priority of all) and remove it
// This is the non-const version that removes the aircraft from the queue
Aircraft* FlightsScheduler::removeNextEmergency()
{
    Aircraft* emergencyAircraft = nullptr;
    int highestPriority = 0;
    
    // First check arrivals for emergencies
    arrivalMutex.lock();
    for (size_t i = 0; i < arrivalQueue.size(); i++)
    {
        // Check if this is an active emergency flight
        if (arrivalQueue[i]->EmergencyNo > 0 && arrivalQueue[i]->isActive)
        {
            // Calculate its priority score
            int priority = arrivalQueue[i]->calculatePriorityScore();
            
            // Check if it's the highest priority emergency so far
            if (priority > highestPriority)
            {
                highestPriority = priority;
                emergencyAircraft = arrivalQueue[i];
            }
        }
    }
    
    // If we found an emergency in arrivals, remove it from the queue
    if (emergencyAircraft != nullptr)
    {
        // Find the aircraft in the queue and remove it
        auto it = std::find(arrivalQueue.begin(), arrivalQueue.end(), emergencyAircraft);
        if (it != arrivalQueue.end())
        {
            arrivalQueue.erase(it);
        }
    }
    arrivalMutex.unlock();
    
    // If no emergency in arrivals, check departures
    if (emergencyAircraft == nullptr)
    {
        departureMutex.lock();
        for (size_t i = 0; i < departureQueue.size(); i++)
        {
            // Check if this is an active emergency flight
            if (departureQueue[i]->EmergencyNo > 0 && departureQueue[i]->isActive)
            {
                // Calculate its priority score
                int priority = departureQueue[i]->calculatePriorityScore();
                
                // Check if it's the highest priority emergency so far
                if (priority > highestPriority)
                {
                    highestPriority = priority;
                    emergencyAircraft = departureQueue[i];
                }
            }
        }
        
        // If we found an emergency in departures, remove it from the queue
        if (emergencyAircraft != nullptr)
        {
            // Find the aircraft in the queue and remove it
            auto it = std::find(departureQueue.begin(), departureQueue.end(), emergencyAircraft);
            if (it != departureQueue.end())
            {
                departureQueue.erase(it);
            }
        }
        departureMutex.unlock();
    }
    
    // Return the highest priority emergency aircraft (or nullptr if no emergencies)
    return emergencyAircraft;
}

// Find any emergency flight in either queue without removing it (const version)
// This version just peeks at the queue and doesn't modify it
Aircraft* FlightsScheduler::getNextEmergency() const
{
    // This version doesn't modify anything, just looks for emergencies
    
    Aircraft* emergencyAircraft = nullptr;
    int highestPriority = 0;
    
    // Check arrivals for emergencies (without modifying the queue)
    for (size_t i = 0; i < arrivalQueue.size(); i++)
    {
        // Check if this is an active emergency flight 
        if (arrivalQueue[i]->EmergencyNo > 0 && arrivalQueue[i]->isActive)
        {
            // Calculate its priority score
            int priority = arrivalQueue[i]->calculatePriorityScore();
            
            // Check if it's the highest priority emergency so far
            if (priority > highestPriority)
            {
                highestPriority = priority;
                emergencyAircraft = arrivalQueue[i];
            }
        }
    }
    
    // If no emergency in arrivals, check departures (without modifying the queue)
    if (emergencyAircraft == nullptr)
    {
        for (size_t i = 0; i < departureQueue.size(); i++)
        {
            // Check if this is an active emergency flight
            if (departureQueue[i]->EmergencyNo > 0 && departureQueue[i]->isActive)
            {
                // Calculate its priority score
                int priority = departureQueue[i]->calculatePriorityScore();
                
                // Check if it's the highest priority emergency so far
                if (priority > highestPriority)
                {
                    highestPriority = priority;
                    emergencyAircraft = departureQueue[i];
                }
            }
        }
    }
    
    // Return the highest priority emergency aircraft (or nullptr if no emergencies)
    // This is just for peeking, not removing from queue
    return emergencyAircraft;
}

// Sort both queues by priority score
void FlightsScheduler::sortQueues()
{
    // Sort arrival queue - no lock needed here because we're called from methods that already lock
    // We want higher priority scores first, so we use this comparison function
    std::sort(arrivalQueue.begin(), arrivalQueue.end(), 
        [](Aircraft* a, Aircraft* b) {
            return a->calculatePriorityScore() > b->calculatePriorityScore();
        });
    
    // Sort departure queue - no lock needed here because we're called from methods that already lock
    // Same comparison as above - higher scores first
    std::sort(departureQueue.begin(), departureQueue.end(), 
        [](Aircraft* a, Aircraft* b) {
            return a->calculatePriorityScore() > b->calculatePriorityScore();
        });
}

// Estimate wait time for an aircraft in the queue (FR5.2)
int FlightsScheduler::estimateWaitTime(Aircraft* aircraft)
{
    int position = -1;
    const int processingTimePerAircraft = 2; // minutes per aircraft
    
    // Check which queue the aircraft is in and find its position
    // We need to lock both queues while searching to avoid race conditions
    
    // First check arrival queue
    arrivalMutex.lock();
    for (size_t i = 0; i < arrivalQueue.size(); i++)
    {
        if (arrivalQueue[i] == aircraft)
        {
            position = static_cast<int>(i);
            break;
        }
    }
    arrivalMutex.unlock();
    
    // If not found in arrivals, check departures
    if (position < 0)
    {
        departureMutex.lock();
        for (size_t i = 0; i < departureQueue.size(); i++)
        {
            if (departureQueue[i] == aircraft)
            {
                position = static_cast<int>(i);
                break;
            }
        }
        departureMutex.unlock();
    }
    
    // If aircraft found, calculate estimated wait time
    if (position >= 0)
    {
        return position * processingTimePerAircraft;
    }
    
    // Aircraft not found in either queue
    return -1;
}

// Schedule flights based on priority - will be enhanced when runway management is implemented
void FlightsScheduler::scheduleFlight()
{
    // This will be implemented as part of the ATCScontroller logic
    // For now, we've laid the groundwork with our priority queues
}

// Get all currently active flights (both arrival and departure)
const std::vector<Aircraft*>& FlightsScheduler::getActiveFlights() const
{
// This method doesn't need to lock anything because it just returns a reference
    // The caller is responsible for not modifying the vector directly
    // If they need to modify it, they should use the addArrival/addDeparture methods
    
    return activeFlights;
}

// ======== SFML Visualization Abstraction Functions ========

/**
 * Get queue statistics for visualization
 * Returns array with [total active, arrivals, departures, emergency]
 */
int* FlightsScheduler::getQueueStatistics() const
{
    // Using static array to avoid memory leaks
    static int stats[4];
    
    // Count emergency aircraft across all active flights
    int emergencyCount = 0;
    for (const auto& aircraft : activeFlights)
    {
        if (aircraft->EmergencyNo > 0) emergencyCount++;
    }
    
    // Fill statistics array
    stats[0] = static_cast<int>(activeFlights.size());  // Total active
    stats[1] = static_cast<int>(arrivalQueue.size());   // Arrivals in queue
    stats[2] = static_cast<int>(departureQueue.size()); // Departures in queue
    stats[3] = emergencyCount;                          // Emergency aircraft
    
    return stats;
}

/**
 * Get all active emergency aircraft
 * Returns vector of pointers to emergency aircraft for visualization
 */
std::vector<Aircraft*> FlightsScheduler::getActiveEmergencyAircraft() const
{
    // Create vector to hold emergency aircraft
    std::vector<Aircraft*> emergencyAircraft;
    
    // Find emergency aircraft in all active flights
    for (Aircraft* aircraft : activeFlights)
    {
        if (aircraft->EmergencyNo > 0)
        {
            emergencyAircraft.push_back(aircraft);
        }
    }
    
    return emergencyAircraft;
}

/**
 * Get all aircraft with violations
 * Returns vector of pointers to aircraft with violations
 */
std::vector<Aircraft*> FlightsScheduler::getAircraftWithViolations() const
{
    // Create vector to hold aircraft with violations
    std::vector<Aircraft*> violatingAircraft;
    
    // Find aircraft with violations in all active flights
    for (Aircraft* aircraft : activeFlights)
    {
        if (aircraft->HasViolation || aircraft->hasActiveViolation)
        {
            violatingAircraft.push_back(aircraft);
        }
    }
    
    return violatingAircraft;
}

/**
 * Get aircraft queue by type
 * Returns vector of pointers to aircraft of specified type
 */
std::vector<Aircraft*> FlightsScheduler::getAircraftByType(AirCraftType type) const
{
    // Create vector to hold aircraft of specified type
    std::vector<Aircraft*> filteredAircraft;
    
    // Find aircraft of specified type in all active flights
    for (Aircraft* aircraft : activeFlights)
    {
        if (aircraft->type == type)
        {
            filteredAircraft.push_back(aircraft);
        }
    }
    
    return filteredAircraft;
}

/**
 * Get aircraft by flight state
 * Returns vector of pointers to aircraft in the specified state
 */
std::vector<Aircraft*> FlightsScheduler::getAircraftByState(FlightState state) const
{
    // Create vector to hold aircraft in specified state
    std::vector<Aircraft*> filteredAircraft;
    
    // Find aircraft in specified state in all active flights
    for (Aircraft* aircraft : activeFlights)
    {
        if (aircraft->state == state)
        {
            filteredAircraft.push_back(aircraft);
        }
    }
    
    return filteredAircraft;
}

/**
 * Get aircraft for visualization at specified coordinates
 * Returns aircraft located at the given display coordinates (if any)
 */
Aircraft* FlightsScheduler::getAircraftAtPosition(int x, int y, int radius) const
{
    // This is useful for mouse interaction in SFML
    for (Aircraft* aircraft : activeFlights)
    {
        // Simple distance calculation to check if position is within radius of aircraft
        int dx = aircraft->x_position - x;
        int dy = aircraft->y_position - y;
        int distanceSquared = dx*dx + dy*dy;
        
        // If position is within radius of aircraft, return it
        if (distanceSquared <= radius*radius)
        {
            return aircraft;
        }
    }
    
    // No aircraft found at specified position
    return nullptr;
}

/**
 * Get status summary text for display
 * Returns formatted text with queue statistics
 */
std::string FlightsScheduler::getStatusSummary() const
{
    std::stringstream summary;
    
    // Get statistics
    int* stats = getQueueStatistics();
    
    // Create summary text
    summary << "Active Flights: " << stats[0] << " | ";
    summary << "Arrivals: " << stats[1] << " | ";
    summary << "Departures: " << stats[2];
    
    // Add emergency count if there are any
    if (stats[3] > 0)
    {
        summary << " | Emergencies: " << stats[3];
    }
    
    return summary.str();
}