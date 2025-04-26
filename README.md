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
├── Makefile            # Build configuration
├── run.sh              # Script to compile and run the project
├── README.md           # This file
└── aircontrolx         # Executable (generated during build)
```

## Requirements

- C++ compiler (g++ with C++11 support)
- Make build system
- Linux/Unix environment

## How to Build and Run

### Option 1: Using the run.sh script

The simplest way to build and run the project is to use the provided shell script:

```bash
./run.sh
```

This will:
1. Clean any previous builds
2. Compile the project
3. Run the resulting executable

### Option 2: Manual compilation

To manually build and run the project:

```bash
# Clean previous build
make clean

# Compile the project
make

# Run the executable
./aircontrolx
```

## Project Modules

1. **Module 1**: System Rules & Restrictions
   - Defines airline restrictions, flight speeds, and violation rules

2. **Module 2**: ATCS Core Functionalities
   - Flight scheduling, runway allocation, and priority handling

3. **Module 3**: Subsystems & Airline Integration
   - Violation detection, AVN generation, and payment processing

## Development Status

This project was developed for the Operating Systems course (Spring 2025).