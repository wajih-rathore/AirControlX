# AirControlX - Automated Air Traffic Control System

This project simulates an Automated Air Traffic Control System (ATCS) at a multi-runway international airport, developed in C++ using core Operating Systems concepts.

## Project Overview

AirControlX manages flight scheduling, runway allocation, air traffic monitoring, and violation tracking for an international airport with multiple runways.

The system simulates:

- Multiple airlines and aircraft types
- Runway allocation based on aircraft direction and type
- Flight movement through various phases
- Speed monitoring and violation detection
- Emergency handling

## Directory Structure

```
AirControlX/
│
├── include/            # Header files
├── src/                # Implementation files
├── obj/                # Object files (generated during build)
├── docs/               # Documentation files
├── build/              # Build directory for CMake (generated during build)
├── run.sh              # Script to configure, build, and run the project
├── CMakeLists.txt      # CMake configuration file
└── README.md           # This file
```

## Prerequisites

- **Operating System**: Linux/Unix environment (e.g., Ubuntu)
- **C++ Compiler**: g++ with C++11 support
- **CMake**: Version 3.10 or higher
- **SFML**: Version 2.5 or higher (for visualization in `VisualSimulator.cpp`)
- **Build Tools**: `make` (for "Unix Makefiles" generator)
- **Git**: For cloning the repository

### Install Prerequisites on Ubuntu

1. **Install Basic Tools**:
   ```bash
   sudo apt update
   sudo apt install build-essential cmake git
   ```

2. **Install SFML**:
   ```bash
   sudo apt install libsfml-dev
   ```
   - This installs SFML libraries and development headers (version 2.6.1 or similar, depending on your system).

## Setup and Build Instructions

This project uses CMake to manage the build process, with a unified `build/` directory for both VS Code and CLion. Follow the instructions below to set up and run the project in your preferred IDE.

### Option 1: Using the `run.sh` Script (Recommended)

The simplest way to configure, build, and run the project is to use the provided shell script:

```bash
./run.sh
```

This will:
1. Configure the project using CMake (`build/` directory, "Unix Makefiles" generator)
2. Build the project
3. Run the resulting executable (`build/aircontrolx`)

### Option 2: Setup in VS Code

1. **Clone the Repository**:
   ```bash
   git clone <repository-url>
   cd AirControlX
   ```

2. **Install VS Code Extensions**:
   - Open VS Code.
   - Go to Extensions (`Ctrl+Shift+X`).
   - Install:
     - **CMake Tools** (by Microsoft)
     - **C/C++** (by Microsoft)

3. **Configure CMake Tools**:
   - Open the project in VS Code: `File > Open Folder > Select AirControlX`.
   - Press `Ctrl+Shift+P`, type "CMake: Edit User-Local CMake Kits," and select it.
   - Edit the kit configuration (e.g., `~/.local/share/CMakeTools/cmake-tools-kits.json`) to use "Unix Makefiles":
     ```json
     [
       {
         "name": "GCC",
         "compilers": {
           "C": "/usr/bin/gcc",
           "CXX": "/usr/bin/g++"
         },
         "preferredGenerator": {
           "name": "Unix Makefiles"
         }
       }
     ]
     ```
   - Press `Ctrl+Shift+P`, type "CMake: Select Kit," and choose "GCC".

4. **Configure the Project**:
   - Press `Ctrl+Shift+P`, type "CMake: Configure," and select it.
   - This generates build files in the `build/` directory.

5. **Build and Run**:
   - Run the `run.sh` script in the terminal:
     ```bash
     ./run.sh
     ```
   - Alternatively, press `Ctrl+Shift+B` to build (if `tasks.json` is set up to run `./run.sh`).

### Option 3: Setup in CLion

1. **Clone the Repository**:
   ```bash
   git clone <repository-url>
   cd AirControlX
   ```

2. **Open in CLion**:
   - Launch CLion.
   - Go to `File > Open`, select the `AirControlX` directory, and choose "Open as Project."

3. **Configure Build Directory and Generator**:
   - Go to `File > Settings > Build, Execution, Deployment > CMake`.
   - Set "Generation path" to `build`.
   - Set "Generator" to "Unix Makefiles".
   - Click "Apply" and "OK."

4. **Reload CMake**:
   - Go to `Tools > CMake > Reload CMake Project`.
   - Check the "CMake" tab for successful configuration.

5. **Set Up Run Configuration**:
   - Go to `Run > Edit Configurations`.
   - Add a "CMake Application" configuration:
     - Name: `Run AirControlX`
     - Target: `aircontrolx`
     - Executable: `build/aircontrolx`
     - Working directory: Path to `AirControlX` directory
     - Click "Apply" and "OK."

6. **Build and Run**:
   - Build: Press `Ctrl+F9` or click the hammer icon.
   - Run: Press `Shift+F10` or click the play icon.

## Adding New Files

The `CMakeLists.txt` uses `file(GLOB ...)` to automatically include all `.cpp` files in the `src/` directory. When you add a new file:

- **VS Code**: Run `./run.sh` or press `Ctrl+Shift+P` > "CMake: Configure" to reconfigure CMake.
- **CLion**: Go to `Tools > CMake > Reload CMake Project`.

This ensures the new file is included in the build.

## Testing SFML Integration

This project uses SFML for visualization (in `VisualSimulator.cpp`). To verify SFML is working, a commented-out test code is included in `main.cpp` under the section `/* sfml testing out code here starting from include statements until the end */`. Uncomment this code, build, and run to check if SFML is set up correctly. The test code opens a simple window with a green circle.

## Project Modules

1. **Module 1**: System Rules & Restrictions
   - Defines airline restrictions, flight speeds, and violation rules

2. **Module 2**: ATCS Core Functionalities
   - Flight scheduling, runway allocation, and priority handling

3. **Module 3**: Subsystems & Airline Integration
   - Violation detection, AVN generation, and payment processing

## Development Status

This project was developed for the Operating Systems course (Spring 2025).