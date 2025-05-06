#include "../include/Radar.h"
#include "../include/AVNGenerator.h" // Include AVNGenerator header for struct definitions
#include <iostream>
#include <unistd.h> // For write()
#include <cstring> // For strncpy

// Constructor initializes radar with default range
Radar::Radar() 
{
    range = 100; // Default radar range in km
    avnPipeWriteEnd = -1; // Initialize pipe file descriptor to invalid value
}

// Set the pipe for AVN Generator communication
void Radar::setAVNPipe(int pipeWriteEnd)
{
    avnPipeWriteEnd = pipeWriteEnd;
    
    // Double check that the pipe is valid
    if (avnPipeWriteEnd <= 0)
    {
        std::cerr << "WARNING: Invalid AVN pipe file descriptor provided to Radar!" << std::endl;
    }
    else 
    {
        std::cout << "AVN pipe set successfully in Radar" << std::endl;
    }
}

// Monitor aircraft and detect violations
int Radar::monitorAirCraft(Aircraft& plane) 
{
    // Get speed limits for the current state
    auto limits = getSpeedLimits(plane.state);
    int minAllowed = limits.first;
    int maxAllowed = limits.second;
    
    // Check if there's a violation
    if (checkSpeedViolation(plane, minAllowed, maxAllowed))
    {
        // If we detect a violation, mark the aircraft
        plane.HasViolation = true;
        return 1; // Return 1 to indicate violation detected
    }
    
    return 0; // No violation
}

// Monitor a collection of aircraft for violations
std::vector<Radar::ViolationInfo> Radar::detectViolations(const std::vector<Aircraft*>& aircraft) 
{
    // This vector will hold all aircraft with violations
    // along with their allowed speed limits
    std::vector<ViolationInfo> violations;
    
    // Loop through all active flights and check for speed violations
    for (Aircraft* plane : aircraft)
    {
        // Skip aircraft that already have active violations
        if (plane->hasActiveViolation)
        {
            continue;
        }
        
        // Get the speed limits for this aircraft's state
        auto limits = getSpeedLimits(plane->state);
        int minAllowed = limits.first;
        int maxAllowed = limits.second;
        
        // Check if there's a violation
        if (checkSpeedViolation(*plane, minAllowed, maxAllowed))
        {
            // Add this aircraft to our violations list
            ViolationInfo info;
            info.aircraft = plane;
            info.minAllowed = minAllowed;
            info.maxAllowed = maxAllowed;
            violations.push_back(info);
        }
    }
    
    return violations;
}

// Process and send a single violation to the AVN Generator
void Radar::processViolation(Aircraft* aircraft, int minAllowed, int maxAllowed)
{
    std::cout << "VIOLATION DETECTED: " << aircraft->FlightNumber 
         << " (" << aircraft->Airline << ") - Speed: " << aircraft->speed 
         << " km/h (Allowed: " << minAllowed << "-" << maxAllowed << " km/h)" << std::endl;
         
    // Mark aircraft as having active violation
    aircraft->hasActiveViolation = true;
    
    // Check if we have a pipe to communicate with AVN Generator
    if (avnPipeWriteEnd <= 0)
    {
        // No pipe set up - can't send violations
        std::cout << "Radar: No AVN pipe set up - can't send violation" << std::endl;
        return;
    }
    
    // Create violation data to send through pipe
    ViolationData violation;
    
    // Copy aircraft data to the struct with proper string handling
    std::strncpy(violation.flightNumber, aircraft->FlightNumber.c_str(), sizeof(violation.flightNumber) - 1);
    violation.flightNumber[sizeof(violation.flightNumber) - 1] = '\0';  // Ensure null-termination
    
    std::strncpy(violation.airLine, aircraft->Airline.c_str(), sizeof(violation.airLine) - 1);
    violation.airLine[sizeof(violation.airLine) - 1] = '\0';  // Ensure null-termination
    
    // Set speed and allowed range
    violation.speed = aircraft->speed;
    violation.minAllowed = minAllowed;
    violation.maxAllowed = maxAllowed;
    
    // Send through pipe to AVN Generator process
    ssize_t bytesWritten = write(avnPipeWriteEnd, &violation, sizeof(ViolationData));
    
    if (bytesWritten != sizeof(ViolationData))
    {
        std::cerr << "ERROR: Failed to send violation data to AVN Generator" << std::endl;
        // In a real system, we might want to retry or queue this for later
    }
    else 
    {
        std::cout << "Violation data sent to AVN Generator process" << std::endl;
    }
}

// Handle violations by sending them to the AVN Generator
void Radar::handleViolations(const std::vector<Aircraft*>& activeFlights)
{
    // This function detects aircraft speed violations and sends them to the AVN Generator process
    
    // Check if we have a pipe to communicate with AVN Generator
    if (avnPipeWriteEnd <= 0)
    {
        // No pipe set up - can't send violations
        std::cout << "Radar: No AVN pipe set up - can't send violations" << std::endl;
        return;
    }
    
    // Detect violations using our existing method
    std::vector<ViolationInfo> violations = detectViolations(activeFlights);
    
    // Process each violation detected
    for (const ViolationInfo& violationInfo : violations)
    {
        // Use our helper method to process and send each violation
        processViolation(violationInfo.aircraft, violationInfo.minAllowed, violationInfo.maxAllowed);
    }
}

// Get speed limits for a given flight state
std::pair<int, int> Radar::getSpeedLimits(FlightState state) 
{
    // Set allowed speed range based on aircraft state
    // Return a pair with <minSpeed, maxSpeed>
    switch (state)
    {
        case FlightState::Holding:
            return std::make_pair(400, 600);
            
        case FlightState::Approach:
            return std::make_pair(240, 290);
            
        case FlightState::Landing:
            return std::make_pair(30, 240); // Landing should be between 30-240
            
        case FlightState::Taxi:
            return std::make_pair(15, 30);
            
        case FlightState::AtGate:
            return std::make_pair(0, 5); // At gate should be nearly stationary
            
        case FlightState::TakeoffRoll:
            return std::make_pair(0, 290);
            
        case FlightState::Climb:
            return std::make_pair(250, 463);
            
        case FlightState::Cruise: 
            return std::make_pair(800, 900);
            
        default:
            // Unknown state - use conservative limits
            return std::make_pair(0, 1000); 
    }
}

// Check if aircraft speed violates limits
bool Radar::checkSpeedViolation(const Aircraft& aircraft, int minAllowed, int maxAllowed) 
{
    // Different states have different violation criteria
    switch (aircraft.state)
    {
        case FlightState::Holding:
            // Holding only has upper limit violation
            return (aircraft.speed > maxAllowed);
            
        case FlightState::Approach:
            // Approach has both lower and upper limit violations
            return (aircraft.speed < minAllowed || aircraft.speed > maxAllowed);
            
        case FlightState::Landing:
            // Landing only has upper limit violation
            return (aircraft.speed > maxAllowed);
            
        case FlightState::Taxi:
            // Taxi only has upper limit violation
            return (aircraft.speed > maxAllowed);
            
        case FlightState::AtGate:
            // Special case for at gate - violation if > 10
            return (aircraft.speed > 10);
            
        case FlightState::TakeoffRoll:
            // Takeoff only has upper limit violation
            return (aircraft.speed > maxAllowed);
            
        case FlightState::Climb:
            // Climb only has upper limit violation
            return (aircraft.speed > maxAllowed);
            
        case FlightState::Cruise:
            // Cruise has both lower and upper limit violations
            return (aircraft.speed < minAllowed || aircraft.speed > maxAllowed);
            
        default:
            // For unknown states, no violation
            return false;
    }
}

// Simulate a violation for testing purposes
void Radar::simulateViolation(const std::string& flightNumber, const std::string& airline, 
                       int speed, int minAllowed, int maxAllowed)
{
    // Check if we have a pipe to communicate with AVN Generator
    if (avnPipeWriteEnd <= 0)
    {
        // No pipe set up - can't send violations
        std::cout << "Radar: No AVN pipe set up - can't simulate violation" << std::endl;
        return;
    }
    
    std::cout << "SIMULATING VIOLATION: " << flightNumber 
         << " (" << airline << ") - Speed: " << speed 
         << " km/h (Allowed: " << minAllowed << "-" << maxAllowed << " km/h)" << std::endl;
    
    // Create violation data to send through pipe
    ViolationData violation;
    
    // Copy data to the struct with proper string handling
    std::strncpy(violation.flightNumber, flightNumber.c_str(), sizeof(violation.flightNumber) - 1);
    violation.flightNumber[sizeof(violation.flightNumber) - 1] = '\0';  // Ensure null-termination
    
    std::strncpy(violation.airLine, airline.c_str(), sizeof(violation.airLine) - 1);
    violation.airLine[sizeof(violation.airLine) - 1] = '\0';  // Ensure null-termination
    
    // Set speed and allowed range
    violation.speed = speed;
    violation.minAllowed = minAllowed;
    violation.maxAllowed = maxAllowed;
    
    // Send through pipe to AVN Generator process
    ssize_t bytesWritten = write(avnPipeWriteEnd, &violation, sizeof(ViolationData));
    
    if (bytesWritten != sizeof(ViolationData))
    {
        std::cerr << "ERROR: Failed to send simulated violation data to AVN Generator" << std::endl;
    }
    else 
    {
        std::cout << "Simulated violation data sent to AVN Generator process" << std::endl;
    }
}

// Alias for simulateViolation to maintain compatibility with ATCScontroller
void Radar::reportViolation(const std::string& flightNumber, const std::string& airline, 
                    int speed, int minAllowed, int maxAllowed)
{
    // Why have two methods that do the same thing? Well, sometimes code evolves and names change.
    // This alias makes it easier to maintain backward compatibility without refactoring everywhere.
    
    // Just call the existing implementation
    simulateViolation(flightNumber, airline, speed, minAllowed, maxAllowed);
}