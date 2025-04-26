#ifndef AIRCONTROLX_AVN_H
#define AIRCONTROLX_AVN_H

#include <string>
#include <utility>

/**
 * AVN class for representing Airspace Violation Notices.
 * Contains details about violations and manages fine calculations.
 */
class AVN {
public:
    std::string avnID;                  // Unique identifier for the violation notice
    std::string flightNumberl;          // Flight identifier
    std::string airLine;                // Airline of the violating aircraft
    int speed;                          // Speed at violation
    std::pair<int, int> allowed;        // Permitted speed range
    int time;                           // Time of violation
    int fineAmount;                     // Calculated fine amount
    bool paid;                          // Payment status
    std::string dueDate;                // Due date for fine payment
    
    // Constructor
    AVN();
    
    // Calculate fine amount based on aircraft type
    void calculateFine();
};

#endif // AIRCONTROLX_AVN_H