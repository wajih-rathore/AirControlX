# Module 3: AirControlX - SFML Visualization and Subsystems Integration

## Overview
Module 3 enhances the AirControlX project with a visual simulation using SFML 2.6.1 and integrates subsystems (ATCS Controller, AVN Generator, Airline Portal, StripePay) as per the project statement. To maintain modularity and prevent clutter in existing files, the implementation is divided into four submodules (3A, 3B, 3C, 3D). Each submodule focuses on specific tasks, ensuring incremental development and minimal disruption to the repository. The UI will be minimal, intuitive, and visually appealing, using imported sprite assets (provided in `/assets/`) for runways, aircraft, and other elements, with a dark theme and smooth animations.

## Repository Structure Modifications
To support sprite-based visualization and modularity, new files and directories will be added:
- `/home/wajih/OS-Project/include/visual/` - Header files for SFML-related classes.
- `/home/wajih/OS-Project/src/visual/` - Source files for SFML-related classes.
- `/home/wajih/OS-Project/assets/` - Sprite assets (e.g., PNGs for runways, aircraft, buttons) and fonts (e.g., `arial.ttf`).
- Update `/home/wajih/OS-Project/CMakeLists.txt` to include SFML 2.6.1 and new directories.
- Existing files (e.g., `Simulation.h/cpp`, `Aircraft.h/cpp`) will have minimal changes, with new functionality encapsulated in separate classes.

## Submodule Breakdown

### Module 3A: SFML Integration and Code Restructuring
**Objective**: Integrate SFML 2.6.1 into the repository, set up the visualization framework, and restructure the codebase to support sprite-based rendering without disrupting existing functionality.

**Tasks**:
1. **Update CMake Configuration**:
   - Add SFML 2.6.1 as a dependency in `CMakeLists.txt`.
   - Link SFML libraries (graphics, window, system) to the project.
   - Include `/include/visual/` and `/src/visual/` for compilation.
   - Add rule to copy `/assets/` (sprites and fonts) to the build directory.

2. **Create Visualization Directory Structure**:
   - Create `/include/visual/` and `/src/visual/` directories.
   - Add header and source files for SFML-related classes (see below).

3. **Implement Visualization Base Class**:
   - Create `include/visual/Visualizer.h` and `src/visual/Visualizer.cpp`.
   - Define a `Visualizer` class to manage the SFML window and basic sprite rendering.
   - Key components:
     - `sf::RenderWindow` (1280x720 resolution, vertical sync enabled).
     - `sf::Font` (load from `/assets/arial.ttf` for text).
     - `sf::Texture` and `sf::Sprite` for a background sprite (`/assets/background.png`).
   - Methods:
     - `initialize()`: Sets up window, loads font and background texture.
     - `render()`: Clears window, draws background sprite, and displays.
     - `handleEvents()`: Processes window close and keyboard events (e.g., Escape to close).
     - `isRunning()`: Checks if window is open.
   - Keep this class lightweight to isolate SFML logic.

4. **Integrate Visualizer with Simulation**:
   - Update `include/Simulation.h` to include a `Visualizer` pointer or reference.
   - Add a `Visualizer` instance in `Simulation::initialize()`.
   - Modify `Simulation::run()` to include an SFML rendering loop:
     ```cpp
     while (visualizer->isRunning() && !timer.isTimeUp()) {
         visualizer->handleEvents();
         update(); // Existing simulation update logic
         visualizer->render();
     }
     ```
   - Update `main.cpp` to initialize `Visualizer` and pass it to `Simulation`.

5. **Add Asset Directory**:
   - Create `/assets/` for sprites (e.g., `background.png`, `arial.ttf`).
   - Ensure `CMakeLists.txt` copies `/assets/` to the build directory.

6. **Minimize Disruption**:
   - Avoid modifying core classes (e.g., `Aircraft`, `ATCScontroller`) beyond adding interfaces.
   - Encapsulate SFML logic in `Visualizer`.
   - Use forward declarations in headers to reduce dependencies.

**Deliverables**:
- Updated `CMakeLists.txt` with SFML 2.6.1 support.
- New `include/visual/Visualizer.h` and `/src/visual/Visualizer.cpp`.
- `/assets/` with `arial.ttf` and `background.png`.
- Modified `Simulation.h/cpp` and `main.cpp` for SFML integration.
- Updated `run.sh` for SFML dependencies.
- A basic SFML window displaying the background sprite, closable via Escape.

### Module 3B: Airport Visualization
**Objective**: Implement the Main Simulation Screen to visualize the airport using sprites for runways, ATC tower, and aircraft, ensuring smooth animations and real-time updates.

**Tasks**:
1. **Create Airport Visualizer Class**:
   - Create `include/visual/AirportVisualizer.h` and `src/visual/AirportVisualizer.cpp`.
   - Define an `AirportVisualizer` class that collaborates with `Visualizer`.
   - Key components:
     - Runway sprites: `sf::Sprite` for RWY-A (`/assets/runway_a.png`), RWY-B (`/assets/runway_b.png`), RWY-C (`/assets/runway_c.png`).
     - ATC tower sprite: `sf::Sprite` (`/assets/tower.png`).
     - Aircraft sprites: `sf::Sprite` for Commercial (`/assets/aircraft_commercial.png`), Cargo (`/assets/aircraft_cargo.png`), Emergency (`/assets/aircraft_emergency.png`).
     - Text labels: `sf::Text` for flight numbers and statuses (16px, white).
     - Timer display: `sf::Text` for 5-minute countdown (24px, bold, white).
   - Methods:
     - `initialize(Simulation& sim)`: Loads sprites and references simulation data.
     - `render(Simulation& sim)`: Draws runways, tower, aircraft, and timer.
     - `updateAnimations(float deltaTime)`: Updates aircraft positions and tower animation.

2. **Render Airport Layout**:
   - Position runway sprites:
     - RWY-A: x=100, y=50 (vertical, North/South arrivals).
     - RWY-B: x=300, y=500 (horizontal, East/West departures).
     - RWY-C: x=600, y=400 (diagonal, Cargo/Emergency).
   - ATC tower: Centered at (x=640, y=360).
   - Add text labels next to runways (e.g., "RWY-A", 16px, white).

3. **Render Aircraft**:
   - Map `Aircraft` positions to SFML coordinates:
     - Use `Aircraft::updatePosition()` for x,y coordinates.
     - Scale coordinates (e.g., 1km = 100px) to fit 1280x720 window.
     - Arrivals: Animate from screen edge (North/South/East/West) to runway.
     - Departures: Animate from gate to runway, then off-screen.
   - Rotate sprites based on `Aircraft::direction` (e.g., North=0°, East=90°).
   - Use appropriate sprite based on `Aircraft::type`.
   - Display flight number and state (e.g., "PIA123: Landing") next to each aircraft.

4. **Animate ATC Tower**:
   - Use sprite sheet (`/assets/tower.png`) with animation frames (e.g., pulsing effect).
   - Cycle frames every 0.2 seconds using `sf::Clock`.
   - Adjust sprite’s `textureRect` for animation.

5. **Display Timer**:
   - Show remaining time (from `Timer::getElapsedSeconds()`) at top-center (x=640, y=20).
   - Format as "MM:SS" (e.g., "05:00" to "00:00").

6. **Integrate with Visualizer**:
   - Update `Visualizer::render()` to call `AirportVisualizer::render()`.
   - Pass `Simulation` reference for data access.
   - Update `Visualizer::handleEvents()` to include spacebar for pause/resume.

**Deliverables**:
- New `include/visual/AirportVisualizer.h` and `/src/visual/AirportVisualizer.cpp`.
- Updated `Visualizer.h/cpp` to use `AirportVisualizer`.
- Main Simulation Screen with:
  - Runway sprites (RWY-A, RWY-B, RWY-C).
  - ATC tower sprite (animated).
  - Aircraft sprites (animated, labeled with flight number and state).
  - Timer (countdown).
- Pause/resume via spacebar.

### Module 3C: Status Panel and Analytics Dashboard
**Objective**: Add a Status Panel to the Main Simulation Screen using sprites for UI elements, displaying runway statuses, flight queues, and analytics, with real-time updates.

**Tasks**:
1. **Create Status Panel Visualizer Class**:
   - Create `include/visual/StatusPanelVisualizer.h` and `src/visual/StatusPanelVisualizer.cpp`.
   - Define a `StatusPanelVisualizer` class for the bottom 30% of the window (y=504 to y=720).
   - Key components:
     - Panel background: `sf::Sprite` (`/assets/panel_background.png`).
     - Runway status boxes: `sf::Sprite` (`/assets/status_box.png`, 3 instances).
     - Buttons: `sf::Sprite` for "Airline Portal" (`/assets/button_portal.png`) and "Pause/Resume" (`/assets/button_pause.png`).
     - Text: `sf::Text` for statuses, queues, and analytics (14-16px, white).
   - Methods:
     - `initialize(Simulation& sim)`: Loads sprites and text.
     - `render(Simulation& sim)`: Draws panel components.
     - `handleButtonClicks(sf::Vector2f mousePos)`: Detects button clicks.

2. **Render Runway Status**:
   - Place status boxes at (x=50, y=520), (x=300, y=520), (x=550, y=520).
   - Display text (16px, white) on each box:
     - "RWY-A: [Occupied by PIA123/Free]" (from `RunwayManager::getStatusReport()`).
     - Color text green if free, red if occupied.

3. **Render Flight Queues**:
   - Display lists at (x=800, y=520) and (x=1000, y=520).
   - For `FlightsScheduler::arrivalQueue` and `departureQueue`:
     - Show up to 5 flights (flight number, airline, priority).
     - Example: "PIA123, PIA, Priority: 85".
   - Use 14px text, white, 20px vertical spacing.

4. **Render Analytics Summary**:
   - At (x=50, y=650), display:
     - "Active Violations: [count]" (from `ATCScontroller::handleViolations()`).
     - Aircraft with active AVNs (flight number, airline, violation type, 14px, white).
   - Limit to 3 aircraft.

5. **Add Buttons**:
   - "Airline Portal" button at (x=1100, y=650).
   - "Pause/Resume" button at (x=50, y=650).
   - Use sprites with hover effect (load `/assets/button_portal_hover.png`, `/assets/button_pause_hover.png` on mouse over).
   - Handle clicks in `handleButtonClicks()`:
     - "Airline Portal" sets flag for screen switch.
     - "Pause/Resume" toggles `Simulation` state.

6. **Integrate with AirportVisualizer**:
   - Update `AirportVisualizer::render()` to call `StatusPanelVisualizer::render()`.
   - Pass `Simulation` reference.
   - Update `Visualizer::handleEvents()` to forward mouse clicks to `StatusPanelVisualizer`.

**Deliverables**:
- New `include/visual/StatusPanelVisualizer.h` and `/src/visual/StatusPanelVisualizer.cpp`.
- Updated `AirportVisualizer.h/cpp` to include Status Panel.
- Status Panel with:
  - Sprite-based runway status boxes.
  - Flight queue lists.
  - Analytics summary.
  - Functional buttons.

### Module 3D: Airline Portal and Subsystems Integration
**Objective**: Implement the Airline Portal Screen using sprites for UI, integrate StripePay and AVN Generator, and add a simulation end summary.

**Tasks**:
1. **Create Airline Portal Visualizer Class**:
   - Create `include/visual/AirlinePortalVisualizer.h` and `src/visual/AirlinePortalVisualizer.cpp`.
   - Define an `AirlinePortalVisualizer` class for the Airline Portal Screen.
   - Key components:
     - Background: `sf::Sprite` (`/assets/portal_background.png`).
     - Header: `sf::Text` ("Airline Portal", 24px, bold, white).
     - Filter section: `sf::Sprite` for dropdown (`/assets/dropdown.png`) and date display.
     - AVN table: `sf::Sprite` for rows (`/assets/table_row.png`).
     - Payment modal: `sf::Sprite` (`/assets/modal.png`) with buttons.
     - Buttons: `sf::Sprite` for "Back" (`/assets/button_back.png`), "Pay" (`/assets/button_pay.png`).
   - Methods:
     - `initialize(AirlinePortal& portal)`: Loads sprites and text.
     - `render(AirlinePortal& portal)`: Draws portal screen.
     - `handleEvents(sf::Event& event, AirlinePortal& portal, StripePayment& payment)`: Processes clicks.
     - `showPaymentModal(AVN& avn)`: Displays payment modal.

2. **Render Airline Portal Screen**:
   - Header at (x=50, y=20).
   - Filter section at (x=50, y=80):
     - Dropdown sprite (display airline names as text, e.g., "PIA, AirBlue").
     - Date text (e.g., "2025-05-05").
   - AVN table at (x=50, y=150, width=1180px):
     - Columns: AVN ID, Flight Number, Aircraft Type, Speed Recorded, Permissible Speed, Fine Amount, Payment Status, Due Date.
     - Rows: Up to 10 AVNs, 14px text, with "Pay" button sprite.
   - Payment modal:
     - Centered, shows AVN ID, Flight Number, Amount.
     - Simulated input (text display, e.g., "PKR 575,000").
     - "Confirm Payment" button (`/assets/button_confirm.png`).

3. **Integrate Subsystems**:
   - **AVN Generator**:
     - Store AVNs in a thread-safe vector.
     - Implement `getAVNsByAirline(string airline, string date)`.
     - Update AVN status on payment.
   - **Airline Portal**:
     - Update `viewAVNs()` to query `AVNGenerator`.
     - Implement `markPaid(AVNID, amount)`.
   - **StripePayment**:
     - Implement `processPayment(AVNID, flightNumber, aircraftType, amount)`.
     - Notify `AVNGenerator` and `AirlinePortal` on success.

4. **Handle Payment Flow**:
   - On "Pay" click, show modal.
   - On "Confirm Payment", call `StripePayment::processPayment()`.
   - Display success ("Payment Successful", 16px, green) or failure (red).
   - Update AVN table and notify `AVNGenerator`.

5. **Add Simulation End Summary**:
   - Create `include/visual/SummaryVisualizer.h` and `src/visual/SummaryVisualizer.cpp`.
   - Define a `SummaryVisualizer` class for the end overlay.
   - Components:
     - Overlay: `sf::Sprite` (`/assets/overlay.png`, semi-transparent).
     - Text: Stats (flights processed, violations, runway utilization).
     - Buttons: "Restart" (`/assets/button_restart.png`), "Exit" (`/assets/button_exit.png`).
   - Methods:
     - `render(Simulation& sim)`: Draws summary.
     - `handleEvents(sf::Event& event)`: Processes clicks.
   - Trigger when `Timer::isTimeUp()`.

6. **Switch Between Screens**:
   - Add state enum in `Visualizer` (`MainSimulation`, `AirlinePortal`, `Summary`).
   - Update `Visualizer::render()` to delegate to appropriate visualizer.
   - Handle transitions:
     - "Airline Portal" button sets `AirlinePortal`.
     - "Back" sets `MainSimulation`.
     - Timer expiry sets `Summary`.
     - "Restart" resets simulation; "Exit" closes window.

**Deliverables**:
- New `include/visual/AirlinePortalVisualizer.h`, `/src/visual/AirlinePortalVisualizer.cpp`.
- New `include/visual/SummaryVisualizer.h`, `/src/visual/SummaryVisualizer.cpp`.
- Updated `Visualizer.h/cpp` for screen states.
- Updated `AVNGenerator.h/cpp`, `AirlinePortal.h/cpp`, `StripePayment.h/cpp`.
- Airline Portal Screen with sprite-based UI and payment flow.
- Simulation end summary with stats and buttons.

## UI Design Principles
- **Minimalism**: Clean layout, dark background (via `/assets/background.png`), whitespace.
- **Color Scheme**: Defined by sprite assets (e.g., blue runways, red aircraft).
- **Typography**: `arial.ttf`, sizes:
  - Headers: 24px, bold.
  - Labels: 16px.
  - Table text: 14px.
- **Animations**:
  - Smooth aircraft movement (interpolation, `deltaTime`).
  - Animated tower (sprite sheet frames).
  - Optional fade transitions for screens.
- **Window**: Fixed 1280x720, sprites scaled to fit.

## Development Plan
- **Module 3A (by April 20, 2025)**: Set up SFML, create `Visualizer`, test window.
- **Module 3B (by April 27, 2025)**: Implement `AirportVisualizer`, test animations.
- **Module 3C (by May 2, 2025)**: Implement `StatusPanelVisualizer`, test updates.
- **Module 3D (by May 6, 2025)**: Implement `AirlinePortalVisualizer`, `SummaryVisualizer`, test subsystems.

## Notes
- Assume sprites are in `/assets/` (e.g., `runway_a.png`, `aircraft_commercial.png`).
- Use SFML 2.6.1 for compatibility.
- Encapsulate SFML logic in `/visual/` classes.
- Use existing mutexes for thread safety.
- Test with multiple aircraft and violations.
- Ensure consistent naming (`CamelCase` for classes, `snake_case` for variables).