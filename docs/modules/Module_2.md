## Module 02 ATCS Core Functionalities [130 Marks]
1. **Flight Entry & Scheduling [15]**  
   - FR1.1: The system shall allow entry of arrival and departure flight data including flight number, airline, aircraft type, direction, priority, and scheduled time.  
   - FR1.2: The system shall generate a schedule queue for both incoming and outgoing flights.  
   - FR1.3: The system shall handle conflicts by automatically rescheduling or rerouting aircraft based on priority and runway availability.

2. **Runway Allocation [15]**  
   - FR2.1: The system shall allocate runways based on flight direction:  
     - RWY-A for arrivals (North/South)  
     - RWY-B for departures (East/West)  
     - RWY-C for cargo, emergency, and overflow  
   - FR2.2: Only one aircraft may use a runway at a time; this must be enforced with thread synchronization.  
   - FR2.3: In case of multiple high-priority arrivals/departures, RWY-C must be used as backup.

3. **Arrival & Departure Flow Management [60]**  
   - FR3.1: The system shall manage aircraft progress through phases:  
     - Holding → Approach → Land → Taxi → Gate (for arrivals),  
     - Gate → Taxi → Takeoff → Departure (for departures).  
   - FR3.2: Aircraft movements shall follow real-world timing and phase transitions controlled by simulation logic.  
   - FR3.3: Arrival priority shall be based on fuel status, emergency level, or type (e.g., Emergency > VIP > Cargo > Commercial).

4. **Priority Handling [20]**  
   - FR4.1: The system shall assign emergency and high-priority flights immediate runway access, pre-empting lower-priority flights.  
   - FR4.2: The system must re-order queues when an emergency flight is registered.

5. **Queue and Delay Management [10]**  
   - FR5.1: Flights unable to be assigned a runway must be queued based on priority and FCFS within priority class.  
   - FR5.2: The system shall calculate estimated wait time for every aircraft.

6. **Status Tracking [10]**  
   - FR6.1: The system must track each aircraft's current status (e.g., Waiting, Taxiing, Taking Off, Cruising, Landing, At Gate).  
   - FR6.2: Status must be updated in real-time based on simulation time and transition logic.

### Deliverable:

1. A code implementing the functionalities to the code developed in module 1.

If the code is not submitted for module 1, there will be an additional severe penalty of 20% marks deducted.