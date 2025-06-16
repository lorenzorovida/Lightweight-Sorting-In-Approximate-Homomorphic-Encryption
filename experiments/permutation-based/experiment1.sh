#!/bin/bash

# Clear the contents of the results
#> results/experiment1.txt

echo "Starting experiment 1" | tee -a results/experiment1.txt

output=$(../../build-release/Sort --random 8 0.01 --permutation --tieoffset)

output="$output, inputs: 8"

echo "$output" | tee -a results/experiment1.txt

output=$(../../build-release/Sort --random 16 0.01 --permutation --tieoffset) 

output="$output, inputs: 16"

echo "$output" | tee -a results/experiment1.txt

output=$(../../build-release/Sort --random 32 0.01 --permutation --tieoffset) 

output="$output, inputs: 32"

echo "$output" | tee -a results/experiment1.txt

output=$(../../build-release/Sort --random 64 0.01 --permutation --tieoffset)

output="$output, inputs: 64"

echo "$output" | tee -a results/experiment1.txt

output=$(../../build-release/Sort --random 128 0.01 --permutation --tieoffset) 

output="$output, inputs: 128"

echo "$output" | tee -a results/experiment1.txt
