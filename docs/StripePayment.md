# StripePay System Documentation

## Overview

The StripePay system is a critical component of AirControlX that handles the payment processing for Airspace Violation Notices (AVNs). It operates as an independent process that communicates with the AirlinePortal and AVN Generator processes through Inter-Process Communication (IPC) mechanisms. This document explains the complete flow of the payment processing system, the technical implementation details, and the reasoning behind various design decisions.

## System Architecture

The StripePay system follows a multi-process design to ensure separation of concerns and robust inter-process communication:

### Process Structure

1. **Main Process (Parent)**:
   - Manages all child processes
   - Creates pipes for inter-process communication
   - Handles clean termination of processes

2. **AVN Generator Process**:
   - Creates and manages AVN documents
   - Receives payment confirmations from StripePay
   - Updates payment status of AVNs

3. **AirlinePortal Process**:
   - Displays AVNs to airline representatives
   - Manages airline account balances
   - Initiates payment requests to StripePay
   - Receives payment status updates from AVN Generator

4. **StripePay Process**:
   - Receives payment requests from AirlinePortal
   - Provides admin approval interface for payments
   - Processes payments (simulated in current implementation)
   - Sends payment confirmations to AVN Generator

## Data Flow Diagram

```
                  Account Balance Check
                  Payment Verification      Payment Approval
                  ┌───────────┐        ┌────────────────────┐
                  │           │        │                    │
┌─────────────────┐      Payment Request      ┌─────────────────┐
│  Airline Portal ├─────────────────────────> │    StripePay    │
└─────────┬───────┘                           └────────┬────────┘
          ▲                                           │
          │                                           │
          │                                           │
          │                                           │
          │                                           │
          │ Updated AVN Data                          │ Payment Confirmation
          │                                           │
          │                                           │
┌─────────┴───────┐                           ┌──────▼─────────┐
│  AVN Generator  │ <─────────────────────────┤  AVN Generator  │
└─────────────────┘                           └─────────────────┘
```

## Technical Implementation Details

### 1. Enhanced Inter-Process Communication (IPC)

The StripePay process communicates with other processes using unnamed pipes with an enhanced PaymentData struct:

```cpp
// Creation of pipes in main.cpp
int airlineToStripePipe[2];  // Airline Portal -> StripePay
int stripeToAvnPipe[2];      // StripePay -> AVN Generator
    
// Create the pipes
if (pipe(airlineToStripePipe) < 0 || 
    pipe(stripeToAvnPipe) < 0) {
    std::cerr << "Failed to create payment pipes. Exiting." << std::endl;
    return 1;
}
```

**Why Unnamed Pipes?**
1. **Process Relationship**: Since our processes have a clear parent-child relationship (forked from main), unnamed pipes are ideal
2. **Data Flow Direction**: Communication is unidirectional, which unnamed pipes handle efficiently
3. **Simplicity**: Easier to implement and debug compared to other IPC mechanisms
4. **Buffer Management**: Built-in buffering simplifies synchronization between sender and receiver

### 2. Enhanced PaymentData Struct

The PaymentData struct has been expanded to include all required fields from Requirements.md Section 5:

```cpp
/**
 * This struct defines the data format for payment notifications
 * sent between processes relating to AVN payment status.
 */
struct PaymentData 
{
    char avnID[30];         // Unique AVN identifier
    char flightNumber[20];  // Aircraft ID (e.g., "PK123")
    char aircraftType[20];  // Type of aircraft (e.g., "Commercial", "Cargo", "Emergency")
    int amountToPay;        // Amount to be paid
    int amountPaid;         // Amount that was actually paid
    bool paid;              // Payment status (true if paid)
};
```

**Design Considerations for Enhanced PaymentData:**
1. **Fixed-Size Arrays**: Ensures consistent memory layout for reliable binary transfer between processes
2. **Binary Type**: Using a struct allows direct binary transfer without serialization
3. **Amount Fields**: Two separate fields track both the required amount and the actual amount paid
4. **Complete Information**: Contains all data required by the StripePay system according to requirements

### 3. Airline Account Balance System

The AirlinePortal now includes a comprehensive account balance management system:

```cpp
/**
 * Structure to hold airline account information
 * This keeps track of each airline's financial status
 */
struct AirlineAccount
{
    int balance;         // Current balance in PKR
    int totalFines;      // Total fines accumulated
    int totalPaid;       // Total amount paid
};

// In AirlinePortal class
private:
    // Airline account balances - maps airline name to account details
    std::map<std::string, AirlineAccount> airlineAccounts;
    std::mutex accountsMutex;
```

**Benefits of the Account System:**
1. **Financial Tracking**: Maintains accurate balance information for each airline
2. **Thread Safety**: Protected by mutex to prevent concurrent access issues
3. **Complete Financial Overview**: Tracks total fines, payments, and current balance

### 4. StripePay Process Architecture Enhancement

The StripePay process has been enhanced with a queue system and user interface for payment approval:

```cpp
/**
 * Structure to store pending payment information
 * This keeps track of payment requests that require admin approval
 */
struct PendingPayment
{
    std::string avnID;          // The AVN ID being paid for
    std::string flightNumber;   // The flight number involved
    std::string aircraftType;   // Type of aircraft (Commercial/Cargo/etc)
    int amountToPay;            // Total amount to be paid
    int amountPaid;             // Amount actually paid
};

class StripePayment 
{
private:
    // File descriptors for pipes
    int airlineToStripePipe[2];  // Airline Portal -> StripePay
    int stripeToAvnPipe[2];      // StripePay -> AVN Generator
    
    // Control flag for process loop
    std::atomic<bool> running;
    
    // Mutex for thread safety
    std::mutex paymentMutex;
    
    // Queue of pending payments requiring admin approval
    std::queue<PendingPayment> pendingPayments;
    std::mutex pendingPaymentsMutex;
    
    // Record of completed payments
    std::vector<PendingPayment> paymentHistory;
    
    // Payment gateway simulation functions
    std::string generateTransactionID();
    void simulatePaymentProcessing(const std::string& avnID, int amount);

public:
    // User interface methods
    void showMenu();
    void displayPendingPayments();
    void approvePayment(int index);
    void viewPaymentHistory();
    
    // ... other methods ...
};
```

**Design Choices Explained:**

1. **Pending Payment Queue**:
   - Stores payment requests awaiting admin approval
   - Implements the requirement for admin approval before finalizing payment
   - Protected by mutex for thread safety

2. **Payment History Vector**:
   - Maintains a record of all completed payments
   - Provides audit trail for payment verification
   - Accessible through the admin interface

3. **Transaction ID Generation**:
   - Creates unique identifiers for each payment transaction
   - Uses high-resolution clock for randomness
   - Provides traceability for payment processing

4. **Interactive Admin Interface**:
   - Menu-driven command line interface for payment administration
   - Allows viewing pending payments, approving specific payments, and viewing history
   - Fulfills the requirement for admin approval of payments

### 5. Payment Request Processing

The enhanced StripePay process now handles payment requests with admin approval:

```cpp
// Process a payment request from AirlinePortal
void StripePayment::processPaymentRequest(const PaymentData& data)
{
    std::cout << "=== Payment Request Received ===" << std::endl;
    std::cout << "AVN ID: " << data.avnID << std::endl;
    std::cout << "Flight Number: " << data.flightNumber << std::endl;
    std::cout << "Aircraft Type: " << data.aircraftType << std::endl;
    std::cout << "Amount To Pay: PKR " << data.amountToPay << std::endl;
    std::cout << "Amount Paid: PKR " << data.amountPaid << std::endl;
    std::cout << "Status: " << (data.paid ? "Already Paid" : "Payment Requested") << std::endl;
    
    // If it's already paid, nothing to do
    if (data.paid)
    {
        std::cout << "AVN " << data.avnID << " is already paid. No further action needed." << std::endl;
        return;
    }
    
    // Add the payment request to pending payments queue
    {
        std::lock_guard<std::mutex> lock(pendingPaymentsMutex);
        
        PendingPayment payment;
        payment.avnID = data.avnID;
        payment.flightNumber = data.flightNumber;
        payment.aircraftType = data.aircraftType;
        payment.amountToPay = data.amountToPay;
        payment.amountPaid = data.amountPaid;
        
        pendingPayments.push(payment);
        
        std::cout << "Payment request added to pending queue. Waiting for admin approval." << std::endl;
    }
    
    // Alert the admin about the new payment request
    std::cout << "\n*** New payment request received! ***" << std::endl;
    std::cout << "Check the pending payments menu to approve." << std::endl;
}
```

### 6. Payment Approval Workflow

The StripePay system now includes an admin approval interface:

```cpp
// Approve a pending payment
void StripePayment::approvePayment(int index)
{
    std::lock_guard<std::mutex> lock(pendingPaymentsMutex);
    
    if (pendingPayments.empty())
    {
        std::cout << "No pending payments to approve." << std::endl;
        return;
    }
    
    if (index < 1 || index > static_cast<int>(pendingPayments.size()))
    {
        std::cout << "Invalid payment index." << std::endl;
        return;
    }
    
    // Create a temporary queue to find the requested payment
    std::queue<PendingPayment> tempQueue;
    PendingPayment selectedPayment;
    int currentIndex = 1;
    
    while (!pendingPayments.empty())
    {
        PendingPayment payment = pendingPayments.front();
        pendingPayments.pop();
        
        if (currentIndex == index)
        {
            selectedPayment = payment;
        }
        else
        {
            tempQueue.push(payment);
        }
        
        currentIndex++;
    }
    
    // Restore all payments except the selected one
    pendingPayments = tempQueue;
    
    // Process the selected payment
    std::cout << "\n=== Payment Details ===" << std::endl;
    std::cout << "AVN ID: " << selectedPayment.avnID << std::endl;
    std::cout << "Flight: " << selectedPayment.flightNumber << std::endl;
    std::cout << "Aircraft Type: " << selectedPayment.aircraftType << std::endl;
    std::cout << "Amount Due: PKR " << selectedPayment.amountToPay << std::endl;
    std::cout << "Amount Paid: PKR " << selectedPayment.amountPaid << std::endl;
    
    // Ask for admin approval
    std::cout << "\nApprove this payment? (y/n): ";
    std::string confirmation;
    std::getline(std::cin, confirmation);
    
    if (confirmation != "y" && confirmation != "Y")
    {
        std::cout << "Payment not approved. Returning to pending queue." << std::endl;
        pendingPayments.push(selectedPayment);
        return;
    }
    
    // Process the payment
    {
        std::lock_guard<std::mutex> paymentLock(paymentMutex);
        
        // Simulate payment processing
        simulatePaymentProcessing(selectedPayment.avnID, selectedPayment.amountPaid);
        
        // Add to payment history
        paymentHistory.push_back(selectedPayment);
        
        // Send confirmation to AVN Generator
        sendPaymentConfirmation(
            selectedPayment.avnID,
            selectedPayment.flightNumber,
            selectedPayment.aircraftType,
            selectedPayment.amountPaid
        );
    }
    
    std::cout << "Payment for AVN " << selectedPayment.avnID << " approved and processed successfully." << std::endl;
}
```

### 7. Enhanced Payment Confirmation

The payment confirmation now includes all the required fields:

```cpp
// Send payment confirmation to AVN Generator with all required fields
void StripePayment::sendPaymentConfirmation(
    const std::string& avnID, 
    const std::string& flightNumber,
    const std::string& aircraftType,
    int amountPaid)
{
    // Create payment confirmation data
    PaymentData confirmationData;
    
    // Copy the AVN ID to the struct with proper string handling
    strncpy(confirmationData.avnID, avnID.c_str(), sizeof(confirmationData.avnID) - 1);
    confirmationData.avnID[sizeof(confirmationData.avnID) - 1] = '\0';
    
    // Copy the flight number
    strncpy(confirmationData.flightNumber, flightNumber.c_str(), sizeof(confirmationData.flightNumber) - 1);
    confirmationData.flightNumber[sizeof(confirmationData.flightNumber) - 1] = '\0';
    
    // Copy the aircraft type
    strncpy(confirmationData.aircraftType, aircraftType.c_str(), sizeof(confirmationData.aircraftType) - 1);
    confirmationData.aircraftType[sizeof(confirmationData.aircraftType) - 1] = '\0';
    
    // Set amount information
    confirmationData.amountPaid = amountPaid;
    
    // Set the payment status to true (payment successful)
    confirmationData.paid = true;
    
    // Send confirmation to AVN Generator
    ssize_t bytesWritten = write(stripeToAvnPipe[1], &confirmationData, sizeof(PaymentData));
    
    if (bytesWritten != sizeof(PaymentData))
    {
        std::cerr << "Error sending payment confirmation to AVN Generator: " << strerror(errno) << std::endl;
    }
    else
    {
        std::cout << "Payment confirmation sent to AVN Generator for AVN " << avnID << std::endl;
    }
}
```

## Complete Enhanced Payment Flow

1. **Balance Management**:
   - AirlinePortal maintains account balances for each airline
   - Initial balances are set during system initialization
   - Airlines can deposit additional funds as needed
   - Balance, total fines, and payment history are tracked for each airline

2. **Payment Initiation**:
   - AirlinePortal displays list of unpaid AVNs to airline representatives
   - Airline representative selects an AVN for payment
   - System checks if airline has sufficient account balance
   - If sufficient, amount is deducted from airline's balance
   - AirlinePortal calls `sendPaymentRequest()` with the AVN ID and amount

3. **Payment Request Transfer**:
   - AirlinePortal creates an enhanced `PaymentData` struct with:
     - AVN ID, Flight Number, and Aircraft Type
     - Amount to pay and amount paid
     - Payment status set to `false` (indicating payment request)
   - Data is written to `airlineToStripePipe` using `write()`
   - Binary data transfers directly to the StripePay process

4. **Payment Request Queue**:
   - StripePay receives payment request and adds it to the pending payments queue
   - Admin is notified of the new payment request
   - Request waits in queue until admin reviews and approves it

5. **Admin Payment Review and Approval**:
   - StripePay admin uses menu interface to view pending payments
   - Admin selects a specific payment for review
   - System displays complete payment details
   - Admin decides to approve or reject the payment
   - If rejected, payment returns to queue for later processing

6. **Payment Processing**:
   - For approved payments, system calls `simulatePaymentProcessing()`
   - A unique transaction ID is generated for the payment
   - Transaction details are displayed to admin
   - Payment is marked as successful and added to payment history

7. **Enhanced Payment Confirmation**:
   - StripePay creates a new enhanced `PaymentData` struct with:
     - Complete AVN ID, Flight Number, and Aircraft Type
     - Amount paid information
     - Payment status set to `true` (indicating successful payment)
   - Confirmation is written to `stripeToAvnPipe`
   - Data transfers directly to the AVN Generator process

8. **Payment Status Update**:
   - AVN Generator receives the confirmation through its main loop
   - `processPayment()` method updates the AVN status
   - Payment status is updated to `paid = true`
   - Updated AVN information is sent to the AirlinePortal
   - AirlinePortal updates its display to show the paid status

9. **Financial Record Updates**:
   - AirlinePortal updates the airline's financial records
   - Total paid amount is increased
   - Outstanding balance is recalculated

## User Interface Features

### AirlinePortal Interface

The AirlinePortal includes these UI features for airline representatives:

```
=====================================
      AIRLINE PORTAL - MAIN MENU       
=====================================
1. View All AVNs
2. View AVNs by Airline
3. View Unpaid AVNs
4. View Paid AVNs
5. Pay an AVN
6. View All Airline Balances
7. View Specific Airline Balance
8. Deposit Funds
0. Exit
=====================================
```

Key functionalities:
- View and filter AVNs by various criteria
- Check account balances and payment history
- Make payments for outstanding AVNs
- Deposit funds to airline accounts

### StripePay Admin Interface

The StripePay system provides an admin interface:

```
=====================================
      STRIPEPAY - PAYMENT PORTAL      
=====================================
1. View Pending Payments (2)
2. Approve Payment
3. View Payment History
0. Exit
=====================================
```

Key functionalities:
- View queue of pending payment requests
- Review and approve individual payments
- View complete payment history
- Monitor transaction details

## Transaction Processing

The StripePay system includes enhanced transaction processing:

1. **Transaction ID Generation**:
   - Uses high-resolution clock and random number generation
   - Creates unique IDs in the format: TXPAY-YYYYMMDD-XXXXXX
   - Ensures traceability of all payment transactions

2. **Simulated Payment Processing**:
   - Represents the steps of a real payment gateway
   - Shows validation, gateway connection, processing, and finalization
   - Provides visual feedback of the payment process for the admin

```cpp
// Simulate payment processing (in a real system, this would connect to a payment gateway)
void StripePayment::simulatePaymentProcessing(const std::string& avnID, int amount)
{
    // Generate a unique transaction ID
    std::string transactionId = generateTransactionID();
    
    std::cout << "\n=== Processing Payment ===" << std::endl;
    std::cout << "Transaction ID: " << transactionId << std::endl;
    std::cout << "AVN ID: " << avnID << std::endl;
    std::cout << "Amount: PKR " << amount << std::endl;
    
    // Simulate the steps of payment processing
    std::cout << "Validating payment details..." << std::endl;
    sleep(1);
    
    std::cout << "Connecting to payment gateway..." << std::endl;
    sleep(1);
    
    std::cout << "Processing transaction..." << std::endl;
    sleep(1);
    
    std::cout << "Finalizing payment..." << std::endl;
    sleep(1);
    
    std::cout << "Payment successfully processed!" << std::endl;
    std::cout << "Transaction ID: " << transactionId << std::endl;
}
```

## Synchronization and Thread Safety

1. **Multiple Mutex Protection**:
   - `paymentMutex`: Protects core payment processing
   - `pendingPaymentsMutex`: Protects the pending payment queue
   - `accountsMutex`: Protects airline account balance management

2. **Queue Management**:
   - Thread-safe operations on payment queues
   - Prevents race conditions during payment approval
   - Ensures data integrity with proper locking

3. **Atomic Control Flags**:
   - Uses `std::atomic<bool>` for process control flags
   - Ensures clean shutdown without race conditions

## Design Benefits

1. **Complete Requirements Implementation**:
   - Fulfills all requirements from Section 5 of Requirements.md
   - Includes AVNID, Aircraft ID, aircraft type, and payment amount
   - Implements user input mechanism for payment approval

2. **Enhanced Financial Management**:
   - Complete airline account tracking
   - Balance verification before payment processing
   - Financial history and reporting

3. **Improved Admin Control**:
   - Payment requests require explicit admin approval
   - Admin can review complete payment details
   - Full transaction history is maintained

4. **Robust Error Handling**:
   - Validation at each step of the payment process
   - Proper error messages for payment failures
   - Thread-safe operations prevent data corruption

5. **Flexible Architecture**:
   - Modular design separates UI, payment processing, and financial management
   - Easy to extend with additional payment methods or features
   - Clear separation of concerns between processes

## Future Enhancements

1. **Advanced Financial Features**:
   - Payment plans for large fines
   - Automatic balance alerts and notifications
   - Financial reporting and analytics

2. **Enhanced User Experience**:
   - Graphical user interfaces for both airline and admin portals
   - Real-time payment status tracking
   - Email notifications for payment events

3. **Security Improvements**:
   - Role-based access control for administrative functions
   - Encryption for sensitive payment data
   - Comprehensive audit logging

4. **Real Payment Gateway**:
   - Integration with actual payment processors
   - Support for multiple payment methods
   - Compliance with financial regulations

## Conclusion

The enhanced StripePay system in AirControlX provides a robust mechanism for processing payments related to airspace violations, with complete airline account management and admin approval workflows. The implementation fully satisfies the requirements specified in Section 5 of Requirements.md, providing:

1. A complete payment system where airline admins can pay challans through StripePay
2. Enhanced data transfer including AVNID, Aircraft ID, aircraft type, and payment amount
3. User input mechanisms for payment approval and account management
4. Proper notifications to both AVN Generator and Airline Portal upon successful payment

The modular design, thorough synchronization, and clear separation of responsibilities ensure that the payment system is reliable, secure, and maintainable. While the current implementation simulates the payment gateway, the architecture is designed to be easily extended with real payment processing in the future.