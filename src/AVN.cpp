#include "../include/AVN.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <random>

using namespace std;

// Constructor initializes AVN with default values
AVN::AVN() {
    avnID = "";
    flightNumber = "";
    airLine = "";
    aircraftType = "";
    speed = 0;
    allowed = make_pair(0, 0);
    issueTime = time(nullptr); // Current time
    
    // Format the issue date
    tm* timeInfo = localtime(&issueTime);
    char buffer[20];
    strftime(buffer, 20, "%Y-%m-%d", timeInfo);
    issueDate = string(buffer);
    
    fineAmount = 0;
    serviceFee = 0;
    totalAmount = 0;
    paid = false;
    calculateDueDate(); // Set due date automatically
}

// Calculate fine amount based on aircraft type
void AVN::calculateFine() {
    // Determine aircraft type based on airline
    if (airLine == "PIA" || airLine == "AirBlue") 
    {
        aircraftType = "Commercial";
        fineAmount = 500000; // Commercial fine
    } 
    else if (airLine == "FedEx" || airLine == "BlueDart") 
    {
        aircraftType = "Cargo";
        fineAmount = 700000; // Cargo fine
    } else if (airLine == "PakistanAirforce" || airLine == "AghaKhanAir") {
        aircraftType = "Emergency";
        fineAmount = 700000; // Same fine as cargo for emergency
    }
    
    // Calculate 15% administrative fee
    serviceFee = static_cast<int>(fineAmount * 0.15);
    
    // Set total amount
    totalAmount = fineAmount + serviceFee;
}

// Generate a unique AVN ID
void AVN::generateAVNID() {
    // Create a random device
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1000, 9999);
    
    // Generate the ID using current time and a random number
    time_t now = time(nullptr);
    tm* timeInfo = localtime(&now);
    
    // Format: AVN-YYYYMMDD-XXXX (XXXX is random number)
    stringstream ss;
    ss << "AVN-" 
       << (timeInfo->tm_year + 1900) 
       << setfill('0') << setw(2) << (timeInfo->tm_mon + 1)
       << setfill('0') << setw(2) << timeInfo->tm_mday 
       << "-" << dist(gen);
    
    avnID = ss.str();
}

// Calculate and set due date (3 days from issuance)
void AVN::calculateDueDate() {
    // Add 3 days to issue time
    time_t dueTime = issueTime + (3 * 24 * 60 * 60);
    tm* timeInfo = localtime(&dueTime);
    
    // Format the due date
    char buffer[20];
    strftime(buffer, 20, "%Y-%m-%d", timeInfo);
    dueDate = string(buffer);
}