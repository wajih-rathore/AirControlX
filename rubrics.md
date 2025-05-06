# Rubrics for AirControlX Project Evaluation
**Total Marks:** 350

## 1. Module 1: System Rules & Restrictions (100 Marks)

| Criteria | Excellent (90-100%) |
|----------|---------------------|
| **Aircraft & Airline Restrictions** (20 marks) | Correctly implements all 6 airlines, aircraft types, and flight counts. Handles cargo/emergency restrictions flawlessly. |
| **Runway Allocation** (20 marks) | Runways assigned per direction and type (A/B/C). Strict synchronization (mutex/semaphores) for single-use enforcement. |
| **Speed Monitoring & AVN** (30 marks) | Full compliance with speed rules for all phases (arrival/departure). AVN triggers correctly for violations. |
| **Simulation Time & Fault Handling** (15 marks) | 5-minute simulation runs smoothly. Ground faults handled (e.g., towing). |
| **Code Structure** (15 marks) | Clean, modular code with classes. Proper use of C++ and OS concepts. |

## 2. Module 2: ATCS Core Functionalities (130 Marks)

| Criteria | Excellent (90-100%) |
|----------|---------------------|
| **Flight Scheduling & Conflict Resolution** (25 marks) | Flights added dynamically. Conflict resolution (priority/FCFS) works perfectly. |
| **Runway Allocation & Sync** (25 marks) | Runway allocation aligns with direction/priority. Thread synchronization robust. |
| **Phase Management** (30 marks) | Seamless transitions (holding → landing, gate → departure). Realistic timing. |
| **Priority Handling** (25 marks) | Emergency flights pre-empt queues. RWY-C used for overflow. |
| **Queue & Status Tracking** (25 marks) | Real-time queue updates. Accurate status tracking (e.g., taxiing, cruising). |

## 3. Module 3: Subsystems & Airline Integration (120 Marks)

| Criteria | Excellent (90-100%) |
|----------|---------------------|
| **ATCS Controller & Violation Detection** (30 marks) | Real-time monitoring of speed/altitude. AVN activation accurate. |
| **AVN Generator & Payment** (30 marks) | AVN details (fine calculation, service fee) fully implemented. Payment confirmation works. |
| **Airline Portal & StripePay** (25 marks) | Portal displays AVNs and payment status. StripePay integration functional. |
| **Simulation & Visualization** (35 marks) | Smooth animations (SFML/OpenGL). Color-coded runways/aircraft. Real-time status updates. |

## 4. General Evaluation (Weighted Across Modules)

| Criteria | Marks | Description |
|----------|-------|-------------|
| **Code Quality** | 20 | Clean, efficient, and well-documented code. Proper use of OS concepts (threading, IPC). |
| **Project Report & Self-Assessment** | 15 | Detailed report explaining design choices. Honest self-assessment form. |
| **Timeliness** | Penalties | -20% per module for late submission (applied to module marks). |
| **Plagiarism** | Zero | Immediate disqualification if detected. |
| **Viva Performance** | 30 | Clear understanding of OS concepts. Ability to justify design decisions and debug issues. |

### Penalties

- **Late Submission:** -20% per module.
- **Plagiarism:** Zero marks for all group members.
- **Incomplete Submission:** Partial marks based on missing components.