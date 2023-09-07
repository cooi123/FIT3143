#!/bin/bash

# Define the number of times you want to run the command
num_runs=5

# Output file
output_file_serial="serial_result.txt"
output_file_parallel="parallel_result.txt"

# Clear the output file if it exists
> "$output_file_serial"
> "$output_file_parallel"

#compile the code
make 

# Loop for the specified number of runs
for ((i=1; i<=num_runs; i++)); do
    ./bloom_serial LITTLE_WOMEN.txt query.txt >> "$output_file_serial"
done

# Loop for the specified number of runs
for ((i=1; i<=num_runs; i++)); do
    ./bloom_parallel LITTLE_WOMEN.txt query.txt >> "$output_file_parallel"
done

# Loop for the specified number of runs
for ((i=1; i<=num_runs; i++)); do
    ./bloom_serial LITTLE_WOMEN.txt MOBY_DICK.txt query.txt >> "$output_file_serial"
done

# Loop for the specified number of runs
for ((i=1; i<=num_runs; i++)); do
    ./bloom_parallel LITTLE_WOMEN.txt MOBY_DICK.txt query.txt >> "$output_file_parallel"
done
# Loop for the specified number of runs
for ((i=1; i<=num_runs; i++)); do
    ./bloom_serial LITTLE_WOMEN.txt MOBY_DICK.txt SHAKESPEARE.txt query.txt >> "$output_file_serial"
done

# Loop for the specified number of runs
for ((i=1; i<=num_runs; i++)); do
    ./bloom_parallel LITTLE_WOMEN.txt MOBY_DICK.txt SHAKESPEARE.txt query.txt >> "$output_file_parallel"
done

# Loop for the specified number of runs
for ((i=1; i<=num_runs; i++)); do
    ./bloom_serial LITTLE_WOMEN.txt MOBY_DICK.txt SHAKESPEARE.txt output.txt query.txt >> "$output_file_serial"
done

# Loop for the specified number of runs
for ((i=1; i<=num_runs; i++)); do
    ./bloom_parallel LITTLE_WOMEN.txt MOBY_DICK.txt SHAKESPEARE.txt output.txt query.txt >> "$output_file_parallel"
done

echo "Script completed."
