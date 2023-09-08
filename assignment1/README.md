# FIT3143 Assignment 1
To submit job slurm script for parallel use `sbatch bloom_parallel_script_job`
To submit job slurm script for serial use `sbatch bloom_serial_script_job`
To compile both bloom_parallel.c and bloom.c use `make`
To compile bloom_parallel.c use `make bloom_parallel`
To compile bloom_serial.c use `make bloom_serial`
Result is collected by running `./run.sh` bash script which runs 5 execution of both serial and parallel for all the text files and compared it with the query.txt