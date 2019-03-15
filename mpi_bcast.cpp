// include necessary libraries
#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <fstream>

// define constants
#define BCAST_LEN 100000
#define MPI_TYPES 12

// declare namespace
using namespace std;

// prototype function(s)
void custom_Bcast(void *buf, int cnt, MPI_Datatype type, int ruut, MPI_Comm communicator);
int validate_Bcast(void *buf, int cnt, MPI_Datatype type, int ruut, MPI_Comm communicator);
double range_rand_double(double low, double high);

int main()
{
  // initialize the MPI environment
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  ofstream out_file; // define output file
  out_file.open(); // open the output file

  // WORDLD SIZE MUST BE LARGER THAN 1, MUST HAVE PROCESS 0 AND PROCESS 1 AT LEAST
  if(world_size < 2 && world_rank == 0)
  {
    cout << "MPI_ERR_TOPOLOGY:WORLD_SIZE => Too few initialized processes.\nThe world size should be at least of size 2.\n";
    out_file << "MPI_ERR_TOPOLOGY:WORLD_SIZE => Too few initialized processes.\nThe world size should be at least of size 2.\n";
    MPI_Abort(MPI_COMM_WORLD, MPI_ERR_TOPOLOGY);
  }


  if(argc != 2 && world_rank == 0)
  {
    cout << "MPI_ERR_ARG:ARGC => Wrong number of command line arguments.\nUse \"mpirun -np <world_size> ./<executable> <root>\" as format.\n";
    out_file << "MPI_ERR_ARG:ARGC => Wrong number of command line arguments.\nUse \"mpirun -np <world_size> ./<executable> <root>\" as format.\n";
    MPI_Abort(MPI_COMM_WORLD, MPI_ERR_ARG);
  }

  if(world_rank == 0)
  {
    start_time = MPI_Wtime(); // wall time at the start of the program  
  }

  // define necessary variables
  int ruut = atoi(argv[1]); // user specified root process for broadcast
  double start_time, alloc_time, cust_time, mpi_lib_time; // declare time variables
  const int curr_rank = 0;
  const int equal_arrs = 1;

  // define necessary arrays
  double** org_Bcast_arr, cust_Bcast_arr, mpi_Bcast_arr;
  mpi_Bcast_arr = calloc(BCAST_LEN, sizeof(double*)); // dynamically allocate original array to broadcast
  cust_Bcast_arr = calloc(BCAST_LEN, sizeof(double*)); // dynamically allocate custom array to broadcast
  mpi_Bcast_arr = calloc(BCAST_LEN, sizeof(double*)); // dynamically allocate default MPI array to broadcast
  mpi_types_arr = MPI_Datatype[MPI_TYPES] = {MPI_CHAR, MPI_UNSIGNED_CHAR, MPI_SIGNED_CHAR, MPI_SHORT,
                                             MPI_UNSIGNED_SHORT, MPI_INT, MPI UNSIGNED, MPI_LONG,
                                             MPI_UNSIGNED_LONG, MPI_FLOAT, MPI_DOUBLE, MPI_LONG_DOUBLE};

  // populate the broadcast array
  for(int i = 0; i < BCAST_LEN; i++)
  {
    org_Bcast_arr[i] = range_rand_double(DBL_MIN, DBL_MAX); // use double macro variables as range
  }


  // collect the start time
  if(world_rank == 0)
  {
    alloc_time = MPI_Wtime() - start_time; // wall time at the start of the program  
  }


  custom_Bcast(org_Bcast_arr, BCAST_LEN, MPI_DOUBLE, ruut, MPI_COMM_WORLD); // broadcast using custom function

  if(world_rank = (ruut + 1) % world_size)
  {
    cust_Bcast_arr = org_Bcast_arr;
  }


  // print the first and last elements of the custom broadcast array
  curr_rank = 0;
  while(curr_rank < world_size)
  {
    if(world_rank == curr_rank)
    {
      cout << "Custom broadcast array[0]: " << cust_Bcast_arr[0] << "..." << "Custom broadcast array[99,999]: " << cust_Bcast_arr[99999] << endl;
      out_file << "Custom broadcast array[0]: " << cust_Bcast_arr[0] << "..." << "Custom broadcast array[99,999]: " << cust_Bcast_arr[99999] << endl;
    }
   curr_rank++;
   MPI_Barrier(MPI_COMM_WORLD); // barricade processes to print messages in correct order
  }


  // collect the custom broadcast time
  if(world_rank == 0)
  {
    cust_time = MPI_Wtime() - start_time - alloc_time; // wall time at the start of the program  
  }


  MPI_Bcast(org_Bcast_arr, BCAST_LEN, MPI_DOUBLE, ruut, MPI_COMM_WORLD); // broadcast using default MPI function

  if(world_rank = (ruut + 1) % world_size)
  {
    mpi_Bcast_arr = org_Bcast_arr;
  }


  // print the first and last elements of the default MPI broadcast array
  curr_rank = 0;
  while(curr_rank < world_size)
  {
    if(world_rank == curr_rank)
    {
      cout << "MPI broadcast array[0]: " << cust_Bcast_arr[0] << "..." << "MPI broadcast array[99,999]: " << cust_Bcast_arr[99999] << endl;
      out_file << "MPI broadcast array[0]: " << cust_Bcast_arr[0] << "..." << "MPI broadcast array[99,999]: " << cust_Bcast_arr[99999] << endl;
    }
   curr_rank++;
   MPI_Barrier(MPI_COMM_WORLD); // barricade processes to print messages in correct order
  }


  // collect the default MPI library broadcast time
  if(world_rank == 0)
  {
    mpi_lib_time = MPI_Wtime() - start_time - alloc_time - cust_time; // wall time at the start of the program  
  }


  // comparing every broadcasted element for equality
  for(int j = 0; j < BCAST_LEN; j++)
  {
    if(cust_Bcast_arr[i] != mpi_Bcast_arr[i])
    {
      equal_arrs = 0;
    }
  }


  if(equal_arrs)
  {
    // THE CUSTOM AND DEFAULT BCAST ARRAYS ARE EQUAL
    cout << "Custom and default are equal." << endl;
    out_file << "Custom and default are equal." << endl;
  }
  else
  {
    // THE CUSTOM AND DEFAULT BCAST ARRAYS ARE NOT EQUAL
    cout << "Custom and default are NOT equal." << endl;
    out_file << "Custom and default are NOT equal." << endl;
  }


  // print the execution times
  cout << "The original broadcast array (org_Bcast_arr[]) was allocated and initialized with random double values in " << alloc_time << " seconds.\n"
  cout << "The array was broadcasted to all other processes using \"custom_Bcast\" in " << cust_time << "seconds.\n"
  cout << "The array was broadcasted to all other processes using \"MPI_Bcast\" in " << mpi_lib_time << "seconds.\n"
  out_file << "The original broadcast array (org_Bcast_arr[]) was allocated and initialized with random double values in " << alloc_time << " seconds.\n"
  out_file << "The array was broadcasted to all other processes using \"custom_Bcast\" in " << cust_time << "seconds.\n"
  out_file << "The array was broadcasted to all other processes using \"MPI_Bcast\" in " << mpi_lib_time << "seconds.\n"


  // cleanup and finalize the MPI environment
  out_file.close();
  free(cust_Bcast_arr);
  MPI_Finalize();
  return 0;
}

// ––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

void custom_Bcast(void* buf, int cnt, MPI_Datatype type, int ruut, MPI_Comm communicator)
{

  if(validate_Bcast(buf, cnt, type, ruut, communicator) != MPI_SUCCESS)
  {
    MPI_Abort(communicator, MPI_ERRORS_RETURN)
  }

  int receiver; // process(es) which will receive data from root
    
  for(receiver = root; (receiver < world_size) && (receiver < root + world_size);)
  {
    if(world_rank == root)
      MPI_Send(buffer, count, datatype, receiver, 1, comm);
    else if(world_rank == receiver)
      MPI_Recv(buffer, count, datatype, root, 1, comm, null);

    if((world_rank >= receiver) && (world_rank < receiver + len))
    {
      custom_Bcast(buffer, count, datatype, receiver, comm);
      break;
    }
    receiver += len;
  }
}

// ––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

int validate_Bcast(void* buf, int cnt, MPI_Datatype type, int ruut, MPI_Comm communicator)
{
  MPI_Comm_size(communicator, &world_size);
  MPI_Comm_rank(communicator, &world_rank);
  
  // validate all broadcast values
  if(buf == NULL) return MPI_ERR_BUFFER;
  if(cnt <= 0) return MPI_ERR_COUNT;
  if(find(begin(mpi_types_arr), end(mpi_types_arr), type) == end(mpi_types_arr)) return MPI_ERR_TYPE;
  if((ruut < 0) || (ruut > world_size - 1)) return MPI_ERR_ROOT;
  if(communicator == MPI_COMM_NULL) return MPI_ERR_COMM;

  MPI_Barrier(communicator);
  return MPI_SUCCESS; // return successful if validated
}

// ––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

double range_rand_double(double low, double high)
{
  double range = high - low; // get the range of values
  return (rand() / double (RAND_MAX) * (range - 1)) + low; // return a random double between high and low
}

// ––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
