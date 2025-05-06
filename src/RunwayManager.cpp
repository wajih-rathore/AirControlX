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

// ======== SFML Visualization Abstraction Functions ========

/**
 * Get all available runways
 * Returns vector of pointers to available runways for visualization
 */
std::vector<RunwayClass*> RunwayManager::getAvailableRunways() const
{
    // This function helps SFML visualizer quickly access available runways
    std::vector<RunwayClass*> availableRunways;
    
    for (size_t i = 0; i < runways.size(); i++)
    {
        // If the runway is not occupied, add a pointer to it in our result vector
        if (!runways[i].isOccupied)
        {
            // The const_cast is safe here because we're not modifying the runway
            // It's just needed because the function is const but returns non-const pointers
            availableRunways.push_back(const_cast<RunwayClass*>(&runways[i]));
        }
    }
    
    return availableRunways;
}

/**
 * Get all occupied runways
 * Returns vector of pointers to occupied runways for visualization
 */
std::vector<RunwayClass*> RunwayManager::getOccupiedRunways() const
{
    // Similar to getAvailableRunways, but for occupied ones
    std::vector<RunwayClass*> occupiedRunways;
    
    for (size_t i = 0; i < runways.size(); i++)
    {
        // If the runway is occupied, add a pointer to it in our result vector
        if (runways[i].isOccupied)
        {
            // Same const_cast reasoning as above
            occupiedRunways.push_back(const_cast<RunwayClass*>(&runways[i]));
        }
    }
    
    return occupiedRunways;
}

/**
 * Get runway usage statistics for visualization
 * Returns array of [total, available, occupied] counts
 */
int* RunwayManager::getRunwayStatistics() const
{
    // This is a handy helper for UI components that need runway stats
    // Using static array to avoid memory leaks since we're returning a pointer
    static int stats[3];
    
    // Count occupied runways
    int occupied = 0;
    for (const auto& runway : runways)
    {
        if (runway.isOccupied) occupied++;
    }
    
    // Fill our statistics array
    stats[0] = static_cast<int>(runways.size());  // Total runways
    stats[1] = static_cast<int>(runways.size()) - occupied;  // Available runways
    stats[2] = occupied;  // Occupied runways
    
    return stats;
}

/**
 * Get runway availability as percentage
 * Returns percentage of available runways (0-100)
 */
float RunwayManager::getAvailabilityPercentage() const
{
    // Avoid division by zero if there are no runways
    if (runways.empty()) return 0.0f;
    
    // Count available runways
    int available = 0;
    for (const auto& runway : runways)
    {
        if (!runway.isOccupied) available++;
    }
    
    // Calculate and return percentage
    return (static_cast<float>(available) / static_cast<float>(runways.size())) * 100.0f;
}

/**
 * Get status text for SFML display
 * Returns formatted text summary of runway statuses
 */
std::string RunwayManager::getStatusSummary() const
{
    // Create a concise summary for SFML status display
    std::stringstream summary;
    
    // Add header
    summary << "Runway Status: ";
    
    // Calculate statistics
    int* stats = getRunwayStatistics();
    
    // Add percentage and counts
    float availPercent = getAvailabilityPercentage();
    summary << static_cast<int>(availPercent) << "% Available (";
    summary << stats[1] << "/" << stats[0] << ")";
    
    return summary.str();
}