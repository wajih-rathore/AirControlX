#include "../include/Radar.h"

// Constructor initializes radar with default range
Radar::Radar() {
    range = 100; // Default radar range in km
}

// Monitor aircraft and detect violations
int Radar::monitorAirCraft(Aircraft& plane) {
    if (plane.HasViolation) {
        return 1; // Return 1 if violation detected
    }
    return 0;
}