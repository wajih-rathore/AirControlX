#include <iostream>
#include<vector>
#include<string>
#include<time.h>
#include<queue>
using namespace std;

enum class AirCraftType {Commercial, Cargo, Emergency, Military, Medical};
enum class FlightState {Holding, Approach, Landing, Taxi, AtGate, TakeoffRoll, Climb, Cruise};
enum class RunwayType {Arrival, Departure, Flexible};
enum class Direction {North, South, East, West};

//Timer Class
class Timer {
    private:
        time_t startTime;
        int durationSeconds;
        bool running;
        
    public:
        Timer();
        void start();
        void stop();
        bool isTimeUp();
        int getElapsedSeconds();
};

class Aircraft {
    public:
        string FlightNumber;
        string Airline; //PIA, AirBlue...
        AirCraftType type;
        Direction direction; // North, South, East, West
        FlightState state;
        Timer timer;
        int speed;
        int EmergencyNo; // 0(No Emergency)
        bool HasViolation;
        bool isFaulty;
        bool isActive;
    
        //********* Update All Values Accordingly
        void SimulateStep();
    
        //Constructor
        Aircraft();
        //Destructor
        ~Aircraft();
        void UpdateSpeed();
        void checkForViolation();
        void checkForEmergency();
        bool isinAir();
        bool isOnGround();
        bool isReadyForTakeOff();
        //Ummm....
        void RequestRunawayAssignment();
        void AssignRunaway();
};

class RunwayClass {
    public:
        string id;
        RunwayType type;
        bool isOccupied;
        //Synchronization
        int turn;
        RunwayClass() {}
        bool tryAssign(Aircraft &plane);
        void release() {}
};

//Log Every Violation
class LogEntry {
    public:
        string flightNumber;
        string violationType;
        int actualSpeed;
        pair<int, int> allowedSpeedRange;
        int TimeStamp;

        LogEntry() {}
};

//Load And Display Graphics
class VisualSimulator {
    public:
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
    public:
        void monitorFlight() {}
        void assignRunway() {}
        void handleViolations() {}
};

//Schdule Flights based on priority
class FlightsScheduler {
    public:
        queue<Aircraft> arrivalQueue;
        queue<Aircraft> departureQueue;
        void scheduleFlight() {}
};

//Violation Challan can be cleared
class StripePayment {
    public:
        bool processPayment(string avnID, int amount) { return true; }
};

//To Determine Violations
class AVN {
    public:
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
    public:
        vector<AVN> avns;
        void viewAVNs() {}
        void markPaid() {}
};

// Airline class to manage airline fleet
class Airline {
    public:
        string name;
        AirCraftType type;
        vector<Aircraft> aircrafts;
        int activeFlights;
        int maxFlights;
        
        Airline();
        bool canScheduleFlight();
        Aircraft* getAvailableAircraft();
};

// 20 AirCrafts
Aircraft Planes[20];
//3 Runways
RunwayClass Runway[3];

// Six airlines as per requirements
Airline PIA;       // 6 aircraft, 4 max flights
Airline AirBlue;   // 4 aircraft, 4 max flights
Airline FedEx;     // 3 aircraft, 2 max flights
Airline PakAirforce; // 2 aircraft, 1 max flight
Airline BlueDart;  // 2 aircraft, 2 max flights
Airline AghaKhanAir; // 2 aircraft, 1 max flight

//Thread that will Simulate a Plane Flight
void *Flight(void *arg);

// Helper functions for simulation
void initializeAirlines();
void initializeRunways();
void simulateFlightMovements();
void handleEmergencies();
void monitorAirspace();
void generateAVNsForViolations();

int main()
{
      
}