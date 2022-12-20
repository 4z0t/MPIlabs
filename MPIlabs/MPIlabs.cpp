

#include "mpi.h"
#include <iostream>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <algorithm>
#define __PRINT__VEC__ true

template<class T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v)
{
#if __PRINT__VEC__ 
	for (const T& e : v)
	{
		out << e << ' ';
	}
#endif
	return out;
}

using std::cout;
using std::cin;
using std::endl;
using std::vector;


void MakeStar(int nodes_count, vector<int>& index, vector<int>& edges)
{
	int ind = nodes_count - 1;
	for (int i = 0; i < nodes_count; i++)
	{
		index.push_back(ind);
		ind++;
	}

	for (int i = 1; i < nodes_count; i++)
	{
		edges.push_back(i);
	}
	for (int i = 1; i < nodes_count; i++)
	{
		edges.push_back(0);
	}
}


vector<int> Range(int start, int end)
{
	vector<int> res(end - start);
	for (int i = 0; i < end - start; i++)
	{
		res[i] = start + i;
	}
	return res;
}


int main(int argc, char** argv)
{
	int proc_num, proc_id;

	MPI::Init(argc, argv);

	proc_num = MPI::CommSize();
	proc_id = MPI::CommRank();


	auto mid = proc_num / 2;

	auto cc = MPI::Group().Include(Range(mid, proc_num)).GetComm();

	vector<int> index;
	vector<int> edges;
	MakeStar(mid, index, edges);
	MPI::CommId graph_comm = MPI::CreateGraph(index, edges);

	if (proc_id < mid)
	{
		if (proc_id == 0)
		{
			for (auto rank : MPI::GraphNeighbors(0, graph_comm))
			{
				cout << proc_id << " graph sending to " << rank << endl;
				MPI::Send<int>(0, rank, 0, graph_comm);
			}
		}
		else
		{

			MPI::Recv<int>(0, 0, graph_comm);
			cout << proc_id << " graph recv" << endl;
		}



	}
	else
	{
		MPI::CommId cart_comm = MPI::CreateCart<1>({ proc_num - mid }, cc);


		MPI::SourceDest sd = MPI::CardShift(cart_comm, 0, 1);

		cout << proc_id << " sending to " << sd.dest + mid << endl;
		MPI::Send<int>(0, sd.dest, 0, cart_comm);
		cout << proc_id << " recv from " << sd.source + mid << endl;
		MPI::Recv<int>(sd.source, 0, cart_comm);
	}


	return 0;
}







