#include "../include/AirlineManager.h"
#include <cstdlib>  // For rand()
#include <ctime>    // For time()

/**
 * Constructor initializes empty airlines collection
 */
AirlineManager::AirlineManager() 
{
    // Nothing to initialize here - airlines will be created in initialize()
}

/**
 * Destructor cleans up dynamically allocated airlines
 */
AirlineManager::~AirlineManager() 
{
    // Clean up dynamically allocated airline objects
    for (auto airline : airlines) {
        delete airline;
    }
    airlines.clear();
}

/**
 * Initialize all airlines with predefined configurations
 * Creates the six standard airlines with their aircraft
 */
void AirlineManager::initialize() 
{
    // Clear any existing airlines
    for (auto airline : airlines) {
        delete airline;
    }
    airlines.clear();
    
    // Create PIA (Commercial)
    Airline* pia = new Airline();
    pia->name = "PIA";
    pia->type = AirCraftType::Commercial;
    pia->maxFlights = 4;
    pia->aircrafts.reserve(pia->maxFlights );
    
    // Create AirBlue (Commercial)
    Airline* airBlue = new Airline();
    airBlue->name = "AirBlue";
    airBlue->type = AirCraftType::Commercial;
    airBlue->maxFlights = 4;
    airBlue->aircrafts.reserve(airBlue->maxFlights);
    
    // Create FedEx (Cargo)
    Airline* fedEx = new Airline();
    fedEx->name = "FedEx";
    fedEx->type = AirCraftType::Cargo;
    fedEx->maxFlights = 2;
    fedEx->aircrafts.reserve(fedEx->maxFlights);
    
    // Create PakAirforce (Military)
    Airline* pakAirforce = new Airline();
    pakAirforce->name = "Pakistan Airforce";
    pakAirforce->type = AirCraftType::Military;
    pakAirforce->maxFlights = 1;
    pakAirforce->aircrafts.reserve(pakAirforce->maxFlights);
    
    // Create BlueDart (Cargo)
    Airline* blueDart = new Airline();
    blueDart->name = "Blue Dart";
    blueDart->type = AirCraftType::Cargo;
    blueDart->maxFlights = 2;
    blueDart->aircrafts.reserve(blueDart->maxFlights);
    
    // Create AghaKhanAir (Medical)
    Airline* aghaKhanAir = new Airline();
    aghaKhanAir->name = "AghaKhan Air";
    aghaKhanAir->type = AirCraftType::Medical;
    aghaKhanAir->maxFlights = 1;
    aghaKhanAir->aircrafts.reserve(aghaKhanAir->maxFlights);
    
    // Add all airlines to collection
    airlines = {pia, airBlue, fedEx, pakAirforce, blueDart, aghaKhanAir};
    
    // Initialize each aircraft in each airline
    for (auto airline : airlines) 
    {
        for (int i = 0; i < static_cast<int>(airline->aircrafts.capacity()); ++i) 
        {
            // Create an aircraft with the airline's info
            airline->aircrafts.emplace_back(i, airline->name, airline->type);
        }
    }
}

/**
 * Get all airlines in the system
 */
std::vector<Airline*>& AirlineManager::getAllAirlines() 
{
    return airlines;
}

/**
 * Get an airline by name
 */
Airline* AirlineManager::getAirlineByName(const std::string& name) 
{
    for (auto airline : airlines) {
        if (airline->name == name) {
            return airline;
        }
    }
    return nullptr;  // Airline not found
}

/**
 * Get a random airline (useful for emergency generation)
 */
Airline* AirlineManager::getRandomAirline() 
{
    if (airlines.empty()) {
        return nullptr;
    }
    
    int randomIndex = rand() % airlines.size();
    return airlines[randomIndex];
}