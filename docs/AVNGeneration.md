# AVN Generation System Documentation

## Overview

The Airspace Violation Notice (AVN) generation system is a critical component of AirControlX that monitors aircraft behavior, detects speed violations, generates violation notices, and facilitates the payment processing workflow. This document explains the complete flow of AVN generation, the technical implementation details, and the reasoning behind design decisions.

## System Architecture

The AVN generation system follows a multi-process design to ensure separation of concerns and robust inter-process communication:

### Process Structure

1. **ATCS Controller Process (Parent)**: 
   - Monitors aircraft speed and detects violations
   - Sends violation data to the AVN Generator via unnamed pipes
   
2. **AVN Generator Process (Child)**:
   - Receives violation data from ATCS Controller
   - Creates and manages AVN documents
   - Forwards AVN information to the Airline Portal
   - Updates payment status when notified by StripePay

3. **Airline Portal Process** (Future Implementation):
   - Displays AVNs to airline representatives
   - Facilitates payment through the StripePay process

4. **StripePay Process** (Future Implementation):
   - Handles the payment flow
   - Sends confirmation back to AVN Generator

## Data Flow Diagram

```
                                  ┌───────────────┐
                                  │  Aircraft in  │
                                  │   Airspace    │
                                  └───────┬───────┘
                                          │
                                          │ Speed Monitoring
                                          ▼
┌─────────────────┐  Detects Violation ┌─────────────────┐
│  Radar System   ├───────────────────>│ ATCS Controller │
└─────────────────┘                    └────────┬────────┘
                                                │
                                                │ Violation Data (via pipe)
                                                ▼
┌─────────────────┐                    ┌─────────────────┐
│  Airline Portal │<───── AVN Data ────│  AVN Generator  │
└────────┬────────┘                    └────────┬────────┘
         │                                      │
         │                                      │ Payment Status
         │ Payment Request                      ▼
         ▼                              ┌─────────────────┐
┌─────────────────┐ Payment Confirmation│    StripePay    │
│     Airline     │<──────────────────────────────────────┘
└─────────────────┘
```

## Technical Implementation Details

### 1. Inter-Process Communication (IPC)

#### Unnamed Pipes
We chose to use unnamed pipes for IPC between processes due to their simplicity, reliability, and built-in synchronization:

```cpp
// Creation of pipes in main.cpp
int atcsToAvnPipe[2];     // ATCS Controller -> AVN Generator
int avnToAirlinePipe[2];  // AVN Generator -> Airline Portal
int stripeToAvnPipe[2];   // StripePay -> AVN Generator
    
// Create the pipes
if (pipe(atcsToAvnPipe) < 0 || 
    pipe(avnToAirlinePipe) < 0 || 
    pipe(stripeToAvnPipe) < 0) {
    std::cerr << "Failed to create pipes. Exiting." << std::endl;
    return 1;
}
```

**Why Unnamed Pipes?**
1. **Process Relationship**: Since our processes have a clear parent-child relationship (forked from main), unnamed pipes are ideal.
2. **Data Flow Direction**: Communication is unidirectional, which unnamed pipes handle efficiently.
3. **Simplicity**: Easier to implement and debug compared to named pipes, message queues, or shared memory.
4. **Buffer Management**: Built-in buffering simplifies synchronization between sender and receiver.

### 2. Data Structures

#### ViolationData Struct

```cpp
// Used for passing violation information from ATCS Controller to AVN Generator
struct ViolationData {
    char flightNumber[10];  // Flight identifier (e.g., "PK123")
    char airLine[20];       // Airline name (e.g., "PIA", "FedEx")
    int speed;              // Recorded speed at violation
    int minAllowed;         // Minimum allowed speed for the state
    int maxAllowed;         // Maximum allowed speed for the state
};
```

**Why Fixed-Size Arrays for Strings?**
1. **Memory Layout**: Using fixed-size arrays ensures consistent memory layout across processes.
2. **Binary Transfer**: Allows direct binary transfer between processes without serialization.
3. **Size Control**: Prevents buffer overflow by enforcing size limits.

#### PaymentData Struct

```cpp
// Used for payment confirmations between StripePay and AVN Generator
struct PaymentData {
    char avnID[20];     // AVN identifier (e.g., "AVN-20250506-1234")
    bool paid;          // Payment status
};
```

#### AVN Class

```cpp
// Represents a complete Airspace Violation Notice
class AVN {
public:
    std::string avnID;                  // Unique identifier
    std::string flightNumber;           // Flight identifier
    std::string airLine;                // Airline name
    std::string aircraftType;           // Type of aircraft
    int speed;                          // Speed at violation
    std::pair<int, int> allowed;        // Permitted speed range
    time_t issueTime;                   // Timestamp
    std::string issueDate;              // Formatted date
    int fineAmount;                     // Base fine
    int serviceFee;                     // Administrative fee
    int totalAmount;                    // Total amount due
    bool paid;                          // Payment status
    std::string dueDate;                // Payment deadline
    
    // Methods
    void calculateFine();
    void generateAVNID();
    void calculateDueDate();
};
```

**Why These Data Types?**
1. **std::string for Text**: Provides convenient string manipulation for formatting and display.
2. **int for Monetary Values**: Using integers avoids floating-point precision issues with currency.
3. **std::pair for Range**: Efficiently stores min/max allowed speed as a logical unit.
4. **time_t for Timestamp**: Standard C++ type for time storage and calculation.

### 3. AVN Generator Process

The AVN Generator is implemented as a separate process (forked from main) to ensure isolation and robustness:

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

**Main Components of AVNGenerator Class:**

```cpp
class AVNGenerator {
private:
    std::atomic<bool> running;              // Control flag for main loop
    std::vector<AVN> avnList;               // List of all AVNs
    mutable std::mutex avnMutex;            // Protects avnList
    sem_t* avnSemaphore;                    // Inter-process synchronization
    int atcsToAvnPipe[2];                   // Pipe from ATCS Controller
    int avnToAirlinePipe[2];                // Pipe to Airline Portal
    int stripeToAvnPipe[2];                 // Pipe from StripePay

public:
    // Lifecycle methods
    AVNGenerator();
    ~AVNGenerator();
    bool initialize(int atcsToAvn[2], int avnToAirline[2], int stripeToAvn[2]);
    void run();
    void stop();
    
    // Processing methods
    void processViolation(const ViolationData& data);
    void processPayment(const PaymentData& data);
    void sendAVNToAirlinePortal(const AVN& avn);
    
    // Access methods
    std::vector<AVN> getAVNsByAirline(const std::string& airline);
    std::vector<AVN> getAllAVNs() const;
    AVN* findAVNByID(const std::string& avnID);
};
```

**Why These Design Choices?**

1. **std::atomic<bool> for Control Flag**:
   - Thread-safe boolean for stopping the process
   - Prevents race conditions when terminating

2. **std::mutex for Thread Safety**:
   - Protects `avnList` from concurrent access
   - Used with `std::lock_guard` for RAII-style locking
   - Ensures data integrity even with multiple violations arriving simultaneously

3. **Named Semaphore for Process Synchronization**:
   - Provides synchronization across different processes
   - Essential for payment status updates that may come from different processes
   - Created with `sem_open("/avn_semaphore", O_CREAT, 0644, 1)`

4. **select() for Efficient I/O Monitoring**:
   - Allows monitoring multiple file descriptors (pipes) simultaneously
   - Sets timeout to avoid busy waiting
   - More efficient than polling individual pipes

### 4. Violation Detection in ATCS Controller

The ATCS Controller continuously monitors aircraft speed and detects violations:

```cpp
void ATCScontroller::handleViolations() {
    // Get active flights
    const std::vector<Aircraft*>& activeFlights = scheduler.getActiveFlights();
    
    // Check each aircraft for violations
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
    }
}
```

### 5. AVN Processing Logic

When a violation is detected, the AVN Generator creates a new AVN:

```cpp
void AVNGenerator::processViolation(const ViolationData& data) {
    // Create new AVN
    AVN newAvn;
    
    // Populate data
    newAvn.flightNumber = data.flightNumber;
    newAvn.airLine = data.airLine;
    newAvn.speed = data.speed;
    newAvn.allowed = std::make_pair(data.minAllowed, data.maxAllowed);
    
    // Generate unique ID
    newAvn.generateAVNID();
    
    // Calculate fine based on aircraft type
    newAvn.calculateFine();
    
    // Add to list with mutex protection
    {
        std::lock_guard<std::mutex> lock(avnMutex);
        avnList.push_back(newAvn);
    }
    
    // Notify Airline Portal
    sendAVNToAirlinePortal(newAvn);
}
```

### 6. Fine Calculation Logic

The fine amount is determined based on the aircraft type:

```cpp
void AVN::calculateFine() {
    // Determine aircraft type based on airline
    if (airLine == "PIA" || airLine == "AirBlue") {
        aircraftType = "Commercial";
        fineAmount = 500000; // PKR 500,000 for Commercial
    } 
    else if (airLine == "FedEx" || airLine == "BlueDart") {
        aircraftType = "Cargo";
        fineAmount = 700000; // PKR 700,000 for Cargo
    } 
    else if (airLine == "PakistanAirforce" || airLine == "AghaKhanAir") {
        aircraftType = "Emergency";
        fineAmount = 700000; // Same fine as cargo for emergency
    }
    
    // Calculate 15% administrative fee
    serviceFee = static_cast<int>(fineAmount * 0.15);
    
    // Set total amount
    totalAmount = fineAmount + serviceFee;
}
```

### 7. Payment Processing

When a payment is received from StripePay, the AVN Generator updates the payment status:

```cpp
void AVNGenerator::processPayment(const PaymentData& data) {
    // Wait on semaphore for cross-process synchronization
    sem_wait(avnSemaphore);
    
    // Use mutex for thread safety within this process
    std::lock_guard<std::mutex> lock(avnMutex);
    
    // Find the AVN by ID
    AVN* avn = findAVNByID(data.avnID);
    if (avn != nullptr) {
        // Update payment status
        avn->paid = data.paid;
        
        // Notify Airline Portal of update
        sendAVNToAirlinePortal(*avn);
    }
    
    // Release semaphore
    sem_post(avnSemaphore);
}
```

## Complete Flow: From Violation to Payment

1. **Violation Detection**:
   - ATCS Controller's `handleViolations()` method continuously monitors aircraft speeds
   - When an aircraft exceeds its state-specific speed limits, a violation is detected
   - Aircraft is marked with `hasActiveViolation = true` to prevent duplicate violations

2. **Violation Data Transfer**:
   - ATCS Controller creates a `ViolationData` struct with flight details and speed information
   - Data is written to the `atcsToAvnPipe` using `write()`
   - Binary data transfers directly to the AVN Generator process

3. **AVN Generation**:
   - AVN Generator's main loop uses `select()` to monitor the pipe for incoming data
   - When data arrives, `processViolation()` is called
   - A new `AVN` object is created with:
     - Generated unique ID (format: "AVN-YYYYMMDD-XXXX")
     - Flight information from violation data
     - Calculated fine amount based on aircraft type
     - 15% administrative service fee
     - Due date (3 days from issuance)
     - Default payment status (unpaid)

4. **AVN Notification**:
   - New AVN is added to the `avnList` (protected by mutex)
   - AVN Generator sends notification to Airline Portal through `avnToAirlinePipe`

5. **Airline Portal Display** (Future Implementation):
   - Airline Portal receives AVN data and displays it to airline representatives
   - Airlines can filter AVNs by airline, date, or payment status
   - Portal provides interface to initiate payment via StripePay

6. **Payment Process** (Future Implementation):
   - Airline representative selects an AVN and clicks "Pay"
   - Payment details are sent to StripePay process
   - StripePay confirms payment and sends confirmation via `stripeToAvnPipe`

7. **Payment Confirmation**:
   - AVN Generator receives payment confirmation in `PaymentData` struct
   - `processPayment()` updates the AVN's payment status (protected by semaphore and mutex)
   - Updated status is sent to Airline Portal
   - ATCS Controller is notified that violation has been cleared

8. **Record Maintenance**:
   - All AVNs remain in the system for historical tracking
   - Airlines can view both active and historical AVNs

## Testing Mechanism

A test mode in main.cpp simulates violations for testing without the full simulation:

```cpp
void runViolationTests(int atcsToAvnPipeWrite) {
    // Test Case 1: Commercial aircraft (PIA) speed violation
    {
        ViolationData testViolation1;
        std::strncpy(testViolation1.flightNumber, "PK123", sizeof(testViolation1.flightNumber) - 1);
        testViolation1.flightNumber[sizeof(testViolation1.flightNumber) - 1] = '\0';
        
        std::strncpy(testViolation1.airLine, "PIA", sizeof(testViolation1.airLine) - 1);
        testViolation1.airLine[sizeof(testViolation1.airLine) - 1] = '\0';
        
        testViolation1.speed = 650;  // Exceeding limit
        testViolation1.minAllowed = 400;
        testViolation1.maxAllowed = 600;
        
        write(atcsToAvnPipeWrite, &testViolation1, sizeof(ViolationData));
    }
    
    // Additional test cases...
}
```

## Synchronization Considerations

1. **Mutex for Thread Safety**:
   - The `avnMutex` protects the shared `avnList` from concurrent access
   - Used in all methods that read from or write to the list
   - Ensures data integrity within the process

2. **Semaphore for Process Synchronization**:
   - The named semaphore (`avnSemaphore`) provides synchronization across different processes
   - Critical when handling payment updates that affect both AVN Generator and Airline Portal

3. **Pipe Buffering**:
   - Unnamed pipes provide built-in buffering
   - If AVN Generator falls behind, data is queued in the pipe's buffer
   - Prevents data loss during high-volume periods

4. **select() Timeout**:
   - The `select()` call in the AVN Generator's main loop includes a timeout
   - Ensures the process can check its control flag periodically
   - Allows clean shutdown when requested

## Design Benefits

1. **Modularity**:
   - Each component (ATCS Controller, AVN Generator, etc.) has clear responsibilities
   - Components communicate through well-defined interfaces (pipes)
   - Easy to extend or modify individual components

2. **Robustness**:
   - Process isolation prevents failures in one component from affecting others
   - Proper synchronization prevents data corruption
   - Error handling for pipe operations and AVN processing

3. **Scalability**:
   - Can handle multiple violations simultaneously
   - Adding more features (e.g., violation types) requires minimal changes

## Conclusion

The AVN Generation system in AirControlX provides a robust mechanism for detecting, recording, and processing airspace violations. By using a multi-process architecture with unnamed pipes for IPC, the system ensures proper separation of concerns while maintaining efficient data flow. The comprehensive data structures and synchronization mechanisms ensure data integrity and reliable operation even under high load conditions.

The design prioritizes:
- Clean separation between violation detection and AVN management
- Thread and process safety through proper synchronization
- Clear data flow between system components
- Extensibility for future enhancements