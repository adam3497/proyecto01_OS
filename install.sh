#sudo dnf install gcc gdb make cmake
#mkdir out
#mkdir out/bin out/decodes out/frequencies out/objects

#!/bin/bash

# Function to check if a command is available
check_dependency() {
    command -v "$1" >/dev/null 2>&1 || { echo >&2 "Error: $1 is required but not installed. Installing..."; sudo dnf install -y "$1"; }
}

# Function to run Makefile
run_makefile() {
    echo "Running Makefile..."
    make 
}

# Check for dependencies
echo "Checking dependencies..."

# Check for gcc
check_dependency "gcc"

# Check for gdb
check_dependency "gdb"

# Check for make
check_dependency "make"

# Check for cmake
check_dependency "cmake"

echo "Dependencies are satisfied."

# Create directories
echo "Creating directories..."
mkdir -p out/bin out/temp out/objects
echo "Directories created successfully."

# Run Makefile
run_makefile

echo "Program ready to run!"
