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
void VisualSimulator::display()
{
    // Clear the screen with a black color
    window.clear(sf::Color::Black);
    
    // Draw the background sprite (Commenting it for Now    )
    //window.draw(backgroundSprite);

    // Draw the cabin sprite
    window.draw(cabinSprite);

    //Draw the runway sprites
    window.draw(runwayASprite);
    window.draw(runwayBSprite);
    window.draw(runwayCSprite);

    // Draw the welcome text
    window.draw(welcomeText);
    
    //Testing Purpose only
    window.draw(commercialAircraftSprite);
    window.draw(cargoAircraftSprite);
    window.draw(militaryAircraftSprite);

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