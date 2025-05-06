#ifndef AIRCONTROLX_RUNWAYMANAGER_H
#define AIRCONTROLX_RUNWAYMANAGER_H

#include <vector>
#include <string>
#include "Runway.h"

/**
 * The RunwayManager class handles all runway-related operations.
 * It manages runway initialization, status tracking, and allocation.
 */
class RunwayManager 
{
private:
    // Collection of runways in the system
    std::vector<RunwayClass> runways;

public:
    // Constructor and destructor
    RunwayManager();
    ~RunwayManager();
    
    // Initialize runways with their configurations
    void initialize();
    
    // Get a runway by index (0-based)
    RunwayClass* getRunwayByIndex(int index);
    
    // Get total number of runways
    int getRunwayCount() const;
    
    // Get formatted runway status report
    std::string getStatusReport() const;
    
    // Check if a specific runway is available
    bool isRunwayAvailable(const std::string& runwayId);
    
    // Get a runway by its ID
    RunwayClass* getRunway(const std::string& runwayId);
    
    // ======== SFML Visualization Abstraction Functions ========
    
    /**
     * Get all available runways
     * Returns vector of pointers to available runways for visualization
     */
    std::vector<RunwayClass*> getAvailableRunways() const;
    
    /**
     * Get all occupied runways
     * Returns vector of pointers to occupied runways for visualization
     */
    std::vector<RunwayClass*> getOccupiedRunways() const;
    
    /**
     * Get runway usage statistics for visualization
     * Returns array of [total, available, occupied] counts
     */
    int* getRunwayStatistics() const;
    
    /**
     * Get runway availability as percentage
     * Returns percentage of available runways (0-100)
     */
    float getAvailabilityPercentage() const;
    
    /**
     * Get status text for SFML display
     * Returns formatted text summary of runway statuses
     */
    std::string getStatusSummary() const;
};

#endif // AIRCONTROLX_RUNWAYMANAGER_H