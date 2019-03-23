// include guard
#ifndef CUSTOM_BCAST_H
#define CUSTOM_BCAST_H

// include necessary libraries
#include <mpi.h>

// define constants
#define MPI_TYPES 12

// prototype function(s)
void custom_Bcast(void *buf, int cnt, MPI_Datatype type, int ruut, MPI_Comm communicator);
int validate_Bcast(void *buf, int cnt, MPI_Datatype type, int ruut, MPI_Comm communicator);

#endif //CUSTOM_BCAST_H