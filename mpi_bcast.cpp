// include necessary libraries
#include <mpi.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cfloat>
#include <cmath>

// define constants
#define BCAST_LEN 100000
#define MPI_TYPES 12

// declare namespace
using namespace std;

// prototype function(s)
double range_rand_double(double low, double high);
void custom_Bcast(void *buf, int cnt, MPI_Datatype type, int ruut, MPI_Comm communicator);
int validate_Bcast(void *buf, int cnt, MPI_Datatype type, int ruut, MPI_Comm communicator, int world_size);

int main(int argc, char* argv[])
{
  // initialize the MPI environment
  MPI_Init(NULL, NULL);
  int world_size, world_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);


  int ruut = atoi(argv[1]); // user specified root process


  // WORLD SIZE MUST BE LARGER THAN 1, MUST HAVE PROCESS 0 AND PROCESS 1 AT LEAST
  if(world_size < 2 && world_rank == ruut)
  {
    cout << "MPI_ERR_TOPOLOGY:WORLD_SIZE => Too few initialized processes.\nThe world size should be at least of size 2.\n";
    MPI_Abort(MPI_COMM_WORLD, MPI_ERR_TOPOLOGY);
  }


  // command line arguments must be the executable and the root
  if(argc != 2 && world_rank == ruut)
  {
    cout << "MPI_ERR_ARG:ARGC => Wrong number of command line arguments.\nUse \"mpirun -np <world_size> ./<executable> <root>\" as format.\n";
    MPI_Abort(MPI_COMM_WORLD, MPI_ERR_ARG);
  }


  // define necessary variables and arrays
  double *org_Bcast_arr, *cust_Bcast_arr, *mpi_Bcast_arr;
  org_Bcast_arr = (double*)malloc(sizeof(double) * BCAST_LEN);
  cust_Bcast_arr = (double*)malloc(sizeof(double) * BCAST_LEN);
  mpi_Bcast_arr = (double*)malloc(sizeof(double) * BCAST_LEN);
  double start_time, alloc_time, cust_time, garb_time, mpi_lib_time; // declare time variables
  int curr_rank = 0;
  int equal_arrs = 1;


  // collect the start time
  if(world_rank == ruut)
  {
    start_time = MPI_Wtime(); // wall time at the start of the program  
  }


  // define and distribute broadcast arrays
  if(world_rank == ruut) // only root gets the original broadcast array
  {
    // populate the original broadcast array
    for(int i = 0; i < BCAST_LEN; i++)
    {
      org_Bcast_arr[i] = range_rand_double(DBL_MIN, DBL_MAX); // use double macro variables as range
    }

    cout << "Original broadcast array[0 1 ... 99,998 99,999] = [" << org_Bcast_arr[0] << " " << org_Bcast_arr[1] << " ... " << org_Bcast_arr[99998] << " " << org_Bcast_arr[99999] << "]\n";
  }
  else // all other processes get the resulting arrays
  {
    // populate the resulting broadcast arrays
    for(int i = 0; i < BCAST_LEN; i++)
    {
      cust_Bcast_arr[i] = (double)-1; // initialize the custom broadcast array with all -1
      mpi_Bcast_arr[i] = (double)0; // initialize the MPI broadcast array with all 0
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
    for(int r = 0; r < BCAST_LEN; r++)
    {
      cust_Bcast_arr[r] = org_Bcast_arr[r];
    }
  }


  // print the first and last elements of the custom broadcast array
  curr_rank = 0;
  while(curr_rank < world_size)
  {
    if(world_rank == curr_rank)
    {
      cout << "For process " << world_rank << ":\n";
      cout << "Custom broadcast array[0 1 ... 99,998 99,999] = [" << cust_Bcast_arr[0] << " " << cust_Bcast_arr[1] << " ... " << cust_Bcast_arr[99998] << " " << cust_Bcast_arr[99999] << "]\n";
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
    mpi_lib_time = MPI_Wtime() - start_time - alloc_time - cust_time - garb_time; // time to broadcast array with default MPI broadcast function 
  }


  // save the default MPI broadcasted array for all but ruut
  if(world_rank != ruut)
  {
    for(int r = 0; r < BCAST_LEN; r++)
    {
      mpi_Bcast_arr[r] = org_Bcast_arr[r];
    }
  }


  // print the first and last elements of the default MPI broadcast array
  curr_rank = 0;
  while(curr_rank < world_size)
  {
    if(world_rank == curr_rank)
    {
      cout << "For process " << world_rank << ":\n";
      cout << "Default MPI broadcast array[0 1 ... 99,998 99,999] = [" << mpi_Bcast_arr[0] << " " << mpi_Bcast_arr[1] << " ... " << mpi_Bcast_arr[99998] << " " << mpi_Bcast_arr[99999] << "]\n";
    }
   curr_rank++;
   MPI_Barrier(MPI_COMM_WORLD); // barricade processes to print messages in correct order
  }


  // compare every broadcasted element for equality between the custom and the defualt MPI
  if(world_rank != ruut)
  {
    for(int j = 0; j < BCAST_LEN; j++)
    {
      if(cust_Bcast_arr[j] != mpi_Bcast_arr[j])
      {
        equal_arrs = 0;
      }
    }
  }


  if(equal_arrs)
  {
    cout << "Custom and default are equal.\n";
  }
  else
  {
    cout << "Custom and default are NOT equal.\n";
  }


  // print the execution times
  if(world_rank == ruut)
  {
    cout << "The original broadcast array (org_Bcast_arr[]) was allocated and initialized with random double values in " << alloc_time << " seconds.\n";
    cout << "The array was broadcasted to all other processes using \"custom_Bcast()\" in " << cust_time << "seconds.\n";
    cout << "The array was broadcasted to all other processes using \"MPI_Bcast()\" in " << mpi_lib_time << "seconds.\n";
  }


  // cleanup and finalize the MPI environment
  free(org_Bcast_arr);
  free(cust_Bcast_arr);
  free(mpi_Bcast_arr);
  MPI_Finalize();
  return 0;
}

/*
Format for MPI_Send() & MPI_Recv() & MPI_Bcast():
  MPI_Send(void* data, int count, MPI_Datatype datatype, int destination, int tag, MPI_Comm communicator);
  MPI_Recv(void* data, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm communicator, MPI_Status* status);
  int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
*/


// ––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

double range_rand_double(double low, double high)
{
  double range = high - low; // get the range of values
  return (rand() / double (RAND_MAX) * (range - 1)) + low; // return a random double between high and low
}

// ––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

void custom_Bcast(void* buf, int cnt, MPI_Datatype type, int ruut, MPI_Comm communicator)
{
  int world_size, world_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);


  int code = validate_Bcast(buf, cnt, type, ruut, communicator, world_size);

  if((code != MPI_SUCCESS) && world_rank == ruut)
  {
    cout << "MPI_ERR_RETURN:VALIDATION => The custom broadcast data failed to validate.\nEnsure that you are passing valid arguments.\n";
    MPI_Abort(communicator, code);
  }


  if(world_rank == ruut) // root broadcasts to all
  {
    cout << "Using process " << ruut << " to broadcast data to all other processes.\n";

    for(int k = 0; k < world_size; k++)
    {
      if(k == ruut)
        continue; // skips the root broadcasting to itself

      MPI_Send(buf, cnt, type, k, ruut, communicator);
    }
  }
  else // all other processes receive from root
  {
    MPI_Recv(buf, cnt, type, ruut, ruut, communicator, MPI_STATUS_IGNORE);
  }
}

// ––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

int validate_Bcast(void* buf, int cnt, MPI_Datatype type, int ruut, MPI_Comm communicator, int world_size)
{
  int type_err = 1;

  // explicitly define all MPI datatypes
  MPI_Datatype mpi_types_arr[MPI_TYPES] = {MPI_CHAR, MPI_UNSIGNED_CHAR, MPI_SIGNED_CHAR, MPI_SHORT,
                                             MPI_UNSIGNED_SHORT, MPI_INT, MPI_UNSIGNED, MPI_LONG,
                                             MPI_UNSIGNED_LONG, MPI_FLOAT, MPI_DOUBLE, MPI_LONG_DOUBLE};
  
  // validate all broadcast values
  if(buf == NULL) return MPI_ERR_BUFFER;
  if(cnt <= 0) return MPI_ERR_COUNT;
  for(int e = 0; e < MPI_TYPES; e++)
  {
    if(type == mpi_types_arr[e])
    {
      type_err = 0;
    }
  }
  if(type_err) return MPI_ERR_TYPE;
  if((ruut < 0) || (ruut > world_size - 1)) return MPI_ERR_ROOT;
  if(communicator == MPI_COMM_NULL) return MPI_ERR_COMM;

  MPI_Barrier(communicator);
  return MPI_SUCCESS; // return successful if validated
}

// ––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

