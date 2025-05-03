#include "../include/RunwayManager.h"
#include <sstream>

/**
 * Constructor initializes an empty vector for runways
 */
RunwayManager::RunwayManager() 
{
    // Nothing to initialize here - runways will be created in initialize()
}

/**
 * Destructor handles cleanup
 */
RunwayManager::~RunwayManager() 
{
    // No dynamic memory to clean up
}

/**
 * Initialize runways with predefined configurations
 */
void RunwayManager::initialize() 
{
    // Clear any existing runways
    runways.clear();
    
    // Create the three standard runways
    
    // RWY-A: North-South alignment (arrivals)    
    runways.emplace_back("RWY-A", RunwayType::Arrival, false);

    // RWY-B: East-West alignment (departures)
    runways.emplace_back("RWY-B", RunwayType::Departure, false);

    // RWY-C: Flexible for cargo/emergency/overflow
    runways.emplace_back("RWY-C", RunwayType::Flexible, false);

}

/**
 * Get a runway by its index
 */
RunwayClass* RunwayManager::getRunwayByIndex(int index) 
{
    if (index >= 0 && index < static_cast<int>(runways.size())) {
        return &runways[index];
    }
    return nullptr; // Invalid index
}

/**
 * Get the total number of runways
 */
int RunwayManager::getRunwayCount() const 
{
    return static_cast<int>(runways.size());
}

/**
 * Generate a formatted status report of all runways
 */
std::string RunwayManager::getStatusReport() const 
{
    std::stringstream report;
    report << "--- RUNWAY STATUS UPDATE ---\n";
    
    for (size_t i = 0; i < runways.size(); ++i) {
        report << runways[i].id << ": " << (runways[i].isOccupied ? "OCCUPIED" : "AVAILABLE") << "\n";
    }
    
    report << "---------------------------";
    return report.str();
}

/**
 * Check if a specific runway is available
 * @param runwayId The ID of the runway to check (e.g., "RWY-A")
 * @return True if the runway exists and is not occupied, false otherwise
 */
bool RunwayManager::isRunwayAvailable(const std::string& runwayId) 
{
    // Search for the runway with the given ID
    for (size_t i = 0; i < runways.size(); i++) 
    {
        if (runways[i].id == runwayId) 
        {
            // Found the runway, return its availability status (opposite of occupied)
            return !runways[i].isOccupied;
        }
    }
    
    // If we got here, the runway ID wasn't found - that's an error!
    // We could throw an exception here, but let's just return false for now
    // since a non-existent runway isn't available anyway
    return false;
}

/**
 * Get a runway by its ID
 * @param runwayId The ID of the runway to get (e.g., "RWY-A")
 * @return Pointer to the runway, or nullptr if not found
 */
RunwayClass* RunwayManager::getRunway(const std::string& runwayId) 
{
    // Search for the runway with the given ID
    for (size_t i = 0; i < runways.size(); i++) 
    {
        if (runways[i].id == runwayId) 
        {
            // Found it! Return a pointer to this runway
            // Don't worry about the & - it's just getting a pointer to an element in the vector
            return &runways[i];
        }
    }
    
    // Runway not found - return nullptr
    // This will trigger the safety checks in assignRunway()
    return nullptr;
}