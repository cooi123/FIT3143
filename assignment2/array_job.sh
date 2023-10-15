#!/bin/bash
#SBATCH --job-name=mpi_omp_job_2_nodes
#SBATCH --output=node_%A_%a.out
#SBATCH --time=00:10:00
#SBATCH --mem=16G
#SBATCH --cpus-per-task=10
#SBATCH --ntasks=14
#SBATCH --ntasks-per-node=14
#SBATCH --partition=defq
#SBATCH --array=3-14   # Change 14 to the maximum number of nodes you want to test

# Load required modules
module load openmpi/4.1.5-gcc-11.2.0-ux65npg

# Get the allocated node size
allocated_nodes_info=$(scontrol show job "$SLURM_JOBID" | grep -E "NodeCnt=|CPUTasksPerNode=|ReqNodeList=")

# Print the allocated node information
echo "Allocated Node Information:"
echo "$allocated_nodes_info"

# Define the command to run (including the output directory as an argument)
output_dir="output_directory_2_nodes${SLURM_ARRAY_TASK_ID}"
mkdir -p "$output_dir"
command="srun -n $SLURM_ARRAY_TASK_ID ./sim $output_dir"

# Execute the simulation command
echo "Running $command"
$command

