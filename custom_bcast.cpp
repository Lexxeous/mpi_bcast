// include H_file
#include <custom_bcast.h>

// declare namespace
using namespace std;

// ––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

void custom_Bcast(void* buf, int cnt, MPI_Datatype type, int ruut, MPI_Comm communicator)
{
  if(validate_Bcast(buf, cnt, type, ruut, communicator) != MPI_SUCCESS)
  {
    cout << "MPI_ERR_RETURN:VALIDATION => The custom broadcast data failed to validate.\nEnsure that you are passing valid arguments.\n";
    out_file << "MPI_ERR_RETURN:VALIDATION => The custom broadcast data failed to validate.\nEnsure that you are passing valid arguments.\n";
    MPI_Abort(communicator, MPI_ERRORS_RETURN);
  }


  if(world_rank == ruut) // root broadcasts to all
  {
    cout << "Using process " << ruut << " to broadcast data to all other processes.\n";
    out_file << "Using process " << ruut << " to broadcast data to all other processes.\n";

    for(k = 0; k < world_size; k++)
    {
      if(k == ruut)
        continue; // skips the root broadcasting to itself

      MPI_Send(buf, cnt, type, k, MPI_ANY_TAG, communicator);
    }
  }
  else // all other processes receive from root
  {
    MPI_Recv(buf, cnt, type, ruut, MPI_ANY_TAG, communicator, MPI_STATUS_IGNORE);
  }
}

// ––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

int validate_Bcast(void* buf, int cnt, MPI_Datatype type, int ruut, MPI_Comm communicator)
{
  // explicitly define all MPI datatypes
  mpi_types_arr = MPI_Datatype[MPI_TYPES] = {MPI_CHAR, MPI_UNSIGNED_CHAR, MPI_SIGNED_CHAR, MPI_SHORT,
                                             MPI_UNSIGNED_SHORT, MPI_INT, MPI_UNSIGNED, MPI_LONG,
                                             MPI_UNSIGNED_LONG, MPI_FLOAT, MPI_DOUBLE, MPI_LONG_DOUBLE};
  
  // validate all broadcast values
  if(buf == NULL) return MPI_ERR_BUFFER;
  if(cnt <= 0) return MPI_ERR_COUNT;
  if(find(begin(mpi_types_arr), end(mpi_types_arr), type) >= end(mpi_types_arr)) return MPI_ERR_TYPE;
  if((ruut < 0) || (ruut > world_size - 1)) return MPI_ERR_ROOT;
  if(communicator == MPI_COMM_NULL) return MPI_ERR_COMM;

  MPI_Barrier(communicator);
  return MPI_SUCCESS; // return successful if validated
}

// ––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
