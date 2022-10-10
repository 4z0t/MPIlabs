﻿

#include "mpi.h"
#include <iostream>


using std::cout;
using std::endl;
int main(int argc, char** argv)
{
	int proc_num, proc_id;

	MPI::Init(argc, argv);
	proc_num = MPI::CommSize(MPI_COMM_WORLD);
	proc_id = MPI::CommRank(MPI_COMM_WORLD);
	switch (proc_id)
	{
	case 0:
		for (int i = 0; i < proc_num - 1; i++)
		{

			int r = MPI::Recv<int>();
			cout << "Received " << r << endl;
		}
		break;
	default:
		cout << "Sending my id to 0 " << endl;
		MPI::Send(proc_id);
		break;
	}
	MPI::Finalize();
	return 0;
}
