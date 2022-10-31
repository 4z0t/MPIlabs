

#include "mpi.h"
#include <iostream>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

using std::cout;
using std::endl;

const int m = 10;

enum class MSGType :int
{
	Response,
	Request,
	Message
};


void root(int proc_num)
{
	MPI::Status s;
	for (int i = 0; i < m; i++)
	{
		for (int k = 1; k < proc_num; k++)
		{
			int msg = MPI::Recv<int>(k, static_cast<int>(MSGType::Message), MPI_COMM_WORLD, s);
			cout << "Root recieved " << msg << " from " << s.MPI_SOURCE << endl;
			for (int j = 1; j < proc_num; j++)
			{
				if (j == s.MPI_SOURCE)continue;
				cout << "Root sending " << msg << " to " << j << " from "<<s.MPI_SOURCE << endl;
				MPI::Send(msg, j, static_cast<int>(MSGType::Message));

				int response = MPI::Recv<int>(j, static_cast<int>(MSGType::Response));
				if (response != j)
					cout << "wrong id" << endl;
			}
		}
	}
	cout << "Root complete" << endl;
}

void branch(int proc_id, int proc_num)
{
	MPI::Status s;
	for (int i = 0; i < m; i++)
	{
		MPI::Send(i, 0, static_cast<int>(MSGType::Message));
		for (int j = 2; j < proc_num; j++)
		{
			int msg = MPI::Recv<int>(0, static_cast<int>(MSGType::Message), MPI_COMM_WORLD, s);
			cout << proc_id << " recieved " << msg << " from " << s.MPI_SOURCE << endl;
			MPI::Send(proc_id, 0, static_cast<int>(MSGType::Response));
		}
	}
	cout << "Sending complete from " << proc_id << endl;
}

int main(int argc, char** argv)
{
	int proc_num, proc_id;

	MPI::Init(argc, argv);
	proc_num = MPI::CommSize(MPI_COMM_WORLD);
	proc_id = MPI::CommRank(MPI_COMM_WORLD);
	switch (proc_id)
	{
	case 0:
		root(proc_num);
		break;
	default:
		branch(proc_id, proc_num);
		break;
	}


	return 0;
}


