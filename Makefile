# define default variables
procs?=1


comp:
	mpiCC -o mpi_bcast mpi_bcast.cpp

run:
	mpi_run -np $(procs) ./mpi_bcast

debug:
	mpiCC -g mpi_test.cpp

gdb:
	mpiexec -d -np $(procs) ./mpi_bcast $(size)
	# mpiexec -n $(procs) gdb ./a.out

clean:
	rm mpi_bcast mpi_bcast.out a.out slurm*

