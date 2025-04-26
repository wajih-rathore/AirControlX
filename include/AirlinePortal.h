#ifndef AIRCONTROLX_AIRLINEPORTAL_H
#define AIRCONTROLX_AIRLINEPORTAL_H

#include <vector>
#include "AVN.h"

/**
 * AirlinePortal class for presenting an interface for airline violations.
 * Used by airlines to view and manage their violation notices.
 */
class AirlinePortal {
public:
    std::vector<AVN> avns;  // List of violation notices
    
    // View all violation notices
    void viewAVNs();
    
    // Mark a violation as paid
    void markPaid();
};

#endif // AIRCONTROLX_AIRLINEPORTAL_H