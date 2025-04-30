#include "../include/FlightsScheduler.h"
#include <algorithm>
#include <ctime>

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

// Find any emergency flight in either queue (highest priority of all)
Aircraft* FlightsScheduler::getNextEmergency()
{
    Aircraft* emergencyAircraft = nullptr;
    int highestPriority = 0;
    
    // First check arrivals for emergencies
    arrivalMutex.lock();
    for (size_t i = 0; i < arrivalQueue.size(); i++)
    {
        // Check if this is an emergency flight
        if (arrivalQueue[i]->EmergencyNo > 0)
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
            // Check if this is an emergency flight
            if (departureQueue[i]->EmergencyNo > 0)
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