#include "../include/AirlinePortal.h"
#include "../include/AVNGenerator.h" // Including for PaymentData struct
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <unistd.h>
#include <sys/select.h>
#include <cstring> // Use C++ version for std::strncpy
#include <string.h>
#include <errno.h>
#include <limits>

// Constructor initializes default values
AirlinePortal::AirlinePortal()
{
    // Set default values
    running.store(false);
    
    // Initialize pipe file descriptors to invalid values
    avnToAirlinePipe[0] = avnToAirlinePipe[1] = -1;
    airlineToStripePipe[0] = airlineToStripePipe[1] = -1;
    
    // Initialize airline accounts
    initializeAirlineAccounts();
}

// Initialize airline accounts with starting balances
void AirlinePortal::initializeAirlineAccounts()
{
    // Lock the accounts map for thread safety during write
    std::lock_guard<std::mutex> lock(accountsMutex);
    
    // Set up initial balances for each airline - giving each a modest starting balance
    // Let's imagine each airline has deposited some funds for potential violations
    airlineAccounts["PIA"] = {1000000, 0, 0};           // 1 million PKR for PIA
    airlineAccounts["AirBlue"] = {800000, 0, 0};        // 800,000 PKR for AirBlue
    airlineAccounts["FedEx"] = {1500000, 0, 0};         // 1.5 million PKR for FedEx
    airlineAccounts["PakistanAirforce"] = {2000000, 0, 0}; // 2 million PKR for Pakistan Airforce
    airlineAccounts["BlueDart"] = {1200000, 0, 0};      // 1.2 million PKR for BlueDart
    airlineAccounts["AghaKhanAir"] = {1500000, 0, 0};   // 1.5 million PKR for AghaKhan Air
    
    std::cout << "Airline accounts initialized with starting balances." << std::endl;
}

// Destructor cleans up resources
AirlinePortal::~AirlinePortal()
{
    // Stop the process if still running
    if (running.load())
    {
        stop();
    }
    
    // Close pipe file descriptors if open
    if (avnToAirlinePipe[0] >= 0) close(avnToAirlinePipe[0]);
    if (avnToAirlinePipe[1] >= 0) close(avnToAirlinePipe[1]);
    if (airlineToStripePipe[0] >= 0) close(airlineToStripePipe[0]);
    if (airlineToStripePipe[1] >= 0) close(airlineToStripePipe[1]);
}

// Initialize the portal with pipe file descriptors
bool AirlinePortal::initialize(int avnToAirline[2], int airlineToStripe[2])
{
    // Store the pipe file descriptors
    avnToAirlinePipe[0] = avnToAirline[0];  // Read end
    avnToAirlinePipe[1] = avnToAirline[1];  // Write end
    
    airlineToStripePipe[0] = airlineToStripe[0];  // Read end
    airlineToStripePipe[1] = airlineToStripe[1];  // Write end
    
    // Validate pipe file descriptors
    if (avnToAirlinePipe[0] < 0 || airlineToStripePipe[1] < 0)
    {
        std::cerr << "AirlinePortal: Invalid pipe file descriptors" << std::endl;
        return false;
    }
    
    std::cout << "AirlinePortal: Initialized successfully" << std::endl;
    return true;
}

// Main process loop
void AirlinePortal::run()
{
    // Set the running flag
    running.store(true);
    
    std::cout << "AirlinePortal: Starting main process loop..." << std::endl;
    std::cout << "=== Welcome to the Airline Portal ===" << std::endl;
    std::cout << "Monitoring for incoming AVNs..." << std::endl;
    
    // Show the user menu right away
    showMenu();
    
    // Set up file descriptor set for select()
    fd_set readFds;
    struct timeval timeout;
    
    // Main loop - continue until stopped
    while (running.load())
    {
        // Reset the file descriptor set
        FD_ZERO(&readFds);
        FD_SET(avnToAirlinePipe[0], &readFds);
        
        // Set timeout for select (0.5 seconds)
        // This allows us to periodically check the running flag and show menu
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;
        
        // Wait for data on the pipe or timeout
        int maxFd = avnToAirlinePipe[0] + 1;
        int activity = select(maxFd, &readFds, NULL, NULL, &timeout);
        
        if (activity < 0)
        {
            // Error occurred
            if (errno != EINTR) // Ignore if interrupted by signal
            {
                std::cerr << "AirlinePortal: select() error: " << strerror(errno) << std::endl;
            }
            continue;
        }
        
        // Check if there's data to read from the AVN Generator
        if (FD_ISSET(avnToAirlinePipe[0], &readFds))
        {
            // Read the AVN data
            PaymentData data;
            ssize_t bytesRead = read(avnToAirlinePipe[0], &data, sizeof(PaymentData));
            
            if (bytesRead == sizeof(PaymentData))
            {
                // Process the received AVN
                processReceivedAVN(data);
                
                // Show menu after processing a new AVN notification
                showMenu();
            }
            else if (bytesRead > 0)
            {
                // Incomplete data received
                std::cerr << "AirlinePortal: Received incomplete data" << std::endl;
            }
            else if (bytesRead == 0)
            {
                // EOF - pipe closed
                std::cerr << "AirlinePortal: Pipe closed by AVN Generator" << std::endl;
                break;
            }
            else
            {
                // Error occurred
                std::cerr << "AirlinePortal: read() error: " << strerror(errno) << std::endl;
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
                    viewAllAVNs();
                    break;
                case 2:
                {
                    std::string airline;
                    std::cout << "Enter airline name (PIA, AirBlue, FedEx, PakistanAirforce, BlueDart, AghaKhanAir): ";
                    std::getline(std::cin, airline);
                    viewAVNsByAirline(airline);
                    break;
                }
                case 3:
                    viewAVNsByStatus(false); // View unpaid
                    break;
                case 4:
                    viewAVNsByStatus(true); // View paid
                    break;
                case 5:
                {
                    std::string avnID;
                    std::cout << "Enter AVN ID to pay: ";
                    std::getline(std::cin, avnID);
                    processUserPayment(avnID);
                    break;
                }
                case 6:
                    viewAllAirlineBalances();
                    break;
                case 7:
                {
                    std::string airline;
                    std::cout << "Enter airline name: ";
                    std::getline(std::cin, airline);
                    viewAirlineBalance(airline);
                    break;
                }
                case 8:
                {
                    std::string airline;
                    int amount;
                    std::cout << "Enter airline name: ";
                    std::getline(std::cin, airline);
                    std::cout << "Enter deposit amount (PKR): ";
                    std::cin >> amount;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    depositFunds(airline, amount);
                    break;
                }
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
    
    std::cout << "AirlinePortal: Exiting main process loop" << std::endl;
}

// Process a received AVN notification
void AirlinePortal::processReceivedAVN(const PaymentData& data)
{
    std::cout << "=== AVN Update Received ===" << std::endl;
    std::cout << "AVN ID: " << data.avnID << std::endl;
    std::cout << "Status: " << (data.paid ? "PAID" : "UNPAID") << std::endl;
    
    // Find existing AVN with this ID, if any
    AVN* existingAVN = findAVNByID(data.avnID);
    
    // Lock the AVN list for thread safety
    std::lock_guard<std::mutex> lock(avnMutex);
    
    if (existingAVN != nullptr)
    {
        // Update existing AVN
        bool wasPaid = existingAVN->paid;
        existingAVN->paid = data.paid;
        
        // If payment status changed to paid, update airline account
        if (!wasPaid && data.paid)
        {
            // Lock the accounts map
            std::lock_guard<std::mutex> accountLock(accountsMutex);
            
            // Update airline's payment records
            auto it = airlineAccounts.find(existingAVN->airLine);
            if (it != airlineAccounts.end())
            {
                it->second.totalPaid += existingAVN->totalAmount;
                std::cout << "Updated payment records for airline " << existingAVN->airLine << std::endl;
            }
        }
        
        std::cout << "Updated existing AVN: " << data.avnID << std::endl;
    }
    else
    {
        // Create a new AVN from the notification
        AVN newAVN;
        newAVN.avnID = data.avnID;
        newAVN.flightNumber = data.flightNumber;
        newAVN.aircraftType = data.aircraftType;
        newAVN.totalAmount = data.amountToPay;
        newAVN.paid = data.paid;
        
        // If this is a new unpaid AVN, update the airline's total fines
        if (!data.paid && !data.flightNumber[0] == '\0')
        {
            // Extract airline name from flight number or use a default
            std::string airlineName;
            if (strncmp(data.flightNumber, "PK", 2) == 0) airlineName = "PIA";
            else if (strncmp(data.flightNumber, "PA", 2) == 0) airlineName = "AirBlue";
            else if (strncmp(data.flightNumber, "FX", 2) == 0) airlineName = "FedEx";
            else if (strncmp(data.flightNumber, "PAF", 3) == 0) airlineName = "PakistanAirforce";
            else if (strncmp(data.flightNumber, "BD", 2) == 0) airlineName = "BlueDart";
            else if (strncmp(data.flightNumber, "AK", 2) == 0) airlineName = "AghaKhanAir";
            else airlineName = "Unknown";
            
            newAVN.airLine = airlineName;
            
            // Lock the accounts map
            std::lock_guard<std::mutex> accountLock(accountsMutex);
            
            // Update airline's fine records
            auto it = airlineAccounts.find(airlineName);
            if (it != airlineAccounts.end())
            {
                it->second.totalFines += data.amountToPay;
                std::cout << "Updated fine records for airline " << airlineName << std::endl;
            }
        }
        
        avnList.push_back(newAVN);
        std::cout << "Added new AVN to list: " << data.avnID << std::endl;
    }
}

// Show the main menu to the user
void AirlinePortal::showMenu()
{
    std::cout << "\n\n=======================================" << std::endl;
    std::cout << "      AIRLINE PORTAL - MAIN MENU       " << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << "1. View All AVNs" << std::endl;
    std::cout << "2. View AVNs by Airline" << std::endl;
    std::cout << "3. View Unpaid AVNs" << std::endl;
    std::cout << "4. View Paid AVNs" << std::endl;
    std::cout << "5. Pay an AVN" << std::endl;
    std::cout << "6. View All Airline Balances" << std::endl;
    std::cout << "7. View Specific Airline Balance" << std::endl;
    std::cout << "8. Deposit Funds" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << "Enter your choice: ";
}

// Process user payment for an AVN with balance check
bool AirlinePortal::processUserPayment(const std::string& avnID)
{
    // Find the AVN
    AVN* avn = findAVNByID(avnID);
    if (avn == nullptr)
    {
        std::cout << "Error: AVN with ID " << avnID << " not found." << std::endl;
        return false;
    }
    
    // Check if already paid
    if (avn->paid)
    {
        std::cout << "AVN " << avnID << " is already paid." << std::endl;
        return false;
    }
    
    // Get the airline name and amount due
    std::string airlineName = avn->airLine;
    int amountDue = avn->totalAmount;
    
    // Check if airline has sufficient balance
    {
        std::lock_guard<std::mutex> lock(accountsMutex);
        auto it = airlineAccounts.find(airlineName);
        if (it == airlineAccounts.end())
        {
            std::cout << "Error: Airline " << airlineName << " not found in account database." << std::endl;
            return false;
        }
        
        if (it->second.balance < amountDue)
        {
            std::cout << "Error: Insufficient funds." << std::endl;
            std::cout << "Amount Due: PKR " << amountDue << std::endl;
            std::cout << "Current Balance: PKR " << it->second.balance << std::endl;
            std::cout << "Need additional PKR " << (amountDue - it->second.balance) << " to complete payment." << std::endl;
            return false;
        }
        
        // Display payment details
        std::cout << "\n=== Payment Details ===" << std::endl;
        std::cout << "AVN ID: " << avnID << std::endl;
        std::cout << "Flight Number: " << avn->flightNumber << std::endl;
        std::cout << "Airline: " << airlineName << std::endl;
        std::cout << "Amount Due: PKR " << amountDue << std::endl;
        std::cout << "Current Balance: PKR " << it->second.balance << std::endl;
        
        // Ask for confirmation
        std::cout << "\nConfirm payment? (y/n): ";
        std::string confirmation;
        std::getline(std::cin, confirmation);
        
        if (confirmation != "y" && confirmation != "Y")
        {
            std::cout << "Payment cancelled by user." << std::endl;
            return false;
        }
        
        // Deduct from airline balance
        it->second.balance -= amountDue;
        std::cout << "PKR " << amountDue << " deducted from " << airlineName << " account." << std::endl;
        std::cout << "New Balance: PKR " << it->second.balance << std::endl;
    }
    
    // Send payment request to StripePay process with amount
    sendPaymentRequest(avnID, amountDue);
    
    std::cout << "Payment request for AVN " << avnID << " sent to StripePay." << std::endl;
    return true;
}

// View balance of a specific airline
void AirlinePortal::viewAirlineBalance(const std::string& airline) const
{
    // Lock the accounts map for thread safety during read
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(accountsMutex));
    
    // Find the airline account
    auto it = airlineAccounts.find(airline);
    if (it == airlineAccounts.end())
    {
        std::cout << "Airline '" << airline << "' not found in account database." << std::endl;
        return;
    }
    
    // Display account details
    const AirlineAccount& account = it->second;
    std::cout << "\n=== Account Details for " << airline << " ===" << std::endl;
    std::cout << "Current Balance: PKR " << account.balance << std::endl;
    std::cout << "Total Fines: PKR " << account.totalFines << std::endl;
    std::cout << "Total Paid: PKR " << account.totalPaid << std::endl;
    std::cout << "Outstanding Amount: PKR " << (account.totalFines - account.totalPaid) << std::endl;
}

// View balances of all airlines
void AirlinePortal::viewAllAirlineBalances() const
{
    // Lock the accounts map for thread safety during read
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(accountsMutex));
    
    // Display table header
    std::cout << "\n============ Airline Account Balances ============\n" << std::endl;
    std::cout << std::left 
              << std::setw(20) << "Airline" 
              << std::setw(15) << "Balance (PKR)"
              << std::setw(15) << "Total Fines"
              << std::setw(15) << "Total Paid"
              << std::setw(15) << "Outstanding"
              << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    // Display each airline account
    for (const auto& entry : airlineAccounts)
    {
        const std::string& airlineName = entry.first;
        const AirlineAccount& account = entry.second;
        int outstanding = account.totalFines - account.totalPaid;
        
        std::cout << std::left 
                  << std::setw(20) << airlineName
                  << std::setw(15) << account.balance
                  << std::setw(15) << account.totalFines
                  << std::setw(15) << account.totalPaid
                  << std::setw(15) << outstanding
                  << std::endl;
    }
    
    std::cout << std::endl;
}

// Deposit funds into an airline account
bool AirlinePortal::depositFunds(const std::string& airline, int amount)
{
    if (amount <= 0)
    {
        std::cout << "Error: Amount must be positive." << std::endl;
        return false;
    }
    
    // Lock the accounts map for thread safety during write
    std::lock_guard<std::mutex> lock(accountsMutex);
    
    // Find the airline account
    auto it = airlineAccounts.find(airline);
    if (it == airlineAccounts.end())
    {
        std::cout << "Airline '" << airline << "' not found in account database." << std::endl;
        return false;
    }
    
    // Add funds to the account
    it->second.balance += amount;
    
    std::cout << "Successfully deposited PKR " << amount << " to " << airline << " account." << std::endl;
    std::cout << "New Balance: PKR " << it->second.balance << std::endl;
    
    return true;
}

// Get airline balance
int AirlinePortal::getAirlineBalance(const std::string& airline) const
{
    // Lock the accounts map for thread safety during read
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(accountsMutex));
    
    // Find the airline account
    auto it = airlineAccounts.find(airline);
    if (it == airlineAccounts.end())
    {
        return 0; // Account not found
    }
    
    return it->second.balance;
}

// Send payment request to StripePay process with amount to pay
void AirlinePortal::sendPaymentRequest(const std::string& avnID, int amountToPay)
{
    // Find the AVN
    AVN* avn = findAVNByID(avnID);
    if (avn == nullptr)
    {
        std::cout << "Error: AVN with ID " << avnID << " not found." << std::endl;
        return;
    }
    
    // Create payment request data
    PaymentData paymentRequest;
    
    // Copy AVN ID to the struct with proper string handling
    strncpy(paymentRequest.avnID, avnID.c_str(), sizeof(paymentRequest.avnID) - 1);
    paymentRequest.avnID[sizeof(paymentRequest.avnID) - 1] = '\0';  // Ensure null-termination
    
    // Copy flight number
    strncpy(paymentRequest.flightNumber, avn->flightNumber.c_str(), sizeof(paymentRequest.flightNumber) - 1);
    paymentRequest.flightNumber[sizeof(paymentRequest.flightNumber) - 1] = '\0';
    
    // Copy aircraft type
    strncpy(paymentRequest.aircraftType, avn->aircraftType.c_str(), sizeof(paymentRequest.aircraftType) - 1);
    paymentRequest.aircraftType[sizeof(paymentRequest.aircraftType) - 1] = '\0';
    
    // Set amount to pay and amount paid
    paymentRequest.amountToPay = avn->totalAmount;
    paymentRequest.amountPaid = amountToPay;
    
    // Set payment status to false (requesting payment)
    paymentRequest.paid = false;
    
    // Send through pipe to StripePay process
    ssize_t bytesWritten = write(airlineToStripePipe[1], &paymentRequest, sizeof(PaymentData));
    
    if (bytesWritten != sizeof(PaymentData))
    {
        std::cerr << "Error sending payment request to StripePay: " << strerror(errno) << std::endl;
    }
    else
    {
        std::cout << "Payment request sent to StripePay for AVN " << avnID << std::endl;
    }
}

// Gracefully stop the process
void AirlinePortal::stop()
{
    std::cout << "AirlinePortal: Stopping..." << std::endl;
    running.store(false);
}

// View all violation notices
void AirlinePortal::viewAllAVNs() const
{
    // Lock the AVN list for thread safety during read
    // Using const_cast to temporarily remove the constness of the mutex
    // This is safe because we're not modifying the AirlinePortal object's state,
    // just using the mutex for synchronization
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(avnMutex));
    
    if (avnList.empty())
    {
        std::cout << "No AVNs found in the system." << std::endl;
        return;
    }
    
    // Display table header
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
    
    std::cout << std::endl;
}

// View AVNs filtered by airline
void AirlinePortal::viewAVNsByAirline(const std::string& airline) const
{
    // Lock the AVN list for thread safety during read
    // Using const_cast to temporarily remove the constness of the mutex
    // This is safe because we're only synchronizing access, not modifying state
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(avnMutex));
    
    // Filter AVNs by airline
    std::vector<AVN> filteredAVNs;
    for (const AVN& avn : avnList)
    {
        if (avn.airLine == airline)
        {
            filteredAVNs.push_back(avn);
        }
    }
    
    if (filteredAVNs.empty())
    {
        std::cout << "No AVNs found for airline: " << airline << std::endl;
        return;
    }
    
    // Display table header
    std::cout << "\n============ AVNs for " << airline << " ============\n" << std::endl;
    std::cout << std::left 
              << std::setw(14) << "AVN ID" 
              << std::setw(12) << "Flight" 
              << std::setw(12) << "Fine (PKR)" 
              << std::setw(12) << "Status"
              << std::setw(14) << "Due Date"
              << std::endl;
    std::cout << std::string(65, '-') << std::endl;
    
    // Display each filtered AVN
    for (const AVN& avn : filteredAVNs)
    {
        std::cout << std::left 
                  << std::setw(14) << avn.avnID 
                  << std::setw(12) << avn.flightNumber 
                  << std::setw(12) << avn.totalAmount
                  << std::setw(12) << (avn.paid ? "PAID" : "UNPAID")
                  << std::setw(14) << avn.dueDate
                  << std::endl;
    }
    
    std::cout << std::endl;
}

// View AVNs filtered by payment status
void AirlinePortal::viewAVNsByStatus(bool paid) const
{
    // Lock the AVN list for thread safety during read
    // Using const_cast to temporarily remove the constness of the mutex
    // This allows us to lock it without modifying any actual state data
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(avnMutex));
    
    // Filter AVNs by payment status
    std::vector<AVN> filteredAVNs;
    for (const AVN& avn : avnList)
    {
        if (avn.paid == paid)
        {
            filteredAVNs.push_back(avn);
        }
    }
    
    if (filteredAVNs.empty())
    {
        std::cout << "No " << (paid ? "paid" : "unpaid") << " AVNs found." << std::endl;
        return;
    }
    
    // Display table header
    std::cout << "\n============ " << (paid ? "Paid" : "Unpaid") << " AVNs ============\n" << std::endl;
    std::cout << std::left 
              << std::setw(14) << "AVN ID" 
              << std::setw(12) << "Flight" 
              << std::setw(15) << "Airline"
              << std::setw(12) << "Fine (PKR)" 
              << std::setw(14) << "Due Date"
              << std::endl;
    std::cout << std::string(65, '-') << std::endl;
    
    // Display each filtered AVN
    for (const AVN& avn : filteredAVNs)
    {
        std::cout << std::left 
                  << std::setw(14) << avn.avnID 
                  << std::setw(12) << avn.flightNumber 
                  << std::setw(15) << avn.airLine
                  << std::setw(12) << avn.totalAmount
                  << std::setw(14) << avn.dueDate
                  << std::endl;
    }
    
    std::cout << std::endl;
}

// Get all AVNs
std::vector<AVN> AirlinePortal::getAllAVNs() const
{
    // Lock the AVN list for thread safety during read
    // Using const_cast to temporarily remove the constness of the mutex
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(avnMutex));
    
    // Return a copy of the AVN list
    return avnList;
}

// Find an AVN by ID
AVN* AirlinePortal::findAVNByID(const std::string& avnID)
{
    // Lock the AVN list for thread safety during read
    std::lock_guard<std::mutex> lock(avnMutex);
    
    // Find the AVN with matching ID
    for (AVN& avn : avnList)
    {
        if (avn.avnID == avnID)
        {
            return &avn;
        }
    }
    
    // Not found
    return nullptr;
}