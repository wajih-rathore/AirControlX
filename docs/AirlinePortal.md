# Airline Portal System Documentation

## Overview

The Airline Portal is a crucial component of AirControlX that serves as an interface for airlines to view and manage their Airspace Violation Notices (AVNs). It allows airline representatives to monitor violation notices, filter them by different criteria, and initiate payments for fines. This document explains the complete functionality of the Airline Portal, its technical implementation details, and the reasoning behind design decisions.

## System Architecture

The Airline Portal operates within the multi-process design of AirControlX, ensuring separation of concerns while maintaining smooth inter-process communication:

### Process Structure

1. **AVN Generator Process**:
   - Creates and manages AVN documents
   - Sends AVN information to the Airline Portal via pipes
   - Updates AVN payment status

2. **Airline Portal Process**:
   - Receives AVN data from AVN Generator
   - Displays AVNs to airline representatives
   - Provides filtering and management capabilities
   - Forwards payment requests to StripePay

3. **StripePay Process**:
   - Processes payment requests from Airline Portal
   - Sends payment confirmations to AVN Generator

## Data Flow Diagram

```
┌─────────────────┐                    ┌─────────────────┐
│  AVN Generator  ├───── AVN Data ────>│  Airline Portal │
└─────────────────┘                    └────────┬────────┘
        ▲                                       │
        │                                       │
        │                                       │
        │                                       │
        │ Payment Status                        │ Payment Request
        │                                       │
        │                                       ▼
┌───────┴─────────┐                     ┌─────────────────┐
│    StripePay    │<────────────────────┤     Airline     │
└─────────────────┘                     └─────────────────┘
```

## Technical Implementation Details

### 1. Inter-Process Communication (IPC)

The Airline Portal uses unnamed pipes for communication with the AVN Generator and StripePay processes. This choice ensures reliable data transfer with built-in buffering and synchronization:

```cpp
// Pipes for communication
int avnToAirlinePipe[2];           // Pipe from AVN Generator to Airline Portal
int airlineToStripePipe[2];        // Pipe from Airline Portal to StripePay
```

**Why Unnamed Pipes?**
1. **Process Relationship**: All processes are created by the main process, making unnamed pipes ideal for communication.
2. **Data Flow Direction**: Each pipe provides a one-way communication channel, which matches our needs perfectly.
3. **Built-in Buffering**: Pipes handle buffering automatically, preventing data loss if the receiver is busy.
4. **Simplicity**: Easier to implement and maintain than more complex IPC mechanisms.

### 2. Data Structures

#### PaymentData Struct

```cpp
// Used for payment communication between processes
struct PaymentData {
    char avnID[20];     // AVN identifier (e.g., "AVN-20250506-1234")
    bool paid;          // Payment status flag
};
```

**Why This Design?**
1. **Fixed-Size Arrays**: Ensures consistent memory layout for binary transfer between processes.
2. **Minimalist Design**: Contains only the essential information needed for payment processing.

#### AVN Class

The Airline Portal uses the same AVN class as the AVN Generator, ensuring data consistency across the system:

```cpp
class AVN {
public:
    std::string avnID;              // Unique identifier
    std::string flightNumber;       // Flight identifier  
    std::string airLine;            // Airline name
    std::string aircraftType;       // Type of aircraft
    int speed;                      // Speed at violation
    int totalAmount;                // Total amount due
    bool paid;                      // Payment status
    std::string dueDate;            // Payment deadline
    // Additional fields...
};
```

### 3. Thread Safety Implementation

The Airline Portal uses mutex locks to ensure thread safety when accessing shared resources:

```cpp
// Mutex to protect AVN list during concurrent access
std::mutex avnMutex;

// Example usage in a method
void AirlinePortal::processReceivedAVN(const PaymentData& data)
{
    // Lock the AVN list for thread safety
    // std::lock_guard is a mutex wrapper that provides RAII-style locking
    std::lock_guard<std::mutex> lock(avnMutex);
    
    // Access or modify the AVN list safely
    // ...
}
```

**Why This Approach?**
1. **RAII Principle**: The `std::lock_guard` automatically handles locking and unlocking when the scope is entered and exited.
2. **Deadlock Prevention**: By consistently using the same locking pattern, we minimize deadlock risks.
3. **Concurrency Protection**: Ensures that the AVN list is never accessed simultaneously by multiple threads.

### 4. Main Loop Implementation

The Airline Portal's main process loop uses `select()` to efficiently monitor incoming AVN data:

```cpp
void AirlinePortal::run()
{
    // Set the running flag
    running.store(true);
    
    // Main loop - continue until stopped
    while (running.load())
    {
        // Set up file descriptor monitoring
        fd_set readFds;
        FD_ZERO(&readFds);
        FD_SET(avnToAirlinePipe[0], &readFds);
        
        // Set timeout (0.5 seconds)
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;
        
        // Wait for data or timeout
        int maxFd = avnToAirlinePipe[0] + 1;
        int activity = select(maxFd, &readFds, NULL, NULL, &timeout);
        
        // Process incoming data if available
        if (activity > 0 && FD_ISSET(avnToAirlinePipe[0], &readFds))
        {
            PaymentData data;
            if (read(avnToAirlinePipe[0], &data, sizeof(PaymentData)) == sizeof(PaymentData))
            {
                processReceivedAVN(data);
            }
        }
    }
}
```

**Why These Design Choices?**
1. **`select()` for I/O Monitoring**: Allows efficient waiting for data without consuming CPU.
2. **Timeout Setting**: The 0.5-second timeout ensures the process can check its control flag periodically.
3. **Atomic Control Flag**: Using `std::atomic<bool>` for the running flag provides thread-safe termination control.

### 5. User Interface Methods

The Airline Portal provides several methods for viewing and filtering AVNs:

```cpp
// View all violation notices
void AirlinePortal::viewAllAVNs() const;

// View AVNs filtered by airline
void AirlinePortal::viewAVNsByAirline(const std::string& airline) const;

// View AVNs filtered by payment status
void AirlinePortal::viewAVNsByStatus(bool paid) const;

// Initiate payment for an AVN
bool AirlinePortal::initiatePayment(const std::string& avnID);
```

These methods use formatted console output to display AVN information in a readable tabular format:

```cpp
void AirlinePortal::viewAllAVNs() const
{
    // Lock for thread safety
    std::lock_guard<std::mutex> lock(avnMutex);
    
    // Display header
    std::cout << "\n============ All Airspace Violation Notices ============\n" << std::endl;
    std::cout << std::left 
              << std::setw(14) << "AVN ID" 
              << std::setw(12) << "Flight" 
              << std::setw(15) << "Airline"
              << std::setw(12) << "Fine (PKR)" 
              << std::setw(12) << "Status"
              << std::setw(14) << "Due Date"
              << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    // Display each AVN
    for (const AVN& avn : avnList)
    {
        std::cout << std::left 
                  << std::setw(14) << avn.avnID 
                  << std::setw(12) << avn.flightNumber 
                  << std::setw(15) << avn.airLine
                  << std::setw(12) << avn.totalAmount
                  << std::setw(12) << (avn.paid ? "PAID" : "UNPAID")
                  << std::setw(14) << avn.dueDate
                  << std::endl;
    }
}
```

### 6. Payment Processing

The payment workflow involves multiple components:

1. **Payment Initiation**:
   ```cpp
   bool AirlinePortal::initiatePayment(const std::string& avnID)
   {
       // Find the AVN
       AVN* avn = findAVNByID(avnID);
       if (avn == nullptr || avn->paid)
       {
           // Error handling
           return false;
       }
       
       // Send payment request to StripePay
       sendPaymentRequest(avnID);
       return true;
   }
   ```

2. **Payment Request Transmission**:
   ```cpp
   void AirlinePortal::sendPaymentRequest(const std::string& avnID)
   {
       // Create payment request
       PaymentData paymentRequest;
       std::strncpy(paymentRequest.avnID, avnID.c_str(), sizeof(paymentRequest.avnID) - 1);
       paymentRequest.avnID[sizeof(paymentRequest.avnID) - 1] = '\0';
       paymentRequest.paid = false;
       
       // Send through pipe to StripePay
       write(airlineToStripePipe[1], &paymentRequest, sizeof(PaymentData));
   }
   ```

3. **Payment Confirmation Reception**:
   - The Airline Portal receives payment status updates through the `avnToAirlinePipe`.
   - These updates come from the AVN Generator after it processes confirmations from StripePay.
   - The received data is processed by `processReceivedAVN()`.

## Complete Flow: From AVN Notification to Payment

1. **AVN Notification Reception**:
   - The AVN Generator creates a new AVN and sends it to the Airline Portal
   - Data is transmitted through `avnToAirlinePipe` as a `PaymentData` struct
   - Airline Portal's main loop detects incoming data using `select()`

2. **AVN Processing**:
   - `processReceivedAVN()` is called to handle the incoming data
   - If the AVN already exists, its payment status is updated
   - If it's a new AVN, it's added to the `avnList`

3. **User Interface Display**:
   - Airline representatives can view all AVNs using `viewAllAVNs()`
   - They can filter AVNs by airline using `viewAVNsByAirline()`
   - They can filter by payment status using `viewAVNsByStatus()`

4. **Payment Initiation**:
   - User selects an unpaid AVN and calls `initiatePayment()` with its ID
   - Method validates the AVN exists and is unpaid
   - Then calls `sendPaymentRequest()` to transmit the request

5. **Payment Processing**:
   - Request is sent to StripePay through `airlineToStripePipe`
   - StripePay processes the payment (external process)
   - Payment confirmation is sent to AVN Generator

6. **Status Update**:
   - AVN Generator updates the AVN's payment status
   - Updated status is sent back to Airline Portal
   - Airline Portal receives the update and processes it using `processReceivedAVN()`
   - The AVN in `avnList` is updated to reflect the new payment status

## Design Benefits

1. **Modularity**:
   - Clear separation between AVN generation, display, and payment processing
   - Each component has well-defined responsibilities
   - Clean interfaces between components via pipes

2. **Thread Safety**:
   - Consistent use of mutexes to protect shared resources
   - RAII-style locking with `std::lock_guard`
   - Atomic flags for process control

3. **Efficient Communication**:
   - Direct binary transfer of data between processes
   - No need for serialization or complex protocols
   - Built-in buffering through pipes

4. **User-Friendly Interface**:
   - Clear tabular display of AVNs
   - Multiple filtering options
   - Simple payment initiation

5. **Error Handling**:
   - Validation of AVNs before payment
   - Proper error checking for pipe operations
   - Clear error messages for users

## Conclusion

The Airline Portal component plays a vital role in the AirControlX system by providing airlines with a clear view of their violations and an interface for managing payments. Through efficient inter-process communication with the AVN Generator and StripePay components, it ensures that airlines always have up-to-date information about their violation notices and payment statuses.

The implementation prioritizes:
- Clean user interface for airline representatives
- Efficient data transfer between system components
- Thread safety and data integrity
- Simple but effective payment workflow

This design allows the Airline Portal to handle multiple concurrent AVNs while maintaining data consistency and providing a responsive interface for airline representatives.