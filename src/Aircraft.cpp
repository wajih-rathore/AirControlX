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
    assignedRunwayIndex = 0; // No runway assigned initially
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
bool Aircraft::isinAir() const {
    return (state == FlightState::Holding || state == FlightState::Approach || 
            state == FlightState::Climb || state == FlightState::Cruise);
}

// Check if aircraft is on the ground
bool Aircraft::isOnGround() const
{
    return (state == FlightState::Taxi || state == FlightState::AtGate);
}

// Check if aircraft is ready for takeoff
bool Aircraft::isReadyForTakeOff() const
{
    return (state == FlightState::AtGate);
}

// Request runway assignment for this aircraft
void Aircraft::RequestRunawayAssignment() {



}

// Assign a runway to this aircraft
void Aircraft::AssignRunaway() {
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

//Have to determine the appropiate spot to call this function
void Aircraft::updatePosition() {
    // Update the position of the aircraft based on its speed and direction
    // This is a placeholder implementation and should be replaced with actual logic
    if (direction == Direction::North) {
        y_position += speed / 60;  // Move North
    } else if (direction == Direction::South) {
        y_position -= speed / 60;  // Move South
    } else if (direction == Direction::East) {
        x_position += speed / 60;  // Move East
    } else if (direction == Direction::West) {
        x_position -= speed / 60;  // Move West
    }
}

// Get current state name as string (for UI display)
std::string Aircraft::getStateName() const {
    switch (state) {
        case FlightState::Holding: return "Holding";
        case FlightState::Approach: return "Approaching";
        case FlightState::Landing: return "Landing";
        case FlightState::Taxi: return "Taxiing";
        case FlightState::AtGate: return "At Gate";
        case FlightState::TakeoffRoll: return "Taking Off";
        case FlightState::Climb: return "Climbing";
        case FlightState::Cruise: return "Cruising";
        default: return "Unknown";
    }
}

// ======== SFML Visualization Abstraction Functions ========

/**
 * Get aircraft display name for SFML UI
 * Returns a formatted string with flight number and current state
 */
std::string Aircraft::getDisplayName() const 
{
    // Format: "PIA-1: Taxiing" or "PIA-1 [EMERG]: Landing"
    std::string displayName = FlightNumber;
    
    // Add emergency indicator if applicable
    if (EmergencyNo > 0) {
        displayName += " [EMERG]";
    }
    
    // Add violation indicator if applicable
    if (HasViolation || hasActiveViolation) {
        displayName += " [VIOL]";
    }
    
    // Add state
    displayName += ": " + getStateName();
    
    return displayName;
}

/**
 * Get aircraft status for SFML rendering
 * Returns a formatted status string with additional information
 */
std::string Aircraft::getStatusText() const 
{
    // Start with the speed
    std::string status = "Speed: " + std::to_string(speed) + " km/h";
    
    // Add runway assignment status if applicable
    if (hasRunwayAssigned) {
        status += " | Runway Assigned";
    }
    
    return status;
}

/**
 * Get appropriate rotation angle for aircraft sprite
 * Maps the aircraft direction to degrees for SFML sprite rotation
 */
float Aircraft::getRotationAngle() const 
{
    // Map direction to rotation angle
    // Note: SFML uses counterclockwise rotation with 0 pointing right
    switch (direction) {
        case Direction::North: return 270.0f;  // Up
        case Direction::South: return 90.0f;   // Down
        case Direction::East: return 0.0f;     // Right
        case Direction::West: return 180.0f;   // Left
        default: return 0.0f;
    }
}

/**
 * Get appropriate asset name for aircraft based on type
 * Returns the appropriate texture filename to use for this aircraft
 */
std::string Aircraft::getAssetName() const 
{
    // Return the appropriate asset filename based on aircraft type
    switch (type) {
        case AirCraftType::Commercial: return "Commercial_Whole.png";
        case AirCraftType::Cargo: return "Cargo_Whole.png";
        case AirCraftType::Military: return "Military_Whole.png";
        case AirCraftType::Emergency: 
        case AirCraftType::Medical:
            // For emergency/medical aircraft, use commercial with a different color
            return "Commercial_Whole.png";
        default: return "Commercial_Whole.png";
    }
}

/**
 * Get the appropriate color for aircraft status in UI
 * Returns RGB values as an array based on aircraft state/emergency
 */
int* Aircraft::getStatusColor() const 
{
    // Allocate a static array to avoid memory leaks
    static int color[3];
    
    // Emergency aircraft are always red
    if (EmergencyNo > 0) {
        color[0] = 255; // R
        color[1] = 0;   // G
        color[2] = 0;   // B
        return color;
    }
    
    // Violation aircraft are yellow
    if (HasViolation || hasActiveViolation) {
        color[0] = 255; // R
        color[1] = 255; // G
        color[2] = 0;   // B
        return color;
    }
    
    // State-specific colors
    switch (state) {
        case FlightState::Landing:
        case FlightState::Approach:
            // Green for landing/approaching
            color[0] = 0;   // R
            color[1] = 255; // G
            color[2] = 128; // B
            break;
        case FlightState::Taxi:
        case FlightState::AtGate:
            // Yellow for ground operations
            color[0] = 255; // R
            color[1] = 255; // G
            color[2] = 0;   // B
            break;
        case FlightState::TakeoffRoll:
        case FlightState::Climb:
        case FlightState::Cruise:
            // Blue for departing
            color[0] = 0;   // R
            color[1] = 191; // G
            color[2] = 255; // B
            break;
        default:
            // White for other states
            color[0] = 255; // R
            color[1] = 255; // G
            color[2] = 255; // B
    }
    
    return color;
}

/**
 * Check if aircraft should be animated in the current frame
 * Helps control animation timing for smooth SFML rendering
 */
bool Aircraft::shouldAnimate(float deltaTime) const 
{
    // This is just a simple implementation
    // For more complex animations, you might use timer values
    
    // Ensure we have a positive delta time
    if (deltaTime <= 0) return false;
    
    // Always animate emergency aircraft for visual emphasis
    if (EmergencyNo > 0) return true;
    
    // Animate based on state - airborne planes animate more frequently
    if (isinAir()) {
        // Animate airborne aircraft every frame
        return true;
    } else {
        // Animate ground aircraft less frequently
        // This simply returns true approximately every 0.5 seconds
        return (static_cast<int>(deltaTime * 2) % 2) == 0;
    }
}

/**
 * Get the priority display level (z-index equivalent)
 * Higher values should be displayed on top of others
 */
int Aircraft::getDisplayPriority() const 
{
    // Emergency aircraft always on top
    if (EmergencyNo > 0) return 100;
    
    // Violation aircraft next
    if (HasViolation || hasActiveViolation) return 90;
    
    // Airborne aircraft next
    if (isinAir()) return 80;
    
    // Active aircraft with runway assigned
    if (isActive && hasRunwayAssigned) return 70;
    
    // Other active aircraft
    if (isActive) return 60;
    
    // All other aircraft
    return 50;
}

/**
 * Get a string representation of aircraft type
 * Useful for SFML text displays
 */
std::string Aircraft::getTypeString() const 
{
    switch (type) {
        case AirCraftType::Commercial: return "Commercial";
        case AirCraftType::Cargo: return "Cargo";
        case AirCraftType::Military: return "Military";
        case AirCraftType::Emergency: return "Emergency";
        case AirCraftType::Medical: return "Medical";
        default: return "Unknown";
    }
}