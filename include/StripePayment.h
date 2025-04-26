#ifndef AIRCONTROLX_STRIPEPAYMENT_H
#define AIRCONTROLX_STRIPEPAYMENT_H

#include <string>

/**
 * StripePayment class for handling violation payments.
 * Processes payments for Airspace Violation Notices (AVNs).
 */
class StripePayment {
public:
    // Process payment for a violation
    bool processPayment(std::string avnID, int amount);
};

#endif // AIRCONTROLX_STRIPEPAYMENT_H