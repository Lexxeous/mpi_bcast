void DoBcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, int groupCount) {
    
    int receiver; // process which will receive his part of data from us and will retranslate it to others
    
    if (groupCount == 1) return; // we already have data we want
    
    goodGroupCount = CalcGoodGroupCount(groupCount);
    len = goodGroupCount / 2;

    for (receiver = root + 1; (receiver < world_size) && (receiver < root + groupCount);) {
	if (world_rank == root)
	    MPI_Send(buffer, count, datatype, receiver, 1, comm);
	else if (world_rank == receiver)
	    MPI_Recv(buffer, count, datatype, root, 1, comm, null);

	if ((world_rank >= receiver) && (world_rank < receiver + len)) {
	    DoBcast(buffer, count, datatype, receiver, comm, len);
	    break;
	}
	receiver += len;
	len /= 2;
    }
}

int RMB_Bcast( void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm ) {
    MPI_Comm_size(comm, &world_size);
    MPI_Comm_rank(comm, &world_rank);
    /* check params are correct */
    if (count <= 0) return MPI_ERR_COUNT;
    if ((root < 0) || (root >= world_size)) return MPI_ERR_ROOT;
    if (buffer == NULL) return MPI_ERR_BUFFER;
    /* I don't know how to check datatype and communicator, so lets think everything is ok */

    DoBcast(buffer, count, datatype, root, comm, world_size);
    MPI_Barrier(comm);
    return MPI_SUCCESS;
}