#ifndef AIRCONTROLX_RUNWAY_H
#define AIRCONTROLX_RUNWAY_H

#include <string>
#include "Common.h"
#include "Aircraft.h"

/**
 * RunwayClass handles runway operations at the airport.
 * Manages runway availability, assignment to aircraft, and tracking usage.
 */
class RunwayClass {
public:
    std::string id;        // Runway identifier (e.g., RWY-A)
    RunwayType type;       // Type of runway (Arrival, Departure, Flexible)
    bool isOccupied;       // Flag for runway occupancy status
    int turn;              // Variable for synchronization between aircraft

    // Constructor
    RunwayClass();
    
    //Constructor with Arguments
    RunwayClass(std::string run, RunwayType Inputtype, bool occupied);

    // Try to assign runway to aircraft
    bool tryAssign(Aircraft &plane);
    
    // Release the runway after use
    void release();

    // ======== SFML Visualization Abstraction Functions ========
    
    /**
     * Get the asset filename for this runway type
     * Returns the appropriate texture filename for SFML rendering
     */
    std::string getAssetName() const;
    
    /**
     * Get runway status text for display
     * Returns formatted text with runway ID and status
     */
    std::string getStatusText() const;
    
    /**
     * Get runway highlight color based on status
     * Returns RGB values as an array based on runway status
     */
    int* getStatusColor() const;
    
    /**
     * Get runway display position (relative coordinates)
     * Returns x,y coordinates in the range of 0.0-1.0 for positioning
     */
    float* getDisplayPosition() const;
    
    /**
     * Get runway type as string
     * Returns a human-readable version of the runway type
     */
    std::string getTypeString() const;
};

#endif // AIRCONTROLX_RUNWAY_H