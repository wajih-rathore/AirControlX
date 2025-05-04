#include "../include/VisualSimulator.h"
#include <iostream>

/**
 * Simple demonstration of the VisualSimulator class with SFML
 * Shows a window with background sprite that can be closed with Escape
 */
int main() 
{
    // Create our visual simulator
    VisualSimulator visualSim;
    
    // Try to load the graphics resources
    if (!visualSim.loadGraphics()) 
    {
        std::cerr << "Failed to load graphics resources. Exiting." << std::endl;
        return 1;
    }
    
    std::cout << "SFML window created successfully!" << std::endl;
    std::cout << "Press ESC key to close the window." << std::endl;
    
    // Main game loop - keep rendering while the window is open
    while (visualSim.running()) 
    {
        // Handle any window events (close button, keyboard input)
        visualSim.handleEvents();
        
        // Render the current frame
        visualSim.display();
        
        // A small sleep to prevent CPU from running at 100%
        // SFML's vertical sync should handle this, but just to be safe
        sf::sleep(sf::milliseconds(10));
    }
    
    std::cout << "Window closed. Exiting program." << std::endl;
    return 0;
}