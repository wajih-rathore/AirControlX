#include <iostream>
#include "../include/Simulation.h"

int main() 
{
    Simulation sim;
    sim.initialize();
    sim.run(); 
    sim.waitForCompletion();

    return 0;
}