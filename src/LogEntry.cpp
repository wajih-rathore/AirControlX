#include "../include/LogEntry.h"
using namespace std;

// Constructor initializes log entry with default values
LogEntry::LogEntry() {
    flightNumber = "";
    violationType = "";
    actualSpeed = 0;
    allowedSpeedRange = make_pair(0, 0);
    TimeStamp = 0;
}