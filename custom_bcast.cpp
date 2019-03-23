// include .h file
#include "custom_bcast.h"

// declare namespace
using namespace std;

// ––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

void custom_Bcast(void* buf, int cnt, MPI_Datatype type, int ruut, MPI_Comm communicator)
{
  int world_size, world_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);


  int code = validate_Bcast(buf, cnt, type, ruut, communicator, world_size);


  if((code != MPI_SUCCESS) && world_rank == ruut)
  {
    cout << "\nMPI_ERR_RETURN:VALIDATION => The custom broadcast data failed to validate.\nEnsure that you are passing valid arguments.\n";
    MPI_Abort(communicator, code);
  }


  if(world_rank == ruut) // root broadcasts to all
  {
    cout << "\nUsing process " << ruut << " as the root to broadcast data to all other processes...\n";

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
