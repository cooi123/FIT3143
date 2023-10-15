#!/bin/bash
#SBATCH --job-name=ev_job
#SBATCH --time=0:04:00
#SBATCH --array=3-14       # Request nodes from 3 to 14

# Calculate the number of tasks based on the array task ID
num_nodes=${SLURM_ARRAY_TASK_ID}
num_tasks=$((num_nodes))  # You can adjust this if needed

# Calculate memory per node based on the number of nodes
mem_per_node=$((4 * num_nodes))  # Adjust this based on your memory requirements

# Set the number of nodes, tasks, and other resource parameters
#SBATCH --nodes=${num_nodes}
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --mem=${mem_per_node}G

# Load any required modules
module load openmpi/4.1.5-gcc-11.2.0-ux65npg

# Define the output directory based on the number of nodes
output_dir="output_directory_${num_nodes}"
mkdir -p "$output_dir"

# Your job commands go here
srun -n ${num_nodes} ./sim "${output_dir}"

