#ifndef AIRCONTROLX_AIRLINEPORTAL_H
#define AIRCONTROLX_AIRLINEPORTAL_H

#include <vector>
#include <string>
#include <mutex>
#include <atomic>
#include <unistd.h>
#include <map>
#include "AVN.h"

// Forward declaration for PaymentData
struct PaymentData;

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

/**
 * AirlinePortal class for presenting an interface for airline violations.
 * Used by airlines to view and manage their violation notices.
 */
class AirlinePortal 
{
private:
    std::vector<AVN> avnList;          // List of all received AVNs
    std::mutex avnMutex;               // Mutex to protect AVN list during concurrent access
    std::atomic<bool> running;         // Flag to control the main process loop
    
    // Airline account balances - maps airline name to account details
    // Using a mutex to protect the map during concurrent access
    std::map<std::string, AirlineAccount> airlineAccounts;
    std::mutex accountsMutex;
    
    // File descriptors for pipes
    int avnToAirlinePipe[2];           // Pipe from AVN Generator to Airline Portal
    int airlineToStripePipe[2];        // Pipe from Airline Portal to StripePay
    
    // Internal methods
    void processReceivedAVN(const PaymentData& data);
    void sendPaymentRequest(const std::string& avnID, int amountToPay);
    
    // Initialize airline accounts with default balances
    void initializeAirlineAccounts();
    
public:
    // Lifecycle methods
    AirlinePortal();
    ~AirlinePortal();
    
    // Initialize the portal with pipe file descriptors
    bool initialize(int avnToAirline[2], int airlineToStripe[2]);
    
    // Main process loop
    void run();
    
    // Gracefully stop the process
    void stop();
    
    // User interface methods
    void viewAllAVNs() const;
    void viewAVNsByAirline(const std::string& airline) const;
    void viewAVNsByStatus(bool paid) const;
    void showMenu();
    
    // Balance management
    void viewAirlineBalance(const std::string& airline) const;
    void viewAllAirlineBalances() const;
    bool depositFunds(const std::string& airline, int amount);
    
    // Payment methods
    bool initiatePayment(const std::string& avnID);
    bool processUserPayment(const std::string& avnID);
    
    // Access methods
    std::vector<AVN> getAllAVNs() const;
    AVN* findAVNByID(const std::string& avnID);
    int getAirlineBalance(const std::string& airline) const;
};

#endif // AIRCONTROLX_AIRLINEPORTAL_H