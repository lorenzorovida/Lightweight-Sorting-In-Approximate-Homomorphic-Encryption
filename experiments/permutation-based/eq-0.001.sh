#!/bin/bash

RESULTS_FILE="results/eq-0.01.txt"


echo "========================================" | tee -a "$RESULTS_FILE"
echo "            EQUAL WITH d=0.01           " | tee -a "$RESULTS_FILE"
echo "========================================" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

for INPUTS in 8 16 32 64 128; do
    echo "----------------------------------------" | tee -a "$RESULTS_FILE"
    echo " Run with inputs: $INPUTS" | tee -a "$RESULTS_FILE"
    echo "----------------------------------------" | tee -a "$RESULTS_FILE"

    values=()

    for i in $(seq 0 $((INPUTS-1))); do
        values+=("0.001")
    done
    
    vector="[${values[*]}]"
    vector="${vector// /, }"

    echo "Input: $vector"

    output=$(/usr/bin/time -l ../../build/Sort --inline "$vector" --delta 0.001 --permutation --tieoffset 2> >(grep "peak memory footprint"))


    echo "$output, inputs: $INPUTS" | tee -a "$RESULTS_FILE"
    echo "" | tee -a "$RESULTS_FILE"
done

echo "========================================" | tee -a "$RESULTS_FILE"
echo "             EXPERIMENT DONE            " | tee -a "$RESULTS_FILE"
echo "========================================" | tee -a "$RESULTS_FILE"
