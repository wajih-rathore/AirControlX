#include "../../include/visual/Visualizer.h"
#include "../../include/Simulation.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>  // Include this for std::sin function

/**
 * Constructor initializes default values
 */
Visualizer::Visualizer() 
{
    // Nothing to initialize here - we'll do it in initialize()
    simulation = nullptr;
    elapsedSeconds = 0;
}

/**
 * Destructor handles cleanup
 */
Visualizer::~Visualizer() 
{
    // SFML handles cleanup of resources automatically when objects are destroyed
}

/**
 * Reset the timer to zero
 */
void Visualizer::resetTimer()
{
    timerClock.restart();
    elapsedSeconds = 0;
}

/**
 * Initialize SFML components
 * 
 * This creates the window, loads fonts and textures, and sets up sprites
 * 
 * @return true if initialization succeeds, false otherwise
 */
bool Visualizer::initialize() 
{
    // Create the main window (1280x720 resolution, with vertical sync)
    window.create(sf::VideoMode(1280, 720), "AirControlX - Air Traffic Control Simulation", sf::Style::Close);
    window.setVerticalSyncEnabled(true);
    
    // Load the font for text rendering
    if (!font.loadFromFile("assets/arial.ttf")) 
    {
        std::cerr << "Error: Could not load font from assets/arial.ttf" << std::endl;
        return false;
    }
    
    // Load the background texture
    if (!backgroundTexture.loadFromFile("assets/background.png")) 
    {
        std::cerr << "Error: Could not load texture from assets/background.png" << std::endl;
        return false;
    }
    
    // Set up the background sprite to cover the entire window
    backgroundSprite.setTexture(backgroundTexture);
    
    // Scale the background to cover the window if needed
    sf::Vector2u textureSize = backgroundTexture.getSize();
    sf::Vector2u windowSize = window.getSize();
    
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    
    backgroundSprite.setScale(scaleX, scaleY);
    
    // Initialize the timer text
    timerText = createText("00:00", window.getSize().x - 100, 20, 24, sf::Color::White);
    timerText.setStyle(sf::Text::Bold);
    
    // Start the timer
    resetTimer();
    
    return true;
}

/**
 * Set the simulation reference
 * 
 * @param sim Pointer to the simulation object
 */
void Visualizer::setSimulation(Simulation* sim) 
{
    simulation = sim;
}

/**
 * Creates a text object with consistent styling
 * 
 * @param content Text content
 * @param x X position
 * @param y Y position
 * @param size Font size
 * @param color Text color
 * @return Configured text object
 */
sf::Text Visualizer::createText(const std::string& content, float x, float y, 
                               unsigned int size, sf::Color color) 
{
    sf::Text text;
    text.setFont(font);
    text.setString(content);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);
    
    // Add a subtle outline for better readability on varied backgrounds
    text.setOutlineColor(sf::Color(0, 0, 0, 150));  // Semi-transparent black
    text.setOutlineThickness(1.0f);
    
    return text;
}

/**
 * Render runway status information
 */
void Visualizer::renderRunwayStatus() 
{
    // Can't render runway status without simulation reference
    if (!simulation) return;
    
    // Runway status panel position
    const float panelX = 50;
    const float panelY = 50;
    
    // Draw panel title
    window.draw(createText("RUNWAY STATUS", panelX, panelY, 24, sf::Color::Yellow));
    
    // For now, just draw placeholder data
    // In Module 3B, we'll integrate with actual runway data
    window.draw(createText("RWY-A: ACTIVE", panelX, panelY + 40, 20, sf::Color::Green));
    window.draw(createText("RWY-B: STANDBY", panelX, panelY + 70, 20, sf::Color::Cyan));
    window.draw(createText("RWY-C: MAINTENANCE", panelX, panelY + 100, 20, sf::Color::Red));
}

/**
 * Render simulation statistics
 */
void Visualizer::renderSimulationStats() 
{
    // Can't render stats without simulation reference
    if (!simulation) return;
    
    // Stats panel position
    const float panelX = 900;
    const float panelY = 50;
    
    // Draw panel title
    window.draw(createText("SIMULATION STATS", panelX, panelY, 24, sf::Color::Yellow));
    
    // Draw simulation time - now showing actual time from the simulation
    std::ostringstream timeStr;
    int elapsedTime = simulation->getElapsedTime();
    int minutes = elapsedTime / 60;
    int seconds = elapsedTime % 60;
    
    timeStr << "Elapsed: " << std::setfill('0') << std::setw(2) << minutes << ":" 
            << std::setfill('0') << std::setw(2) << seconds;
            
    window.draw(createText(timeStr.str(), panelX, panelY + 40, 20, sf::Color::White));
    
    // Calculate remaining time
    int remainingTime = simulation->getRemainingTime();
    int remainingMinutes = remainingTime / 60;
    int remainingSeconds = remainingTime % 60;
    
    std::ostringstream remainingTimeStr;
    remainingTimeStr << "Remaining: " << std::setfill('0') << std::setw(2) << remainingMinutes << ":" 
                     << std::setfill('0') << std::setw(2) << remainingSeconds;
    
    window.draw(createText(remainingTimeStr.str(), panelX, panelY + 70, 20, sf::Color::White));
    
    // Draw simulation status based on paused state
    std::string statusText = simulation->isPausedState() ? "PAUSED" : "RUNNING";
    sf::Color statusColor = simulation->isPausedState() ? sf::Color::Yellow : sf::Color::Green;
    
    window.draw(createText("Status: " + statusText, panelX, panelY + 130, 20, statusColor));
    
    // Add instruction text
    window.draw(createText("Press SPACE to pause/resume", panelX, panelY + 160, 16, sf::Color(200, 200, 200)));
    window.draw(createText("Press ESC to exit", panelX, panelY + 185, 16, sf::Color(200, 200, 200)));
}

/**
 * Render aircraft list with enhanced visual style
 */
void Visualizer::renderAircraftList() 
{
    // Can't render aircraft list without simulation reference
    if (!simulation) return;
    
    // Aircraft list panel position
    const float panelX = 50;
    const float panelY = 200;
    
    // Draw panel title with subtle background for better visibility
    sf::RectangleShape titleBg(sf::Vector2f(300, 35));
    titleBg.setPosition(panelX - 10, panelY - 5);
    titleBg.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(titleBg);
    
    window.draw(createText("ACTIVE AIRCRAFT", panelX, panelY, 24, sf::Color::Yellow));
    
    // For now, draw placeholder data with a nicer background
    // In Module 3B, we'll integrate with actual aircraft data
    const int entryCount = 3;
    const std::string flights[entryCount] = {
        "PIA-1: Landing",
        "FedEx-2: Taxiing",
        "AirBlue-3: Departing"
    };
    
    const sf::Color flightColors[entryCount] = {
        sf::Color(0, 255, 128),  // Green for landing
        sf::Color(255, 255, 0),  // Yellow for taxiing 
        sf::Color(0, 191, 255)   // Blue for departing
    };
    
    // Draw each flight with a semi-transparent background
    for (int i = 0; i < entryCount; i++) {
        float y = panelY + 40 + (i * 30);
        
        // Background for each entry
        sf::RectangleShape entryBg(sf::Vector2f(250, 25));
        entryBg.setPosition(panelX - 5, y - 5);
        entryBg.setFillColor(sf::Color(0, 0, 40, 150));
        window.draw(entryBg);
        
        // Flight text
        window.draw(createText(flights[i], panelX, y, 18, flightColors[i]));
    }
}

/**
 * Render emergency status with visual enhancements
 */
void Visualizer::renderEmergencyStatus() 
{
    // Can't render emergency status without simulation reference
    if (!simulation) return;
    
    // Emergency panel position
    const float panelX = 50;
    const float panelY = 400;
    
    // Create a subtle background for the emergency status
    sf::RectangleShape emergencyBg(sf::Vector2f(400, 35));
    emergencyBg.setPosition(panelX - 10, panelY - 5);
    emergencyBg.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(emergencyBg);
    
    // No emergency indicator (default state)
    window.draw(createText("EMERGENCY STATUS: NONE", panelX, panelY, 24, sf::Color::Green));
    
    // For Module 3A, we'll add a pulsating effect to draw attention to emergencies
    // This will be a placeholder until Module 3B when we integrate with actual emergency data
    static float pulseTime = 0.0f;
    static sf::Clock pulseClock;
    
    // Create a pulsing effect for demonstration
    pulseTime += pulseClock.restart().asSeconds();
    float alpha = (std::sin(pulseTime * 3.0f) + 1.0f) * 0.5f * 255.0f;
    
    // Sample emergency indicator (will be populated with real data later)
    sf::RectangleShape emergencyIndicator(sf::Vector2f(380, 30));
    emergencyIndicator.setPosition(panelX, panelY + 40);
    emergencyIndicator.setFillColor(sf::Color(150, 0, 0, static_cast<sf::Uint8>(alpha)));
    window.draw(emergencyIndicator);
    
    // Will be replaced with actual emergency data in Module 3B
}

/**
 * Render the timer in the top right corner
 */
void Visualizer::renderTimer()
{
    // Check if we need to update the elapsed seconds
    int newSeconds = static_cast<int>(timerClock.getElapsedTime().asSeconds());
    
    if (newSeconds != elapsedSeconds)
    {
        // Update our elapsed seconds counter
        elapsedSeconds = newSeconds;
        
        // Calculate minutes and seconds
        int minutes = elapsedSeconds / 60;
        int seconds = elapsedSeconds % 60;
        
        // Format the time as "MM:SS"
        std::ostringstream timeStr;
        timeStr << std::setfill('0') << std::setw(2) << minutes << ":"
                << std::setfill('0') << std::setw(2) << seconds;
        
        // Update the timer text
        timerText.setString(timeStr.str());
    }
    
    // Add a subtle background for better visibility
    sf::RectangleShape timerBg(sf::Vector2f(80, 35));
    timerBg.setPosition(timerText.getPosition().x - 5, timerText.getPosition().y - 5);
    timerBg.setFillColor(sf::Color(0, 0, 0, 150));
    
    // Draw background and timer
    window.draw(timerBg);
    window.draw(timerText);
}

/**
 * Basic rendering method
 * 
 * Clears the window, draws the background and UI components, and displays the result
 */
void Visualizer::render() 
{
    // Clear the window with black color
    window.clear(sf::Color::Black);
    
    // Draw the background sprite
    window.draw(backgroundSprite);
    
    // Draw UI components
    renderRunwayStatus();
    renderSimulationStats();
    renderAircraftList();
    renderEmergencyStatus();
    renderTimer();  // Add timer rendering
    
    // Display what was drawn (swap buffers)
    window.display();
}

/**
 * Handle window events
 * 
 * Processes window close and keyboard events
 */
void Visualizer::handleEvents() 
{
    sf::Event event;
    while (window.pollEvent(event)) 
    {
        // Close the window if the close button or Escape key is pressed
        if (event.type == sf::Event::Closed ||
            (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) 
        {
            window.close();
        }
        
        // Toggle pause when space is pressed
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
        {
            if (simulation)
            {
                simulation->togglePause();
            }
        }
    }
}

/**
 * Check if the window is still open
 * 
 * @return true if the window is open, false otherwise
 */
bool Visualizer::isRunning() const 
{
    return window.isOpen();
}