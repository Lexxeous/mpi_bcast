#!/bin/bash
#SBATCH --nodes=1
#SBATCH --tasks-per-node=3
#SBATCH --time=00:10:00

mpirun -np 3 ./mpi_bcast 1