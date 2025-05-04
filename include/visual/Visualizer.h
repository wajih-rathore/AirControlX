#ifndef AIRCONTROLX_VISUALIZER_H
#define AIRCONTROLX_VISUALIZER_H

#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include <vector>

// Forward declaration to avoid circular dependency
class Simulation;

/**
 * Visualizer class for AirControlX
 * Handles the main SFML window and basic rendering operations
 * This is the base class for the visualization system in Module 3
 */
class Visualizer 
{
private:
    // SFML window for rendering
    sf::RenderWindow window;
    
    // Font for text rendering
    sf::Font font;
    
    // Background texture and sprite
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    
    // Reference to simulation (for accessing data)
    Simulation* simulation;
    
    // Timer variables
    sf::Clock timerClock;
    int elapsedSeconds;
    sf::Text timerText;
    
    // Helper methods for rendering different UI components
    void renderRunwayStatus();
    void renderSimulationStats();
    void renderAircraftList();
    void renderEmergencyStatus();
    void renderTimer();  // New method to render the timer
    
    // Helper method to create text with consistent styling
    sf::Text createText(const std::string& content, float x, float y, 
                       unsigned int size = 20, sf::Color color = sf::Color::White);
    
public:
    // Constructor and destructor
    Visualizer();
    ~Visualizer();
    
    // Initialize SFML components
    bool initialize();
    
    // Set simulation reference
    void setSimulation(Simulation* sim);
    
    // Basic rendering method
    void render();
    
    // Handle window events (close, keyboard input)
    void handleEvents();
    
    // Check if the window is still open
    bool isRunning() const;
    
    // Reset timer
    void resetTimer();
};

#endif // AIRCONTROLX_VISUALIZER_H