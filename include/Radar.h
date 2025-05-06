#ifndef AIRCONTROLX_RADAR_H
#define AIRCONTROLX_RADAR_H

#include "Aircraft.h"
#include <vector>
#include <string>

/**
 * Radar class for monitoring aircraft speed and detecting violations.
 * Used to track aircraft and report violations to the ATC system.
 */
class Radar 
{
public:
    int range;  // Radar range in kilometers
    
    // Struct for violation information including aircraft and speed limits
    struct ViolationInfo 
    {
        Aircraft* aircraft;
        int minAllowed;
        int maxAllowed;
    };
    
    // Constructor
    Radar();
    
    // Set the pipe for AVN Generator communication
    void setAVNPipe(int pipeWriteEnd);
    
    // Monitor a single aircraft and detect violations
    // Returns 1 if violation detected, 0 otherwise
    int monitorAirCraft(Aircraft& plane);
    
    // Monitor all active flights for violations
    // Returns vector of aircraft pointers that have violations with their allowed speed ranges
    std::vector<ViolationInfo> detectViolations(const std::vector<Aircraft*>& aircraft);
    
    // Handle violations by sending them to the AVN Generator
    void handleViolations(const std::vector<Aircraft*>& activeFlights);
    
    // Get speed limits for a given flight state
    // Returns min and max allowed speed as a pair
    std::pair<int, int> getSpeedLimits(FlightState state);
    
    // Check if aircraft speed violates limits
    // Returns true if violation detected, false otherwise
    bool checkSpeedViolation(const Aircraft& aircraft, int minAllowed, int maxAllowed);
    
    // Simulate a violation for testing purposes
    void simulateViolation(const std::string& flightNumber, const std::string& airline, 
                          int speed, int minAllowed, int maxAllowed);
    
    // Alias for simulateViolation to maintain compatibility with ATCScontroller
    void reportViolation(const std::string& flightNumber, const std::string& airline, 
                        int speed, int minAllowed, int maxAllowed);
    
private:
    int avnPipeWriteEnd; // File descriptor for pipe to AVN Generator process
    
    // Process and send a single violation to the AVN Generator
    void processViolation(Aircraft* aircraft, int minAllowed, int maxAllowed);
};

#endif // AIRCONTROLX_RADAR_H