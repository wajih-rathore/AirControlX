#include "../include/Runway.h"
using namespace std;

// Constructor initializes runway properties
RunwayClass::RunwayClass() {
    id = "";
    type = RunwayType::Arrival;
    isOccupied = false;
    turn = 0;
}

RunwayClass::RunwayClass(string run, RunwayType Inputtype, bool occupied ) {
    id = run;
    type = Inputtype;
    isOccupied = occupied;
    turn = 0;
}

// Try to assign runway to an aircraft based on compatibility
bool RunwayClass::tryAssign(Aircraft &plane) {
    if (isOccupied) return false;
    
    // Assign runway based on direction
    switch(plane.direction) {
        case Direction::North:
        case Direction::South:
            if (type == RunwayType::Arrival || type == RunwayType::Flexible) {
                isOccupied = true;
                return true;
            }
            break;
        case Direction::East:
        case Direction::West:
            if (type == RunwayType::Departure || type == RunwayType::Flexible) {
                isOccupied = true;
                return true;
            }
            break;
    }
    
    // Special case for cargo
    if (plane.type == AirCraftType::Cargo && type == RunwayType::Flexible) {
        isOccupied = true;
        return true;
    }
    
    return false;
}

// Release the runway after use
void RunwayClass::release() {
    isOccupied = false;
    turn++;
}

// ======== SFML Visualization Abstraction Functions ========

/**
 * Get the asset filename for this runway type
 * Returns the appropriate texture filename for SFML rendering
 */
std::string RunwayClass::getAssetName() const
{
    // Return the appropriate asset name based on runway ID
    if (id == "RWY-A") return "RunwayA.png";
    if (id == "RWY-B") return "RunwayB.png";
    if (id == "RWY-C") return "RunwayC.png";
    
    // Default case - should not happen with proper initialization
    return "RunwayA.png";
}

/**
 * Get runway status text for display
 * Returns formatted text with runway ID and status
 */
std::string RunwayClass::getStatusText() const
{
    std::string status = id + " (";
    
    // Add type information
    status += getTypeString();
    
    // Add occupancy status
    status += isOccupied ? ", OCCUPIED)" : ", AVAILABLE)";
    
    return status;
}

/**
 * Get runway highlight color based on status
 * Returns RGB values as an array based on runway status
 */
int* RunwayClass::getStatusColor() const
{
    // Using static array to avoid memory leaks
    static int color[3];
    
    if (isOccupied) {
        // Red for occupied runway
        color[0] = 255; // R
        color[1] = 0;   // G
        color[2] = 0;   // B
    } else {
        // Green for available runway
        color[0] = 0;   // R
        color[1] = 255; // G
        color[2] = 0;   // B
    }
    
    return color;
}

/**
 * Get runway display position (relative coordinates)
 * Returns x,y coordinates in the range of 0.0-1.0 for positioning
 */
float* RunwayClass::getDisplayPosition() const
{
    // Using static array to avoid memory leaks
    static float position[2];
    
    // Set default position in middle of screen
    position[0] = 0.5f; // x - centered horizontally
    position[1] = 0.5f; // y - centered vertically
    
    // Adjust position based on runway ID
    if (id == "RWY-A") {
        // RWY-A in left side of display
        position[0] = 0.25f;
        position[1] = 0.4f;
    } else if (id == "RWY-B") {
        // RWY-B in middle of display
        position[0] = 0.5f;
        position[1] = 0.6f;
    } else if (id == "RWY-C") {
        // RWY-C in right side of display
        position[0] = 0.75f;
        position[1] = 0.4f;
    }
    
    return position;
}

/**
 * Get runway type as string
 * Returns a human-readable version of the runway type
 */
std::string RunwayClass::getTypeString() const
{
    switch (type) {
        case RunwayType::Arrival: return "Arrivals";
        case RunwayType::Departure: return "Departures";
        case RunwayType::Flexible: return "Flex";
        default: return "Unknown";
    }
}