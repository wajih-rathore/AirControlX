#include <iostream>
#include "../include/Simulation.h"

int main() 
{
    // Initialize and run the simulation with SFML visualization
    Simulation sim;
    
    // Initialize components (including visualizer)
    if (!sim.initialize()) {
        std::cerr << "Failed to initialize the simulation!" << std::endl;
        return 1;
    }
    
    // Run simulation with integrated SFML render loop
    if (!sim.run()) {
        std::cerr << "Error running simulation!" << std::endl;
        return 1;
    }
    
    // The waitForCompletion function is now just for cleanup
    sim.waitForCompletion();
    
    std::cout << "Simulation ended successfully." << std::endl;
    return 0;
}