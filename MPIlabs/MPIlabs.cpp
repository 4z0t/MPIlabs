

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

	for (int i = 0; i < nodes_count * 2; i++)
	{
		edges.push_back(i % nodes_count);
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





	MPI::CommId cart_comm = MPI::CreateCart(Range(0, proc_num));

	vector<int> index;
	vector<int> edges;
	MakeStar(proc_num, index, edges);

	MPI::CommId graph_comm = MPI::CreateGraph(index, edges);



	return 0;
}







