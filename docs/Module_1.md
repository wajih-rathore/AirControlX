OPERATING SYSTEMS - Spring 2025

# Project Title: AirControlX - Automated Air Traffic Control System
**Total Marks:** 350  
**Due Dates:**

- Module 1: 20 April-2025
- Module 2: 30-April-2025
- Module 3: 06-May-2025
- Demos: 07-10 May -2025

## Instructions

- This project is required to be completed in a group. A Group of 2 students are allowed
- No Cross-section groups are allowed.
- Zero marks will be awarded to the students involved in plagiarism.
- Project will be submitted in modules on given deadlines; however, evaluation will be done collectively in demonstrations.
- Failure to submit on the module deadline will result in deduction of -20% in each phase of project.
- You have to submit a single zip folder of .c/.cpp files.
- Folder name should be your name and Roll No.
- Also submit self-assessment form in final submission. Bring self-assessment form in hard form for demo.
- Be prepared for viva and demos after the submission of the project.

## Project Statement
AirControlX is a simulation of an Automated Air Traffic Control System (ATCS) at a multirunway international airport. This system will be developed in **C / C++** using core Operating Systems concepts such as process Management, Threading, Synchronization (mutexes, semaphores, Scheduling algorithms and Inter-process communication (IPC). The project includes live flight management, real-time analytics, airspace regulation enforcement, airline violation processing, and simulated payment. A visual simulation of runway operations and flight activity will also be implemented.

## Module 1: System Rules & Restrictions [100 Marks]
### Restrictions

1. **Airlines**

There will be six airlines simulated in the system as given in the following table:

| Airline                     | Type       | Aircrafts | Flights |
|-----------------------------|------------|-----------|---------|
| PIA                         | Commercial | 6         | 4       |
| AirBlue                     | Commercial | 4         | 4       |
| FedEx                       | Cargo      | 3         | 2       |
| Pakistan Airforce           | Military   | 2         | 1       |
| Blue Dart                   | Cargo      | 2         | 2       |
| AghaKhan Air Ambulance      | Medical    | 2         | 1       |

Flights are the aircrafts in operation.

2. **There are three Aircraft Types**

- Commercial Flight: Regular passenger aircraft.
- Cargo Flight: Heavy freight aircraft operating under restricted windows.
- Emergency Flight: MedEvac, military, or diverted flights with top priority.

3. **There are three runways available at the airport:**

- RWY-A: North-South alignment (arrivals)
- RWY-B: East-West alignment (departures)
- RWY-C: Flexible for cargo/emergency/overflow

Only one aircraft may use a runway at any given time.

4. **Flight Arrivals and Dispatching**

| Direction | Flight Type               | Time            | Emergency Probability          |
|-----------|---------------------------|-----------------|--------------------------------|
| North     | International Arrivals    | Every 3 minutes | 10% (low fuel/diversion)       |
| South     | Domestic Arrivals         | Every 2 minutes | 5% (air ambulance)             |
| East      | International Departures  | Every 2.5 minutes | 15% (military/priority)      |
| West      | Domestic Departures       | Every 4 minutes | 20% (VIP or urgent cargo)      |

Flights are assigned runways based on direction:  
RWY-A: North/South arrivals  
RWY-B: East/West departures  
RWY-C: Backup/emergency/cargo use

5. **Cargo Flight Restrictions**

RWY-C is exclusively used for cargo.  
In each simulation, there must be 1 cargo flight.  
Flights violating this restriction are denied or rescheduled unless marked high priority.

6. **Flight Speed Monitoring**

There are three Aircraft speed is based on the flight phase and altitude. Radar thread continuously monitors speed and phase, enforcing realistic restrictions.

**Arrival Simulation: Initial Speed**  
For arrivals, the initial speed will be set similarly based on the aircraft's approach to landing:

1. **Holding Phase** (in holding pattern before landing):  
   Initial Speed: Set between 400-600 km/h, based on the aircraft's approach and ATC instructions.
2. **Approach Phase** (final descent toward runway):  
   Initial Speed: Set between 240-290 km/h. This will be adjusted as the aircraft gets closer to the runway.  
   Violation Detection: If the aircraft's speed is below 240 km/h or above 290 km/h, a violation will occur.
3. **Landing Phase:**  
   Initial Speed: Set to 240 km/h at the start of the landing phase.  
   Deceleration: The aircraft will gradually decelerate to 30 km/h or lower.  
   Violation Detection: If the aircraft fails to decelerate below 30 km/h, a violation is triggered.
4. **Taxi Phase** (after landing):  
   Initial Speed: Randomly set between 15 km/h and 30 km/h (within allowed taxi range).
5. **At Gate** (Initial Speed):  
   Initial Speed: 0 km/h (aircraft is stationary).

**Arrival Flight Speed Rules**

| Aircraft State | Expected Speed (km/h) | Speed Range | Violation Criteria |
|----------------|----------------------|-------------|--------------------|
| Holding        | 400-600              | 400-600     | Speed exceeds 600 km/h → Violation (too fast to approach). The aircraft is expected to hold 1 KM around the airport. |
| Approach       | 240-290              | 240-290     | Speed below 240 km/h or above 290 km/h → Violation |
| Landing        | 240 → 30             | 240 → 30    | Speed exceeds 240 km/h or fails to slow below 30 km/h → Violation |
| Taxi           | 15-30                | 15-30       | Speed exceeds 30 km/h → Violation (not appropriate for taxi) |
| At Gate        | 0-5                  | 0-5         | Speed exceeds 10 km/h → Violation (should be stationary) |

**Departure Simulation: Initial Speed**  
For departures, the initial speed will be set based on the phase the aircraft is in:

1. **At Gate** (Initial Speed):  
   Initial Speed: 0 km/h (aircraft is stationary at the gate)  
   Transition to Taxi: When the aircraft begins taxiing, its speed will increase to match the Taxi phase speed range (15-30 km/h).
2. **Taxi Phase** (after Gate):  
   Initial Speed: Set to a random value between 15 km/h and 30 km/h (within the allowed taxiing speed range).  
   Transition to Takeoff Roll: Once at the runway, the speed will increase based on the Takeoff Roll phase speed.
3. **Takeoff Roll** (on the runway):  
   Initial Speed: 0 km/h (starts from standstill).  
   Speed must comply with the aircraft's current phase and ATC rules. Violations will trigger AVNs.

7. **Airspace Violation Notice (AVN)**

Every aircraft starts with Inactive AVN status.  
If a flight exceeds speed limits, an AVN is activated (like a challan).  
Violations activate an AVN, billed to the airline

8. **Simulation Time**

Each simulation run is 5 minutes long. That is, you are expected to show a timer, demonstrating the various flights that land or depart from the airport.

8. **Fault Handling (Ground Only)**

Ground faults (e.g., brake failure, hydraulic leak) may occur during taxi/gate operations. Faulty aircraft are towed and removed from queues.

### Deliverable:

1. A code implementing the aforementioned restrictions via **C / C++** structures and classes. Please note that this code will serve as the foundation for the next modules in the project.

If the code is not submitted for module 1, there will be a severe penalty of 20% marks deducted.