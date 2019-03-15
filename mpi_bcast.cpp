// include necessary libraries
#include <mpi.h>
#include <iostream>
#include <cstdlib>

#define BCAST_LEN 100000

// declare namespace
using namespace std;

// prototype function(s)
void custom_Bcast(void *buf, int cnt, MPI_Datatype type, int src, MPI_Comm group);
int validate_Bcast(void *buf, int cnt, MPI_Datatype type, int src, MPI_Comm group);

int main()
{
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  if(validate_Bcast() <= 0)
  {
    MPI_Abort()
  }

  double start_time, cust_time, lib_time;

  if(world_rank == root)
  {
    start_time = MPI_Wtime(); // wall time at the start of the program  
  }


  custom_Bcast(buffer, count, datatype, root, MPI_COMM_WORLD);


  if(world_rank == root)
  {
    cust_time = MPI_Wtime() - start_time; // wall time at the start of the program  
  }


  MPI_Bcast(buffer, count, datatype, root, MPI_COMM_WORLD);


  if(world_rank == root)
  {
    lib_time = MPI_Wtime() - start_time - cust_time; // wall time at the start of the program  
  }

}


void custom_Bcast(void *buf, int cnt, MPI_Datatype type, int src, MPI_Comm group)
{
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

int validate_Bcast(void *buf, int cnt, MPI_Datatype type, int src, MPI_Comm group)
{
  MPI_Comm_size(comm, &world_size);
  MPI_Comm_rank(comm, &world_rank);
  
  if(count <= 0) return MPI_ERR_COUNT;
  if((root < 0) || (root >= world_size)) return MPI_ERR_ROOT;
  if(buffer == NULL) return MPI_ERR_BUFFER;
  // VALIDATE DATATYPE
  // VALIDATE COMM

  MPI_Barrier(comm);
  return MPI_SUCCESS;
}