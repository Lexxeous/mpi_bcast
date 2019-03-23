// include necessary libraries
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <custom_bcast.h>

// define constants
#define BCAST_LEN 100000

// declare namespace
using namespace std;

// prototype function(s)
double range_rand_double(double low, double high)

int main()
{
  // initialize the MPI environment
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);


  ofstream out_file; // define output file
  out_file.open(); // open the output file


  int ruut = atoi(argv[1]); // user specified root process


  // WORLD SIZE MUST BE LARGER THAN 1, MUST HAVE PROCESS 0 AND PROCESS 1 AT LEAST
  if(world_size < 2 && world_rank == ruut)
  {
    cout << "MPI_ERR_TOPOLOGY:WORLD_SIZE => Too few initialized processes.\nThe world size should be at least of size 2.\n";
    out_file << "MPI_ERR_TOPOLOGY:WORLD_SIZE => Too few initialized processes.\nThe world size should be at least of size 2.\n";
    MPI_Abort(MPI_COMM_WORLD, MPI_ERR_TOPOLOGY);
  }


  // command line arguments must be the executable and the root
  if(argc != 2 && world_rank == ruut)
  {
    cout << "MPI_ERR_ARG:ARGC => Wrong number of command line arguments.\nUse \"mpirun -np <world_size> ./<executable> <root>\" as format.\n";
    out_file << "MPI_ERR_ARG:ARGC => Wrong number of command line arguments.\nUse \"mpirun -np <world_size> ./<executable> <root>\" as format.\n";
    MPI_Abort(MPI_COMM_WORLD, MPI_ERR_ARG);
  }


  if(world_rank == ruut)
  {
    start_time = MPI_Wtime(); // wall time at the start of the program  
  }


  // define necessary variables
  double start_time, alloc_time, cust_time, garb_time, mpi_lib_time; // declare time variables
  const int curr_rank = 0;
  const int equal_arrs = 1;


  // define and distribute broadcast arrays
  if(world_rank == ruut) // only root gets the original broadcast array
  {
    double** org_Bcast_arr;
    org_Bcast_arr = calloc(BCAST_LEN, sizeof(double*)); // dynamically allocate original array to broadcast

    // populate the original broadcast array
    for(int i = 0; i < BCAST_LEN; i++)
    {
      org_Bcast_arr[i] = range_rand_double(DBL_MIN, DBL_MAX); // use double macro variables as range
    }

    cout << "Original broadcast array[0 1 ... 99,998 99,999]: " << org_Bcast_arr[0] << org_Bcast_arr[1] << "..." << org_Bcast_arr[99998] << org_Bcast_arr[99999] << endl;
    out_file << "Original broadcast array[0 1 ... 99,998 99,999]: " << org_Bcast_arr[0] << org_Bcast_arr[1] << "..." << org_Bcast_arr[99998] << org_Bcast_arr[99999] << endl;
  }
  else // all other processes get the resulting arrays
  {
    double** cust_Bcast_arr, mpi_Bcast_arr;
    cust_Bcast_arr = calloc(BCAST_LEN, sizeof(double*)); // dynamically allocate custom array to broadcast
    mpi_Bcast_arr = calloc(BCAST_LEN, sizeof(double*)); // dynamically allocate default MPI array to broadcast

    // populate the resulting broadcast arrays
    for(int i = 0; i < BCAST_LEN; i++)
    {
      cust_Bcast_arr[i] = -1; // initialize the custom broadcast array with all -1
      mpi_Bcast_arr[i] = 0; // initialize the MPI broadcast array with all 0
    }
  }


  // collect the allocation time
  if(world_rank == ruut)
  {
    alloc_time = MPI_Wtime() - start_time; // time to create and initialize broadcast arrays
  }

  MPI_Barrier(MPI_COMM_WORLD);
  custom_Bcast(org_Bcast_arr, BCAST_LEN, MPI_DOUBLE, ruut, MPI_COMM_WORLD); // broadcast using custom function


  // collect the custom broadcast time
  if(world_rank == ruut)
  {
    cust_time = MPI_Wtime() - start_time - alloc_time; // time to broadcast array with custom broadcast function 
  }


  // save the custom broadcasted array for all but ruut
  if(world_rank != ruut)
  {
    cust_Bcast_arr = org_Bcast_arr;
  }


  // print the first and last elements of the custom broadcast array
  curr_rank = 0;
  while(curr_rank < world_size)
  {
    if(world_rank == curr_rank)
    {
      cout << "For process " << world_rank << ":\n";
      out_file << "For process " << world_rank << ":\n";
      cout << "Custom broadcast array[0 1 ... 99,998 99,999]: " << cust_Bcast_arr[0] << cust_Bcast_arr[1] << "..." << cust_Bcast_arr[99998] << cust_Bcast_arr[99999] << endl;
      out_file << "Custom broadcast array[0 1 ... 99,998 99,999]: " << cust_Bcast_arr[0] << cust_Bcast_arr[1] << "..." << cust_Bcast_arr[99998] << cust_Bcast_arr[99999] << endl;
    }
   curr_rank++;
   MPI_Barrier(MPI_COMM_WORLD); // barricade processes to print messages in correct order
  }


  // collect the garbage time between broadcasts
  if(world_rank == ruut)
  {
    garb_time = MPI_Wtime() - start_time - alloc_time - cust_time;
  }


  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(org_Bcast_arr, BCAST_LEN, MPI_DOUBLE, ruut, MPI_COMM_WORLD); // broadcast using default MPI function


  // collect the default MPI library broadcast time
  if(world_rank == ruut)
  {
    mpi_lib_time = MPI_Wtime() - start_time - alloc_time - cust_time - garb1_time; // time to broadcast array with default MPI broadcast function 
  }

  // save the default MPI broadcasted array for all but ruut
  if(world_rank != ruut)
  {
    mpi_Bcast_arr = org_Bcast_arr;
  }


  // print the first and last elements of the default MPI broadcast array
  curr_rank = 0;
  while(curr_rank < world_size)
  {
    if(world_rank == curr_rank)
    {
      cout << "For process " << world_rank << ":\n";
      out_file << "For process " << world_rank << ":\n";
      cout << "MPI broadcast array[0]: " << cust_Bcast_arr[0] << "..." << "MPI broadcast array[99,999]: " << cust_Bcast_arr[99999] << endl;
      out_file << "MPI broadcast array[0]: " << cust_Bcast_arr[0] << "..." << "MPI broadcast array[99,999]: " << cust_Bcast_arr[99999] << endl;
    }
   curr_rank++;
   MPI_Barrier(MPI_COMM_WORLD); // barricade processes to print messages in correct order
  }


  // compare every broadcasted element for equality between the custom and the defualt MPI
  if(world_rank != ruut)
  {
    for(int j = 0; j < BCAST_LEN; j++)
    {
      if(cust_Bcast_arr[i] != mpi_Bcast_arr[i])
      {
        equal_arrs = 0;
      }
    }
  }


  if(equal_arrs)
  {
    cout << "Custom and default are equal.\n";
    out_file << "Custom and default are equal.\n";
  }
  else
  {
    cout << "Custom and default are NOT equal.\n";
    out_file << "Custom and default are NOT equal.\n";
  }


  // print the execution times
  if(world_rank == ruut)
  {
    cout << "The original broadcast array (org_Bcast_arr[]) was allocated and initialized with random double values in " << alloc_time << " seconds.\n";
    cout << "The array was broadcasted to all other processes using \"custom_Bcast\" in " << cust_time << "seconds.\n";
    cout << "The array was broadcasted to all other processes using \"MPI_Bcast\" in " << mpi_lib_time << "seconds.\n";
    out_file << "The original broadcast array (org_Bcast_arr[]) was allocated and initialized with random double values in " << alloc_time << " seconds.\n";
    out_file << "The array was broadcasted to all other processes using \"custom_Bcast\" in " << cust_time << "seconds.\n";
    out_file << "The array was broadcasted to all other processes using \"MPI_Bcast\" in " << mpi_lib_time << "seconds.\n";
  }


  // cleanup and finalize the MPI environment
  out_file.close();
  free(cust_Bcast_arr);
  MPI_Finalize();
  return 0;
}

/*
Format for MPI_Send() & MPI_Recv()
  MPI_Send(void* data, int count, MPI_Datatype datatype, int destination, int tag, MPI_Comm communicator);
  MPI_Recv(void* data, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm communicator, MPI_Status* status);
*/


// ––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

double range_rand_double(double low, double high)
{
  double range = high - low; // get the range of values
  return (rand() / double (RAND_MAX) * (range - 1)) + low; // return a random double between high and low
}

// ––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

