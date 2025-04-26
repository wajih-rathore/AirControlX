#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>

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
        for (size_t i = 0; i < airline->aircrafts.size(); i++) 
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




/* 

dear sagar :) 
sfml test karne k liyye comment out the above code 
and uncomment the below code, mere end pe toh chal raha hai

*/



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