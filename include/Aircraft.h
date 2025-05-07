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
    int assignedRunwayIndex;   // Index of the assigned runway (0, 1, 2)

    // Constructor
    Aircraft(int index, const std::string& airlineName, AirCraftType aircraftType);

    // Destructor
    ~Aircraft();
    
    void cleanAircraft();

    // Simulate one step of aircraft movement
    void SimulateStep();
    
    // Update aircraft speed based on current state
    void UpdateSpeed();
    
    // Check for speed limit violations
    void checkForViolation();
    
    // Generate random emergencies based on direction
    void checkForEmergency();
    
    // Check if aircraft is airborne
    bool isinAir() const;
    
    int calculatePriorityScore() const;
    // Check if aircraft is on the ground
    bool isOnGround() const;
    
    // Check if aircraft is ready for takeoff
    bool isReadyForTakeOff() const;
    
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

    // ======== SFML Visualization Abstraction Functions ========
    
    /**
     * Get aircraft display name for SFML UI
     * Returns a formatted string with flight number and current state
     */
    std::string getDisplayName() const;
    
    /**
     * Get aircraft status for SFML rendering
     * Returns a formatted status string with additional information
     */
    std::string getStatusText() const;
    
    /**
     * Get appropriate rotation angle for aircraft sprite
     * Maps the aircraft direction to degrees for SFML sprite rotation
     */
    float getRotationAngle() const;
    
    /**
     * Get appropriate asset name for aircraft based on type
     * Returns the appropriate texture filename to use for this aircraft
     */
    std::string getAssetName() const;
    
    /**
     * Get the appropriate color for aircraft status in UI
     * Returns RGB values as an array based on aircraft state/emergency
     */
    int* getStatusColor() const;
    
    /**
     * Check if aircraft should be animated in the current frame
     * Helps control animation timing for smooth SFML rendering
     */
    bool shouldAnimate(float deltaTime) const;
    
    /**
     * Get the priority display level (z-index equivalent)
     * Higher values should be displayed on top of others
     */
    int getDisplayPriority() const;
    
    /**
     * Get a string representation of aircraft type
     * Useful for SFML text displays
     */
    std::string getTypeString() const;
    
    // ======== Testing Functions ========
    
    /**
     * Enable or disable high probability emergency testing mode
     * Use this function to demonstrate emergency aircraft handling
     * 
     * @param enable - Set to true to increase emergency probability to 80%, false for normal probability
     */
    static void setEmergencyTestingMode(bool enable);
    
    /**
     * Check if emergency testing mode is currently enabled
     * 
     * @return true if testing mode is enabled, false otherwise
     */
    static bool isEmergencyTestingEnabled();
};

#endif // AIRCONTROLX_AIRCRAFT_H