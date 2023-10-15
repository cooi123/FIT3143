#!/bin/bash
#SBATCH --job-name=ev_sim
#SBATCH --time=0:03:00
#SBATCH --nodes=10           # Request 2 nodes
#SBATCH --ntasks-per-node=1 # Request 1 task (process) per node
#SBATCH --cpus-per-task=4   # Request 4 CPU cores per task
#SBATCH --mem=8G            # Request 8GB of memory per node

# Load any required modules
module load openmpi/4.1.5-gcc-11.2.0-ux65npg


# Your job commands go here
srun -n 9 ./sim output_directory_10_nodes_9
