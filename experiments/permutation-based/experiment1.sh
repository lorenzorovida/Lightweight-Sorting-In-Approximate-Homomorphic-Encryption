#!/bin/bash

# Clear the contents of the results
#> results/experiment1.txt

echo "Starting experiment 1" | tee -a results/experiment1.txt

output=$(../../build/Sort --random 8 --delta 0.01 --permutation --tieoffset)

output="$output, inputs: 8"

echo "$output" | tee -a results/experiment1.txt

output=$(../../build/Sort --random 16 -delta 0.01 --permutation --tieoffset) 

output="$output, inputs: 16"

echo "$output" | tee -a results/experiment1.txt

output=$(../../build/Sort --random 32 -delta 0.01 --permutation --tieoffset) 

output="$output, inputs: 32"

echo "$output" | tee -a results/experiment1.txt

output=$(../../build/Sort --random 64 -delta 0.01 --permutation --tieoffset)

output="$output, inputs: 64"

echo "$output" | tee -a results/experiment1.txt

output=$(../../build/Sort --random 128 -delta 0.01 --permutation --tieoffset) 

output="$output, inputs: 128"

echo "$output" | tee -a results/experiment1.txt
