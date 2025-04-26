cmake_minimum_required(VERSION 3.10)
project(AirControlX LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

# Set C++ standard and compiler flags
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread")

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Dynamically collect all .cpp files in src/
file(GLOB SOURCES "src/*.cpp")

# Build executable into build/
add_executable(aircontrolx ${SOURCES})
target_link_libraries(aircontrolx PRIVATE sfml-graphics sfml-window sfml-system)
set_target_properties(aircontrolx PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Generate compile_commands.json for CLion
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)