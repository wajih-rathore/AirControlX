#include "../include/VisualSimulator.h"
#include "../include/AVNGenerator.h"
#include "../include/ATCScontroller.h"
#include "../include/RunwayManager.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>
#include <cstring> // Use C++ style header for C string functions
#include <thread>
#include <chrono>

// Process IDs
pid_t avnGeneratorPid = -1;
pid_t airlinePortalPid = -1;
pid_t stripePayPid = -1;

// Flag for testing mode
bool isTestMode = false;

// Signal handler to clean up child processes on exit
void cleanupProcesses(int signal) 
{
    std::cout << "Cleaning up processes..." << std::endl;
    
    // Kill child processes if they're still running
    if (avnGeneratorPid > 0) 
    {
        kill(avnGeneratorPid, SIGTERM);
    }
    if (airlinePortalPid > 0) 
    {
        kill(airlinePortalPid, SIGTERM);
    }
    if (stripePayPid > 0) 
    {
        kill(stripePayPid, SIGTERM);
    }
    
    exit(0);
}

/**
 * This function simulates aircraft speed violations for testing the AVN Generator
 * 
 * @param atcsToAvnPipeWrite Write end of pipe to AVN Generator
 */
void runViolationTests(int atcsToAvnPipeWrite)
{
    std::cout << "\n======= RUNNING AVN GENERATOR TESTS =======" << std::endl;
    std::cout << "Sending test violations to AVN Generator..." << std::endl;
    
    // Test Case 1: Commercial aircraft (PIA) speed violation
    {
        ViolationData testViolation1;
        std::strncpy(testViolation1.flightNumber, "PK123", sizeof(testViolation1.flightNumber) - 1);
        testViolation1.flightNumber[sizeof(testViolation1.flightNumber) - 1] = '\0';
        
        std::strncpy(testViolation1.airLine, "PIA", sizeof(testViolation1.airLine) - 1);
        testViolation1.airLine[sizeof(testViolation1.airLine) - 1] = '\0';
        
        testViolation1.speed = 650;  // Exceeding speed limit for holding phase
        testViolation1.minAllowed = 400;
        testViolation1.maxAllowed = 600;
        
        write(atcsToAvnPipeWrite, &testViolation1, sizeof(ViolationData));
        std::cout << "Test 1: Sent Commercial violation - PK123 (PIA) at 650 km/h" << std::endl;
        
        // Pause to allow processing
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    // Test Case 2: Cargo aircraft (FedEx) speed violation
    {
        ViolationData testViolation2;
        std::strncpy(testViolation2.flightNumber, "FX456", sizeof(testViolation2.flightNumber) - 1);
        testViolation2.flightNumber[sizeof(testViolation2.flightNumber) - 1] = '\0';
        
        std::strncpy(testViolation2.airLine, "FedEx", sizeof(testViolation2.airLine) - 1);
        testViolation2.airLine[sizeof(testViolation2.airLine) - 1] = '\0';
        
        testViolation2.speed = 300;  // Exceeding speed limit for approach phase
        testViolation2.minAllowed = 240;
        testViolation2.maxAllowed = 290;
        
        write(atcsToAvnPipeWrite, &testViolation2, sizeof(ViolationData));
        std::cout << "Test 2: Sent Cargo violation - FX456 (FedEx) at 300 km/h" << std::endl;
        
        // Pause to allow processing
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    // Test Case 3: Emergency aircraft (PakistanAirforce) speed violation
    {
        ViolationData testViolation3;
        std::strncpy(testViolation3.flightNumber, "PAF789", sizeof(testViolation3.flightNumber) - 1);
        testViolation3.flightNumber[sizeof(testViolation3.flightNumber) - 1] = '\0';
        
        std::strncpy(testViolation3.airLine, "PakistanAirforce", sizeof(testViolation3.airLine) - 1);
        testViolation3.airLine[sizeof(testViolation3.airLine) - 1] = '\0';
        
        testViolation3.speed = 35;  // Exceeding speed limit for taxi phase
        testViolation3.minAllowed = 15;
        testViolation3.maxAllowed = 30;
        
        write(atcsToAvnPipeWrite, &testViolation3, sizeof(ViolationData));
        std::cout << "Test 3: Sent Emergency violation - PAF789 (PakistanAirforce) at 35 km/h" << std::endl;
    }
    
    std::cout << "Test violations sent. AVN Generator should be processing them..." << std::endl;
    std::cout << "============================================\n" << std::endl;
}

/**
 * Main entry point for AirControlX
 * Creates the main visualization and forks child processes
 */
int main(int argc, char* argv[]) 
{
    // Check for test mode argument
    if (argc > 1 && std::string(argv[1]) == "--test")
    {
        isTestMode = true;
        std::cout << "Running in test mode" << std::endl;
    }
    
    // Set up signal handler for clean termination
    signal(SIGINT, cleanupProcesses);
    signal(SIGTERM, cleanupProcesses);
    
    // Create pipes for inter-process communication
    int atcsToAvnPipe[2];     // ATCS Controller -> AVN Generator
    int avnToAirlinePipe[2];  // AVN Generator -> Airline Portal
    int stripeToAvnPipe[2];   // StripePay -> AVN Generator
    
    // Create the pipes
    if (pipe(atcsToAvnPipe) < 0 || 
        pipe(avnToAirlinePipe) < 0 || 
        pipe(stripeToAvnPipe) < 0) 
    {
        std::cerr << "Failed to create pipes. Exiting." << std::endl;
        return 1;
    }
    
    // Fork AVN Generator Process
    avnGeneratorPid = fork();
    
    if (avnGeneratorPid < 0) 
    {
        std::cerr << "Failed to fork AVN Generator process. Exiting." << std::endl;
        return 1;
    }
    else if (avnGeneratorPid == 0) 
    {
        // Child process - AVN Generator
        // This is where our AVN Generator process code runs
        AVNGenerator avnGenerator;
        avnGenerator.initialize(atcsToAvnPipe, avnToAirlinePipe, stripeToAvnPipe);
        avnGenerator.run();
        
        // This should never be reached as avnGenerator.run() has an infinite loop
        exit(0);
    }
    
    // Parent process continues...
    std::cout << "AVN Generator process forked with PID: " << avnGeneratorPid << std::endl;
    
    // Close unused ends of pipes in parent process
    close(atcsToAvnPipe[0]);   // Parent doesn't read from ATCS->AVN pipe
    close(avnToAirlinePipe[1]); // Parent doesn't write to AVN->Airline pipe
    close(stripeToAvnPipe[0]);  // Parent doesn't read from StripePay->AVN pipe
    
    // Set up ATCS Controller with pipe to AVN Generator
    ATCScontroller atcsController;
    RunwayManager runwayManager;
    
    // Connect the controller to the runway manager
    atcsController.setRunwayManager(&runwayManager);
    
    // Set the pipe for sending violations to AVN Generator
    atcsController.setAVNPipe(atcsToAvnPipe[1]);
    
    // If we're in test mode, just run the tests and exit
    if (isTestMode)
    {
        std::cout << "Running in test mode - executing AVN Generator tests" << std::endl;
        
        // Run test cases
        runViolationTests(atcsToAvnPipe[1]);
        
        // Wait for a few seconds to let the AVN Generator process the test cases
        std::cout << "Waiting for AVN Generator to process test cases..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        
        std::cout << "Test run complete. Cleaning up..." << std::endl;
        cleanupProcesses(0);
        return 0;
    }
    
    // Create our visual simulator
    VisualSimulator visualSim;
    
    // Try to load the graphics resources
    if (!visualSim.loadGraphics()) 
    {
        std::cerr << "Failed to load graphics resources. Exiting." << std::endl;
        cleanupProcesses(0);  // Clean up before exiting
        return 1;
    }
    
    std::cout << "SFML window created successfully!" << std::endl;
    std::cout << "Press ESC key to close the window." << std::endl;
    
    // Main game loop - keep rendering while the window is open
    while (visualSim.running()) 
    {
        // Handle any window events (close button, keyboard input)
        visualSim.handleEvents();
        
        // Run ATCS monitoring and violation detection
        atcsController.monitorFlight();
        atcsController.handleViolations();
        
        // Render the current frame
        visualSim.display();
        
        // A small sleep to prevent CPU from running at 100%
        // SFML's vertical sync should handle this, but just to be safe
        sf::sleep(sf::milliseconds(10));
    }
    
    std::cout << "Window closed. Cleaning up..." << std::endl;
    
    // Clean up processes and exit
    cleanupProcesses(0);
    return 0;
}