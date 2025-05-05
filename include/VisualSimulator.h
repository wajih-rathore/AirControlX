#ifndef AIRCONTROLX_VISUALSIMULATOR_H
#define AIRCONTROLX_VISUALSIMULATOR_H

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

/**
 * VisualSimulator class for graphical representation of the simulation.
 * Handles loading graphics and displaying the simulation visually.
 */
class VisualSimulator 
{
private:
    // SFML window for our simulation display
    sf::RenderWindow window;
    
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

public:
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
};

#endif // AIRCONTROLX_VISUALSIMULATOR_H