#include "../include/AVN.h"
using namespace std;

// Constructor initializes AVN with default values
AVN::AVN() {
    avnID = "";
    flightNumberl = "";
    airLine = "";
    speed = 0;
    allowed = make_pair(0, 0);
    time = 0;
    fineAmount = 0;
    paid = false;
    dueDate = "";
}

// Calculate fine amount based on aircraft type
void AVN::calculateFine() {
    // Calculate fine based on aircraft type
    if (airLine == "PIA" || airLine == "AirBlue") {
        fineAmount = 500000; // Commercial fine
    } else if (airLine == "FedEx" || airLine == "BlueDart") {
        fineAmount = 700000; // Cargo fine
    }
    
    // Add 15% administrative fee
    fineAmount += static_cast<int>(fineAmount * 0.15);
}