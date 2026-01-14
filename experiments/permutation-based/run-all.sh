#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Function to run a script and track peak memory
run_script() {
    local script="$1"

    echo
    echo "========================================"
    echo "Running: $(basename "$script")"
    echo "========================================"

    # Run script in background
    bash "$script" &
    local pid=$!

    local peak=0
    # Monitor memory usage until script finishes
    while kill -0 "$pid" 2>/dev/null; do
        # RSS in KB
        mem=$(ps -o rss= -p "$pid" 2>/dev/null | awk '{sum+=$1} END{print sum+0}')
        (( mem > peak )) && peak=$mem
        sleep 0.1
    done

    # Capture any final memory usage after exit
    mem=$(ps -o rss= -p "$pid" 2>/dev/null | awk '{sum+=$1} END{print sum+0}')
    (( mem > peak )) && peak=$mem

    echo "Peak RAM: ${peak} KB"
}

# Loop through all .sh files except this one
for script in "$SCRIPT_DIR"/*.sh; do
    [[ "$(basename "$script")" == "run_all.sh" ]] && continue
    run_script "$script"
done
