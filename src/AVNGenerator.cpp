#include "../include/AVNGenerator.h"
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <errno.h>
#include <algorithm>

/**
 * Constructor initializes the AVN Generator with default values
 * 
 * We're creating a named semaphore for synchronization between processes.
 * Named semaphores can be accessed from different processes using the same name.
 */
AVNGenerator::AVNGenerator() 
{
    // Create a named semaphore with initial value of 1 (acts like a mutex)
    // O_CREAT: Create if it doesn't exist
    // 0644: Set permissions (rw-r--r--)
    // 1: Initial value (1 = unlocked)
    avnSemaphore = sem_open("/avn_semaphore", O_CREAT, 0644, 1);
    if (avnSemaphore == SEM_FAILED) 
    {
        std::cerr << "Failed to create semaphore: " << strerror(errno) << std::endl;
    }
    
    // Set to true - will be used to control the main loop
    running.store(true);
    
    // Initialize pipe file descriptors to invalid values
    atcsToAvnPipe[0] = atcsToAvnPipe[1] = -1;
    avnToAirlinePipe[0] = avnToAirlinePipe[1] = -1;
    stripeToAvnPipe[0] = stripeToAvnPipe[1] = -1;
}

/**
 * Destructor cleans up resources used by the AVN Generator
 * 
 * We need to close the semaphore and remove it from the system.
 * We also close any open pipe file descriptors to prevent resource leaks.
 */
AVNGenerator::~AVNGenerator() 
{
    // Signal the process to stop
    stop();
    
    // Close and unlink the named semaphore
    if (avnSemaphore != SEM_FAILED) 
    {
        sem_close(avnSemaphore);
        sem_unlink("/avn_semaphore");
    }
    
    // Close active pipe file descriptors
    if (atcsToAvnPipe[0] >= 0) close(atcsToAvnPipe[0]);
    if (avnToAirlinePipe[1] >= 0) close(avnToAirlinePipe[1]);
    if (stripeToAvnPipe[0] >= 0) close(stripeToAvnPipe[0]);
}

/**
 * Initialize the AVN Generator with pipe file descriptors
 * 
 * This method sets up the pipe file descriptors and prepares
 * the AVN Generator for communication with other processes.
 * 
 * @param atcsToAvn Pipe from ATCS to AVN Generator
 * @param avnToAirline Pipe from AVN Generator to Airline Portal
 * @param stripeToAvn Pipe from StripePay to AVN Generator
 * @return true if initialization succeeded, false otherwise
 */
bool AVNGenerator::initialize(int atcsToAvn[2], int avnToAirline[2], int stripeToAvn[2]) 
{
    // Store pipe file descriptors
    atcsToAvnPipe[0] = atcsToAvn[0];    // Read end of ATCS -> AVN pipe
    atcsToAvnPipe[1] = atcsToAvn[1];    // Write end (will be closed)
    
    avnToAirlinePipe[0] = avnToAirline[0]; // Read end (will be closed)
    avnToAirlinePipe[1] = avnToAirline[1]; // Write end of AVN -> Airline pipe
    
    stripeToAvnPipe[0] = stripeToAvn[0]; // Read end of StripePay -> AVN pipe
    stripeToAvnPipe[1] = stripeToAvn[1]; // Write end (will be closed)
    
    // Close unused ends of pipes
    // This is critical for proper pipe operation in a forked process
    // If we don't close unused ends, we might never get "end of file" signals
    close(atcsToAvnPipe[1]);   // We don't write to ATCS
    close(avnToAirlinePipe[0]); // We don't read from Airline Portal
    close(stripeToAvnPipe[1]);  // We don't write to StripePay
    
    std::cout << "AVN Generator initialized with pipe file descriptors:" << std::endl;
    std::cout << "  ATCS -> AVN read: " << atcsToAvnPipe[0] << std::endl;
    std::cout << "  AVN -> Airline write: " << avnToAirlinePipe[1] << std::endl;
    std::cout << "  StripePay -> AVN read: " << stripeToAvnPipe[0] << std::endl;
    
    return true;
}

/**
 * Runs the main AVN Generator process loop
 * 
 * This is the heart of the AVN Generator process. It continuously 
 * monitors its input pipes for incoming data:
 * - Violation data from ATCS Controller
 * - Payment notifications from StripePay
 * 
 * We use select() to efficiently wait for data on multiple file descriptors.
 */
void AVNGenerator::run() 
{
    std::cout << "AVN Generator process running (PID: " << getpid() << ")" << std::endl;
    
    // File descriptor set for select()
    fd_set readFds;
    
    // Find the highest file descriptor for select()
    int maxFd = std::max(atcsToAvnPipe[0], stripeToAvnPipe[0]);
    
    // Timeout for select() - we'll check every 0.5 seconds
    struct timeval timeout;
    
    // Allocate space for our incoming data
    ViolationData violationData;
    PaymentData paymentData;
    
    // Main process loop
    while (running.load()) 
    {
        // Reset file descriptor set for each iteration
        FD_ZERO(&readFds);
        FD_SET(atcsToAvnPipe[0], &readFds);
        FD_SET(stripeToAvnPipe[0], &readFds);
        
        // Set timeout to 0.5 seconds
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000; // 500ms
        
        // Wait for data with timeout
        int ready = select(maxFd + 1, &readFds, NULL, NULL, &timeout);
        
        // Check for errors
        if (ready < 0) 
        {
            // An error occurred - only report if it's not due to interruption
            if (errno != EINTR) 
            {
                std::cerr << "Select error in AVN Generator: " << strerror(errno) << std::endl;
            }
            continue;
        }
        
        // Check if there's data from ATCS Controller
        if (FD_ISSET(atcsToAvnPipe[0], &readFds)) 
        {
            ssize_t bytesRead = read(atcsToAvnPipe[0], &violationData, sizeof(ViolationData));
            if (bytesRead > 0) 
            {
                // Got violation data - process it
                processViolation(violationData);
            }
            else if (bytesRead == 0) 
            {
                // Pipe closed - parent process probably terminated
                std::cout << "ATCS -> AVN pipe closed. Parent process may have terminated." << std::endl;
            }
            else 
            {
                // Error reading from pipe
                std::cerr << "Error reading from ATCS pipe: " << strerror(errno) << std::endl;
            }
        }
        
        // Check if there's data from StripePay
        if (FD_ISSET(stripeToAvnPipe[0], &readFds)) 
        {
            ssize_t bytesRead = read(stripeToAvnPipe[0], &paymentData, sizeof(PaymentData));
            if (bytesRead > 0) 
            {
                // Got payment data - process it
                processPayment(paymentData);
            }
            else if (bytesRead == 0) 
            {
                // Pipe closed - StripePay process probably terminated
                std::cout << "StripePay -> AVN pipe closed. StripePay process may have terminated." << std::endl;
            }
            else 
            {
                // Error reading from pipe
                std::cerr << "Error reading from StripePay pipe: " << strerror(errno) << std::endl;
            }
        }
    }
    
    std::cout << "AVN Generator process stopping..." << std::endl;
}

/**
 * Stop the AVN Generator process gracefully
 */
void AVNGenerator::stop() 
{
    running.store(false);
}

/**
 * Process violation data from ATCS Controller
 * 
 * This method creates a new AVN from the violation data,
 * calculates the fine, and adds it to the system.
 * 
 * @param data Violation data from ATCS Controller
 */
void AVNGenerator::processViolation(const ViolationData& data) 
{
    // Create a new AVN for the violation
    AVN newAvn;
    
    // Populate the AVN with data from the violation
    newAvn.flightNumber = data.flightNumber;
    newAvn.airLine = data.airLine;
    newAvn.speed = data.speed;
    newAvn.allowed = std::make_pair(data.minAllowed, data.maxAllowed);
    
    // Generate a unique ID for the AVN
    newAvn.generateAVNID();
    
    // Calculate the fine amount based on airline/aircraft type
    newAvn.calculateFine();
    
    // Log the new AVN
    std::cout << "===== NEW AIRSPACE VIOLATION NOTICE =====" << std::endl;
    std::cout << "AVN ID: " << newAvn.avnID << std::endl;
    std::cout << "Flight: " << newAvn.flightNumber 
              << " (Airline: " << newAvn.airLine << " - Type: " << newAvn.aircraftType << ")" << std::endl;
    std::cout << "Speed: " << newAvn.speed << " km/h"
              << " (Allowed range: " << newAvn.allowed.first << "-" << newAvn.allowed.second << " km/h)" << std::endl;
    std::cout << "Fine: PKR " << newAvn.fineAmount 
              << " + Service Fee: PKR " << newAvn.serviceFee 
              << " = Total: PKR " << newAvn.totalAmount << std::endl;
    std::cout << "Issue Date: " << newAvn.issueDate << " (Due: " << newAvn.dueDate << ")" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    // Use mutex to protect the AVN list during the update
    // This prevents race conditions if multiple violations arrive simultaneously
    {
        std::lock_guard<std::mutex> lock(avnMutex);
        
        // Add to AVN list
        avnList.push_back(newAvn);
    }
    
    // Send the new AVN to the Airline Portal
    sendAVNToAirlinePortal(newAvn);
}

/**
 * Process payment notification from StripePay
 * 
 * This method updates the payment status of an AVN
 * and notifies the Airline Portal of the change.
 * 
 * We use both a semaphore (for inter-process synchronization)
 * and a mutex (for thread safety within this process).
 * 
 * @param data Payment data from StripePay
 */
void AVNGenerator::processPayment(const PaymentData& data) 
{
    // Wait on the semaphore to ensure exclusive access across processes
    sem_wait(avnSemaphore);
    
    // Now use the mutex for thread safety within this process
    std::lock_guard<std::mutex> lock(avnMutex);
    
    // Find the AVN by ID
    AVN* avn = findAVNByID(data.avnID);
    if (avn != nullptr) 
    {
        // Update payment status
        avn->paid = data.paid;
        
        std::cout << "AVN Payment Status Updated:" << std::endl;
        std::cout << "  AVN ID: " << avn->avnID << std::endl;
        std::cout << "  Flight: " << avn->flightNumber << " (" << avn->airLine << ")" << std::endl;
        std::cout << "  Status: " << (avn->paid ? "PAID" : "UNPAID") << std::endl;
        std::cout << "  Amount: PKR " << avn->totalAmount << std::endl;
        
        // Send updated AVN to Airline Portal
        sendAVNToAirlinePortal(*avn);
    }
    else 
    {
        std::cerr << "Payment received for unknown AVN ID: " << data.avnID << std::endl;
    }
    
    // Release the semaphore
    sem_post(avnSemaphore);
}

/**
 * Send AVN information to the Airline Portal
 * 
 * This method serializes AVN data and sends it through
 * the pipe to the Airline Portal process.
 * 
 * @param avn The AVN to send
 */
void AVNGenerator::sendAVNToAirlinePortal(const AVN& avn) 
{
    // For now, we'll just send the AVN ID and payment status
    // In a real system, we would serialize the entire AVN object
    PaymentData notifyData;
    
    // Copy the AVN ID to the struct with proper string handling
    strncpy(notifyData.avnID, avn.avnID.c_str(), sizeof(notifyData.avnID) - 1);
    notifyData.avnID[sizeof(notifyData.avnID) - 1] = '\0';  // Ensure null-termination
    
    // Set the payment status
    notifyData.paid = avn.paid;
    
    // Write to pipe - check for errors
    ssize_t bytesWritten = write(avnToAirlinePipe[1], &notifyData, sizeof(PaymentData));
    if (bytesWritten != sizeof(PaymentData)) 
    {
        std::cerr << "Error sending AVN to Airline Portal: " << strerror(errno) << std::endl;
    }
    else 
    {
        std::cout << "AVN " << avn.avnID << " notification sent to Airline Portal" << std::endl;
    }
}

/**
 * Get AVNs associated with a specific airline
 * 
 * This method returns all AVNs for a given airline,
 * which is useful for the Airline Portal to display
 * violations for a specific airline.
 * 
 * @param airline Name of the airline
 * @return Vector of AVNs for the specified airline
 */
std::vector<AVN> AVNGenerator::getAVNsByAirline(const std::string& airline) 
{
    // Use mutex to protect the AVN list during read
    std::lock_guard<std::mutex> lock(avnMutex);
    
    std::vector<AVN> filteredAVNs;
    
    // Filter AVNs by airline
    for (const AVN& avn : avnList) 
    {
        if (avn.airLine == airline) 
        {
            filteredAVNs.push_back(avn);
        }
    }
    
    return filteredAVNs;
}

/**
 * Get all AVNs in the system
 * 
 * This method returns all AVNs, which is useful for
 * testing and UI display.
 * 
 * @return Vector of all AVNs
 */
std::vector<AVN> AVNGenerator::getAllAVNs() const 
{
    // We have to cast away the const for the mutex
    // This is safe because we're not actually modifying the AVNGenerator object
    // We're just using the mutex for synchronization
    std::mutex& mutableMutex = const_cast<std::mutex&>(avnMutex);
    std::lock_guard<std::mutex> lock(mutableMutex);
    
    // Return a copy of the AVN list
    return avnList;
}

/**
 * Find an AVN by its unique ID
 * 
 * @param avnID The unique AVN ID to find
 * @return Pointer to the AVN if found, nullptr otherwise
 */
AVN* AVNGenerator::findAVNByID(const std::string& avnID) 
{
    // No need for mutex here since this is called from methods that already lock
    for (AVN& avn : avnList) 
    {
        if (avn.avnID == avnID) 
        {
            return &avn;
        }
    }
    
    return nullptr;
}