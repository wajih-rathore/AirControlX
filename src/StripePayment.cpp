#include "../include/StripePayment.h"
#include "../include/AVNGenerator.h" // Including for PaymentData struct
#include <iostream>
#include <unistd.h>
#include <sys/select.h>
#include <cstring>
#include <string.h>
#include <errno.h>
#include <iomanip>
#include <random>
#include <sstream>
#include <limits>
#include <chrono>

// Constructor initializes default values
StripePayment::StripePayment()
{
    // Set default values
    running.store(false);
    
    // Initialize pipe file descriptors to invalid values
    airlineToStripePipe[0] = airlineToStripePipe[1] = -1;
    stripeToAvnPipe[0] = stripeToAvnPipe[1] = -1;
}

// Destructor cleans up resources
StripePayment::~StripePayment()
{
    // Stop the process if still running
    if (running.load())
    {
        stop();
    }
    
    // Close pipe file descriptors if open
    if (airlineToStripePipe[0] >= 0) close(airlineToStripePipe[0]);
    if (airlineToStripePipe[1] >= 0) close(airlineToStripePipe[1]);
    if (stripeToAvnPipe[0] >= 0) close(stripeToAvnPipe[0]);
    if (stripeToAvnPipe[1] >= 0) close(stripeToAvnPipe[1]);
}

// Initialize with pipe file descriptors
bool StripePayment::initialize(int airlineToStripe[2], int stripeToAvn[2])
{
    // Store the pipe file descriptors
    airlineToStripePipe[0] = airlineToStripe[0];  // Read end
    airlineToStripePipe[1] = airlineToStripe[1];  // Write end
    
    stripeToAvnPipe[0] = stripeToAvn[0];  // Read end
    stripeToAvnPipe[1] = stripeToAvn[1];  // Write end
    
    // Validate pipe file descriptors
    if (airlineToStripePipe[0] < 0 || stripeToAvnPipe[1] < 0)
    {
        std::cerr << "StripePay: Invalid pipe file descriptors" << std::endl;
        return false;
    }
    
    std::cout << "StripePay: Initialized successfully" << std::endl;
    return true;
}

// Generate a unique transaction ID
std::string StripePayment::generateTransactionID()
{
    // Seed with high-resolution clock for better randomness
    // Using std::chrono and std::mt19937 to generate a good quality transaction ID
    auto now = std::chrono::high_resolution_clock::now();
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(
                       now.time_since_epoch()).count();
    
    std::mt19937 rng(static_cast<unsigned int>(nanoseconds));
    std::uniform_int_distribution<int> dist(100000, 999999);
    
    // Create a transaction ID with format: TXPAY-YYYYMMDD-XXXXXX
    std::time_t t = std::time(nullptr);
    std::tm* now_tm = std::localtime(&t);
    
    std::stringstream ss;
    ss << "TXPAY-"
       << (now_tm->tm_year + 1900)
       << std::setfill('0') << std::setw(2) << (now_tm->tm_mon + 1)
       << std::setfill('0') << std::setw(2) << now_tm->tm_mday
       << "-" << dist(rng);
    
    return ss.str();
}

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

// Main process loop 
void StripePayment::run()
{
    // Set the running flag
    running.store(true);
    
    std::cout << "StripePay: Starting main process loop..." << std::endl;
    std::cout << "=== Welcome to the StripePay Service ===" << std::endl;
    std::cout << "Monitoring for incoming payment requests..." << std::endl;
    
    // Show the menu initially
    showMenu();
    
    // Set up file descriptor set for select()
    fd_set readFds;
    struct timeval timeout;
    
    // Main loop - continue until stopped
    while (running.load())
    {
        // Reset the file descriptor set
        FD_ZERO(&readFds);
        FD_SET(airlineToStripePipe[0], &readFds);
        
        // Set timeout for select (0.5 seconds)
        // This allows us to periodically check the running flag and check for user input
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;
        
        // Wait for data on the pipe or timeout
        int maxFd = airlineToStripePipe[0] + 1;
        int activity = select(maxFd, &readFds, NULL, NULL, &timeout);
        
        if (activity < 0)
        {
            // Error occurred
            if (errno != EINTR) // Ignore if interrupted by signal
            {
                std::cerr << "StripePay: select() error: " << strerror(errno) << std::endl;
            }
            continue;
        }
        
        // Check if there's data to read from the AirlinePortal
        if (FD_ISSET(airlineToStripePipe[0], &readFds))
        {
            // Read the payment request data
            PaymentData data;
            ssize_t bytesRead = read(airlineToStripePipe[0], &data, sizeof(PaymentData));
            
            if (bytesRead == sizeof(PaymentData))
            {
                // Process the received payment request
                processPaymentRequest(data);
                
                // Show menu after processing
                showMenu();
            }
            else if (bytesRead > 0)
            {
                // Incomplete data received
                std::cerr << "StripePay: Received incomplete data" << std::endl;
            }
            else if (bytesRead == 0)
            {
                // EOF - pipe closed
                std::cerr << "StripePay: Pipe closed by Airline Portal" << std::endl;
                break;
            }
            else
            {
                // Error occurred
                std::cerr << "StripePay: read() error: " << strerror(errno) << std::endl;
            }
        }
        
        // Check for user input
        if (std::cin.rdbuf()->in_avail() > 0) // If there's user input available
        {
            int choice;
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer
            
            switch (choice)
            {
                case 1:
                    displayPendingPayments();
                    break;
                case 2:
                {
                    int index;
                    std::cout << "Enter payment index to approve: ";
                    std::cin >> index;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    approvePayment(index);
                    break;
                }
                case 3:
                    viewPaymentHistory();
                    break;
                case 0:
                    stop();
                    break;
                default:
                    std::cout << "Invalid option. Try again." << std::endl;
            }
            
            // Show the menu again after processing user input
            std::cout << "\nPress Enter to return to the menu...";
            std::cin.get(); // Wait for user to press Enter
            showMenu();
        }
    }
    
    std::cout << "StripePay: Exiting main process loop" << std::endl;
}

// Show the main menu to the user
void StripePayment::showMenu()
{
    // Get the count of pending payments
    size_t pendingCount;
    {
        std::lock_guard<std::mutex> lock(pendingPaymentsMutex);
        pendingCount = pendingPayments.size();
    }
    
    std::cout << "\n\n=======================================" << std::endl;
    std::cout << "      STRIPEPAY - PAYMENT PORTAL      " << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << "1. View Pending Payments (" << pendingCount << ")" << std::endl;
    std::cout << "2. Approve Payment" << std::endl;
    std::cout << "3. View Payment History" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << "Enter your choice: ";
}

// Display all pending payments
void StripePayment::displayPendingPayments()
{
    std::lock_guard<std::mutex> lock(pendingPaymentsMutex);
    
    if (pendingPayments.empty())
    {
        std::cout << "No pending payments found." << std::endl;
        return;
    }
    
    std::cout << "\n============ Pending Payments ============\n" << std::endl;
    std::cout << std::left
              << std::setw(5) << "Index"
              << std::setw(15) << "AVN ID"
              << std::setw(12) << "Flight"
              << std::setw(15) << "Aircraft Type"
              << std::setw(15) << "Amount Due"
              << std::setw(15) << "Amount Paid"
              << std::endl;
    std::cout << std::string(75, '-') << std::endl;
    
    // Create a copy of the queue to display
    std::queue<PendingPayment> tempQueue = pendingPayments;
    int index = 1;
    
    while (!tempQueue.empty())
    {
        const PendingPayment& payment = tempQueue.front();
        
        std::cout << std::left
                  << std::setw(5) << index++
                  << std::setw(15) << payment.avnID
                  << std::setw(12) << payment.flightNumber
                  << std::setw(15) << payment.aircraftType
                  << std::setw(15) << payment.amountToPay
                  << std::setw(15) << payment.amountPaid
                  << std::endl;
        
        tempQueue.pop();
    }
    
    std::cout << std::endl;
}

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

// View payment history
void StripePayment::viewPaymentHistory()
{
    if (paymentHistory.empty())
    {
        std::cout << "No payment history found." << std::endl;
        return;
    }
    
    std::cout << "\n============ Payment History ============\n" << std::endl;
    std::cout << std::left
              << std::setw(15) << "AVN ID"
              << std::setw(12) << "Flight"
              << std::setw(15) << "Aircraft Type"
              << std::setw(15) << "Amount Due"
              << std::setw(15) << "Amount Paid"
              << std::endl;
    std::cout << std::string(72, '-') << std::endl;
    
    for (const auto& payment : paymentHistory)
    {
        std::cout << std::left
                  << std::setw(15) << payment.avnID
                  << std::setw(12) << payment.flightNumber
                  << std::setw(15) << payment.aircraftType
                  << std::setw(15) << payment.amountToPay
                  << std::setw(15) << payment.amountPaid
                  << std::endl;
    }
    
    std::cout << std::endl;
}

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
    
    {
        // Add the payment request to pending payments queue
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

// Process payment for a violation
bool StripePayment::processPayment(std::string avnID, int amount)
{
    // In a real system, this would connect to a payment gateway
    // For simulation, we'll just return success
    std::cout << "Payment of PKR " << std::setw(6) << std::setfill(' ') << amount 
              << " processed successfully for AVN " << avnID << std::endl;
    return true;
}

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

// Gracefully stop the process
void StripePayment::stop()
{
    std::cout << "StripePay: Stopping..." << std::endl;
    running.store(false);
}