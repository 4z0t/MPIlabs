

#include "mpi.h"
#include <iostream>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

template<class T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v)
{
	for (const T& e : v)
	{
		out << e << ' ';
	}
	return out;
}

using std::cout;
using std::endl;
using std::vector;

const int m = 10;

enum class MSGType :int
{
	Response,
	Request,
	Message
};



int main(int argc, char** argv)
{
	int proc_num, proc_id;

	MPI::Init(argc, argv);
	proc_num = MPI::CommSize();
	proc_id = MPI::CommRank();

	for (int i = 0; i < m; i++)
	{
		vector<int> data(proc_num, 0);
		vector<int> res(proc_num, 0);
		if (proc_id != 0)
		{
			data[proc_id] = proc_id*(i+1);
		}
		MPI::Reduce(data, res, MPI::Operation::Sum);

		if (proc_id == 0)
		{
			cout << "Root: \tRecived messages: " << res << '\n';
		}

		MPI::Bcast(res);
		if (proc_id != 0)
		{
			cout << "Branch " << proc_id << ": \tRecived messages: " << res << '\n';
		}
		MPI::Barrier();
	}
	switch (proc_id)
	{
	case 0:
		cout << "Root complete" << endl;

		break;
	default:
		cout << "Sending complete from " << proc_id << endl;

		break;
	}

	return 0;
}


