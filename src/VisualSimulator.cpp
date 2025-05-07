#include "../include/VisualSimulator.h"

/**
 * Constructor initializes default values
 * Sets up isRunning flag to false initially
 */
VisualSimulator::VisualSimulator()
{
    // Initialize our running flag to false until everything is set up
    isRunning = false;
    runwayMgr = nullptr;
    screenWidth = 1200;
    screenHeight = 600;

}

/**
 * Destructor handles cleanup
 * SFML handles most resource cleanup automatically when objects are destroyed
 */
VisualSimulator::~VisualSimulator()
{
    // SFML will handle cleanup of window, textures, etc.
}

/**
 * Load graphics resources for visual simulation
 * This initializes our SFML window, textures, sprites, and text
 * 
 * @return true if all resources loaded successfully, false otherwise
 */
bool VisualSimulator::loadGraphics()
{
    // Create the main window
    window.create(sf::VideoMode(1200, 600), "AirControlX - Air Traffic Control Simulation", sf::Style::Close);
    window.setVerticalSyncEnabled(true);  // Enable V-Sync to prevent screen tearing

    // Load the background texture
    if (!backgroundTexture.loadFromFile("assets/background.png"))
    {
        std::cerr << "Error: Failed to load background texture!" << std::endl;
        return false;
    }

    backgroundSprite.setTexture(backgroundTexture);

    // Scale background to fit window
    sf::Vector2u textureSize = backgroundTexture.getSize();
    sf::Vector2u windowSize = window.getSize();

    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;

    backgroundSprite.setScale(scaleX, scaleY);

    // Load font
    if (!font.loadFromFile("assets/arial.ttf"))
    {
        std::cerr << "Warning: Failed to load font, text will not display properly!" << std::endl;
    }

    // Welcome text setup
    welcomeText.setFont(font);
    welcomeText.setString("Welcome to AirControlX - Press Escape to Exit");
    welcomeText.setCharacterSize(20);
    welcomeText.setFillColor(sf::Color::White);
    welcomeText.setOutlineColor(sf::Color(0, 0, 0, 160));
    welcomeText.setOutlineThickness(0.5f);
    welcomeText.setPosition(660.0f, 10.0f);

    // Load cabin texture
    if (!cabinTexture.loadFromFile("assets/Cabin.png"))
    {
        std::cerr << "Error: Failed to load cabin texture!" << std::endl;
        return false;
    }

    float leftGap = 160.0f;
    cabinSprite.setTexture(cabinTexture);
    cabinSprite.setScale(0.5f, 0.59f);

    // Position cabin on the right side of the window
    sf::FloatRect cabinBounds = cabinSprite.getGlobalBounds();
    float cabinX = windowSize.x - cabinBounds.width;
    cabinSprite.setPosition(cabinX + leftGap, 0);

    // Load runway textures
    if (!runwayATexture.loadFromFile("assets/RunwayA.png") || 
        !runwayBTexture.loadFromFile("assets/RunwayB.png") ||
        !runwayCTexture.loadFromFile("assets/RunwayC.png"))
    {
        std::cerr << "Error: Failed to load runway textures!" << std::endl;
        return false;
    }


    runwayASprite.setTexture(runwayATexture);
    runwayASprite.setScale(0.38f, 0.2f);
    runwayASprite.setPosition(0, 0);

    float runwayAHeight = runwayASprite.getGlobalBounds().height;

    runwayBSprite.setTexture(runwayBTexture);
    runwayBSprite.setScale(0.38f, 0.2f);
    runwayBSprite.setPosition(0, runwayAHeight);

    float runwayBHeight = runwayBSprite.getGlobalBounds().height;

    runwayCSprite.setTexture(runwayCTexture);
    runwayCSprite.setScale(0.38f, 0.2f);
    runwayCSprite.setPosition(0, runwayAHeight + runwayBHeight);

    // Load aircraft textures
    if (!commercialAircraftTexture.loadFromFile("assets/Commercial.png") ||
        !cargoAircraftTexture.loadFromFile("assets/Cargo.png") ||
        !militaryAircraftTexture.loadFromFile("assets/Military.png"))
    {
        std::cerr << "Error: Failed to load aircraft textures!" << std::endl;
        return false;
    }


    float planeScale = 0.12f;
    commercialAircraftSprite.setTexture(commercialAircraftTexture);
    commercialAircraftSprite.setPosition(0, 100);
    commercialAircraftSprite.setScale(planeScale, planeScale + 0.02f);

    cargoAircraftSprite.setTexture(cargoAircraftTexture);
    cargoAircraftSprite.setPosition(200, 0);
    cargoAircraftSprite.setScale(planeScale, planeScale);

    militaryAircraftSprite.setTexture(militaryAircraftTexture);
    militaryAircraftSprite.setPosition(400, 0);
    militaryAircraftSprite.setScale(planeScale, planeScale);

    //Set Default Aircraft Sprites for Runway Aircraft
    for (int i = 0; i < 3; i++)
    {
        aircraftSpriteRunway[i].setTexture(commercialAircraftTexture);
        aircraftSpriteRunway[i].setScale(planeScale, planeScale);
        aircraftSpriteRunway[i].setPosition(0, 0);
    }

    // All resources loaded successfully
    isRunning = true;
    return true;
}

/**
 * Display the simulation visually
 * This is our main rendering function that clears the screen, 
 * draws our sprites and text, and displays them
 */
/*void VisualSimulator::display()
{
    // Clear the screen with a black color
    window.clear(sf::Color::Black);
    
    // Render all visual elements using our helper functions
    renderBackground();
    renderRunways();
    renderAircraft();
    renderUI();
    
    // Display everything we just drew (swaps buffers)
    window.display();
}*/

void VisualSimulator::display()
{
    // Clear the screen with a black color
    window.clear(sf::Color::Black);

    
    // Draw the runway sprites
    window.draw(runwayASprite);
    window.draw(runwayBSprite);
    window.draw(runwayCSprite);

    
    //Check which runway is occupied and draw the aircraft on it
    if (runwayOccupied[0]) {
        window.draw(aircraftSpriteRunway[0]);
    }
    if (runwayOccupied[1]) {
        window.draw(aircraftSpriteRunway[1]);
    }
    if (runwayOccupied[2]) {
        window.draw(aircraftSpriteRunway[2]);
    }
    // Draw the cabin sprite
    window.draw(cabinSprite);
    
    // Draw the welcome text
    window.draw(welcomeText);
    // Display everything we just drew (swap buffers)
    window.display();
}

/**
 * Handle window events
 * Processes window close button and ESC key to exit
 */
void VisualSimulator::handleEvents()
{
    // Create an event object to store the polled event
    sf::Event event;
    
    // Process all pending events
    while (window.pollEvent(event))
    {
        // Check if the user pressed the window's close button
        if (event.type == sf::Event::Closed)
        {
            window.close();
            isRunning = false;
        }
        
        // Check if the user pressed the Escape key
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
        {
            // Humorously log that the user is trying to escape from ATC duty!
            std::cout << "Escape detected! Air traffic controller abandoning duty!" << std::endl;
            window.close();
            isRunning = false;
        }
       
    }
}

/**
 * Check if the window is still running
 * 
 * @return true if the window is open and simulator is running, false otherwise
 */
bool VisualSimulator::running() const
{
    return isRunning && window.isOpen();
}

// ======== SFML Integration Abstraction Functions ========

/**
 * Set the list of aircraft to visualize
 * Updates internal reference to aircraft for rendering
 */
void VisualSimulator::setAircraftList(const std::vector<Aircraft*>& aircraft)
{
    // Store reference to aircraft list for visualization
    aircraftList = aircraft;
    
    // When aircraft list changes, clear sprite cache to ensure it's regenerated
    aircraftSpriteCache.clear();
}

/**
 * Set runway manager for runway visualization
 * Updates internal reference to runway manager
 */
void VisualSimulator::setRunwayManager(RunwayManager* manager)
{
    // Store reference to runway manager for visualization
    runwayMgr = manager;
}

/**
 * Create aircraft sprite based on aircraft type
 * Returns sprite object configured with proper texture and position
 */
sf::Sprite VisualSimulator::createAircraftSprite(const Aircraft* aircraft)
{
    // If we already have a cached sprite for this aircraft, return it
    if (aircraftSpriteCache.find(aircraft->FlightNumber) != aircraftSpriteCache.end())
    {
        return aircraftSpriteCache[aircraft->FlightNumber];
    }
    
    sf::Sprite sprite;
    
    // Select the appropriate texture based on aircraft type
    // This makes our code way more maintainable than having duplicate logic!
    switch (aircraft->type)
    {
        case AirCraftType::Commercial:
            sprite.setTexture(commercialAircraftTexture);
            break;
        case AirCraftType::Cargo:
            sprite.setTexture(cargoAircraftTexture);
            break;
        case AirCraftType::Military:
        case AirCraftType::Emergency:
        case AirCraftType::Medical:
            // For now, using military texture for emergency/medical too
            sprite.setTexture(militaryAircraftTexture);
            break;
        default:
            // Fallback to commercial if we have an unknown type
            // Better than crashing, right? ¯\_(ツ)_/¯
            sprite.setTexture(commercialAircraftTexture);
    }
    
    // Scale the sprite appropriately
    sprite.setScale(0.12f, 0.12f);
    
    // Set the origin to center for easier positioning and rotation
    // This makes the sprite rotate around its center, not the top-left corner
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2, bounds.height / 2);
    
    // Set initial position based on aircraft coordinates
    sf::Vector2f screenPos = mapWorldToScreen(aircraft->x_position, aircraft->y_position);
    sprite.setPosition(screenPos);
    
    // Set rotation based on aircraft direction
    sprite.setRotation(aircraft->getRotationAngle());
    
    // Cache the sprite for future use
    aircraftSpriteCache[aircraft->FlightNumber] = sprite;
    
    return sprite;
}

void VisualSimulator::animateLanding(int runwayIndex, int planeType, std::string PlaneType, int direction) {
    if (runwayIndex < 0 || runwayIndex >= 3) {
        std::cerr << "Invalid runway index: " << runwayIndex << std::endl;
        return;
    }

    // Check if the runway is already occupied
    if (runwayOccupied[runwayIndex]) {
        std::cerr << "Runway " << runwayIndex << " is already occupied!" << std::endl;
        return;
    }

    // Mark the runway as occupied
    runwayOccupied[runwayIndex] = true;

    // Set the aircraft type for this runway
    aircraftType[runwayIndex] = PlaneType;

    if(runwayIndex == 2) {
        runway3status = +1; // Emergency Arrival
    } 
    // Select the appropriate aircraft sprite based on planeType
    switch (planeType) {
        case 0: // Commercial
            aircraftSpriteRunway[runwayIndex] = commercialAircraftSprite;
            break;
        case 1: // Cargo
            aircraftSpriteRunway[runwayIndex] = cargoAircraftSprite;
            break;
        case 2: // Military
            aircraftSpriteRunway[runwayIndex] = militaryAircraftSprite;
            break;
        default:
            std::cerr << "Invalid plane type: " << planeType << std::endl;
            runwayOccupied[runwayIndex] = false;
            return;
    }

    aircraftSpriteRunway[runwayIndex].rotate(20); // Rotate the aircraft sprite to face the runway

    // Adjust the scaling of the sprite
    aircraftSpriteRunway[runwayIndex].setScale(0.1f, 0.1f);

   position[runwayIndex][0] = -40; 
   position[runwayIndex][1] =  runwayASprite.getGlobalBounds().height * runwayIndex ;


    // Set the direction for the animation
    this->direction[runwayIndex] = direction;

    // Restart the animation clock for this runway
    animationClockRunways[runwayIndex].restart();

}void animateDeparture(int runwayIndex, int planeType, std::string PlaneType, int direction){}

void VisualSimulator::animateDeparture(int runwayIndex, int planeType, std::string PlaneType, int direction) {
    if (runwayIndex < 0 || runwayIndex >= 3) {
        std::cerr << "Invalid runway index: " << runwayIndex << std::endl;
        return;
    }

    
    
    
    // Check if the runway is already occupied
    if (runwayOccupied[runwayIndex]) {
        std::cerr << "Runway " << runwayIndex << " is already occupied!" << std::endl;
        return;
    }

    // Mark the runway as occupied
    runwayOccupied[runwayIndex] = true;

    // Set the aircraft type for this runway
    aircraftType[runwayIndex] = PlaneType;

    if (runwayIndex == 2) {
        runway3status = -1; // Emergency Departure
    }

    // Select the appropriate aircraft sprite based on planeType
    switch (planeType) {
        case 0: // Commercial
            aircraftSpriteRunway[runwayIndex] = commercialAircraftSprite;
            break;
        case 1: // Cargo
            aircraftSpriteRunway[runwayIndex] = cargoAircraftSprite;
            break;
        case 2: // Military
            aircraftSpriteRunway[runwayIndex] = militaryAircraftSprite;
            break;
        default:
            std::cerr << "Invalid plane type: " << planeType << std::endl;
            runwayOccupied[runwayIndex] = false;
            return;
    }

    // Adjust the scaling of the sprite
    aircraftSpriteRunway[runwayIndex].setScale(0.1f, 0.1f);

    // Set the initial position of the aircraft based on the direction
   position[runwayIndex][0] = -10; // Start position for the aircraft
   position[runwayIndex][1] =  runwayASprite.getGlobalBounds().height * runwayIndex + 100;

    // Set the direction for the animation
    this->direction[runwayIndex] = direction;

    // Restart the animation clock for this runway
    animationClockRunways[runwayIndex].restart();

}

void VisualSimulator::updateRunway_1_AircraftsPosition() {
    int runwayIndex = 0; // Runway 1 corresponds to index 0

    if (!runwayOccupied[runwayIndex]) {
        return;
    }

    float elapsedTime = animationClockRunways[runwayIndex].getElapsedTime().asSeconds();


    // Movement phases
    if (elapsedTime <= 3.0f) {
        position[runwayIndex][0] += 3; // Approach
        if(position[runwayIndex][1] < 115)
             position[runwayIndex][1] += 1; // Simulate Downward movement 
    } else if (elapsedTime <= 5.0f) {
        if(aircraftSpriteRunway[runwayIndex].getRotation() > 0)
        {
            aircraftSpriteRunway[runwayIndex].rotate(-2); // Rotate to face the runway
            position[runwayIndex][1] += 4; // Simulate Downward movement 
        }
        position[runwayIndex][0] += 6; // Landing
    } else if (elapsedTime <= 7.0f) {
        if(aircraftSpriteRunway[runwayIndex].getRotation() > 0)
        {
            aircraftSpriteRunway[runwayIndex].rotate(-2); // Rotate to face the runway
            position[runwayIndex][1] += 4; // Simulate Downward movement 
        }
        position[runwayIndex][0] += 4; // Taxi
    } else {
        runwayOccupied[runwayIndex] = false;

        return;
    }

    // Update sprite position
    aircraftSpriteRunway[runwayIndex].setPosition(position[runwayIndex][0], position[runwayIndex][1]);
}

void VisualSimulator::updateRunway_2_AircraftsPosition() {
    int runwayIndex = 1; // Runway 2 corresponds to index 1

    // Check if the runway is occupied
    if (!runwayOccupied[runwayIndex]) {
        return; // No animation to update
    }

    // Get the elapsed time since the animation started
    float elapsedTime = animationClockRunways[runwayIndex].getElapsedTime().asSeconds();

    // Update position and state based on elapsed time
    if (elapsedTime <= 2.0f) {
        // Taxi Phase (2 seconds)
        position[runwayIndex][0] += 4; // Slow taxiing movement
    } else if (elapsedTime <= 4.0f) {
        // Takeoff Roll Phase (2 seconds)
        position[runwayIndex][0] += 5; // Faster movement during takeoff roll
    } else if (elapsedTime <= 5.2f) {
        // Climb Phase (2 seconds)
        if(aircraftSpriteRunway[runwayIndex].getRotation() > -12)
        {
            aircraftSpriteRunway[runwayIndex].rotate(-1); // Rotate to face the runway
        }
        position[runwayIndex][0] += 4; // Moderate movement during climb
        position[runwayIndex][1] -= 1; // Simulate upward movement
    } else if (elapsedTime <= 6.0f) {
        // Cruise Phase (2 seconds)
        if(aircraftSpriteRunway[runwayIndex].getRotation() < 0)
        {
            aircraftSpriteRunway[runwayIndex].rotate(+2); // Rotate to face the runway
        }
        position[runwayIndex][0] += 3; // Smooth movement during cruise
        position[runwayIndex][1] -= 0.5; 
    } else {
        // Departure complete
        runwayOccupied[runwayIndex] = false; // Mark the runway as free
    }
    aircraftSpriteRunway[runwayIndex].setPosition(position[runwayIndex][0], position[runwayIndex][1]);
}
void VisualSimulator::updateRunway_3_AircraftsPosition() {
    int runwayIndex = 2; // Runway 3 corresponds to index 2

    // Check if the runway is occupied
    if (!runwayOccupied[runwayIndex]) {
        return; // No animation to update
    }

    // Get the elapsed time since the animation started
    float elapsedTime = animationClockRunways[runwayIndex].getElapsedTime().asSeconds();

    if (runway3status == 1) {
        // Movement phases
        if (elapsedTime <= 3.0f) {
            position[runwayIndex][0] += 3; // Approach
            if(position[runwayIndex][1] < 550)
                position[runwayIndex][1] += 1; // Simulate Downward movement 
        } else if (elapsedTime <= 5.0f) {
            if(aircraftSpriteRunway[runwayIndex].getRotation() > 0)
            {
                aircraftSpriteRunway[runwayIndex].rotate(-20); // Rotate to face the runway
                position[runwayIndex][1] += 40; // Simulate Downward movement 
            }
            position[runwayIndex][0] += 6; // Landing
        } else if (elapsedTime <= 7.0f) {
            position[runwayIndex][0] += 4; // Taxi
        } else {
            runwayOccupied[runwayIndex] = false;
            return;
        }
    } else if (runway3status == -1) {
        // Update position and state based on elapsed time
        if (elapsedTime <= 2.0f) {
            // Taxi Phase (2 seconds)
            position[runwayIndex][0] += 4; // Slow taxiing movement
        } else if (elapsedTime <= 4.0f) {
            // Takeoff Roll Phase (2 seconds)
            position[runwayIndex][0] += 5; // Faster movement during takeoff roll
        } else if (elapsedTime <= 5.2f) {
            // Climb Phase (2 seconds)
            if(aircraftSpriteRunway[runwayIndex].getRotation() > -12)
            {
                aircraftSpriteRunway[runwayIndex].rotate(-1); // Rotate to face the runway
            }
            position[runwayIndex][0] += 4; // Moderate movement during climb
            position[runwayIndex][1] -= 1; // Simulate upward movement
        } else if (elapsedTime <= 6.0f) {
            // Cruise Phase (2 seconds)
            if(aircraftSpriteRunway[runwayIndex].getRotation() < 0)
            {
                aircraftSpriteRunway[runwayIndex].rotate(+2); // Rotate to face the runway
            }
            position[runwayIndex][0] += 3; // Smooth movement during cruise
            position[runwayIndex][1] -= 0.5; 
        } else {
            // Departure complete
            runwayOccupied[runwayIndex] = false; // Mark the runway as free
        }
    }
    aircraftSpriteRunway[runwayIndex].setPosition(position[runwayIndex][0], position[runwayIndex][1]);
}

void VisualSimulator::checkForArrivalsOrDepartures() {
    // Iterate through all airlines
    for (Airline* airline : airlines) {
        // Iterate through all aircraft in the airline
        int index = 0;
        for (Aircraft& aircraft : airline->aircrafts) {
            // Check if the aircraft has been assigned a runway and is active
            if (aircraft.hasRunwayAssigned && aircraft.isActive) {
                int runwayIndex = aircraft.assignedRunwayIndex;
        
                if(aircraft.type == AirCraftType::Emergency  || aircraft.type == AirCraftType::Cargo) {
                     runwayIndex = 2; // Emergency aircraft use Runway 3
                } else if(aircraft.direction == Direction::North || aircraft.direction == Direction::South) {
                    runwayIndex = 0; // North/South aircraft use Runway 1
                } else if (aircraft.direction == Direction::East || aircraft.direction == Direction::West) {
                    runwayIndex = 1; // East/West aircraft use Runway 2
                }

                // Ensure the runway index is valid
                if (runwayIndex < 0 || runwayIndex >= 3) {
                    std::cerr << "Invalid runway index for aircraft " << aircraft.FlightNumber <<" value : "<<runwayIndex<< std::endl;
                    continue;
                }
                
                int aircraftTypeArg = 1;
                if(aircraft.type == AirCraftType::Commercial) {
                    aircraftTypeArg = 0;
                } else if (aircraft.type == AirCraftType::Cargo) {
                    aircraftTypeArg = 1;
                } else if (aircraft.type == AirCraftType::Military) {
                    aircraftTypeArg = 2;
                }
                int direct = 1;
                if(aircraft.direction == Direction::West || aircraft.direction == Direction::North) {
                    direct = 1;
                } else if (aircraft.direction == Direction::East || aircraft.direction == Direction::South) {
                    direct = -1;
                }
                // Check if the runway is not already occupied
                if (!runwayOccupied[runwayIndex]) {
                    // Determine if the aircraft is arriving or departing
                    if (index % 2 == 0) {
                        // Call animateLanding for arriving aircraft
                        animateLanding(runwayIndex, aircraftTypeArg, aircraft.FlightNumber, direct);
                    } else {
                        // Call animateDeparture for departing aircraft
                        animateDeparture(runwayIndex, aircraftTypeArg, aircraft.FlightNumber, direct);
                    }

                }
            }
            index++;
        }
    }
}

void VisualSimulator::Update(){
    // Check for new arrivals or departures
    checkForArrivalsOrDepartures();

    // Update aircraft positions on each runway
    if(runwayOccupied[0]) {
        updateRunway_1_AircraftsPosition();
    }
    if(runwayOccupied[1]) {
        updateRunway_2_AircraftsPosition();
    }
    if(runwayOccupied[2]) {
        updateRunway_3_AircraftsPosition();
    }
   
}

/**
 * Update aircraft sprite position and rotation
 * Adjusts sprite based on aircraft state and position
 */
void VisualSimulator::updateAircraftSprite(sf::Sprite& sprite, const Aircraft* aircraft)
{
    // Update position
    sf::Vector2f screenPos = mapWorldToScreen(aircraft->x_position, aircraft->y_position);
    sprite.setPosition(screenPos);
    
    // Update rotation
    sprite.setRotation(aircraft->getRotationAngle());
    
    // Apply visual effects based on aircraft state
    // Reset color first (in case it was previously modified)
    sprite.setColor(sf::Color::White);
    
    // Apply color tint for special cases
    if (aircraft->EmergencyNo > 0)
    {
        // Red tint for emergency aircraft
        sprite.setColor(sf::Color(255, 100, 100, 255));  // Reddish
    }
    else if (aircraft->HasViolation || aircraft->hasActiveViolation)
    {
        // Yellow tint for aircraft with violations
        sprite.setColor(sf::Color(255, 255, 100, 255));  // Yellowish
    }
    
    // Update cached sprite
    aircraftSpriteCache[aircraft->FlightNumber] = sprite;
}

/**
 * Create runway sprite based on runway type
 * Returns sprite object configured with proper texture and position
 */
sf::Sprite VisualSimulator::createRunwaySprite(const RunwayClass* runway)
{
    sf::Sprite sprite;
    
    // Select the appropriate texture based on runway id
    if (runway->id == "RWY-A")
    {
        sprite.setTexture(runwayATexture);
    }
    else if (runway->id == "RWY-B")
    {
        sprite.setTexture(runwayBTexture);
    }
    else if (runway->id == "RWY-C")
    {
        sprite.setTexture(runwayCTexture);
    }
    
    // Scale the sprite appropriately
    sprite.setScale(0.38f, 0.2f);
    
    // Position based on runway's relative position
    float* pos = runway->getDisplayPosition();
    float x = pos[0] * screenWidth;
    float y = pos[1] * screenHeight;
    
    // Apply position
    sprite.setPosition(x, y);
    
    // If runway is occupied, add visual indicator
    if (runway->isOccupied)
    {
        // This is a simple color tint, but you could do more complex effects
        sprite.setColor(sf::Color(255, 200, 200)); // Light red tint
    }
    
    return sprite;
}

/**
 * Create text object for UI elements
 * Returns configured text object ready for display
 */
sf::Text VisualSimulator::createText(const std::string& content, float x, float y, 
                                    unsigned int size, sf::Color color)
{
    sf::Text text;
    text.setFont(font);
    text.setString(content);
    text.setCharacterSize(size);  // Font size in pixels
    text.setFillColor(color);
    text.setOutlineColor(sf::Color(0, 0, 0, 160));  // Semi-transparent black outline
    text.setOutlineThickness(0.5f);
    text.setPosition(x, y);
    
    return text;
}

/**
 * Map world coordinates to screen coordinates
 * Converts simulation position to pixel position
 */
sf::Vector2f VisualSimulator::mapWorldToScreen(float worldX, float worldY)
{
    // This is a simple linear mapping that you'll probably want to customize
    // based on your world coordinate system's range
    
    // Define the world bounds (adjust these to match your simulation)
    const float worldMinX = -1000.0f;
    const float worldMaxX = 1000.0f;
    const float worldMinY = -1000.0f;
    const float worldMaxY = 1000.0f;
    
    // Calculate the proportion of the world space
    float propX = (worldX - worldMinX) / (worldMaxX - worldMinX);
    float propY = (worldY - worldMinY) / (worldMaxY - worldMinY);
    
    // Map to screen space
    float screenX = propX * screenWidth;
    float screenY = propY * screenHeight;
    
    return sf::Vector2f(screenX, screenY);
}

/**
 * Map screen coordinates to world coordinates
 * Converts pixel position to simulation position
 */
sf::Vector2f VisualSimulator::mapScreenToWorld(float screenX, float screenY)
{
    // This is the inverse of mapWorldToScreen
    // Define the world bounds (should match those in mapWorldToScreen)
    const float worldMinX = -1000.0f;
    const float worldMaxX = 1000.0f;
    const float worldMinY = -1000.0f;
    const float worldMaxY = 1000.0f;
    
    // Calculate the proportion of the screen space
    float propX = screenX / screenWidth;
    float propY = screenY / screenHeight;
    
    // Map to world space
    float worldX = worldMinX + propX * (worldMaxX - worldMinX);
    float worldY = worldMinY + propY * (worldMaxY - worldMinY);
    
    return sf::Vector2f(worldX, worldY);
}

/**
 * Highlight selected aircraft or runway
 * Draws selection indicator around the specified object
 */
void VisualSimulator::drawSelectionHighlight(sf::RenderTarget& target, sf::FloatRect bounds, 
                                           sf::Color color)
{
    // Create a rectangle shape to highlight the selected object
    sf::RectangleShape highlight;
    
    // Make the highlight slightly larger than the object
    highlight.setSize(sf::Vector2f(bounds.width + 6, bounds.height + 6));
    highlight.setPosition(bounds.left - 3, bounds.top - 3);
    
    // Set the color and make it semi-transparent
    highlight.setFillColor(sf::Color::Transparent);
    highlight.setOutlineColor(color);
    highlight.setOutlineThickness(2.0f);
    
    // Draw the highlight
    target.draw(highlight);
}

/**
 * Show detailed information panel for an object
 * Draws info panel with detailed status of aircraft or runway
 */
void VisualSimulator::showInfoPanel(sf::RenderTarget& target, const sf::Vector2f& position, 
                                  const std::string& info)
{
    // Create the background rectangle
    sf::RectangleShape panel;
    
    // Calculate panel size based on text length
    float panelWidth = 10.0f + info.length() * 7.0f;  // Rough estimate of width needed
    float panelHeight = 80.0f;  // Fixed height for simplicity
    
    panel.setSize(sf::Vector2f(panelWidth, panelHeight));
    panel.setPosition(position);
    panel.setFillColor(sf::Color(0, 0, 0, 200));  // Semi-transparent black
    panel.setOutlineColor(sf::Color::White);
    panel.setOutlineThickness(1.0f);
    
    // Draw the panel
    target.draw(panel);
    
    // Create and draw the text
    sf::Text infoText = createText(info, position.x + 5.0f, position.y + 5.0f, 14, sf::Color::White);
    target.draw(infoText);
}

// Helper functions for rendering elements
void VisualSimulator::renderBackground()
{
    window.draw(backgroundSprite);
}

void VisualSimulator::renderRunways()
{
    // Make sure we have a runway manager
    if (!runwayMgr) return;
    
    // Draw each runway based on its state
    for (int i = 0; i < runwayMgr->getRunwayCount(); i++)
    {
        RunwayClass* runway = runwayMgr->getRunwayByIndex(i);
        if (runway)
        {
            // Create or update the runway sprite
            sf::Sprite runwaySprite = createRunwaySprite(runway);
            
            // Draw the runway
            window.draw(runwaySprite);
            
            // Draw runway information text
            std::string statusText = runway->getStatusText();
            
            // Position the text below the runway
            sf::FloatRect bounds = runwaySprite.getGlobalBounds();
            sf::Text text = createText(statusText, bounds.left, bounds.top + bounds.height + 5.0f, 12);
            
            // If runway is occupied, make text red
            if (runway->isOccupied)
            {
                text.setFillColor(sf::Color::Red);
            }
            
            window.draw(text);
        }
    }
}

void VisualSimulator::renderAircraft()
{
    // Skip if no aircraft to render
    if (aircraftList.empty()) return;
    
    // Draw each aircraft in the list
    for (Aircraft* aircraft : aircraftList)
    {
        // Skip inactive aircraft
        if (!aircraft->isActive) continue;
        
        // Create or update the aircraft sprite
        sf::Sprite aircraftSprite = createAircraftSprite(aircraft);
        updateAircraftSprite(aircraftSprite, aircraft);
        
        // Draw the aircraft
        window.draw(aircraftSprite);
        
        // Draw aircraft information text
        std::string displayText = aircraft->getDisplayName();
        
        // Position the text above the aircraft
        sf::FloatRect bounds = aircraftSprite.getGlobalBounds();
        sf::Text text = createText(displayText, bounds.left, bounds.top - 20.0f, 12);
        
        // Set text color based on aircraft state
        int* color = aircraft->getStatusColor();
        text.setFillColor(sf::Color(color[0], color[1], color[2]));
        
        window.draw(text);
    }
}

void VisualSimulator::renderUI()
{
    // Draw welcome text at the top
    window.draw(welcomeText);
    
    // Draw cabin sprite on left side
    window.draw(cabinSprite);
    
    // Draw status panel
    renderStatusPanel();
}

void VisualSimulator::renderStatusPanel()
{
    // Create a status panel in the bottom-right corner
    sf::RectangleShape panel;
    panel.setSize(sf::Vector2f(300.0f, 100.0f));
    panel.setPosition(screenWidth - 310.0f, screenHeight - 110.0f);
    panel.setFillColor(sf::Color(0, 0, 0, 180));  // Semi-transparent black
    panel.setOutlineColor(sf::Color::White);
    panel.setOutlineThickness(1.0f);
    
    // Draw the panel
    window.draw(panel);
    
    // Draw runway status if we have a runway manager
    if (runwayMgr)
    {
        sf::Text runwayStatus = createText(runwayMgr->getStatusSummary(), 
                                          screenWidth - 300.0f, screenHeight - 100.0f, 14);
        window.draw(runwayStatus);
    }
    
    // Draw aircraft count
    std::string countText = "Aircraft: " + std::to_string(aircraftList.size());
    sf::Text aircraftCount = createText(countText, 
                                     screenWidth - 300.0f, screenHeight - 80.0f, 14);
    window.draw(aircraftCount);
    
    // Draw time information (using system time)
    time_t now = time(nullptr);
    std::string timeText = "Simulation Time: " + std::string(ctime(&now));
    sf::Text timeDisplay = createText(timeText, 
                                    screenWidth - 300.0f, screenHeight - 60.0f, 14);
    window.draw(timeDisplay);
}