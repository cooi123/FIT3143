#!/bin/bash
#SBATCH --job-name=mpi_simulation_max
#SBATCH --output=mpi_simulation_max.out
#SBATCH --error=mpi_simulation_max.err
#SBATCH --nodes=14
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=16


# Load MPI module (modify according to your system's configuration)
module load mpi

# Define the command to run
command="mpirun -n 14 -oversubscribe ./sim"


# Execute the simulation command
echo "Running $command"
$command

