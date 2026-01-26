#!/bin/bash

RESULTS_FILE="results/random-0.001.txt"

# Optional: clear previous results
# > "$RESULTS_FILE"

echo "========================================" | tee -a "$RESULTS_FILE"
echo "          RANDOM WITH d=0.001           " | tee -a "$RESULTS_FILE"
echo "========================================" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

for INPUTS in 16 32 64 128 256 512 1024 2048 4096 8192; do
    echo "----------------------------------------" | tee -a "$RESULTS_FILE"
    echo " Run with inputs: $INPUTS" | tee -a "$RESULTS_FILE"
    echo "----------------------------------------" | tee -a "$RESULTS_FILE"

    output=$(/usr/bin/time -l ../../build/Sort --random "$INPUTS" --delta 0.001 --network --toy2> >(grep "peak memory footprint"))

    echo "$output, inputs: $INPUTS" | tee -a "$RESULTS_FILE"
    echo "" | tee -a "$RESULTS_FILE"
done

echo "========================================" | tee -a "$RESULTS_FILE"
echo "                   DONE                 " | tee -a "$RESULTS_FILE"
echo "========================================" | tee -a "$RESULTS_FILE"
