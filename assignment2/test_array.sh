#!/bin/bash
#SBATCH --job-name=test
#SBATCH --time=0:00:30
#SBATCH --array=3-5       # Request nodes from 3 to 14

# Calculate the number of tasks based on the array task ID
num_nodes=${SLURM_ARRAY_TASK_ID}
num_tasks=$((num_nodes))  # You can adjust this if needed
cpus_per_task=4           # Adjust the number of CPU cores per task as needed
mem_per_node=8G           # Adjust the memory per node as needed

# Set the number of nodes, tasks, and other resource parameters
#SBATCH --nodes=${num_nodes}
#SBATCH --ntasks-per-node=${num_tasks}
#SBATCH --cpus-per-task=${cpus_per_task}
#SBATCH --mem=${mem_per_node}


allocated_nodes_info=$(scontrol show job "$SLURM_JOBID" | grep -E "NodeCnt=|CPUTasksPerNode=|ReqNodeList=")

# Print the allocated node information
echo "Allocated Node Information:"
echo "$allocated_nodes_info"


