#include "../include/Aircraft.h"
#include <cstdlib>
#include <algorithm>
#include <ctime>
using namespace std;

Aircraft::Aircraft(int index, const std::string& airlineName, AirCraftType aircraftType) {
    aircraftIndex = index;
    Airline = airlineName;
    type = aircraftType;

    // Construct unique flight number like "PIA"
    FlightNumber = airlineName + "-" + std::to_string(index );

    // Default initializations
    direction = Direction::North;
    state = FlightState::Holding;
    speed = 0;
    EmergencyNo = 0;
    HasViolation = false;
    isFaulty = false;
    isActive = false;
    x_position = -1;
    y_position = -1;
    queueEntryTime = 0;
    hasRunwayAssigned = false;
}

// Destructor for aircraft class
Aircraft::~Aircraft() {
    // No dynamic memory to free
}

// Simulate one step of aircraft movement
void Aircraft::SimulateStep() {
    UpdateSpeed();
    checkForViolation();
    checkForEmergency();
}

// Update aircraft speed based on its current state
void Aircraft::UpdateSpeed() {
    // Speed update logic based on flight state
    switch(state) {
        case FlightState::Holding:
            speed = 400 + rand() % 201; // 400-600 km/h
            break;
        case FlightState::Approach:
            speed = 240 + rand() % 51;  // 240-290 km/h
            break;
        case FlightState::Landing:
            speed = max(30, speed - 20); // Gradually decelerate to 30 km/h
            break;
        case FlightState::Taxi:
            speed = 15 + rand() % 16;    // 15-30 km/h
            break;
        case FlightState::AtGate:
            speed = rand() % 6;          // 0-5 km/h
            break;
        case FlightState::TakeoffRoll:
            speed = min(290, speed + 15); // Accelerate up to 290 km/h
            break;
        case FlightState::Climb:
            speed = 250 + rand() % 214;   // 250-463 km/h
            break;
        case FlightState::Cruise:
            speed = 800 + rand() % 101;   // 800-900 km/h
            break;
    }
}

// Check if aircraft is violating speed limits based on its current state
void Aircraft::checkForViolation() {
    HasViolation = false;
    
    switch(state) {
        case FlightState::Holding:
            if (speed > 600) HasViolation = true;
            break;
        case FlightState::Approach:
            if (speed < 240 || speed > 290) HasViolation = true;
            break;
        case FlightState::Landing:
            if (speed > 240 || (speed < 30 && state == FlightState::Landing)) HasViolation = true;
            break;
        case FlightState::Taxi:
            if (speed > 30) HasViolation = true;
            break;
        case FlightState::AtGate:
            if (speed > 10) HasViolation = true;
            break;
        case FlightState::TakeoffRoll:
            if (speed > 290) HasViolation = true;
            break;
        case FlightState::Climb:
            if (speed > 463) HasViolation = true;
            break;
        case FlightState::Cruise:
            if (speed < 800 || speed > 900) HasViolation = true;
            break;
    }
}

// Randomly generate emergency based on flight direction
void Aircraft::checkForEmergency() {
    int chance = rand() % 100;
    
    if (EmergencyNo > 0) return; // Already in emergency
    
    switch(direction) {
        case Direction::North: // International Arrivals - 10%
            if (chance < 10) EmergencyNo = 1 + rand() % 3; // Random emergency 1-3
            break;
        case Direction::South: // Domestic Arrivals - 5%
            if (chance < 5) EmergencyNo = 1 + rand() % 3;
            break;
        case Direction::East: // International Departures - 15%
            if (chance < 15) EmergencyNo = 1 + rand() % 3;
            break;
        case Direction::West: // Domestic Departures - 20%
            if (chance < 20) EmergencyNo = 1 + rand() % 3;
            break;
    }
}

// Check if aircraft is in an airborne state
bool Aircraft::isinAir() {
    return (state == FlightState::Holding || state == FlightState::Approach || 
            state == FlightState::Climb || state == FlightState::Cruise);
}

// Check if aircraft is on the ground
bool Aircraft::isOnGround() {
    return (state == FlightState::Taxi || state == FlightState::AtGate);
}

// Check if aircraft is ready for takeoff
bool Aircraft::isReadyForTakeOff() {
    return (state == FlightState::AtGate);
}

// Request runway assignment for this aircraft
void Aircraft::RequestRunawayAssignment() {
    // Implementation to be added in Module 2
}

// Assign a runway to this aircraft
void Aircraft::AssignRunaway() {
    // Implementation to be added in Module 2
}

// Calculate priority score for flight scheduling
// Higher score = higher priority (easier to understand this way)
int Aircraft::calculatePriorityScore() const {
    int score = 0;
    
    // ================ PRIORITY FACTORS ================
    // As per FR3.3 and FR4.1, priority is based on:
    // 1. Emergency status (highest priority)
    // 2. Aircraft type 
    // 3. Wait time (for FCFS within priority)
    
    // 1. Emergency priority - The big cheese of priorities!
    // Emergency flights get the VIP express lane treatment
    if (EmergencyNo > 0) {
        score += 10000 * EmergencyNo;  // Emergency level multiplier
        // With this big number, emergencies will always be first
        // Unless you have like 100 aircraft waiting for 100000 seconds... which would be bad.
    }
    
    // 2. Aircraft type priority - Not all planes are created equal
    switch (type) {
        case AirCraftType::Medical:
            score += 1000;  // Medical flights (second highest priority)
            break;
        case AirCraftType::Military:
            score += 800;   // Military operations
            break;
        case AirCraftType::Cargo:
            score += 600;   // Cargo flights
            break;
        case AirCraftType::Commercial:
        default:
            score += 400;   // Regular commercial flights
            break;
    }
    
    // 3. Wait time factor - The longer you wait, the angrier you get
    // This implements FCFS within same priority class (FR5.1)
    if (queueEntryTime > 0) {
        time_t currentTime = time(NULL);
        int waitTime = static_cast<int>(currentTime - queueEntryTime);
        score += waitTime;  // 1 point per second of waiting
    }
    
    return score;
}