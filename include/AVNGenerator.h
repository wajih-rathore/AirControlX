#ifndef AIRCONTROLX_AVNGENERATOR_H
#define AIRCONTROLX_AVNGENERATOR_H

#include <vector>
#include <mutex>
#include <semaphore.h>
#include <unistd.h>
#include <atomic>
#include "AVN.h"

/**
 * This struct defines the data format for violation information 
 * sent from the ATCS Controller to the AVN Generator process.
 * We use a fixed-size struct for easy transmission through unnamed pipes.
 */
struct ViolationData 
{
    char flightNumber[20];  // Flight ID (e.g., "PK123")
    char airLine[20];       // Airline name (e.g., "PIA")
    int speed;              // Recorded speed in km/h at violation time
    int minAllowed;         // Minimum allowed speed for current phase
    int maxAllowed;         // Maximum allowed speed for current phase
};

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

/**
 * AVNGenerator class manages the creation and tracking of Airspace Violation Notices (AVNs).
 * 
 * This class runs as a separate process and communicates via unnamed pipes with:
 * 1. ATCS Controller - Receives violation data
 * 2. Airline Portal - Sends AVN information
 * 3. StripePay Process - Receives payment confirmations
 * 
 * It implements Module 3 requirements for AVN generation and management.
 */
class AVNGenerator 
{
private:
    std::vector<AVN> avnList;           // List of all generated AVNs
    std::mutex avnMutex;                // Mutex to protect AVN list during concurrent access
    sem_t *avnSemaphore;                // Named semaphore for synchronization across processes
    std::atomic<bool> running;          // Flag to control the main process loop
    
    // File descriptors for unnamed pipes
    int atcsToAvnPipe[2];               // Pipe from ATCS Controller to AVN Generator
    int avnToAirlinePipe[2];            // Pipe from AVN Generator to Airline Portal
    int stripeToAvnPipe[2];             // Pipe from StripePay to AVN Generator
    
public:
    /**
     * Constructor initializes the AVN Generator with default values.
     * The process requires proper initialization via initialize() before running.
     */
    AVNGenerator();
    
    /**
     * Destructor cleans up resources used by the AVN Generator.
     * Closes open file descriptors and frees semaphores.
     */
    ~AVNGenerator();
    
    /**
     * Initializes the AVN Generator with pipe file descriptors.
     * 
     * @param atcsToAvn Pipe from ATCS Controller to AVN Generator [0:read, 1:write]
     * @param avnToAirline Pipe from AVN Generator to Airline Portal [0:read, 1:write]
     * @param stripeToAvn Pipe from StripePay to AVN Generator [0:read, 1:write]
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize(int atcsToAvn[2], int avnToAirline[2], int stripeToAvn[2]);
    
    /**
     * Runs the main AVN Generator process loop.
     * Continuously monitors for incoming violation data and payment notifications.
     * This method should be called after fork() in the child process.
     */
    void run();
    
    /**
     * Stop the AVN Generator process gracefully
     */
    void stop();
    
    /**
     * Process a violation reported by the ATCS Controller.
     * Creates a new AVN and adds it to the list.
     * 
     * @param data Violation data containing flight information
     */
    void processViolation(const ViolationData& data);
    
    /**
     * Process a payment notification from the StripePay process.
     * Updates the payment status of the corresponding AVN.
     * 
     * @param data Payment data containing AVN ID and payment status
     */
    void processPayment(const PaymentData& data);
    
    /**
     * Send AVN information to the Airline Portal.
     * 
     * @param avn The AVN to send
     */
    void sendAVNToAirlinePortal(const AVN& avn);
    
    /**
     * Get all AVNs associated with a specific airline.
     * Used by the Airline Portal to display violations.
     * 
     * @param airline Name of the airline
     * @return Vector of AVNs for the specified airline
     */
    std::vector<AVN> getAVNsByAirline(const std::string& airline);
    
    /**
     * Get all AVNs in the system (for testing and UI display)
     * 
     * @return Vector of all AVNs
     */
    std::vector<AVN> getAllAVNs() const;
    
    /**
     * Find an AVN by its unique ID.
     * 
     * @param avnID The unique AVN ID to find
     * @return Pointer to the AVN if found, nullptr otherwise
     */
    AVN* findAVNByID(const std::string& avnID);
};

#endif // AIRCONTROLX_AVNGENERATOR_H