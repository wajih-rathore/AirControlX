#ifndef AIRCONTROLX_AVN_H
#define AIRCONTROLX_AVN_H

#include <string>
#include <utility>
#include <ctime>

/**
 * AVN class for representing Airspace Violation Notices.
 * Contains details about violations and manages fine calculations.
 */
class AVN {
public:
    std::string avnID;                  // Unique identifier for the violation notice
    std::string flightNumber;           // Flight identifier (corrected typo from flightNumberl)
    std::string airLine;                // Airline of the violating aircraft
    std::string aircraftType;           // Type of aircraft (Commercial/Cargo/Emergency)
    int speed;                          // Speed at violation
    std::pair<int, int> allowed;        // Permitted speed range
    time_t issueTime;                   // Timestamp when AVN was issued
    std::string issueDate;              // Date when AVN was issued (formatted string)
    int fineAmount;                     // Calculated fine amount
    int serviceFee;                     // 15% administrative fee
    int totalAmount;                    // Total amount including service fee
    bool paid;                          // Payment status
    std::string dueDate;                // Due date for fine payment
    
    // Constructor
    AVN();
    
    // Calculate fine amount based on aircraft type
    void calculateFine();
    
    // Generate a unique AVN ID
    void generateAVNID();
    
    // Calculate and set due date (3 days from issuance)
    void calculateDueDate();
};

#endif // AIRCONTROLX_AVN_H