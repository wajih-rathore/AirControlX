#include <iostream>
#include<vector>
#include<string>
#include<time.h>
#include<queue>
using namespace std;

enum class AirCraftType {Commercial, Cargo, Emergency };
enum class FlightState {Holding, Approach, Landing, Taxi, AtGate, TakeoffRoll, Climb, Cruise};
enum class RunwayType {Arrival, Departure, Flexible};

//Timer Class
class Aircraft {
public:
    string FlightNumber;
    string Airline; //PIA, AirBlue...
    AirCraftType type;

    // 0 - 360 degree
    int direction;
    // Or
    string direction; //North, East, South, West
    Timer timer;
    int speed;
    int EmergencyNo; // 0(No Emergency)
    bool HasViolation;
    bool isFaulty;

    //********* Update All Values Accordingly
    void SimulateStep() {};

    //Constructor
    Aircraft() {};
    //Destructor
    ~Aircraft() {};
    void UpdateSpeed() {};
    void updateSpeed() {};
    void checkForViolation() {};
    void checkForEmergency() {};
    bool isinAir() { return true; }
    bool isOnGround() { return true; }
    bool isReadyForTakeOff() { return true; }
    //Ummm....
    void RequestRunawayAssignment() {}
    void AssignRunaway() {}

};
class RunwayClass {
    string id;
    RunwayType type;
    bool isOccupied;
    //Synchronization
    int turn;
    RunwayClass() {}
    bool tryAssign(Aircraft &plane) {}
    void release() {}
};

//Log Every Violation
class LogEntry {
    string flightNumber;
    string violationType;
    int actualSpeed;
    pair<int, int> allowedSpeedRange;
    int TimeStamp;

    LogEntry() {}
};
//Load And Display Graphics
class VisualSimulator {
    void LoadGraphics() {}
    void Display() {}
};
//Monitor AirCraft Speed and Detect Violations
class Radar {
public:
    int range;
    int monitorAirCraft(Aircraft& plane) { return 0; }
};
//Monitor and Manage Air Traffic
class ATCScontroller {
    void monitorFlight() {}
    void assignRunway() {}
    void handleViolations() {}
};
//Schdule Flights based on priority
class FlightsScheduler {
    queue<Aircraft> arrivalQueue;
    queue<Aircraft> departureQueue;
    void scheduleFlight() {}
};
//Violation Challan can be cleared
class StripePayment {
    bool processPayment(string avnID, int amount) { return true; }
};
//To Determine Violations
class AVN {
    string avnID;
    string flightNumberl;
    string airLine;
    int speed;
    pair<int, int> allowed;
    int time;
    int fineAmount;
    bool paid;
    string dueDate;
    void calculateFine() {}
};
//Presents an Interface for All Current AirCraft Violations
class AirlinePortal {
    vector<AVN> avns;
    void viewAVNs() {}
    void markPaid() {}
};

//Umm...
class Timer { };

// 20 AirCrafts
Aircraft Planes[20];
//3 Runways
RunwayClass Runway[3];

//Thread that will Simulate a Plane Flight
void *Flight(void *arg) {



}

int main()
{
      
}


