#!/bin/bash

# Clean everything first, then delegate to start.sh
echo "Cleaning all build artifacts and dependencies..."
make fclean

# Now run start.sh which will install dependencies and build everything
./start.sh

