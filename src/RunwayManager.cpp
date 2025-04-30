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
    RunwayClass rwyA;
    rwyA.id = "RWY-A";
    rwyA.type = RunwayType::Arrival;
    runways.push_back(rwyA);
    
    // RWY-B: East-West alignment (departures)
    RunwayClass rwyB;
    rwyB.id = "RWY-B";
    rwyB.type = RunwayType::Departure;
    runways.push_back(rwyB);
    
    // RWY-C: Flexible for cargo/emergency/overflow
    RunwayClass rwyC;
    rwyC.id = "RWY-C";
    rwyC.type = RunwayType::Flexible;
    runways.push_back(rwyC);
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