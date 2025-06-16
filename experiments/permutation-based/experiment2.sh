#!/bin/bash

# Clear the contents of the results
#> results/experiment2.txt

echo "Starting experiment 2" | tee -a results/experiment2.txt

output=$(../../build-release/Sort --random 8 0.001 --permutation) 
# Mem: 2.17 GB

output="$output, inputs: 8"

echo "$output" | tee -a results/experiment2.txt

output=$(../../build-release/Sort --random 16 0.001 --permutation) 
# Mem: 2.53 GB

output="$output, inputs: 16"

echo "$output" | tee -a results/experiment2.txt

output=$(../../build-release/Sort --random 32 0.001 --permutation)  
# Mem: 2.75 GB

output="$output, inputs: 32"

echo "$output" | tee -a results/experiment2.txt

output=$(../../build-release/Sort --random 64 0.001 --permutation)  
# Mem: 2.95 GB

output="$output, inputs: 64"

echo "$output" | tee -a results/experiment2.txt

output=$(../../build-release/Sort --random 128 0.001 --permutation)  

output="$output, inputs: 128"

echo "$output" | tee -a results/experiment2.txt
