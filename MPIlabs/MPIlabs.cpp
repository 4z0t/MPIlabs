

#include "mpi.h"
#include <iostream>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
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



template<class T>
vector<T> Mult(const vector<T>& p1, const vector<T>& p2)
{
	vector<T> p3(p1.size() + p2.size() - 1, T{});

	for (size_t i = 0; i < p1.size(); i++)
	{
		for (size_t j = 0; j < p2.size(); j++)
		{
			p3[i + j] += p1[i] * p2[j];
		}
	}
	return p3;
}



vector<double> MakeRandomPoly(int power, double maxv, double minv)
{
	vector<double> p(power + 1, 0.0);
	for (auto& v : p)
	{
		double f = (double)(std::rand()) / RAND_MAX;
		v = minv + f * (maxv - minv);
	}
	return p;
}


template<typename T>
void Merge(vector<T>& v, const size_t mid)
{
	vector<T> left(v.begin(), v.begin() + mid);
	vector<T> right(v.begin() + mid, v.end());

	size_t i1 = 0, i2 = 0;
	size_t iv = 0;


	while (i1 < left.size() && i2 < right.size()) {
		if (left[i1] <= right[i2]) {
			v[iv] = left[i1];
			i1++;
		}
		else
		{
			v[iv] = right[i2];
			i2++;
		}
		iv++;
	}

	while (i1 < left.size()) {
		v[iv] = left[i1];
		i1++;
		iv++;
	}

	while (i2 < right.size()) {
		v[iv] = right[i2];
		i2++;
		iv++;
	}
}

template<typename T>
vector<T> Merge(const vector<T>& v1, const  vector<T>& v2)
{
	vector<T> res(v1.size(), v2.size());

}

int main(int argc, char** argv)
{
	int proc_num, proc_id;

	MPI::Init(argc, argv);
	/*proc_num = MPI::CommSize();
	proc_id = MPI::CommRank();

	int dims[]{ 1 };
	MPI::CreateCart<1>(MPI::COMM_WORLD, dims);*/


	vector<double> a = { 1,3,5,2,4,8 };
	Merge(a, 3);
	cout << a;

	return 0;
}