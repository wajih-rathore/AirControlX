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
    int x_position, y_position;   // The Current (x, y) position for the plane 
    bool HasViolation;         // Flag for speed/rule violations
    bool hasActiveViolation;   // Flag for current active violation with AVN
    bool isFaulty;             // Flag for faulty aircraft
    bool isActive;             // Flag for active flight
    int aircraftIndex;         // Tells which index the aircraft belongs to in AirLine
    time_t queueEntryTime;     // When the aircraft entered the queue (for FCFS)
    bool hasRunwayAssigned;    // Whether a runway has been assigned

    // Constructor
    Aircraft(int index, const std::string& airlineName, AirCraftType aircraftType);

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
    
    int calculatePriorityScore() const;
 // Check if aircraft is on the ground
    bool isOnGround();
    
    // Check if aircraft is ready for takeoff
    bool isReadyForTakeOff();
    
    // Request runway assignment
    void RequestRunawayAssignment();
    
    // Assign runway to aircraft
    void AssignRunaway();

    //Update the position of the aircraft
    void updatePosition();
    
    // Clear active violation after payment
    void clearActiveViolation();
    
    // Get current state name as string (for UI display)
    std::string getStateName() const;
};

#endif // AIRCONTROLX_AIRCRAFT_H