#!/usr/bin/env bash
clear
echo "Configuring AirControlX..."
cmake -B build

echo "Building AirControlX..."
cmake --build build --target aircontrolx

echo "Running AirControlX..."
./build/aircontrolx