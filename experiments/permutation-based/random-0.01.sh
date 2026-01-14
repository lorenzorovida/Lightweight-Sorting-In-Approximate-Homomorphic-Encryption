#!/bin/bash

RESULTS_FILE="results/random-0.01.txt"

# Optional: clear previous results
# > "$RESULTS_FILE"

echo "========================================" | tee -a "$RESULTS_FILE"
echo "           RANDOM WITH d=0.01           " | tee -a "$RESULTS_FILE"
echo "========================================" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

for INPUTS in 8 16 32 64 128; do
    echo "----------------------------------------" | tee -a "$RESULTS_FILE"
    echo " Run with inputs: $INPUTS" | tee -a "$RESULTS_FILE"
    echo "----------------------------------------" | tee -a "$RESULTS_FILE"

output=$(/usr/bin/time -l ../../build/Sort --random "$INPUTS" --delta 0.01 --permutation --tieoffset 2> >(grep "peak memory footprint"))

    echo "$output, inputs: $INPUTS" | tee -a "$RESULTS_FILE"
    echo "" | tee -a "$RESULTS_FILE"
done

echo "========================================" | tee -a "$RESULTS_FILE"
echo "                   DONE                 " | tee -a "$RESULTS_FILE"
echo "========================================" | tee -a "$RESULTS_FILE"
