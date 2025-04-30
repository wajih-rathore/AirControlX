#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include<pthread.h>

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

//Threads, Each Thread is an Aircraft
pthread_t plane_thread[20] = {0};
int threadIndex = 0;

// Thread that will simulate a plane flight
void *Flight(void *arg)
{
    Aircraft* plane = static_cast<Aircraft*>(arg);
    cout<<"I'm a plane hehe";
    cout<<plane->FlightNumber<<endl;

    // Exit the thread cleanly
    pthread_exit(nullptr);  // Clean exit
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
    
    /* Starting Part to be Uncommented Later on

    // Seed the random number generator
    srand(time(NULL));
    
    // Initialize airlines and runways
    initializeAirlines();
    initializeRunways();
    
    cout << "AirControlX - Automated Air Traffic Control System" << endl;
    cout << "Module 1: System Rules & Restrictions" << endl;
    
    // Display the dummy menu on a clean terminal
    displayDummyMenu();
    
    // Wait for user input before exiting
    int choice;
    cin >> choice;
    
    cout << "You selected option " << choice << endl;
    cout << "This is a dummy implementation. Functionality coming soon!" << endl;
    */
    
    initializeAirlines();

    // Call the Function to create Threads of Aircrafts for all airlines
    launchThreadsForAirline(&PIA);
    launchThreadsForAirline(&AirBlue);
    launchThreadsForAirline(&FedEx);
    launchThreadsForAirline(&PakAirforce);
    launchThreadsForAirline(&BlueDart);
    launchThreadsForAirline(&AghaKhanAir);

    // Wait for all threads to finish
    for (int i = 0; i < threadIndex; ++i) 
        pthread_join(plane_thread[i], nullptr);


    return 0;
}




/*

#include <SFML/Graphics.hpp>

int main()
{
    // Create a window with the title "SFML Test"
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Test");

    // Create a green circle shape
    sf::CircleShape circle(100.f);
    circle.setFillColor(sf::Color::Green);
    circle.setPosition(350.f, 250.f); // Center the circle in the window

    // Main loop
    while (window.isOpen())
    {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear the window
        window.clear(sf::Color::Black);

        // Draw the circle
        window.draw(circle);

        // Display the window
        window.display();
    }

    return 0;
}

*/