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
class Timer 
{
    private:
        time_t startTime;
        int durationSeconds;
        bool running;
        
    public:
        Timer() 
        {
            startTime = 0;
            durationSeconds = 0;
            running = false;
        }
        
        void start() 
        {
            startTime = time(NULL);
            running = true;
        }
        
        void stop() 
        {
            running = false;
        }
        
        bool isTimeUp() 
        {
            if (!running) return false;
            return (getElapsedSeconds() >= durationSeconds);
        }
        
        int getElapsedSeconds() 
        {
            if (!running) return 0;
            return static_cast<int>(time(NULL) - startTime);
        }
};

// Aircraft class to represent a flight
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
    
        // Constructor
        Aircraft() 
        {
            FlightNumber = "";
            Airline = "";
            type = AirCraftType::Commercial;
            direction = Direction::North;
            state = FlightState::Holding;
            speed = 0;
            EmergencyNo = 0;
            HasViolation = false;
            isFaulty = false;
            isActive = false;
        }
        
        // Destructor
        ~Aircraft() {}
        
        // Simulate one step of the aircraft's movement
        void SimulateStep() 
        {
            UpdateSpeed();
            checkForViolation();
            checkForEmergency();
        }
        
        // Update aircraft speed based on its current state
        void UpdateSpeed() 
        {
            // Speed update logic based on flight state
            switch(state) 
            {
                case FlightState::Holding:
                    speed = 400 + rand() % 201; // 400-600 km/h
                    break;
                case FlightState::Approach:
                    speed = 240 + rand() % 51; // 240-290 km/h
                    break;
                case FlightState::Landing:
                    speed = max(30, speed - 20); // Gradually decelerate to 30 km/h
                    break;
                case FlightState::Taxi:
                    speed = 15 + rand() % 16; // 15-30 km/h
                    break;
                case FlightState::AtGate:
                    speed = rand() % 6; // 0-5 km/h
                    break;
                case FlightState::TakeoffRoll:
                    speed = min(290, speed + 15); // Accelerate up to 290 km/h
                    break;
                case FlightState::Climb:
                    speed = 250 + rand() % 214; // 250-463 km/h
                    break;
                case FlightState::Cruise:
                    speed = 800 + rand() % 101; // 800-900 km/h
                    break;
            }
        }
        
        // Check if aircraft is violating speed limits
        void checkForViolation() 
        {
            HasViolation = false;
            
            switch(state) 
            {
                case FlightState::Holding:
                    if (speed > 600) HasViolation = true;
                    break;
                case FlightState::Approach:
                    if (speed < 240 || speed > 290) HasViolation = true;
                    break;
                case FlightState::Landing:
                    if (speed > 240 || (speed < 30 && state == FlightState::Landing)) HasViolation = true;
                    break;
                case FlightState::Taxi:
                    if (speed > 30) HasViolation = true;
                    break;
                case FlightState::AtGate:
                    if (speed > 10) HasViolation = true;
                    break;
                case FlightState::TakeoffRoll:
                    if (speed > 290) HasViolation = true;
                    break;
                case FlightState::Climb:
                    if (speed > 463) HasViolation = true;
                    break;
                case FlightState::Cruise:
                    if (speed < 800 || speed > 900) HasViolation = true;
                    break;
            }
        }
        
        // Randomly generate emergency based on flight direction
        void checkForEmergency() 
        {
            int chance = rand() % 100;
            
            if (EmergencyNo > 0) return; // Already in emergency
            
            switch(direction) 
            {
                case Direction::North: // International Arrivals - 10%
                    if (chance < 10) EmergencyNo = 1 + rand() % 3; // Random emergency 1-3
                    break;
                case Direction::South: // Domestic Arrivals - 5%
                    if (chance < 5) EmergencyNo = 1 + rand() % 3;
                    break;
                case Direction::East: // International Departures - 15%
                    if (chance < 15) EmergencyNo = 1 + rand() % 3;
                    break;
                case Direction::West: // Domestic Departures - 20%
                    if (chance < 20) EmergencyNo = 1 + rand() % 3;
                    break;
            }
        }
        
        bool isinAir() 
        {
            return (state == FlightState::Holding || state == FlightState::Approach || state == FlightState::Climb || state == FlightState::Cruise);
        }
        
        bool isOnGround() 
        {
            return (state == FlightState::Taxi || state == FlightState::AtGate);
        }
        
        bool isReadyForTakeOff() 
        {
            return (state == FlightState::AtGate);
        }
        
        void RequestRunawayAssignment() 
        {
            // Request runway assignment
        }
        
        void AssignRunaway() 
        {
            // Assign runway
        }
};

// Runway class to manage runway operations
class RunwayClass 
{
    public:
        string id;
        RunwayType type;
        bool isOccupied;
        // Synchronization
        int turn;
        
        RunwayClass() 
        {
            id = "";
            type = RunwayType::Arrival;
            isOccupied = false;
            turn = 0;
        }
        
        bool tryAssign(Aircraft &plane) 
        {
            if (isOccupied) return false;
            
            // Assign runway based on direction
            switch(plane.direction) 
            {
                case Direction::North:
                case Direction::South:
                    if (type == RunwayType::Arrival || type == RunwayType::Flexible) 
                    {
                        isOccupied = true;
                        return true;
                    }
                    break;
                case Direction::East:
                case Direction::West:
                    if (type == RunwayType::Departure || type == RunwayType::Flexible) 
                    {
                        isOccupied = true;
                        return true;
                    }
                    break;
            }
            
            // Special case for cargo
            if (plane.type == AirCraftType::Cargo && type == RunwayType::Flexible) 
            {
                isOccupied = true;
                return true;
            }
            
            return false;
        }
        
        void release() 
        {
            isOccupied = false;
            turn++;
        }
};

// Log every violation
class LogEntry 
{
    public:
        string flightNumber;
        string violationType;
        int actualSpeed;
        pair<int, int> allowedSpeedRange;
        int TimeStamp;

        LogEntry() 
        {
            flightNumber = "";
            violationType = "";
            actualSpeed = 0;
            TimeStamp = 0;
        }
};

// Load and display graphics
class VisualSimulator 
{
    public:
        void LoadGraphics() {}
        void Display() {}
};

// Monitor aircraft speed and detect violations
class Radar 
{
    public:
        int range;
        
        Radar() 
        {
            range = 100; // Default radar range in km
        }
        
        int monitorAirCraft(Aircraft& plane) 
        {
            if (plane.HasViolation) 
            {
                return 1; // Return 1 if violation detected
            }
            return 0;
        }
};

// Monitor and manage air traffic
class ATCScontroller 
{
    public:
        void monitorFlight() {}
        void assignRunway() {}
        void handleViolations() {}
};

// Schedule flights based on priority
class FlightsScheduler 
{
    public:
        queue<Aircraft> arrivalQueue;
        queue<Aircraft> departureQueue;
        void scheduleFlight() {}
};

// Violation challan can be cleared
class StripePayment 
{
    public:
        bool processPayment(string avnID, int amount) { return true; }
};

// To determine violations
class AVN 
{
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
        
        AVN() 
        {
            avnID = "";
            flightNumberl = "";
            airLine = "";
            speed = 0;
            time = 0;
            fineAmount = 0;
            paid = false;
            dueDate = "";
        }
        
        void calculateFine() 
        {
            // Calculate fine based on aircraft type
            if (airLine == "PIA" || airLine == "AirBlue") 
            {
                fineAmount = 500000; // Commercial fine
            } else if (airLine == "FedEx" || airLine == "BlueDart") 
            {
                fineAmount = 700000; // Cargo fine
            }
            
            // Add 15% administrative fee
            fineAmount += static_cast<int>(fineAmount * 0.15);
        }
};

// Presents an interface for all current aircraft violations
class AirlinePortal 
{
    public:
        vector<AVN> avns;
        
        void viewAVNs() {}
        void markPaid() {}
};

// Airline class to manage airline fleet
class Airline 
{
    public:
        string name;
        AirCraftType type;
        vector<Aircraft> aircrafts;
        int activeFlights;
        int maxFlights;
        
        Airline() 
        {
            name = "";
            type = AirCraftType::Commercial;
            activeFlights = 0;
            maxFlights = 0;
        }
        
        bool canScheduleFlight() 
        {
            return activeFlights < maxFlights;
        }
        
        Aircraft* getAvailableAircraft() 
        {
            for (auto& aircraft : aircrafts) 
            {
                if (!aircraft.isActive) 
                {
                    return &aircraft;
                }
            }
            return nullptr; // No available aircraft
        }
};

// 20 AirCrafts
Aircraft Planes[20];
// 3 Runways
RunwayClass Runway[3];

// Six airlines as per requirements
Airline PIA;       // 6 aircraft, 4 max flights
Airline AirBlue;   // 4 aircraft, 4 max flights
Airline FedEx;     // 3 aircraft, 2 max flights
Airline PakAirforce; // 2 aircraft, 1 max flight
Airline BlueDart;  // 2 aircraft, 2 max flights
Airline AghaKhanAir; // 2 aircraft, 1 max flight

// Thread that will simulate a plane flight
void *Flight(void *arg);

// Helper functions for simulation
void initializeAirlines() 
{
    // Initialize PIA (Commercial)
    PIA.name = "PIA";
    PIA.type = AirCraftType::Commercial;
    PIA.maxFlights = 4;
    PIA.aircrafts.resize(6);
    
    // Initialize AirBlue (Commercial)
    AirBlue.name = "AirBlue";
    AirBlue.type = AirCraftType::Commercial;
    AirBlue.maxFlights = 4;
    AirBlue.aircrafts.resize(4);
    
    // Initialize FedEx (Cargo)
    FedEx.name = "FedEx";
    FedEx.type = AirCraftType::Cargo;
    FedEx.maxFlights = 2;
    FedEx.aircrafts.resize(3);
    
    // Initialize PakAirforce (Military)
    PakAirforce.name = "Pakistan Airforce";
    PakAirforce.type = AirCraftType::Military;
    PakAirforce.maxFlights = 1;
    PakAirforce.aircrafts.resize(2);
    
    // Initialize BlueDart (Cargo)
    BlueDart.name = "Blue Dart";
    BlueDart.type = AirCraftType::Cargo;
    BlueDart.maxFlights = 2;
    BlueDart.aircrafts.resize(2);
    
    // Initialize AghaKhanAir (Medical)
    AghaKhanAir.name = "AghaKhan Air";
    AghaKhanAir.type = AirCraftType::Medical;
    AghaKhanAir.maxFlights = 1;
    AghaKhanAir.aircrafts.resize(2);
    
    // Initialize each aircraft in each airline
    for (auto& airline : {&PIA, &AirBlue, &FedEx, &PakAirforce, &BlueDart, &AghaKhanAir}) 
    {
        for (int i = 0; i < airline->aircrafts.size(); i++) 
        {
            airline->aircrafts[i].Airline = airline->name;
            airline->aircrafts[i].type = airline->type;
            airline->aircrafts[i].FlightNumber = airline->name + "-" + to_string(i+1);
            airline->aircrafts[i].isActive = false;
        }
    }
}

void initializeRunways() 
{
    // RWY-A: North-South alignment (arrivals)
    Runway[0].id = "RWY-A";
    Runway[0].type = RunwayType::Arrival;
    
    // RWY-B: East-West alignment (departures)
    Runway[1].id = "RWY-B";
    Runway[1].type = RunwayType::Departure;
    
    // RWY-C: Flexible for cargo/emergency/overflow
    Runway[2].id = "RWY-C";
    Runway[2].type = RunwayType::Flexible;
}

void simulateFlightMovements() 
{
    // Implement simulation of flight movements
}

void handleEmergencies() 
{
    // Implement handling of emergencies
}

void monitorAirspace() 
{
    // Implement monitoring of airspace for violations
}

void generateAVNsForViolations() 
{
    // Implement generation of AVNs for violations
}

int main() 
{
    // Seed the random number generator
    srand(time(NULL));
    
    // Initialize airlines and runways
    initializeAirlines();
    initializeRunways();
    
    cout << "AirControlX - Automated Air Traffic Control System" << endl;
    cout << "Module 1: System Rules & Restrictions" << endl;
    
    // Setup basic simulation for 5 minutes
    // To be completed...
    
    return 0;
}