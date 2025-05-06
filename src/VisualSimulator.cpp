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
    // Create the main window 1280x600 resolution, with vertical sync)
    window.create(sf::VideoMode(1200, 600), "AirControlX - Air Traffic Control Simulation", sf::Style::Close);
    window.setVerticalSyncEnabled(true);  // Enable V-Sync to prevent screen tearing
    
    // Load the background texture from the assets folder
    if (!backgroundTexture.loadFromFile("assets/background.png"))
    {
        std::cerr << "Error: Failed to load background texture!" << std::endl;
        return false;
    }
    // Set up the background sprite to cover the entire window
    backgroundSprite.setTexture(backgroundTexture);
    
    // Scale the background to fit the window if needed
    sf::Vector2u textureSize = backgroundTexture.getSize();
    sf::Vector2u windowSize = window.getSize();
    
    // Calculate scaling factors for X and Y dimensions
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    
    // Apply the calculated scale to the sprite
    backgroundSprite.setScale(scaleX, scaleY);
    
    // Load font for text rendering
    if (!font.loadFromFile("assets/arial.ttf"))
    {
        std::cerr << "Warning: Failed to load font, text will not display properly!" << std::endl;
        // Not returning false here as we can continue without text
    }
    
    // Set up welcome text
    welcomeText.setFont(font);
    welcomeText.setString("Welcome to AirControlX - Press Escape to Exit");
    welcomeText.setCharacterSize(20);  // Font size in pixels
    welcomeText.setFillColor(sf::Color::White);
    welcomeText.setOutlineColor(sf::Color(0, 0, 0, 160));  // Semi-transparent black outline
    welcomeText.setOutlineThickness(0.5f);
    
    // Center the text at the top of the screen
    sf::FloatRect textBounds = welcomeText.getLocalBounds();
    welcomeText.setPosition( 10.0f ,10.0f ); // Set Y position to 10 pixels from the top
    
    // Load other textures and sprites as needed (runways, aircraft, etc.)
    if (!cabinTexture.loadFromFile("assets/Cabin.png"))
    {
        std::cerr << "Error: Failed to load cabin texture!" << std::endl;
        return false;
    }
    cabinSprite.setTexture(cabinTexture);
    //Setting its position such that it occupies the left half of the screen
    cabinSprite.setPosition(0, 0);
    cabinSprite.setScale(0.5f, 0.59f); // Scale down to fit the window


    // Load runway textures and set up sprites
    if (!runwayATexture.loadFromFile("assets/RunwayA.png") || 
        !runwayBTexture.loadFromFile("assets/RunwayB.png") ||
        !runwayCTexture.loadFromFile("assets/RunwayC.png"))
    {
        std::cerr << "Error: Failed to load runway textures!" << std::endl;
        return false;
    }
    //Calculate the point where the right side of the cabin ends
    float cabinWidth = cabinSprite.getGlobalBounds().width;
    //Set the Gap between the cabin and the runway
    float gap = 10.0f; // Gap in pixels

    runwayASprite.setTexture(runwayATexture);
    runwayASprite.setPosition(cabinWidth + gap, 0); // Set position as needed
    runwayASprite.setScale(0.3f, 0.2f); // Scale down to fit the window

    //Set the Position of the runway B such that they are on the right side of the cabin and below the runway A
    float runwayAHeight = runwayASprite.getGlobalBounds().height;
    runwayBSprite.setTexture(runwayBTexture);
    runwayBSprite.setPosition(cabinWidth + gap, runwayAHeight); // Set position as needed
    runwayBSprite.setScale(0.3f, 0.2f); // Scale down to fit the window
    
    //Set the Position of the runway C such that they are on the right side of the cabin and below the runway B
    float runwayBHeight = runwayBSprite.getGlobalBounds().height;
    runwayCSprite.setTexture(runwayCTexture);
    runwayCSprite.setPosition(cabinWidth + gap, runwayBHeight + runwayAHeight); // Set position as needed
    runwayCSprite.setScale(0.3f, 0.2f); // Scale down to fit the window

    // Load aircraft textures and set up sprites
    if (!commercialAircraftTexture.loadFromFile("assets/Commercial.png") ||
        !cargoAircraftTexture.loadFromFile("assets/Cargo.png") ||
        !militaryAircraftTexture.loadFromFile("assets/Military.png"))
    {
        std::cerr << "Error: Failed to load aircraft textures!" << std::endl;
        return false;
    }
    commercialAircraftSprite.setTexture(commercialAircraftTexture);
    commercialAircraftSprite.setPosition(0, 0); // Set position as needed
    commercialAircraftSprite.setScale(0.1f, 0.1f); // Scale down to fit the window
    cargoAircraftSprite.setTexture(cargoAircraftTexture);
    cargoAircraftSprite.setPosition(200, 0); // Set position as needed
    cargoAircraftSprite.setScale(0.1f, 0.1f); // Scale down to fit the window
    militaryAircraftSprite.setTexture(militaryAircraftTexture);
    militaryAircraftSprite.setPosition(400, 0); // Set position as needed
    militaryAircraftSprite.setScale(0.1f, 0.1f); // Scale down to fit the window


    // All resources loaded successfully, set running flag to true
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

    // Draw the cabin sprite
    window.draw(cabinSprite);

    // Draw the runway sprites
    window.draw(runwayASprite);
    window.draw(runwayBSprite);
    window.draw(runwayCSprite);

    // Draw the welcome text
    window.draw(welcomeText);

    // Testing Purpose only
    window.draw(commercialAircraftSprite);
    window.draw(cargoAircraftSprite);
    window.draw(militaryAircraftSprite);

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
    sprite.setScale(0.1f, 0.1f);
    
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
    sprite.setScale(0.3f, 0.2f);
    
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