

#include <iostream>
#include "mpi.h"

int main(int argc, char** argv)
{
	int proc_num, proc_id;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
	std::cout << "number of proc " << proc_num << "\tid " << proc_id << std::endl;
	MPI_Finalize();

	return 0;
}
