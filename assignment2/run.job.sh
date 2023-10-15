#!/bin/bash
#SBATCH --job-name=mpi_simulation
#SBATCH --output=%x_%A_%a.out
#SBATCH --error=%x_%A_%a.err
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=8
#SBATCH --array=1-10  # Change 10 to the maximum processor number you want to test

# Load MPI module (modify according to your system's configuration)
module load mpi

make

# Define the command to run
command="mpirun -n $SLURM_ARRAY_TASK_ID -oversubscribe ./sim"

# Create a directory to save the output for this run
output_dir="output_${SLURM_ARRAY_TASK_ID}"
mkdir -p "$output_dir"

# Change to the output directory
cd "$output_dir"

# Execute the simulation command
echo "Running $command"
$command
