#!/bin/bash

# Simple script to play the game (assumes everything is already built)
# Usage: ./start-play.sh [HEIGHT] [WIDTH]
# Defaults: HEIGHT=20 WIDTH=30

HEIGHT=${1:-20}
WIDTH=${2:-30}

make run-game HEIGHT=$HEIGHT WIDTH=$WIDTH

