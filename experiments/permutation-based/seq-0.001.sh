#!/bin/bash

RESULTS_FILE="results/seq-0.001.txt"



# Optional: clear previous results
# > "$RESULTS_FILE"

echo "========================================" | tee -a "$RESULTS_FILE"
echo "        SEQUENTIAL WITH d=0.001         " | tee -a "$RESULTS_FILE"
echo "========================================" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

for INPUTS in 8 16 32 64 128; do
    echo "----------------------------------------" | tee -a "$RESULTS_FILE"
    echo " Run with inputs: $INPUTS" | tee -a "$RESULTS_FILE"
    echo "----------------------------------------" | tee -a "$RESULTS_FILE"

    values=()

    for i in $(seq 0 $((INPUTS-1))); do
        values+=("0.$(printf "%03d" "$i")")
    done

    vector="[${values[*]}]"
    vector="${vector// /, }"

    output=$(/usr/bin/time -l ../../build/Sort --inline "$vector" --delta 0.001 --permutation --tieoffset 2> >(grep "peak memory footprint"))

    echo "$output, inputs: $INPUTS" | tee -a "$RESULTS_FILE"
    echo "" | tee -a "$RESULTS_FILE"
done

echo "========================================" | tee -a "$RESULTS_FILE"
echo "             EXPERIMENT DONE            " | tee -a "$RESULTS_FILE"
echo "========================================" | tee -a "$RESULTS_FILE"
