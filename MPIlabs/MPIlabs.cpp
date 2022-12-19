

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






int main(int argc, char** argv)
{
	int proc_num, proc_id;

	MPI::Init(argc, argv);

	proc_num = MPI::CommSize();
	proc_id = MPI::CommRank();




	proc_num = MPI::CommSize();
	proc_id = MPI::CommRank();


	MPI::CreateCart<1>({ proc_num });




	return 0;
}







