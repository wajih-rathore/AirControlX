#ifndef AIRCONTROLX_VISUALSIMULATOR_H
#define AIRCONTROLX_VISUALSIMULATOR_H

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "Aircraft.h"
#include "RunwayManager.h"
#include "Airline.h"
/**
 * VisualSimulator class for graphical representation of the simulation.
 * Handles loading graphics and displaying the simulation visually.
 */
class VisualSimulator 
{
private:

    // SFML window for our simulation display
    sf::RenderWindow window;
    
    //Is the Animation running?
    bool runwayOccupied[3]; // Array to track runway occupancy status

    // Background texture and sprite
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    //Cabin Texture and sprite
    sf::Texture cabinTexture;
    sf::Sprite cabinSprite;

    //Runway A Texture and sprite
    sf::Texture runwayATexture;
    sf::Sprite runwayASprite;

    //Runway B Texture and sprite
    sf::Texture runwayBTexture;
    sf::Sprite runwayBSprite;

    //Runway C Texture and sprite
    sf::Texture runwayCTexture;
    sf::Sprite runwayCSprite;
    
    //Commercial Aircraft Texture and sprite
    sf::Texture commercialAircraftTexture;
    sf::Sprite commercialAircraftSprite;

    //Cargo Aircraft Texture and sprite
    sf::Texture cargoAircraftTexture;
    sf::Sprite cargoAircraftSprite;

    //Military Aircraft Texture and sprite
    sf::Texture militaryAircraftTexture;
    sf::Sprite militaryAircraftSprite;
    
    // Font for text rendering
    sf::Font font;
    
    // Text to display on screen
    sf::Text welcomeText;
    
    // Flag to track if the simulator is running
    bool isRunning;

    // Cache for rendered aircraft sprites
    std::map<std::string, sf::Sprite> aircraftSpriteCache;
    
    // Animation clock for coordinating animations
    sf::Clock animationClock;
    
    // Reference to active aircraft list for visualization
    std::vector<Aircraft*> aircraftList;
    
    // Reference to runway manager for runway visualization
    RunwayManager* runwayMgr;
    
    // Screen dimensions
    int screenWidth;
    int screenHeight;
    
    // Helper functions for rendering elements
    void renderBackground();
    void renderRunways();
    void renderAircraft();
    void renderUI();
    void renderStatusPanel();

public:

    std::vector<Airline*> airlines;

    // Constructor and destructor
    VisualSimulator();
    ~VisualSimulator();
    
    // Load graphics resources
    bool loadGraphics();
    
    // Display the simulation visually (main render loop)
    void display();

    // Handle window events (close, keyboard input)
    void handleEvents();
    
    // Check if window is still running
    bool running() const;

    //Landing And Departure Animation
    void animateLanding(int runwayIndex, int planeType, std::string PlaneType);
    void animateDeparture(int runwayIndex, int planeType, std::string PlaneType);
    
    // Animate aircraft landing
    
    // ======== SFML Integration Abstraction Functions ========
    
    /**
     * Set the list of aircraft to visualize
     * Updates internal reference to aircraft for rendering
     */
    void setAircraftList(const std::vector<Aircraft*>& aircraft);
    
    /**
     * Set runway manager for runway visualization
     * Updates internal reference to runway manager
     */
    void setRunwayManager(RunwayManager* manager);
    
    /**
     * Create aircraft sprite based on aircraft type
     * Returns sprite object configured with proper texture and position
     */
    sf::Sprite createAircraftSprite(const Aircraft* aircraft);
    
    /**
     * Update aircraft sprite position and rotation
     * Adjusts sprite based on aircraft state and position
     */
    void updateAircraftSprite(sf::Sprite& sprite, const Aircraft* aircraft);
    
    /**
     * Create runway sprite based on runway type
     * Returns sprite object configured with proper texture and position
     */
    sf::Sprite createRunwaySprite(const RunwayClass* runway);
    
    /**
     * Create text object for UI elements
     * Returns configured text object ready for display
     */
    sf::Text createText(const std::string& content, float x, float y, 
                        unsigned int size = 16, sf::Color color = sf::Color::White);
    
    /**
     * Map world coordinates to screen coordinates
     * Converts simulation position to pixel position
     */
    sf::Vector2f mapWorldToScreen(float worldX, float worldY);
    
    /**
     * Map screen coordinates to world coordinates
     * Converts pixel position to simulation position
     */
    sf::Vector2f mapScreenToWorld(float screenX, float screenY);
    
    /**
     * Highlight selected aircraft or runway
     * Draws selection indicator around the specified object
     */
    void drawSelectionHighlight(sf::RenderTarget& target, sf::FloatRect bounds, 
                               sf::Color color = sf::Color(255, 255, 0, 128));
    
    /**
     * Show detailed information panel for an object
     * Draws info panel with detailed status of aircraft or runway
     */
    void showInfoPanel(sf::RenderTarget& target, const sf::Vector2f& position, 
                      const std::string& info);
};

#endif // AIRCONTROLX_VISUALSIMULATOR_H