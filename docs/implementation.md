# AirControlX - Automated Air Traffic Control System: Implementation Guide

## Table of Contents

1. [Project Overview](#project-overview)
2. [System Architecture](#system-architecture)
3. [Project Flow](#project-flow)
   - [System Initialization](#system-initialization)
   - [Core Simulation Flow](#core-simulation-flow)
   - [Process Communication](#process-communication)
   - [Flight Thread Lifecycle](#flight-thread-lifecycle)
   - [Runway Management Flow](#runway-management-flow)
   - [Violation Detection Flow](#violation-detection-flow)
   - [Visualization](#visualization)
4. [Rubrics Implementation](#rubrics-implementation)
   - [Module 1: System Rules & Restrictions](#module-1-system-rules--restrictions)
   - [Module 2: ATCS Core Functionalities](#module-2-atcs-core-functionalities)
   - [Module 3: Subsystems & Airline Integration](#module-3-subsystems--airline-integration)
5. [Class Documentation](#class-documentation)
6. [Key Features and Algorithms](#key-features-and-algorithms)

---

## Project Overview

AirControlX is an Automated Air Traffic Control System simulation that models an international airport with multiple runways. The system integrates various components including flight management, runway allocation, air traffic monitoring, and violation tracking. It's developed using C++ with a strong emphasis on Operating Systems concepts like process management, threading, synchronization, and inter-process communication.

### Key Features

- Simulation of multiple airlines and aircraft types
- Threading for concurrent flight simulations
- Real-time monitoring of flight speeds and violation detection
- Automated Airspace Violation Notice (AVN) generation and payment processing
- SFML-based visualization of aircraft movements and airport facilities
- Multi-process architecture with IPC for subsystem communication

---

## System Architecture

The AirControlX system is designed using a modular architecture with multiple processes and threads to handle different aspects of the air traffic control system. The architecture follows these key principles:

1. **Multi-Process Design**: Core functionality is distributed across separate processes
2. **Thread-based Flight Simulation**: Each aircraft runs in its own thread
3. **IPC via Pipes**: Inter-process communication through unnamed pipes
4. **Mutex-based Synchronization**: Thread safety using mutexes and atomic variables
5. **Shared Resources**: Runways and controllers accessed through thread-safe interfaces

### Architecture Diagram

```
                     ┌───────────────────────────────────────────┐
                     │              Main Process                 │
                     │                                           │
                     │  ┌────────────┐    ┌─────────────────┐    │
                     │  │ ATCS       │    │ Runway Manager  │    │
                     │  │ Controller │◄─┐ │                 │    │
                     │  └────┬───────┘  │ └─────────────────┘    │
                     │       │       Runway                      │
                     │       │    Assignment                     │
                     │  Speed│          │                        │
                     │ Monitor│         │                        │
                     │       │          ▼                        │
                     │       │    ┌─────────────┐                │
                     │       │    │ Simulation  │                │
                     │       │    │   Manager   │                │
                     │       │    └─────┬───────┘                │
                     │       │          │                        │
                     │       │      Creates                      │
                     │       │          ▼                        │
                     │       │    ┌─────────────┐                │
                     │       │    │ Aircraft    │                │
                     │       │    │  Threads    │                │
                     │       │    └─────────────┘                │
                     │       │                                   │
                     │       ▼                                   │
┌───────────────┐    │  ┌────────────┐                           │    ┌───────────────┐
│ AVN Generator ◄────┼──┤ Violation  │     ┌─────────────────┐   │    │Airline Portal │
│   Process     │    │  │ Detection  │     │Visual Simulator  │   │    │   Process     │
└───────┬───────┘    │  └────────────┘     └─────────────────┘   │    └───────┬───────┘
        │            └───────────────────────────────────────────┘            │
        │                                                                     │
        │                      AVN Data (via pipe)                            │
        └─────────────────────────────────────────────────────────────────────►
                                                                              │
                                                                              │
        ┌────────────────────────────────────────────────┐                    │
        │                                                │                    │
        │                  Payment Requests                                   │
        ▼                                                ▼                    │
┌───────────────┐                                ┌───────────────┐            │
│  StripePay    │◄───────────────────────────────┤   Airlines    ◄────────────┘
│   Process     │                                │               │
└───────────────┘                                └───────────────┘
```

---

## Project Flow

### System Initialization

The system begins execution in `main.cpp`, which performs the following steps:

#### 1. Process Creation
This diagram shows how the main process creates child processes:

```
┌───────────┐              ┌────────────────┐
│   main    │─fork()──────►│ AVN Generator  │
│  process  │              │    Process     │
└─────┬─────┘              └────────────────┘
      │
      │                    ┌────────────────┐
      ├─fork()────────────►│ Airline Portal │
      │                    │    Process     │
      │                    └────────────────┘
      │
      │                    ┌────────────────┐
      └─fork()────────────►│   StripePay    │
                           │    Process     │
                           └────────────────┘
```

The initialization sequence in the main process:

1. **Create Pipes for IPC**:
   ```cpp
   int atcsToAvnPipe[2];     // ATCS Controller -> AVN Generator
   int avnToAirlinePipe[2];  // AVN Generator -> Airline Portal
   int stripeToAvnPipe[2];   // StripePay -> AVN Generator
   
   if (pipe(atcsToAvnPipe) < 0 || 
       pipe(avnToAirlinePipe) < 0 || 
       pipe(stripeToAvnPipe) < 0) {
       std::cerr << "Failed to create pipes. Exiting." << std::endl;
       return 1;
   }
   ```

2. **Fork AVN Generator Process**:
   ```cpp
   avnGeneratorPid = fork();
   if (avnGeneratorPid == 0) {
       // Child process - AVN Generator
       AVNGenerator avnGenerator;
       avnGenerator.initialize(atcsToAvnPipe, avnToAirlinePipe, stripeToAvnPipe);
       avnGenerator.run();
       exit(0);
   }
   ```

3. **Fork Airline Portal Process**:
   ```cpp
   airlinePortalPid = fork();
   if (airlinePortalPid == 0) {
       // Child process - Airline Portal
       AirlinePortal airlinePortal;
       airlinePortal.initialize(avnToAirlinePipe, airlineToStripePipe);
       airlinePortal.run();
       exit(0);
   }
   ```

4. **Fork StripePay Process**:
   ```cpp
   stripePayPid = fork();
   if (stripePayPid == 0) {
       // Child process - StripePay
       StripePayment stripePay;
       stripePay.initialize(airlineToStripePipe, stripeToAvnPipe);
       stripePay.run();
       exit(0);
   }
   ```

#### 2. Core Component Initialization

Next, the main process initializes the core components:

```
┌───────────────┐     ┌───────────────┐
│ ATCSController│     │ RunwayManager │
│               │◄───►│               │
└───────┬───────┘     └───────────────┘
        │
        │ Sets AVN Pipe
        ▼
┌───────────────┐
│ Flight        │
│ Scheduler     │
└───────────────┘
```

```cpp
// Set up ATCS Controller with pipe to AVN Generator
ATCScontroller atcsController;
RunwayManager runwayManager;

// Connect the controller to the runway manager
atcsController.setRunwayManager(&runwayManager);

// Set the pipe for sending violations to AVN Generator
atcsController.setAVNPipe(atcsToAvnPipe[1]);
```

#### 3. Visualization Setup

Finally, the main process sets up the visual simulator:

```cpp
// Create our visual simulator
VisualSimulator visualSim;

// Try to load the graphics resources
if (!visualSim.loadGraphics()) {
    std::cerr << "Failed to load graphics resources. Exiting." << std::endl;
    cleanupProcesses(0);  // Clean up before exiting
    return 1;
}
```

### Core Simulation Flow

The primary simulation flow involves multiple components working together. This diagram shows the detailed sequence of operations:

```
┌─────────────┐          ┌─────────────┐          ┌─────────────┐         ┌─────────────┐       ┌─────────────┐
│    Main     │          │   ATCS      │          │ Simulation  │         │  Aircraft   │       │    AVN      │
│   Process   │          │ Controller  │          │  Manager    │         │  Threads    │       │ Generator   │
└──────┬──────┘          └──────┬──────┘          └──────┬──────┘         └──────┬──────┘       └──────┬──────┘
       │                        │                        │                       │                     │
       │ Initialize             │                        │                       │                     │
       │────────────────────────►                        │                       │                     │
       │                        │                        │                       │                     │
       │ Create Aircraft Threads│                        │                       │                     │
       │───────────────────────────────────────────────► │                       │                     │
       │                        │                        │ Launch Threads        │                     │
       │                        │                        │───────────────────────►                     │
       │                        │                        │                       │                     │
       │ Main Loop              │                        │                       │                     │
       ├───────────┐            │                        │                       │                     │
       │           │            │                        │                       │                     │
       │ Handle Events          │                        │                       │                     │
       ├───────────┘            │                        │                       │                     │
       │                        │                        │                       │ Update Position     │
       │                        │                        │                       ├───────────┐         │
       │                        │                        │                       │           │         │
       │                        │                        │                       │ Report Status       │
       │                        │                        │◄──────────────────────┼───────────┘         │
       │                        │                        │                       │                     │
       │                        │ Monitor Flights        │                       │                     │
       │                        │◄───────────────────────┤                       │                     │
       │                        │                        │                       │                     │
       │                        │ Check Speed            │                       │                     │
       │                        ├───────────┐            │                       │                     │
       │                        │           │            │                       │                     │
       │                        │ Detect Violations      │                       │                     │
       │                        ├───────────┘            │                       │                     │
       │                        │                        │                       │                     │
       │                        │ Report Violations      │                       │                     │
       │                        │─────────────────────────────────────────────────────────────────────►│
       │                        │                        │                       │                     │
       │                        │                        │                       │ Request Runway      │
       │                        │                        │◄──────────────────────┤                     │
       │                        │                        │                       │                     │
       │                        │ Assign Runway          │                       │                     │
       │                        │◄───────────────────────┤                       │                     │
       │                        │                        │                       │                     │
       │                        │                        │ Runway Assignment     │                     │
       │                        │                        │───────────────────────►                     │
       │                        │                        │                       │                     │
       │ Render Frame           │                        │                       │                     │
       ├───────────┐            │                        │                       │                     │
       │           │            │                        │                       │                     │
       │ Display Updates        │                        │                       │                     │
       ├───────────┘            │                        │                       │                     │
       │                        │                        │                       │                     │
```

### Flight Thread Lifecycle

Each aircraft in the simulation is managed by its own thread. The lifecycle of a flight thread includes:

```
                       ┌─────────────────────────┐
                       │      Flight Thread      │
                       │      Initialization     │
                       └───────────┬─────────────┘
                                   │
                                   ▼
          ┌─────────────────────────────────────────────────┐
          │       Determine Flight Type (Arrival/Departure) │
          └───────────────────────┬─────────────────────────┘
                                  │
                  ┌───────────────┴────────────────┐
                  │                                │
    ┌─────────────▼────────────┐      ┌────────────▼─────────────┐
    │    ARRIVAL SEQUENCE      │      │     DEPARTURE SEQUENCE   │
    └─────────────┬────────────┘      └────────────┬─────────────┘
                  │                                │
    ┌─────────────▼────────────┐      ┌────────────▼─────────────┐
    │  Set Direction (N/S)     │      │  Set Direction (E/W)     │
    └─────────────┬────────────┘      └────────────┬─────────────┘
                  │                                │
    ┌─────────────▼────────────┐      ┌────────────▼─────────────┐
    │  Initial State: Holding  │      │  Initial State: At Gate  │
    └─────────────┬────────────┘      └────────────┬─────────────┘
                  │                                │
    ┌─────────────▼────────────┐      ┌────────────▼─────────────┐
    │  Add to Arrival Queue    │      │  Add to Departure Queue  │
    └─────────────┬────────────┘      └────────────┬─────────────┘
                  │                                │
    ┌─────────────▼────────────┐      ┌────────────▼─────────────┐
    │  Wait for Runway         │      │  Wait for Runway         │
    │  Assignment              │      │  Assignment              │
    └─────────────┬────────────┘      └────────────┬─────────────┘
                  │                                │
                  │                                │
                  │                                │
    ┌─────────────▼────────────┐      ┌────────────▼─────────────┐
    │  State: Approach         │      │  State: Taxi             │
    └─────────────┬────────────┘      └────────────┬─────────────┘
                  │                                │
    ┌─────────────▼────────────┐      ┌────────────▼─────────────┐
    │  State: Landing          │      │  State: Takeoff Roll     │
    └─────────────┬────────────┘      └────────────┬─────────────┘
                  │                                │
    ┌─────────────▼────────────┐      ┌────────────▼─────────────┐
    │  State: Taxi             │      │  State: Climb            │
    └─────────────┬────────────┘      └────────────┬─────────────┘
                  │                                │
    ┌─────────────▼────────────┐      ┌────────────▼─────────────┐
    │  State: At Gate          │      │  State: Cruise           │
    └─────────────┬────────────┘      └────────────┬─────────────┘
                  │                                │
    ┌─────────────▼────────────┐      ┌────────────▼─────────────┐
    │  Release Runway          │      │  Release Runway          │
    └─────────────┬────────────┘      └────────────┬─────────────┘
                  │                                │
                  └────────────────┬───────────────┘
                                   │
                                   ▼
                       ┌─────────────────────────┐
                       │    Flight Complete      │
                       │   Thread Terminates     │
                       └─────────────────────────┘
```

Implementation from SimulationManager::flightThreadFunction():

1. **Flight Initialization**:
   ```cpp
   Aircraft* plane = args->aircraft;
   plane->isActive = true;
   ```

2. **Arrival Flow**:
   ```cpp
   // Arrival flow - either North or South
   plane->direction = (plane->aircraftIndex % 4 == 0) ? Direction::North : Direction::South;
   
   // For arrivals, start at Holding state
   plane->state = FlightState::Holding;
   
   // Add to arrival queue
   atcController->scheduleArrival(plane);
   ```

3. **Departure Flow**:
   ```cpp
   // Departure flow - either East or West
   plane->direction = (plane->aircraftIndex % 4 == 1) ? Direction::East : Direction::West;
   
   // For departures, start at the gate
   plane->state = FlightState::AtGate;
   
   // Add to departure queue
   atcController->scheduleDeparture(plane);
   ```

4. **Runway Wait Loop**:
   ```cpp
   // Wait loop for runway assignment
   int waitTime = 0;
   while (!plane->hasRunwayAssigned && waitTime < 30) {
       sleep(1);  // Check every second if runway assigned
       waitTime++;
       
       // Every 5 seconds, print status update with estimated wait time
       if (waitTime % 5 == 0) {
           int estimatedWait = atcController->getScheduler()->estimateWaitTime(plane);
           manager->logMessage("Flight " + plane->FlightNumber + " holding, estimated wait: " + 
                              std::to_string(estimatedWait) + " minutes");
       }
   }
   ```

5. **Phase Transitions**:
   For arrivals:
   ```cpp
   // Approach phase
   plane->state = FlightState::Approach;
   plane->UpdateSpeed();
   sleep(3);
   
   // Landing phase
   plane->state = FlightState::Landing;
   plane->UpdateSpeed();
   sleep(2);
   
   // Taxi phase
   plane->state = FlightState::Taxi;
   plane->UpdateSpeed();
   sleep(2);
   
   // At gate
   plane->state = FlightState::AtGate;
   plane->UpdateSpeed();
   ```

   For departures:
   ```cpp
   // Taxi phase
   plane->state = FlightState::Taxi;
   plane->UpdateSpeed();
   sleep(2);
   
   // Takeoff phase
   plane->state = FlightState::TakeoffRoll;
   plane->UpdateSpeed();
   sleep(2);
   
   // Climb phase
   plane->state = FlightState::Climb;
   plane->UpdateSpeed();
   sleep(2);
   
   // Cruise phase
   plane->state = FlightState::Cruise;
   plane->UpdateSpeed();
   ```

6. **Runway Release**:
   ```cpp
   // Release the runway
   for (int i = 0; i < runwayManager->getRunwayCount(); i++) {
       RunwayClass* runway = runwayManager->getRunwayByIndex(i);
       if (runway && runway->isOccupied) {
           runway->release();
           break;
       }
   }
   ```

7. **Flight Completion**:
   ```cpp
   // Flight is now complete
   plane->isActive = false;
   manager->logMessage("Flight " + plane->FlightNumber + " has completed its journey");
   ```

### Runway Management Flow

Runway assignment is a critical aspect of the air traffic control system. Here's the detailed flow:

```
                 ┌───────────────────────────┐
                 │     ATCScontroller        │
                 │     assignRunway()        │
                 └────────────┬──────────────┘
                              │
                              ▼
                 ┌───────────────────────────┐
                 │  Get next aircraft from   │
                 │       scheduler           │
                 └────────────┬──────────────┘
                              │
                              ▼
                 ┌───────────────────────────┐
                 │   Determine direction     │
                 │      and priority         │
                 └────────────┬──────────────┘
                              │
                  ┌───────────┴────────────┐
                  │                        │
┌─────────────────▼────────┐    ┌──────────▼────────────────┐
│ Emergency/Cargo Flight?  │    │ Regular Commercial Flight │
└─────────────┬────────────┘    └──────────┬────────────────┘
              │                             │
┌─────────────▼────────────┐    ┌──────────▼────────────────┐
│   Try Runway C first     │    │ Check direction for A/B   │
└─────────────┬────────────┘    └──────────┬────────────────┘
              │                             │
              │                  ┌──────────┴────────────┐
              │                  │                       │
              │      ┌──────────▼──────────┐  ┌─────────▼──────────┐
              │      │  North/South?       │  │  East/West?        │
              │      └──────────┬──────────┘  └─────────┬──────────┘
              │                 │                       │
              │      ┌──────────▼──────────┐  ┌─────────▼──────────┐
              │      │   Try Runway A      │  │  Try Runway B      │
              │      └──────────┬──────────┘  └─────────┬──────────┘
              │                 │                       │
              └─────────────────┼───────────────────────┘
                                │
                                ▼
                      ┌─────────────────────┐
                      │ Successful          │
                      │ assignment?         │
                      └──────┬──────────────┘
                             │
          ┌────────────────┐ │ ┌─────────────────────┐
          │ Yes            │ │ │ No                  │
          ▼                │ │ ▼                     │
┌─────────────────────┐    │ │    ┌─────────────────────┐
│ Mark hasRunway      │    │ │    │ Try other runways   │
│ Assigned = true     │    │ │    │ in fallback order   │
└─────────┬───────────┘    │ │    └─────────┬───────────┘
          │                │ │              │
          │                │ │              │
          └────────────────┼─┼──────────────┘
                           │ │
                           ▼ ▼
           ┌────────────────────────────┐
           │   Return success/failure   │
           └────────────────────────────┘
```

Implementation from ATCScontroller::assignRunway():

1. **Get Next Aircraft**:
   ```cpp
   // Get next arrival/departure based on scheduling algorithm
   Aircraft* nextArrival = scheduler.getNextArrival();
   Aircraft* nextDeparture = scheduler.getNextDeparture();
   
   // Determine which to process first (based on priority/timing)
   Aircraft* aircraft = determineNextAircraft(nextArrival, nextDeparture);
   ```

2. **Check Emergency Status**:
   ```cpp
   // Emergency flights get priority for Runway C
   if (aircraft->EmergencyNo > 0) {
       RunwayClass* rwC = runwayManager->getRunwayByName("C");
       if (rwC && rwC->tryAssign(aircraft)) {
           aircraft->hasRunwayAssigned = true;
           return true;
       }
   }
   ```

3. **Direction-Based Assignment**:
   ```cpp
   // Direction-based assignment for regular flights
   switch (aircraft->direction) {
       case Direction::North:
       case Direction::South: {
           // N/S traffic uses Runway A
           RunwayClass* rwA = runwayManager->getRunwayByName("A");
           if (rwA && rwA->tryAssign(aircraft)) {
               aircraft->hasRunwayAssigned = true;
               return true;
           }
           break;
       }
           
       case Direction::East:
       case Direction::West: {
           // E/W traffic uses Runway B
           RunwayClass* rwB = runwayManager->getRunwayByName("B");
           if (rwB && rwB->tryAssign(aircraft)) {
               aircraft->hasRunwayAssigned = true;
               return true;
           }
           break;
       }
   }
   ```

4. **Fallback Assignment**:
   ```cpp
   // If preferred runway is unavailable, try others in order
   RunwayClass* rwC = runwayManager->getRunwayByName("C");
   if (rwC && rwC->tryAssign(aircraft)) {
       aircraft->hasRunwayAssigned = true;
       return true;
   }
   
   // Try any other available runway as last resort
   RunwayClass* rwA = runwayManager->getRunwayByName("A");
   if (rwA && rwA->tryAssign(aircraft)) {
       aircraft->hasRunwayAssigned = true;
       return true;
   }
   
   RunwayClass* rwB = runwayManager->getRunwayByName("B");
   if (rwB && rwB->tryAssign(aircraft)) {
       aircraft->hasRunwayAssigned = true;
       return true;
   }
   ```

### Violation Detection Flow

The violation detection and AVN generation process is a critical component of the system:

```
                     ┌─────────────────────┐
                     │  ATCScontroller     │
                     │  handleViolations() │
                     └─────────┬───────────┘
                               │
                               ▼
                     ┌─────────────────────┐
                     │  Get active flights │
                     └─────────┬───────────┘
                               │
                               ▼
                     ┌─────────────────────┐
                     │  For each aircraft  │◄────┐
                     └─────────┬───────────┘     │
                               │                 │
                               ▼                 │
                     ┌─────────────────────┐     │
                     │ Already has active  │     │
                     │    violation?       │─Yes─┘
                     └─────────┬───────────┘
                               │ No
                               ▼
                     ┌─────────────────────┐
                     │ Check speed limits  │
                     │  based on state     │
                     └─────────┬───────────┘
                               │
                               ▼
                     ┌─────────────────────┐
                     │   Violation         │─No───┐
                     │   detected?         │      │
                     └─────────┬───────────┘      │
                               │ Yes              │
                               ▼                  │
                     ┌─────────────────────┐      │
                     │ Mark aircraft as    │      │
                     │ having violation    │      │
                     └─────────┬───────────┘      │
                               │                  │
                               ▼                  │
                     ┌─────────────────────┐      │
                     │ Create Violation    │      │
                     │     Data            │      │
                     └─────────┬───────────┘      │
                               │                  │
                               ▼                  │
                     ┌─────────────────────┐      │
                     │ Send to AVN         │      │
                     │ Generator via pipe  │      │
                     └─────────┬───────────┘      │
                               │                  │
                               └──────────────────┘
                                        │
                                        ▼
                                ┌───────────────┐
                                │ Next Aircraft │
                                └───────────────┘

```

Implementation from ATCScontroller::handleViolations():

1. **Get Active Flights**:
   ```cpp
   // Get active flights
   const std::vector<Aircraft*>& activeFlights = scheduler.getActiveFlights();
   ```

2. **Check Each Aircraft**:
   ```cpp
   for (Aircraft* aircraft : activeFlights) {
       if (aircraft->hasActiveViolation) {
           continue;  // Skip if already has an active violation
       }
       
       // Check speed against limits based on state
       bool speedViolation = false;
       int minAllowed = 0;
       int maxAllowed = 0;
       
       // Set allowed range based on aircraft state
       switch (aircraft->state) {
           case FlightState::Holding:
               minAllowed = 400;
               maxAllowed = 600;
               speedViolation = (aircraft->speed > maxAllowed);
               break;
           // ...other states...
       }
   }
   ```

3. **Create and Send Violation**:
   ```cpp
   // If violation detected, send to AVN Generator
   if (speedViolation) {
       // Mark aircraft
       aircraft->hasActiveViolation = true;
       
       // Create violation data
       ViolationData violation;
       strncpy(violation.flightNumber, aircraft->FlightNumber.c_str(), sizeof(violation.flightNumber) - 1);
       strncpy(violation.airLine, aircraft->Airline.c_str(), sizeof(violation.airLine) - 1);
       violation.speed = aircraft->speed;
       violation.minAllowed = minAllowed;
       violation.maxAllowed = maxAllowed;
       
       // Send through pipe
       write(avnPipeWriteEnd, &violation, sizeof(ViolationData));
   }
   ```

### Process Communication

Inter-process communication occurs through a system of unnamed pipes. This diagram shows the detailed data flow:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                            Main Process                                     │
│                                                                             │
│  ┌───────────────┐     ViolationData      ┌────────────┐                    │
│  │ ATCS          │─────────────────────── │────────────┼────────────┐       │
│  │ Controller    │  write(atcsToAvnPipe)  │            │            │       │
│  └───────────────┘                        │            │            ▼       │
└──────────────────────────────────────────────────────────────────────────────┘
                                               │            │  ┌───────────────┐
                                               │            │  │ AVN Generator │
                                               │            │  │   Process     │
                                               │            │  │               │
                                               │            │  │               │
┌─────────────────────────────────────────────────────────────────────────────┐
│                                             │            │  │ ┌─────────────┐│
│  ┌───────────────┐      AVNData            │            └──┼─►   Read      ││
│  │   Airline     │◄───────────────────────────────────────┼──┤ViolationData ││
│  │   Portal      │  read(avnToAirlinePipe) │               │ └─────────────┘│
│  │   Process     │                         │               │ ┌─────────────┐│
│  │               │     PaymentRequest      │               │ │Create AVN   ││
│  │               ├─────────────────────────┼───────────────┼─►             ││
│  │               │ write(airlineToStripePipe)              │ └─────────────┘│
│  └───────────────┘                         │               │ ┌─────────────┐│
└──────────────────────────────────────────────────────────────│Send AVN Data││
                                             │               │ └─────────────┘│
                                             │               └───────────────┬┘
┌─────────────────────────────────────────────────────────────────────────────┐
│                                             │                               │
│  ┌───────────────┐      PaymentData         │                               │
│  │   StripePay   │─────────────────────────────────────────────────────────►│
│  │   Process     │   write(stripeToAvnPipe) │                               │
│  │               │◄────────────────────────┐                                │
│  │               │  read(airlineToStripePipe)                               │
│  └───────────────┘                                                          │
└─────────────────────────────────────────────────────────────────────────────┘
```

Key data structures used in IPC:

1. **ViolationData** (ATCS to AVN Generator):
   ```cpp
   struct ViolationData {
       char flightNumber[10];  // Flight identifier
       char airLine[20];       // Airline name
       int speed;              // Recorded speed at violation
       int minAllowed;         // Minimum allowed speed
       int maxAllowed;         // Maximum allowed speed
   };
   ```

2. **AVNData** (AVN Generator to Airline Portal):
   ```cpp
   struct AVNData {
       char avnID[20];         // AVN identifier
       char flightNumber[10];  // Flight identifier
       char airLine[20];       // Airline name
       int fineAmount;         // Base fine
       int totalAmount;        // Total amount due
       bool paid;              // Payment status
       char dueDate[11];       // Payment deadline
   };
   ```

3. **PaymentRequest** (Airline Portal to StripePay):
   ```cpp
   struct PaymentRequest {
       char avnID[20];         // AVN identifier
       char airLine[20];       // Airline making payment
       int amount;             // Payment amount
   };
   ```

4. **PaymentData** (StripePay to AVN Generator):
   ```cpp
   struct PaymentData {
       char avnID[20];         // AVN identifier
       bool paid;              // Payment status
       int transactionID;      // Payment transaction ID
   };
   ```

### Visualization

The visualization system uses SFML to render the simulation state:

```
┌─────────────────────────────────────────────────────────────┐
│                                                             │
│                     ┌────────────────────┐                  │
│                     │                    │                  │
│                     │                    │                  │
│                     │      Runway A      │                  │
│                     │                    │                  │
│                     │                    │                  │
│                     └────────────────────┘                  │
│                                                             │
│  ┌───────────┐                                ┌─────────────┐
│  │           │          ┌────────┐            │             │
│  │  Runway C │          │  ATC   │            │  Runway B   │
│  │           │          │ Tower  │            │             │
│  └───────────┘          └────────┘            └─────────────┘
│                                                             │
│      ┌───┐                                        ┌───┐     │
│      │   │                                        │   │     │
│      │✈️  │ <- Aircraft with type-specific sprite │✈️  │     │
│      │   │                                        │   │     │
│      └───┘                                        └───┘     │
│                                                             │
├─────────────────────────────────────────────────────────────┤
│                     Status Panel                            │
│                                                             │
│  Active Flights: 12    Violations: 3    Time Left: 4:32     │
│                                                             │
│  Flight    Airline   Status     Direction   Speed   Runway  │
│  PK123     PIA       Holding    North       550     -       │
│  FX456     FedEx     Taxi       South       30      A       │
│  PAF789    PAF       Landing    West        180     B       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

The visualization process flow:

```
┌───────────────┐        ┌───────────────┐        ┌───────────────┐
│ Main Process  │        │ SimulationMgr │        │VisualSimulator│
└───────┬───────┘        └───────┬───────┘        └───────┬───────┘
        │                        │                        │
        │ Initialize             │                        │
        │─────────────────────────────────────────────────►
        │                        │                        │
        │ Main Loop              │                        │
        ├───────────┐            │                        │
        │           │            │                        │
        │ handleEvents()         │                        │
        ├─────────────────────────────────────────────────►
        │           │            │                        │
        │           │            │ Update Visual Data     │
        │           │            │─────────────────────────►
        │           │            │                        │
        │ display() │            │                        │
        ├─────────────────────────────────────────────────►
        │           │            │                        │
        │           │            │ renderRunways()        │
        │           │            │                      ┌─┴───────┐
        │           │            │                      │         │
        │           │            │                      │         │
        │           │            │ renderAircraft()     │         │
        │           │            │                      │         │
        │           │            │                      │         │
        │           │            │ renderStatusPanel()  │         │
        │           │            │                      │         │
        │           │            │                      └─┬───────┘
        │           │            │                        │
        │           │            │ Swap Buffers           │
        │           │            │                        │
        │           │            │                        │
        │           └────────────┴────────────────────────┘
        │
        │ Repeat until
        │ window closed
        └───────────┘
```

The visualization implementation uses these key components:

1. **VisualSimulator::display()**:
   ```cpp
   void VisualSimulator::display() {
       // Clear the previous frame
       window.clear(sf::Color(30, 30, 30));
       
       // Draw the background
       window.draw(backgroundSprite);
       
       // Draw the runways
       renderRunways();
       
       // Draw all active aircraft
       renderAircraft();
       
       // Render the UI elements
       renderStatusPanel();
       
       // Swap buffers to display the new frame
       window.display();
   }
   ```

2. **Aircraft Rendering**:
   ```cpp
   void VisualSimulator::renderAircraft() {
       for (const auto& aircraft : aircraftList) {
           // Skip if not active
           if (!aircraft->isActive) continue;
           
           // Select appropriate sprite based on type
           sf::Sprite* sprite = nullptr;
           if (aircraft->type == AircraftType::Commercial) {
               sprite = &commercialSprite;
           } else if (aircraft->type == AircraftType::Cargo) {
               sprite = &cargoSprite;
           } else if (aircraft->type == AircraftType::Military) {
               sprite = &militarySprite;
           }
           
           // Skip if no sprite selected
           if (sprite == nullptr) continue;
           
           // Position based on flight state and direction
           sf::Vector2f position = calculateAircraftPosition(aircraft);
           sprite->setPosition(position);
           
           // Rotation based on direction
           float rotation = getRotationForDirection(aircraft->direction);
           sprite->setRotation(rotation);
           
           // Color based on status
           if (aircraft->hasActiveViolation) {
               sprite->setColor(sf::Color(255, 100, 100)); // Red tint for violations
           } else if (aircraft->EmergencyNo > 0) {
               sprite->setColor(sf::Color(255, 255, 100)); // Yellow tint for emergencies
           } else {
               sprite->setColor(sf::Color::White);
           }
           
           // Draw the sprite
           window.draw(*sprite);
           
           // Draw flight number below aircraft
           renderFlightNumber(aircraft, position);
       }
   }
   ```

---

## Rubrics Implementation

### Module 1: System Rules & Restrictions

#### Aircraft & Airline Restrictions (20 marks)

| Requirement | Implementation |
|-------------|---------------|
| 6 airlines with aircraft types | `AirlineManager::initialize()` creates 6 airlines (PIA, AirBlue, FedEx, PakistanAirforce, BlueDart, AghaKhanAir) with appropriate types and flight counts |
| Cargo/emergency restrictions | `AirlineManager::initialize()` sets type to Cargo for FedEx/BlueDart, Military for PakistanAirforce, and assigns appropriate aircraft limits |

**Key Implementation**:
- `AirlineManager.cpp` creates airlines with predefined configurations
- `Aircraft.h` defines aircraft types (Commercial, Cargo, Military, Medical)
- Max flight counts enforced in `AirlineManager::initialize()`

#### Runway Allocation (20 marks)

| Requirement | Implementation |
|-------------|---------------|
| Directional allocation | `ATCScontroller::assignRunway()` assigns RWY-A for N/S, RWY-B for E/W, RWY-C for cargo/emergency |
| Synchronization | `RunwayClass::tryAssign()` uses mutexes to ensure single aircraft access |

**Key Implementation**:
- `RunwayManager.cpp` initializes three runways with different orientations
- `ATCScontroller::assignRunway()` enforces direction-based allocation rules
- Mutex locking in runway assignment prevents concurrent access

#### Speed Monitoring & AVN (30 marks)

| Requirement | Implementation |
|-------------|---------------|
| Phase-based speed limits | `Radar::checkSpeed()` enforces different limits for each flight phase |
| AVN triggers | `Radar::handleViolations()` detects violations and triggers AVN creation |

**Key Implementation**:
- `Radar.cpp` contains violation detection logic
- Speed limits defined per phase in accordance with requirements
- `ViolationData` struct used to communicate violation details to AVN Generator

#### Simulation Time & Fault Handling (15 marks)

| Requirement | Implementation |
|-------------|---------------|
| 5-minute simulation | `Simulation::initialize()` sets `simulationDuration = 300` (5 minutes) |
| Ground faults | `SimulationManager::createRandomEmergency()` generates faults for aircraft on ground |

**Key Implementation**:
- `Timer.cpp` tracks elapsed time with 5-minute countdown
- Fault handling removes affected aircraft from queues

#### Code Structure (15 marks)

| Requirement | Implementation |
|-------------|---------------|
| Modular code with classes | Project organized into 20+ classes with clear separation of concerns |
| Proper use of C++ and OS concepts | Uses inheritance, encapsulation, mutexes, semaphores, threads |

**Key Implementation**:
- Classes organized by functionality (Aircraft, Airline, Controller, etc.)
- Thread safety provided by consistent mutex usage
- Proper memory management with constructors/destructors

### Module 2: ATCS Core Functionalities

#### Flight Scheduling & Conflict Resolution (25 marks)

| Requirement | Implementation |
|-------------|---------------|
| Dynamic flight addition | `FlightsScheduler::addArrival()` and `addDeparture()` add flights to queues |
| Conflict resolution | `FlightsScheduler::getNextArrival()` prioritizes by emergency status, then FCFS |

**Key Implementation**:
- Priority queue implementation in `FlightsScheduler.cpp`
- Conflict resolution based on flight priority and arrival time

#### Runway Allocation & Synchronization (25 marks)

| Requirement | Implementation |
|-------------|---------------|
| Direction-based allocation | `ATCScontroller::assignRunway()` allocates runways based on aircraft direction |
| Thread synchronization | Mutex protection in `RunwayClass::tryAssign()` ensures thread safety |

**Key Implementation**:
- `ATCScontroller.cpp` contains runway assignment logic
- Mutexes protect shared runway resources from concurrent access
- RWY-C used for overflow when A/B are occupied

#### Phase Management (30 marks)

| Requirement | Implementation |
|-------------|---------------|
| Flight phase transitions | `SimulationManager::flightThreadFunction()` manages phase transitions |
| Realistic timing | Each phase has appropriate duration in simulation time |

**Key Implementation**:
- State machine in flight thread handles transitions between phases
- Timing based on real-world airport operations
- Phase progression: Holding → Approach → Land → Taxi → Gate for arrivals
- Phase progression: Gate → Taxi → Takeoff → Departure for departures

#### Priority Handling (25 marks)

| Requirement | Implementation |
|-------------|---------------|
| Emergency preemption | `ATCScontroller::assignRunway()` prioritizes emergency flights |
| Queue reordering | `FlightsScheduler::reorderForEmergency()` bumps emergencies to front |

**Key Implementation**:
- Priority levels defined in `Aircraft.h` (Emergency > VIP > Cargo > Commercial)
- Emergency detection triggers queue reordering
- RWY-C allocation for emergency flights even when busy

#### Queue & Status Tracking (25 marks)

| Requirement | Implementation |
|-------------|---------------|
| Real-time queue updates | `FlightsScheduler::updateQueues()` refreshes queues with current status |
| Status tracking | `Aircraft::updateStatus()` tracks current flight phase |

**Key Implementation**:
- Estimated wait time calculation in `FlightsScheduler::estimateWaitTime()`
- Real-time status updates displayed in UI and logs
- Queue visualization in status panel

### Module 3: Subsystems & Airline Integration

#### ATCS Controller & Violation Detection (30 marks)

| Requirement | Implementation |
|-------------|---------------|
| Real-time monitoring | `ATCScontroller::monitorFlight()` checks aircraft positions and speeds |
| AVN activation | `Radar::handleViolations()` communicates with AVN Generator via pipe |

**Key Implementation**:
- Speed monitoring system in `Radar.cpp`
- Violation tracking with flight identification
- Pipe-based communication to AVN Generator

#### AVN Generator & Payment (30 marks)

| Requirement | Implementation |
|-------------|---------------|
| Fine calculation | `AVNGenerator::calculateFineAmount()` sets 500k for Commercial, 700k for Cargo, plus 15% fee |
| Payment confirmation | `AVNGenerator::processPayment()` updates payment status on confirmation |

**Key Implementation**:
- AVN ID generation with timestamps
- Thread-safe AVN list with mutex protection
- Due date calculation (3 days from issuance)
- Payment status tracking and updates

#### Airline Portal & StripePay (25 marks)

| Requirement | Implementation |
|-------------|---------------|
| Portal displays | `AirlinePortal::viewAVNsByAirline()` shows airline-specific AVNs |
| StripePay integration | `StripePayment::processPaymentRequest()` handles payment processing |

**Key Implementation**:
- AirlinePortal display filters (by airline, status)
- StripePay admin approval interface
- Account balance tracking for airlines
- Payment history and transaction IDs

#### Simulation & Visualization (35 marks)

| Requirement | Implementation |
|-------------|---------------|
| SFML graphics | `VisualSimulator::loadGraphics()` loads runway and aircraft sprites |
| Color-coding | Different sprites for different aircraft types and runways |
| Status updates | `VisualSimulator::renderStatusPanel()` shows real-time flight information |

**Key Implementation**:
- SFML integration in `Visualizer` classes
- Sprite-based rendering with rotation based on direction
- Animation for aircraft movement and tower
- UI panels for status information and controls
- Airline Portal visualization with AVN display and payment interface

---

## Class Documentation

### Core Simulation Classes

#### `Simulation`
Main coordinator class that initializes and manages the simulation lifecycle.

**Key Methods**:
- `initialize()`: Creates and connects all simulation components
- `run()`: Executes the main simulation loop
- `update()`: Updates simulation state (called in render loop)
- `togglePause()`: Pauses or resumes simulation

#### `SimulationManager`
Handles thread creation and management for aircraft and controllers.

**Key Methods**:
- `flightThreadFunction()`: Thread function for aircraft simulation
- `atcControllerThreadFunction()`: Thread function for ATC controller
- `launchAirlineThreads()`: Creates threads for all aircraft in an airline
- `logMessage()`: Thread-safe console logging

#### `AirlineManager`
Manages the collection of airlines and their initialization.

**Key Methods**:
- `initialize()`: Creates airlines with predefined configurations
- `getAirlineByIndex()`: Retrieves airline by index
- `getAirlineByName()`: Retrieves airline by name

#### `RunwayManager`
Manages the airport's runways and their allocation.

**Key Methods**:
- `initialize()`: Creates the three runways (A, B, C)
- `getRunway()`: Retrieves runway by ID
- `getRunways()`: Returns all runways

### Air Traffic Control Classes

#### `ATCScontroller`
Central controller for managing air traffic and runway assignments.

**Key Methods**:
- `monitorFlight()`: Monitors aircraft positions and speeds
- `assignRunway()`: Allocates runways based on direction and priority
- `handleViolations()`: Detects and processes violations
- `scheduleArrival()`: Adds arrival flight to scheduler
- `scheduleDeparture()`: Adds departure flight to scheduler

#### `FlightsScheduler`
Manages flight queues for arrivals and departures.

**Key Methods**:
- `addArrival()`: Adds aircraft to arrival queue
- `addDeparture()`: Adds aircraft to departure queue
- `getNextArrival()`: Gets next aircraft for arrival (priority-based)
- `getNextDeparture()`: Gets next aircraft for departure (priority-based)
- `estimateWaitTime()`: Calculates estimated wait time for aircraft

#### `Radar`
Monitors aircraft speeds and detects violations.

**Key Methods**:
- `checkSpeed()`: Verifies if aircraft speed is within limits
- `handleViolations()`: Processes detected violations
- `reportViolation()`: Sends violation data to AVN Generator

### Aircraft Classes

#### `Aircraft`
Represents an individual aircraft with its properties and state.

**Key Methods**:
- `setFlightDetails()`: Sets flight number and airline
- `updateStatus()`: Updates flight status based on phase
- `getSpeed()`: Returns current speed
- `getDirection()`: Returns current direction

#### `Airline`
Represents an airline company with its fleet of aircraft.

**Key Methods**:
- `addAircraft()`: Adds aircraft to airline's fleet
- `getAircraftByIndex()`: Retrieves aircraft by index
- `getName()`: Returns airline name
- `getType()`: Returns airline's primary aircraft type

### Subsystem Classes

#### `AVNGenerator`
Generates and manages Airspace Violation Notices.

**Key Methods**:
- `run()`: Main process loop for AVN generation
- `processViolation()`: Creates AVN from violation data
- `calculateFineAmount()`: Calculates fine based on aircraft type
- `processPayment()`: Updates AVN status when payment is confirmed

#### `AirlinePortal`
Interface for airlines to view violations and manage payments.

**Key Methods**:
- `run()`: Main process loop for airline portal
- `viewAVNsByAirline()`: Displays AVNs for specific airline
- `viewAVNsByStatus()`: Filters AVNs by payment status
- `sendPaymentRequest()`: Initiates payment process

#### `StripePayment`
Handles payment processing for violations.

**Key Methods**:
- `run()`: Main process loop for payment processing
- `processPaymentRequest()`: Handles incoming payment requests
- `approvePayment()`: Admin interface for payment approval
- `simulatePaymentProcessing()`: Simulates payment gateway
- `sendPaymentConfirmation()`: Notifies AVN Generator of successful payment

### Visualization Classes

#### `Visualizer`
Base class for SFML visualization components.

**Key Methods**:
- `initialize()`: Sets up SFML window and loads resources
- `render()`: Main rendering function
- `handleEvents()`: Processes window events

#### `VisualSimulator`
Handles the graphical representation of the simulation.

**Key Methods**:
- `loadGraphics()`: Loads sprite resources
- `display()`: Updates and draws the visualization
- `renderRunways()`: Draws runway sprites
- `renderAircraft()`: Draws aircraft sprites with appropriate rotation

---

## Key Features and Algorithms

### Thread-Safe Resource Access

All shared resources (runways, queues, AVN lists) use mutexes for thread safety:

```cpp
// Example from RunwayClass::tryAssign()
std::lock_guard<std::mutex> lock(runwayMutex);
if (isOccupied) {
    return false;
}
isOccupied = true;
// Assign aircraft to runway
```

This pattern is consistently used across the codebase to ensure thread safety:

- `FlightScheduler` uses `schedulerMutex` to protect flight queue operations
- `AVNGenerator` uses `avnMutex` to protect the AVN list
- `SimulationManager` uses `consoleMutex` for thread-safe console output
- `VisualSimulator` uses `visualDataMutex` to protect shared visualization data

### Priority-Based Scheduling

The flight scheduler uses a comprehensive priority system for queue management:

```cpp
// Implementation of priority-based queue retrieval
Aircraft* FlightsScheduler::getNextArrival() {
    std::lock_guard<std::mutex> lock(schedulerMutex);
    
    if (arrivalQueue.empty()) {
        return nullptr;
    }
    
    // First priority: Emergency flights (Level 3 highest)
    for (int level = 3; level > 0; --level) {
        for (auto it = arrivalQueue.begin(); it != arrivalQueue.end(); ++it) {
            if ((*it)->EmergencyNo == level) {
                Aircraft* aircraft = *it;
                arrivalQueue.erase(it);
                return aircraft;
            }
        }
    }
    
    // Second priority: Cargo/Military flights
    for (auto it = arrivalQueue.begin(); it != arrivalQueue.end(); ++it) {
        if ((*it)->type == AircraftType::Cargo || 
            (*it)->type == AircraftType::Military) {
            Aircraft* aircraft = *it;
            arrivalQueue.erase(it);
            return aircraft;
        }
    }
    
    // Finally, regular commercial flights (FCFS)
    Aircraft* aircraft = arrivalQueue.front();
    arrivalQueue.pop_front();
    return aircraft;
}
```

### Inter-Process Communication System

The AirControlX system uses a sophisticated pipe-based IPC system that connects four separate processes:

```
┌──────────────┐     ┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│Main Process  │     │AVN Generator │     │Airline Portal│     │ StripePay    │
│(ATCS, Runway)│     │              │     │              │     │              │
└──────┬───────┘     └──────┬───────┘     └──────┬───────┘     └──────┬───────┘
       │                    │                    │                    │
       │ ViolationData      │                    │                    │
       │───────────────────►│                    │                    │
       │                    │                    │                    │
       │                    │  AVNData           │                    │
       │                    │───────────────────►│                    │
       │                    │                    │                    │
       │                    │                    │  PaymentRequest    │
       │                    │                    │───────────────────►│
       │                    │                    │                    │
       │                    │   PaymentData      │                    │
       │                    │◄───────────────────┼────────────────────┘
       │                    │                    │
```

Pipe creation:
```cpp
int atcsToAvnPipe[2];     // Violation data
int avnToAirlinePipe[2];  // AVN notifications
int airlineToStripePipe[2]; // Payment requests
int stripeToAvnPipe[2];   // Payment confirmations

// Create all pipes
pipe(atcsToAvnPipe);
pipe(avnToAirlinePipe);
pipe(airlineToStripePipe);
pipe(stripeToAvnPipe);
```

The IPC system includes these key features:
- Binary data transfer using structs with fixed-size arrays
- Non-blocking I/O using `select()` in receiver processes
- Error handling for pipe operations (read/write failures)
- Process-specific pipe end management (closing unused ends)

### Flight State Machine

Each aircraft follows a state machine that dictates its behavior:

```
                ┌─────────┐
                │ Initial │
                └────┬────┘
                     │
        ┌────────────┴────────────┐
        │                         │
┌───────▼─────────┐     ┌─────────▼───────┐
│   ARRIVAL       │     │   DEPARTURE     │
└───────┬─────────┘     └─────────┬───────┘
        │                         │
┌───────▼─────────┐     ┌─────────▼───────┐
│    Holding      │     │    At Gate      │
└───────┬─────────┘     └─────────┬───────┘
        │                         │
┌───────▼─────────┐     ┌─────────▼───────┐
│    Approach     │     │      Taxi       │
└───────┬─────────┘     └─────────┬───────┘
        │                         │
┌───────▼─────────┐     ┌─────────▼───────┐
│    Landing      │     │   Takeoff Roll  │
└───────┬─────────┘     └─────────┬───────┘
        │                         │
┌───────▼─────────┐     ┌─────────▼───────┐
│      Taxi       │     │      Climb      │
└───────┬─────────┘     └─────────┬───────┘
        │                         │
┌───────▼─────────┐     ┌─────────▼───────┐
│    At Gate      │     │      Cruise     │
└───────┬─────────┘     └─────────┬───────┘
        │                         │
        └─────────────┬───────────┘
                      │
              ┌───────▼─────────┐
              │    Complete     │
              └─────────────────┘
```

Each state has:
- Specific speed limits (enforced by Radar)
- Phase-specific duration (simulated with sleep())
- Associated visualization positions/animations
- Specific requirements for runway usage

### Violation Detection Algorithm

The violation detection system in Radar.cpp follows this algorithm:

```
Function checkSpeed(Aircraft aircraft):
    Set speedViolation = false
    Set allowedRange based on aircraft state:
        Case Holding:
            minSpeed = 400, maxSpeed = 600
            If aircraft.speed < minSpeed OR aircraft.speed > maxSpeed:
                speedViolation = true
        Case Approach:
            minSpeed = 240, maxSpeed = 290
            If aircraft.speed < minSpeed OR aircraft.speed > maxSpeed:
                speedViolation = true
        Case Landing:
            minSpeed = 140, maxSpeed = 190
            If aircraft.speed < minSpeed OR aircraft.speed > maxSpeed:
                speedViolation = true
        Case Taxi:
            minSpeed = 15, maxSpeed = 30
            If aircraft.speed < minSpeed OR aircraft.speed > maxSpeed:
                speedViolation = true
        // Similar cases for all other flight states
    
    If speedViolation AND NOT aircraft.hasActiveViolation:
        Set aircraft.hasActiveViolation = true
        Create violationData with flight details and speed limits
        Send violationData through pipe to AVN Generator
```

### Fine Calculation System

The AVN Generator calculates fines using a sophisticated algorithm based on aircraft type:

```cpp
void AVN::calculateFine() {
    // Base fine determined by aircraft type
    if (aircraftType == "Commercial") {
        fineAmount = 500000;  // PKR 500,000 for commercial
    } else if (aircraftType == "Cargo" || aircraftType == "Military") {
        fineAmount = 700000;  // PKR 700,000 for cargo/military
    } else if (aircraftType == "Medical") {
        fineAmount = 350000;  // PKR 350,000 for medical
    }
    
    // Additional factors
    
    // Speed excess factor: 10% extra per 10% over limit
    float exceedFactor = (speed - allowed.second) / static_cast<float>(allowed.second);
    if (exceedFactor > 0) {
        // Calculate additional percentage (10% per 10% over)
        float additionalPercent = exceedFactor * 100.0f;
        fineAmount += static_cast<int>(fineAmount * (additionalPercent / 10.0f) * 0.1f);
    }
    
    // Service fee (15% administrative fee)
    serviceFee = static_cast<int>(fineAmount * 0.15);
    
    // Total amount
    totalAmount = fineAmount + serviceFee;
    
    // Calculate due date (3 days from issuance)
    time_t dueTime = issueTime + (3 * 24 * 60 * 60);  // 3 days in seconds
    struct tm* dueTimeInfo = localtime(&dueTime);
    
    char dueDateBuffer[11];
    strftime(dueDateBuffer, sizeof(dueDateBuffer), "%Y-%m-%d", dueTimeInfo);
    dueDate = std::string(dueDateBuffer);
}
```

### Real-time SFML Visualization

The visualization system uses SFML to render the simulation state in real-time. Key components include:

1. **Sprite Management**:
   ```cpp
   // Load aircraft sprites
   if (!commercialTexture.loadFromFile("assets/Commercial.png") ||
       !cargoTexture.loadFromFile("assets/Cargo.png") ||
       !militaryTexture.loadFromFile("assets/Military.png")) {
       return false;
   }
   
   commercialSprite.setTexture(commercialTexture);
   cargoSprite.setTexture(cargoTexture);
   militarySprite.setTexture(militaryTexture);
   
   // Load runway sprites
   // ...
   ```

2. **Position Calculation**:
   ```cpp
   // Calculate aircraft position based on state/direction
   sf::Vector2f calculateAircraftPosition(const Aircraft* aircraft) {
       // Base position (center of window)
       sf::Vector2f position(windowWidth / 2.0f, windowHeight / 2.0f);
       
       // Position based on flight state
       float distanceFromCenter = 0.0f;
       switch (aircraft->state) {
           case FlightState::Holding:
               distanceFromCenter = 300.0f;
               break;
           case FlightState::Approach:
               distanceFromCenter = 200.0f;
               break;
           // ...other states
       }
       
       // Adjust for direction
       switch (aircraft->direction) {
           case Direction::North:
               position.y -= distanceFromCenter;
               break;
           case Direction::South:
               position.y += distanceFromCenter;
               break;
           case Direction::East:
               position.x += distanceFromCenter;
               break;
           case Direction::West:
               position.x -= distanceFromCenter;
               break;
       }
       
       return position;
   }
   ```

3. **UI Rendering**:
   ```cpp
   void VisualSimulator::renderStatusPanel() {
       // Background
       sf::RectangleShape panel(sf::Vector2f(windowWidth, 150));
       panel.setPosition(0, windowHeight - 150);
       panel.setFillColor(sf::Color(20, 20, 30, 230));
       window.draw(panel);
       
       // Stats text
       sf::Text statsText;
       statsText.setFont(font);
       statsText.setCharacterSize(14);
       statsText.setFillColor(sf::Color::White);
       statsText.setPosition(10, windowHeight - 140);
       
       // Build status string
       std::stringstream ss;
       ss << "Active Flights: " << aircraftList.size();
       ss << "   Violations: " << countViolations();
       ss << "   Time Left: " << formatTime(timeRemaining);
       
       statsText.setString(ss.str());
       window.draw(statsText);
       
       // Flight table headers
       // ...
       
       // Flight rows
       // ...
   }
   ```

### Parallel Processing Model

AirControlX implements a sophisticated parallel processing model:

```
┌─────────────────────────────────────────────────────────────────┐
│                                                                 │
│                       Main Process                              │
│                                                                 │
│  ┌─────────────┐  ┌─────────────┐  ┌────────────────────────┐   │
│  │ Main Thread │  │ ATC Thread  │  │ Visualization Thread   │   │
│  └─────────────┘  └─────────────┘  └────────────────────────┘   │
│                                                                 │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌────────┐  │
│  │Flight Thread│  │Flight Thread│  │Flight Thread│  │  ...   │  │
│  │     #1      │  │     #2      │  │     #3      │  │        │  │
│  └─────────────┘  └─────────────┘  └─────────────┘  └────────┘  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐
│                 │  │                 │  │                 │
│  AVN Generator  │  │ Airline Portal  │  │   StripePay     │
│    Process      │  │    Process      │  │    Process      │
│                 │  │                 │  │                 │
└─────────────────┘  └─────────────────┘  └─────────────────┘
```

This model provides:
1. **CPU Core Utilization**: Distributes load across available cores
2. **Fault Isolation**: Process crashes don't affect other components
3. **Real Concurrency**: True parallel execution of simulation components
4. **Simplified Design**: Each component has a clear, focused responsibility

### Memory Management Strategy

The AirControlX system implements careful memory management:

1. **Ownership Hierarchy**:
   ```
   AirlineManager -> Airline -> Aircraft
                         ↑
                         └── FlightScheduler (references only)
   ```

2. **Resource Cleanup**:
   ```cpp
   // SimulationManager destructor
   SimulationManager::~SimulationManager() {
       // Clean up all threads
       for (auto& threadId : aircraftThreads) {
           pthread_cancel(threadId);
           pthread_detach(threadId);
       }
       
       // Destroy synchronization primitives
       pthread_mutex_destroy(&consoleMutex);
       pthread_mutex_destroy(&visualDataMutex);
   }
   ```

3. **RAII Pattern**: Used consistently for all resource management
   ```cpp
   // In RunwayClass::tryAssign()
   {
       std::lock_guard<std::mutex> lock(runwayMutex);
       // Resource is automatically released when lock goes out of scope
       // ...
   }
   ```

The combination of these design patterns ensures that the AirControlX system effectively balances performance, robustness, and code clarity while adhering to the requirements specified in the rubrics.