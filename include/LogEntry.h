#ifndef AIRCONTROLX_LOGENTRY_H
#define AIRCONTROLX_LOGENTRY_H

#include <string>
#include <utility>

/**
 * LogEntry class for tracking violations and important events.
 * Used to maintain records of speed violations and other rule breaches.
 */
class LogEntry {
public:
    std::string flightNumber;            // Flight identifier
    std::string violationType;           // Type of violation
    int actualSpeed;                     // Recorded speed at violation
    std::pair<int, int> allowedSpeedRange; // Permitted speed range
    int TimeStamp;                       // Time when violation occurred

    // Constructor
    LogEntry();
};

#endif // AIRCONTROLX_LOGENTRY_H