1.
Files "custom_bcast.h" and "custom_bcast.cpp" contain the variables, functions, definitions, & libraries necessary to implement my custom version of a 'root-to-others' MPI broadcast function: "void custom_Bcast(void* buf, int cnt, MPI_Datatype type, int ruut, MPI_Comm communicator)".

2.
The file "mpi_bcast.cpp" contains print statements to confirm proper operation, the custom and default implementations of MPI broadcast, and wall times to isolate the desired execution times.

3.
The file "Makefile" contains 2 default variables (<procs> & <root>) that can be changed for the file targets.
The "comp" target compiles and links "mpi_bcast.cpp" with the "custom_bcast" library into an executable called "mpi_bcast".
The "run" target runs "mpi_bcast" with the previously mentioned default variables.
The "clean" target removes the executable and any extraneous output files.
NOTE: You can use the commands "make comp" and "make run" to get proper output to the screen, but it will not provide you with any SLURM output unless you follow step 4, instead.

4.
The file "mpi_job.sh" contains the SBATCH directives and Bash commands necessary to run a batch job with the desired number of processes and tasks per node. Modify this file as desired and run accordingly. There are also print statements that output SLURM environment variables for ease of data organization and documentation.
Run "sbatch mpi_job.sh" after you have made appropriate changes to the shell file and successfully compiled using "make comp". You can also change the root for the broadcast by changing the second command line argument.

5. Retrieve the "slurm-<job_id>.out" file however you see fit.



––––––––––––––––––––––––––––––– EXAMPLE OUTPUT WITH 12 PROCESSES ON ONE NODE: –––––––––––––––––––––––––––––––


Comparing the execution time between the default library version of "MPI_Bcast()" and a custom implementation using only point-to-point communication...

2 : Original broadcast array[0 1 ... 99998 99999] = [0 0 ... 0 0]
2 : Initialized custom broadcast array[0 1 ... 99998 99999] = [-1 -1 ... -1 -1]
2 : Initialized MPI broadcast array[0 1 ... 99998 99999] = [-2 -2 ... -2 -2]
1 : Original broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]

Using process 1 as the root to broadcast data to all other processes...
0 : Custom broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
1 : Custom broadcast array[0 1 ... 99998 99999] = [0 0 ... 0 0]
2 : Custom broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
3 : Custom broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
4 : Custom broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
5 : Custom broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
6 : Custom broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
7 : Custom broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
8 : Custom broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
9 : Custom broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
10 : Custom broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
11 : Custom broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]

0 : Default MPI broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
1 : Default MPI broadcast array[0 1 ... 99998 99999] = [0 0 ... 0 0]
2 : Default MPI broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
3 : Default MPI broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
4 : Default MPI broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
5 : Default MPI broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
6 : Default MPI broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
7 : Default MPI broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
8 : Default MPI broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
9 : Default MPI broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
10 : Default MPI broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]
11 : Default MPI broadcast array[0 1 ... 99998 99999] = [7.2892e+307 6.48761e+306 ... 7.79705e+307 8.53471e+307]

The custom and default broadcast implementations are equivalent.

The original broadcast array (org_Bcast_arr[]) was allocated and initialized with random double values in 0.005437 seconds.
The array was broadcasted to all other processes using "custom_Bcast()" in 0.002417 seconds.
The array was broadcasted to all other processes using "MPI_Bcast()" in 0.000636 seconds.

Number of nodes:
SLURM_JOB_NUM_NODES = 1.
Tasks per node:
SLURM_CPUS_ON_NODE = 12.

––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––