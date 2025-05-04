#include "../../include/visual/Visualizer.h"
#include "../../include/Simulation.h"
#include <iostream>
#include <sstream>
#include <iomanip>

/**
 * Constructor initializes default values
 */
Visualizer::Visualizer() 
{
    // Nothing to initialize here - we'll do it in initialize()
    simulation = nullptr;
}

/**
 * Destructor handles cleanup
 */
Visualizer::~Visualizer() 
{
    // SFML handles cleanup of resources automatically when objects are destroyed
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
    
    // Draw simulation time
    std::ostringstream timeStr;
    timeStr << "Elapsed: " << std::setfill('0') << std::setw(2) << 0 << ":" 
            << std::setfill('0') << std::setw(2) << 0;
            
    window.draw(createText(timeStr.str(), panelX, panelY + 40, 20, sf::Color::White));
    
    // Draw aircraft counts
    window.draw(createText("Active Flights: 0", panelX, panelY + 70, 20, sf::Color::White));
    window.draw(createText("In Queue: 0", panelX, panelY + 100, 20, sf::Color::White));
    
    // Draw simulation status
    window.draw(createText("Status: RUNNING", panelX, panelY + 130, 20, sf::Color::Green));
}

/**
 * Render aircraft list
 */
void Visualizer::renderAircraftList() 
{
    // Can't render aircraft list without simulation reference
    if (!simulation) return;
    
    // Aircraft list panel position
    const float panelX = 50;
    const float panelY = 200;
    
    // Draw panel title
    window.draw(createText("ACTIVE AIRCRAFT", panelX, panelY, 24, sf::Color::Yellow));
    
    // For Module 3A, just draw placeholder data
    // In Module 3B, we'll integrate with actual aircraft data
    window.draw(createText("PIA-1: Landing", panelX, panelY + 40, 18, sf::Color::White));
    window.draw(createText("FedEx-2: Taxiing", panelX, panelY + 70, 18, sf::Color::White));
    window.draw(createText("AirBlue-3: Departing", panelX, panelY + 100, 18, sf::Color::White));
}

/**
 * Render emergency status
 */
void Visualizer::renderEmergencyStatus() 
{
    // Can't render emergency status without simulation reference
    if (!simulation) return;
    
    // Emergency panel position
    const float panelX = 50;
    const float panelY = 400;
    
    // No emergency indicator (default state)
    window.draw(createText("EMERGENCY STATUS: NONE", panelX, panelY, 24, sf::Color::Green));
    
    // For Module 3A, use placeholder data
    // In Module 3B, we'll integrate with actual emergency data
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