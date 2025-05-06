## Module 3: Subsystems & Airline Integration [120 Marks]
**Functional requirements:** There will be processes managing the ATC, AVN Generator, AirlinePortal and StripePayment

1. **ATCS Controller Process [30]**

   **AirControlX will have these components to monitor and manage air traffic violations**  
   - **Violation Detection:**  
     AirControlX will monitor the speed, altitude, and position of every aircraft in the airspace. If any aircraft exceeds the designated airspace boundary, altitude limit, or speed restriction, the system will notify the AVN Generator Process.  
   - **Violation Status Updates:**  
     The system will update the violation status to "active" for any aircraft found violating airspace or flight parameters. This status will be tracked and available for the air traffic controllers to take appropriate actions.

2. **ATC Analytics**

   AirControlX will also maintain detailed analytics about the air traffic within the controlled airspace.  
   - **2.1 Air Traffic Analytics:**  
     AirControlX Dashboard display the following data:  
     - Number of Active Violations: It will keep track of the number of aircraft currently in violation of flight rules (e.g., altitude, speed, and airline name).  
     - Aircraft with Active Violations: The system will display the aircraft's ID and relevant violation status for air traffic controllers to assess and address the situation.  
     - The simulation shall visually represent aircraft in different travel states (taxiing, takeoff, etc.).  
     - Runway occupancy and flight movement must be animated using a C++ graphics library (e.g., SFML/OpenGL).

2. **AVN Generator Process [30]**  
   - a. The AVN Generator creates a detailed Airspace Violation Notice including:  
   - b. AVN ID, Airline name, Flight number, Aircraft type, Speed recorded vs. permissible, AVN issuance date and time, Total fine amount (including service fee), Payment status (default: unpaid), and Due date (3 days from issuance). The Detail of payment is calculated as follows:  
     - i. PKR 500,000 (Commercial)  
     - ii. PKR 700,000 (Cargo)  
     - iii. A 15% administrative service fee is added to the total  
   - c. This information is forwarded to the Airline Portal and StripePay process.  
   - d. Upon receiving a successful payment confirmation from StripePay, the AVN Generator:  
     - i. Updates the payment status to "paid"  
     - ii. Sends confirmation to the Airline Portal  
     - iii. Notifies the ATC controller that the airline has cleared the violation

3. **Airline Portal [20]**  
   - i. The Airline Portal is accessible by airline representatives using the aircraft ID and AVN issue date/time.  
   - ii. Airlines admin can view all active and historical AVNs, including:  
     - AVN ID  
     - Aircraft ID (Flight Number)  
     - Aircraft type  
     - Payment status (paid/unpaid/overdue)  
     - Total fine amount including service charges  
   - iii. On receiving a successful payment alert from the StripePay process, the Airline Portal updates the corresponding AVN entry to reflect the payment and displays all associated details.

4. **StripePay Process [20]**  
   - i. Airline Admin can pay the challan by using another process called StripePayment.  
   - ii. Stripepayment will receive AVNID, Aircraft ID (Flight Number), airctaft type and amount to be paid.  
   - iii. Airline Admin will enter the paid amount and the stripePayment process will send successful payment status to AVN Generator Process and Airline Portal

5. **Simulation [20]**

The system will simulate and visualize the Air Traffic Control flow at the airport. You may use libraries such as SFML or openGL or any graphic library in C++ to create graphical representations of the intersection, vehicles, and traffic light states. The graphical output will include:

- visual representation of Aircraft, runways and Air traffic Control (ATC) Tower.
- color coded runways and aircraft to be distinguished in simulation
- Animated movement of Aircraft.
- Arrival Flights with flight status
- departure Flights with flight status

### Deliverable:

1. A Complete code implementing the additional functionalities in the code developed in module 1 and module 2.
2. If the code is not submitted for module 3, there will be an additional severe penalty of 20% marks deducted.
3. Final submission includes self-assessment form.
4. Self-assessment form will be uploaded later.

Happy Coding