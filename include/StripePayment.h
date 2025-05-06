#ifndef AIRCONTROLX_STRIPEPAYMENT_H
#define AIRCONTROLX_STRIPEPAYMENT_H

#include <string>
#include <atomic>
#include <mutex>
#include <vector>
#include <queue>
#include <map>
#include "AVN.h"

// Forward declaration for PaymentData
struct PaymentData;

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

/**
 * StripePayment class for handling violation payments.
 * Processes payments for Airspace Violation Notices (AVNs).
 */
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
    // Constructors and destructors
    StripePayment();
    ~StripePayment();
    
    // Initialize with pipe file descriptors
    bool initialize(int airlineToStripe[2], int stripeToAvn[2]);
    
    // Main process loop
    void run();
    
    // Gracefully stop the process
    void stop();
    
    // Process a payment request from AirlinePortal
    void processPaymentRequest(const PaymentData& data);
    
    // Process payment for a violation
    bool processPayment(std::string avnID, int amount);
    
    // Send payment confirmation to AVN Generator
    void sendPaymentConfirmation(const std::string& avnID, const std::string& flightNumber, 
                                const std::string& aircraftType, int amountPaid);
                                
    // User interface methods
    void showMenu();
    void displayPendingPayments();
    void approvePayment(int index);
    void viewPaymentHistory();
};

#endif // AIRCONTROLX_STRIPEPAYMENT_H