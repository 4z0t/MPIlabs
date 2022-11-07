

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
		vector<int> res;
		if (proc_id != 0)
		{
			data[proc_id] = proc_id * (i + 1);
		}
		res = MPI::Gather(data, proc_num * proc_num);
		if (proc_id == 0)
		{
			cout << "Root: \t\tRecived messages: " << res << endl;
			//combine
			for (int j = 0; j < proc_num; j++)
			{
				for (int k = 0; k < proc_num; k++)
				{
					res[k + j * proc_num] = res[k + k * proc_num];
				}
			}
		}

		res = MPI::Scatter(res, proc_num);
		if (proc_id != 0)
		{
			cout << "Branch " << proc_id << ": \tRecived messages: " << res << endl;
		}
		MPI::Barrier();
		if (proc_id == 0)
			cout << '\n';
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


