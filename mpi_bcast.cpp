// include necessary libraries
#include "custom_bcast.h"

// define constants
#define BCAST_LEN 100000

// declare namespace
using namespace std;

// prototype function(s)
double range_rand_double(double low, double high);

int main(int argc, char* argv[])
{
  // initialize the MPI environment
  MPI_Init(NULL, NULL);
  int world_size, world_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);


  double start_time, alloc_time, cust_time, garb_time, mpi_lib_time; // declare time variables
  int ruut = atoi(argv[1]); // user specified root process
  srand(time(NULL)); // seed the randomizer


  // ruut must be between 0 and world_size - 1, inclusive
  if((ruut < 0) || (ruut > world_size - 1))
  {
    cout << "MPI_ERR_ROOT:ROOT => Invalid root.\nRoot must be between 0 and (<world_size> - 1), inclusive.\n";
    MPI_Abort(MPI_COMM_WORLD, MPI_ERR_ROOT);
  }


  // world size must be larger than 1, can't only broadcast to yourself
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


  // collect the start time
  if(world_rank == ruut)
  {
    start_time = MPI_Wtime(); // wall time at the start of the program
    cout << "Comparing the execution time between the default library version of \"MPI_Bcast()\" and a custom implementation using only point-to-point communication...\n";
  }


  // define necessary variables and arrays
  double *org_Bcast_arr, *cust_Bcast_arr, *mpi_Bcast_arr;
  org_Bcast_arr = (double*)calloc(BCAST_LEN, sizeof(double));
  cust_Bcast_arr = (double*)calloc(BCAST_LEN, sizeof(double));
  mpi_Bcast_arr = (double*)calloc(BCAST_LEN, sizeof(double));
  int curr_rank = 0;
  int equal_arrs = 1;


  MPI_Barrier(MPI_COMM_WORLD); // for proper order of printing


  // define and distribute broadcast arrays
  if(world_rank == ruut) // only root gets the original broadcast array
  {
    // populate the original broadcast array
    for(int i = 0; i < BCAST_LEN; i++)
    {
      org_Bcast_arr[i] = range_rand_double(DBL_MIN, DBL_MAX); // use double macro variables as range
    }

    cout << world_rank << " : Original broadcast array[0 1 ... 99998 99999] = ["
         << org_Bcast_arr[0] << " " << org_Bcast_arr[1] << " ... " << org_Bcast_arr[99998] << " " << org_Bcast_arr[99999] << "]\n";
  }
  else // all other processes get the resulting arrays
  {
    // populate the resulting broadcast arrays
    for(int i = 0; i < BCAST_LEN; i++)
    {
      cust_Bcast_arr[i] = (double)-1; // initialize the custom broadcast array with all -1
      mpi_Bcast_arr[i] = (double)-2; // initialize the MPI broadcast array with all -2
    }

    if(world_rank == (ruut + 1) % world_size) // next process that is not root
    {
      cout << "\n" << world_rank << " : Original broadcast array[0 1 ... 99998 99999] = ["
           << org_Bcast_arr[0] << " " << org_Bcast_arr[1] << " ... " << org_Bcast_arr[99998] << " " << org_Bcast_arr[99999] << "]\n";

      cout << world_rank << " : Initialized custom broadcast array[0 1 ... 99998 99999] = ["
           << cust_Bcast_arr[0] << " " << cust_Bcast_arr[1] << " ... " << cust_Bcast_arr[99998] << " " << cust_Bcast_arr[99999] << "]\n";

      cout << world_rank << " : Initialized MPI broadcast array[0 1 ... 99998 99999] = ["
           << mpi_Bcast_arr[0] << " " << mpi_Bcast_arr[1] << " ... " << mpi_Bcast_arr[99998] << " " << mpi_Bcast_arr[99999] << "]\n";   
    }
  }


  // collect the allocation time
  if(world_rank == ruut)
  {
    alloc_time = MPI_Wtime() - start_time; // time to create and initialize broadcast arrays
  }


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
      cout << world_rank << " : Custom broadcast array[0 1 ... 99998 99999] = ["
           << cust_Bcast_arr[0] << " " << cust_Bcast_arr[1] << " ... " << cust_Bcast_arr[99998] << " " << cust_Bcast_arr[99999] << "]\n";
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


  if(world_rank == ruut)
    cout << endl; // for spacing


  // print the first and last elements of the default MPI broadcast array
  curr_rank = 0;
  while(curr_rank < world_size)
  {
    if(world_rank == curr_rank)
    {
      cout << world_rank << " : Default MPI broadcast array[0 1 ... 99998 99999] = ["
           << mpi_Bcast_arr[0] << " " << mpi_Bcast_arr[1] << " ... " << mpi_Bcast_arr[99998] << " " << mpi_Bcast_arr[99999] << "]\n";
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


  MPI_Barrier(MPI_COMM_WORLD); // for proper order of printing


  if(world_rank == ruut)
  {
    if(equal_arrs)
    {
      cout << "\nThe custom and default broadcast implementations are equivalent.\n";
    }
    else
    {
      cout << "\nThe custom and default broadcast implementations are NOT equivalent.\n";
    }
  }


  // print the execution times
  if(world_rank == ruut)
  {
    cout << "\nThe original broadcast array (org_Bcast_arr[]) was allocated and initialized with random double values in " << alloc_time << " seconds.\n";
    cout << "The array was broadcasted to all other processes using \"custom_Bcast()\" in " << cust_time << " seconds.\n";
    cout << "The array was broadcasted to all other processes using \"MPI_Bcast()\" in " << mpi_lib_time << " seconds.\n";
  }


  MPI_Barrier(MPI_COMM_WORLD); // for proper order of printing


  if(world_rank == ruut)
    cout << endl; // for spacing


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
  // get the range of values
  double range = high - low;

  // return the random value
  return (rand() / double (RAND_MAX) * (range - 1)) + low;
}

// ––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
