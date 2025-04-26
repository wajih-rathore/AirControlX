#ifndef AIRCONTROLX_COMMON_H
#define AIRCONTROLX_COMMON_H

#include <string>

// Enumeration for different aircraft types
enum class AirCraftType {Commercial, Cargo, Emergency, Military, Medical};

// Enumeration for different flight states/phases
enum class FlightState {Holding, Approach, Landing, Taxi, AtGate, TakeoffRoll, Climb, Cruise};

// Enumeration for different runway types
enum class RunwayType {Arrival, Departure, Flexible};

// Enumeration for flight directions
enum class Direction {North, South, East, West};

#endif // AIRCONTROLX_COMMON_H