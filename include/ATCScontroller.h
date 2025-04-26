#ifndef AIRCONTROLX_ATCSCONTROLLER_H
#define AIRCONTROLX_ATCSCONTROLLER_H

/**
 * ATCScontroller class for monitoring and managing air traffic.
 * Responsible for overseeing all flights and handling violations.
 */
class ATCScontroller {
public:
    // Monitor flights in the airspace
    void monitorFlight();
    
    // Assign runways to aircraft
    void assignRunway();
    
    // Handle detected violations
    void handleViolations();
};

#endif // AIRCONTROLX_ATCSCONTROLLER_H