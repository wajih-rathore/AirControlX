#ifndef AIRCONTROLX_AIRCRAFT_H
#define AIRCONTROLX_AIRCRAFT_H

#include <string>
#include "Common.h"
#include "Timer.h"

/**
 * Aircraft class representing a flight in the AirControlX system.
 * Handles individual flight operations, state transitions, and rule compliance monitoring.
 */
class Aircraft {
public:
    std::string FlightNumber;  // Unique identifier for the aircraft
    std::string Airline;       // Airline operating the aircraft (PIA, AirBlue, etc.)
    AirCraftType type;         // Type of aircraft (Commercial, Cargo, etc.)
    Direction direction;       // Direction of flight (North, South, East, West)
    FlightState state;         // Current flight state/phase
    Timer timer;               // Timer for tracking flight events
    int speed;                 // Current speed in km/h
    int EmergencyNo;           // Emergency level (0 for no emergency)
    pair<int, int> position;   //The Current (x, y) position for the plane 
    bool HasViolation;         // Flag for speed/rule violations
    bool isFaulty;             // Flag for faulty aircraft
    bool isActive;             // Flag for active flight

    // Constructor
    Aircraft();
    
    // Destructor
    ~Aircraft();
    
    // Simulate one step of aircraft movement
    void SimulateStep();
    
    // Update aircraft speed based on current state
    void UpdateSpeed();
    
    // Check for speed limit violations
    void checkForViolation();
    
    // Generate random emergencies based on direction
    void checkForEmergency();
    
    // Check if aircraft is airborne
    bool isinAir();
    
    // Check if aircraft is on the ground
    bool isOnGround();
    
    // Check if aircraft is ready for takeoff
    bool isReadyForTakeOff();
    
    // Request runway assignment
    void RequestRunawayAssignment();
    
    // Assign runway to aircraft
    void AssignRunaway();
};

#endif // AIRCONTROLX_AIRCRAFT_H