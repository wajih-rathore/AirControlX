#include "../include/Runway.h"
using namespace std;

// Constructor initializes runway properties
RunwayClass::RunwayClass() {
    id = "";
    type = RunwayType::Arrival;
    isOccupied = false;
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