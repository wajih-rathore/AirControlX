#include "../include/VisualSimulator.h"

/**
 * Constructor initializes default values
 * Sets up isRunning flag to false initially
 */
VisualSimulator::VisualSimulator()
{
    // Initialize our running flag to false until everything is set up
    isRunning = false;
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
    // Create the main window (1280x720 resolution, with vertical sync)
    window.create(sf::VideoMode(1280, 720), "AirControlX - Air Traffic Control Simulation", sf::Style::Close);
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
    welcomeText.setPosition(
        (windowSize.x - textBounds.width) / 2.0f,
        30.0f  // 50 pixels from top
    );
    
    // All resources loaded successfully, set running flag to true
    isRunning = true;
    return true;
}

/**
 * Display the simulation visually
 * This is our main rendering function that clears the screen, 
 * draws our sprites and text, and displays them
 */
void VisualSimulator::display()
{
    // Clear the screen with a black color
    window.clear(sf::Color::Black);
    
    // Draw the background sprite
    window.draw(backgroundSprite);
    
    // Draw the welcome text
    window.draw(welcomeText);
    
    // Display everything we just drew (swaps buffers)
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