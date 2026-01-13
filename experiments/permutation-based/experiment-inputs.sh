#!/bin/bash

mkdir -p results

# Clear the contents of the results
#> results/experiment-inputs.txt

echo "Starting experiment with custom inputs" | tee -a results/experiment1.txt

output=$(../../build/Sort --file "../../inputs/duplicates-0.001.txt" --delta 0.001 --permutation --tieoffset)

output="$output, duplicates, 0.001"

echo "$output" | tee -a results/experiment-inputs.txt

output=$(../../build/Sort --file "../../inputs/noduplicates-0.001.txt" --delta 0.001 --permutation)

output="$output, noduplicates, 0.001"

echo "$output" | tee -a results/experiment-inputs.txt

output=$(../../build/Sort --file "../../inputs/stress-equal-128-0.001.txt" --delta 0.001 --permutation --tieoffset)

output="$output, stress equal 128, 0.001"

echo "$output" | tee -a results/experiment-inputs.txt

output=$(../../build/Sort --file "../../inputs/sample.txt" --delta 0.001 --permutation --tieoffset)

output="$output, sample random 128, 0.001"

echo "$output" | tee -a results/experiment-inputs.txt

output=$(../../build/Sort --file "../../inputs/stress-seq-128-0.001.txt" --delta 0.001 --permutation)

output="$output, stress sequential, 0.001"

echo "$output" | tee -a results/experiment-inputs.txt

