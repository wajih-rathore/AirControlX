#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h> // For sleep function

// Include all header files
#include "Common.h"
#include "Aircraft.h"
#include "Airline.h"
#include "FlightsScheduler.h"
#include "ATCScontroller.h"
#include "Radar.h"
#include "Runway.h"
#include "Timer.h"
#include "VisualSimulator.h"
#include "AirlinePortal.h"
#include "LogEntry.h"
#include "AVN.h"
#include "StripePayment.h"

using namespace std;

// 3 Runways
RunwayClass Runway[3];

// Six airlines as per requirements
Airline PIA;       // 6 aircraft, 4 max flights
Airline AirBlue;   // 4 aircraft, 4 max flights
Airline FedEx;     // 3 aircraft, 2 max flights
Airline PakAirforce; // 2 aircraft, 1 max flight
Airline BlueDart;  // 2 aircraft, 2 max flights
Airline AghaKhanAir; // 3 aircraft, 1 max flight

// ATC controller (global so it can be accessed from flight threads)
ATCScontroller atcController;

//Threads, Each Thread is an Aircraft
pthread_t plane_thread[20] = {0};
int threadIndex = 0;

// Mutex to protect console output to prevent garbled text
pthread_mutex_t console_mutex = PTHREAD_MUTEX_INITIALIZER;

// Thread that will simulate a plane flight
void *Flight(void *arg)
{
    // Cast the argument back to an Aircraft pointer
    Aircraft* plane = static_cast<Aircraft*>(arg);
    
    // Mark the plane as active
    plane->isActive = true;
    
    // Lock the console mutex to prevent garbled output
    pthread_mutex_lock(&console_mutex);
    cout << "Flight " << plane->FlightNumber << " is now active" << endl;
    pthread_mutex_unlock(&console_mutex);

    // Set flight direction based on type (for simulation purposes)
    // North/South for arrivals, East/West for departures
    // We'll use a simple rule: even index = arrival, odd index = departure
    if (plane->aircraftIndex % 2 == 0) 
    {
        // Arrival flow - either North or South
        plane->direction = (plane->aircraftIndex % 4 == 0) ? Direction::North : Direction::South;
        
        // For arrivals, start at Holding state
        plane->state = FlightState::Holding;
        
        // Add to arrival queue
        atcController.scheduleArrival(plane);
        
        pthread_mutex_lock(&console_mutex);
        cout << "Flight " << plane->FlightNumber << " entering from "  << (plane->direction == Direction::North ? "North" : "South") << " has entered the arrival queue" << endl;
        pthread_mutex_unlock(&console_mutex);
        
        // Simulate the arrival sequence
        
        // Holding phase - waiting for runway assignment
        int waitTime = 0;
        while (!plane->hasRunwayAssigned && waitTime < 30) 
        {
            sleep(1);  // Check every second if runway assigned
            waitTime++;
            
            // Every 5 seconds, print status update with estimated wait time
            if (waitTime % 5 == 0) 
            {
                int estimatedWait = atcController.getScheduler()->estimateWaitTime(plane);
                
                pthread_mutex_lock(&console_mutex);
                cout << "Flight " << plane->FlightNumber << " holding, estimated wait: " << estimatedWait << " minutes" << endl;
                pthread_mutex_unlock(&console_mutex);
            }
        }
        
        // If runway assigned, proceed with landing sequence
        if (plane->hasRunwayAssigned) 
        {
            pthread_mutex_lock(&console_mutex);
            cout << "Flight " << plane->FlightNumber << " has been assigned a runway!" << endl;
            pthread_mutex_unlock(&console_mutex);
            
            // Approach phase
            plane->state = FlightState::Approach;
            plane->UpdateSpeed();
            sleep(3);
            
            // Landing phase
            plane->state = FlightState::Landing;
            plane->UpdateSpeed();
            sleep(2);
            
            // Taxi phase
            plane->state = FlightState::Taxi;
            plane->UpdateSpeed();
            sleep(2);
            
            // At gate
            plane->state = FlightState::AtGate;
            plane->UpdateSpeed();
            
            pthread_mutex_lock(&console_mutex);
            cout << "Flight " << plane->FlightNumber << " has arrived at gate" << endl;
            pthread_mutex_unlock(&console_mutex);
            
            // Release the runway now that we're at the gate
            for (int i = 0; i < 3; i++) 
            {
                if (Runway[i].isOccupied) 
                {
                    Runway[i].release();
                    break;
                }
            }
        } 
        else 
        {
            pthread_mutex_lock(&console_mutex);
            cout << "Flight " << plane->FlightNumber << " timed out waiting for runway!" << endl;
            pthread_mutex_unlock(&console_mutex);
        }
    } 
    else 
    {
        // Departure flow - either East or West
        plane->direction = (plane->aircraftIndex % 4 == 1) ? Direction::East : Direction::West;
        
        // For departures, start at the gate
        plane->state = FlightState::AtGate;
        
        // Add to departure queue
        atcController.scheduleDeparture(plane);
        
        pthread_mutex_lock(&console_mutex);
        cout << "Flight " << plane->FlightNumber << " departing to " << (plane->direction == Direction::East ? "East" : "West") << " has entered the departure queue" << endl;
        pthread_mutex_unlock(&console_mutex);
        
        // Simulate the departure sequence
        
        // At gate phase - waiting for runway assignment
        int waitTime = 0;
        while (!plane->hasRunwayAssigned && waitTime < 30) 
        {
            sleep(1);  // Check every second if runway assigned
            waitTime++;
            
            // Every 5 seconds, print status update with estimated wait time
            if (waitTime % 5 == 0) 
            {
                int estimatedWait = atcController.getScheduler()->estimateWaitTime(plane);
                
                pthread_mutex_lock(&console_mutex);
                cout << "Flight " << plane->FlightNumber << " at gate, estimated wait: " << estimatedWait << " minutes" << endl;
                pthread_mutex_unlock(&console_mutex);
            }
        }
        
        // If runway assigned, proceed with takeoff sequence
        if (plane->hasRunwayAssigned) 
        {
            pthread_mutex_lock(&console_mutex);
            cout << "Flight " << plane->FlightNumber << " has been assigned a runway!" << endl;
            pthread_mutex_unlock(&console_mutex);
            
            // Taxi phase
            plane->state = FlightState::Taxi;
            plane->UpdateSpeed();
            sleep(2);
            
            // Takeoff phase
            plane->state = FlightState::TakeoffRoll;
            plane->UpdateSpeed();
            sleep(2);
            
            // Climb phase
            plane->state = FlightState::Climb;
            plane->UpdateSpeed();
            sleep(2);
            
            // Cruise phase
            plane->state = FlightState::Cruise;
            plane->UpdateSpeed();
            
            pthread_mutex_lock(&console_mutex);
            cout << "Flight " << plane->FlightNumber << " has reached cruising altitude" << endl;
            pthread_mutex_unlock(&console_mutex);
            
            // Release the runway now that we're airborne
            for (int i = 0; i < 3; i++) 
            {
                if (Runway[i].isOccupied) 
                {
                    Runway[i].release();
                    break;
                }
            }
        } 
        else 
        {
            pthread_mutex_lock(&console_mutex);
            cout << "Flight " << plane->FlightNumber << " timed out waiting for runway!" << endl;
            pthread_mutex_unlock(&console_mutex);
        }
    }
    
    // Flight is now complete or timed out
    plane->isActive = false;
    
    pthread_mutex_lock(&console_mutex);
    cout << "Flight " << plane->FlightNumber << " has completed its journey" << endl;
    pthread_mutex_unlock(&console_mutex);
    
    // Exit thread
    pthread_exit(nullptr);
}

// Helper functions for simulation
void initializeAirlines() 
{
    // Initialize PIA (Commercial)
    PIA.name = "PIA";
    PIA.type = AirCraftType::Commercial;
    PIA.maxFlights = 4;
    PIA.aircrafts.reserve(6); 
    
    // Initialize AirBlue (Commercial)
    AirBlue.name = "AirBlue";
    AirBlue.type = AirCraftType::Commercial;
    AirBlue.maxFlights = 4;
    AirBlue.aircrafts.reserve(4);
    
    // Initialize FedEx (Cargo)
    FedEx.name = "FedEx";
    FedEx.type = AirCraftType::Cargo;
    FedEx.maxFlights = 2;
    FedEx.aircrafts.reserve(3);
    
    // Initialize PakAirforce (Military)
    PakAirforce.name = "Pakistan Airforce";
    PakAirforce.type = AirCraftType::Military;
    PakAirforce.maxFlights = 1;
    PakAirforce.aircrafts.reserve(2);
    
    // Initialize BlueDart (Cargo)
    BlueDart.name = "Blue Dart";
    BlueDart.type = AirCraftType::Cargo;
    BlueDart.maxFlights = 2;
    BlueDart.aircrafts.reserve(2);
    
    // Initialize AghaKhanAir (Medical)
    AghaKhanAir.name = "AghaKhan Air";
    AghaKhanAir.type = AirCraftType::Medical;
    AghaKhanAir.maxFlights = 1;
    AghaKhanAir.aircrafts.reserve(3);
    
    // Initialize each aircraft in each airline while passing suitable
    // arguments to the constructor
    for (auto& airline : {&PIA, &AirBlue, &FedEx, &PakAirforce, &BlueDart, &AghaKhanAir}) 
{
    for (int i = 0; i < static_cast<int>(airline->aircrafts.capacity()); ++i) 
    {
        // Emplace_back will create an object while passing the arguments to Constructor
        airline->aircrafts.emplace_back(i, airline->name, airline->type); 
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

// Function to display a dummy menu on a clean terminal
void displayDummyMenu() {
    // Clear the terminal screen
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    
    cout << "=======================================" << endl;
    cout << "       AirControlX Terminal Menu       " << endl;
    cout << "=======================================" << endl;
    cout << "1. View Active Flights" << endl;
    cout << "2. Check Runway Status" << endl;
    cout << "3. Schedule New Flight" << endl;
    cout << "4. Handle Emergency" << endl;
    cout << "5. Monitor Airspace" << endl;
    cout << "6. Generate Reports" << endl;
    cout << "7. Exit" << endl;
    cout << "=======================================" << endl;
    cout << "Enter your choice: ";
}

bool launchThreadsForAirline(Airline* airline)
{
    // Create a thread for each aircraft in the airline
    for (size_t i = 0; i < airline->aircrafts.size(); ++i) 
    {
        // Launch a thread for each aircraft
        int result = pthread_create(&plane_thread[threadIndex], NULL, Flight, (void*)&airline->aircrafts[i]);
        
        if (result != 0) {
            // Return false if thread creation fails
            cerr << "Error creating thread for Aircraft " << airline->aircrafts[i].FlightNumber << endl;
            return false;
        }

        // Increment thread index to use the next available thread slot
        ++threadIndex;
    }

    return true; // Successfully created threads for all aircrafts
}


int main() 
{
    // Seed the random number generator
    srand(time(NULL));
    
    cout << "AirControlX - Automated Air Traffic Control System" << endl;
    cout << "Module 2: Flight Scheduling Implementation" << endl << endl;
    
    // Initialize airlines and runways
    cout << "Initializing airlines and runways..." << endl;
    initializeAirlines();
    initializeRunways();
    
    // Create a separate thread for the ATC controller to monitor flights
    pthread_t atcThread;
    pthread_create(&atcThread, NULL, [](void *) -> void* {
        cout << "ATC controller active - monitoring flights" << endl;
        
        // Run the controller for the duration of the simulation
        for (int i = 0; i < 300; i++) { // 5 minutes (300 seconds)
            atcController.monitorFlight();
            sleep(1);  // Check every second
            
            // Every 20 seconds, print runway status
            if (i % 20 == 0) {
                pthread_mutex_lock(&console_mutex);
                cout << "\n--- RUNWAY STATUS UPDATE ---" << endl;
                for (int j = 0; j < 3; j++) {
                    cout << Runway[j].id << ": " 
                         << (Runway[j].isOccupied ? "OCCUPIED" : "AVAILABLE") << endl;
                }
                cout << "---------------------------\n" << endl;
                pthread_mutex_unlock(&console_mutex);
            }
        }
        
        return nullptr;
    }, nullptr);
    
    // Create random emergency for testing (1 in 3 chance)
    if (rand() % 3 == 0) {
        int randomAirlineIndex = rand() % 6;
        Airline* airlines[] = {&PIA, &AirBlue, &FedEx, &PakAirforce, &BlueDart, &AghaKhanAir};
        Airline* randomAirline = airlines[randomAirlineIndex];
        
        if (!randomAirline->aircrafts.empty()) {
            int randomAircraftIndex = rand() % randomAirline->aircrafts.size();
            randomAirline->aircrafts[randomAircraftIndex].EmergencyNo = 1 + rand() % 3;
            
            cout << "EMERGENCY ALERT: " << randomAirline->aircrafts[randomAircraftIndex].FlightNumber 
                 << " has declared emergency level " 
                 << randomAirline->aircrafts[randomAircraftIndex].EmergencyNo << endl;
        }
    }
    
    cout << "Launching aircraft threads..." << endl;
    sleep(1);  // Short pause for readability
    
    // Launch aircraft threads
    launchThreadsForAirline(&PIA);
    sleep(1);  // Stagger launches to make output more readable
    
    launchThreadsForAirline(&AirBlue);
    sleep(1);
    
    launchThreadsForAirline(&FedEx);
    sleep(1);
    
    launchThreadsForAirline(&PakAirforce);
    sleep(1);
    
    launchThreadsForAirline(&BlueDart);
    sleep(1);
    
    launchThreadsForAirline(&AghaKhanAir);
    
    cout << "All aircraft launched, simulation running..." << endl;
    
    // Wait for all aircraft threads to finish
    for (int i = 0; i < threadIndex; ++i) {
        pthread_join(plane_thread[i], nullptr);
    }
    
    // Join the ATC controller thread
    pthread_join(atcThread, nullptr);
    
    cout << "\nSimulation complete!" << endl;
    
    return 0;
}