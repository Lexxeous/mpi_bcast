# define default variables
procs?=3
ruut?=1


comp:
	mpiCC -o mpi_bcast mpi_bcast.cpp custom_bcast.cpp

run:
	mpi_run -np $(procs) ./mpi_bcast $(ruut)

debug:
	mpiCC -g mpi_test.cpp

gdb:
	mpiexec -d -np $(procs) ./mpi_bcast $(ruut)
	# mpiexec -n $(procs) gdb ./a.out

clean:
	rm mpi_bcast mpi_bcast.out a.out slurm*