#!/bin/bash
#SBATCH --nodes=1
#SBATCH --tasks-per-node=2
#SBATCH --time=00:05:00

mpirun -np 2 ./mpi_bcast 1

echo "Number of nodes:"
echo -n "SLURM_JOB_NUM_NODES = "
echo -n $SLURM_JOB_NUM_NODES # SLURM environment variable for the number of nodes
echo "."

echo "Tasks per node:"
echo -n "SLURM_CPUS_ON_NODE = "
echo -n $SLURM_CPUS_ON_NODE # SLURM environment variable for the tasks per node
echo "."