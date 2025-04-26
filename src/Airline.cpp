#include "../include/Airline.h"
using namespace std;

// Constructor initializes airline with default values
Airline::Airline() {
    name = "";
    type = AirCraftType::Commercial;
    activeFlights = 0;
    maxFlights = 0;
}

// Check if airline can schedule more flights
bool Airline::canScheduleFlight() {
    return activeFlights < maxFlights;
}

// Get an available aircraft for scheduling
Aircraft* Airline::getAvailableAircraft() {
    for (auto& aircraft : aircrafts) {
        if (!aircraft.isActive) {
            return &aircraft;
        }
    }
    return nullptr; // No available aircraft
}