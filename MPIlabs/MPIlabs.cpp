

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





void FillRandom(vector<double>& v, double maxv, double minv)
{
	for (size_t i = 0; i < v.size(); i++)
	{
		double f = (double)(std::rand()) / RAND_MAX;
		v[i] = minv + f * (maxv - minv);
	}
}


bool CheckSort(const vector<double>& v)
{
	for (size_t i = 1; i < v.size(); i++)
	{
		if (v[i - 1] > v[i])return false;
	}
	return true;
}

template<typename T>
void Merge(vector<T>& v, const vector<T>& left, const vector<T>& right)
{

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
void Merge(vector<T>& v, const size_t mid)
{
	vector<T> left(v.begin(), v.begin() + mid);
	vector<T> right(v.begin() + mid, v.end());

	return Merge(v, left, right);
}



class TreeGroup
{
public:
	//TreeGroup() :level(0) {}
	TreeGroup(size_t level, int left, int right) : level(level), left(left), right(right)
	{
		group = MPI::Group().Include({ left, right });
		cout << MPI::CommRank() << " Formed group with " << left << " " << right << " level: " << level << endl;
	}


	size_t GetLevel() const { return level; }
	size_t GetLeft() const { return left; }
	size_t GetRight() const { return right; }
	const MPI::Group& GetGroup()const { return group; }


	~TreeGroup()
	{
	}

private:
	size_t level;
	int left, right;
	MPI::Group group;
};





int DivideOnGroups(vector<TreeGroup>& groups, size_t level, int start, int end)
{
	if (start == end)return start;
	if (end - start == 1)
	{
		int  proc_id = MPI::CommRank();
		//if (start == proc_id || end == proc_id)
		groups.push_back(TreeGroup(level, start, end));
	}
	else
	{
		int left = DivideOnGroups(groups, level + 1, start, (start + end) / 2);
		int right = DivideOnGroups(groups, level + 1, (start + end) / 2 + 1, end);
		int  proc_id = MPI::CommRank();
		//if (left == proc_id || right == proc_id)
		groups.push_back(TreeGroup(level, left, right));
	}
	return start;
}

const TreeGroup& GetLowestLevelOf(const vector<TreeGroup>& groups)
{
	size_t min_level = std::numeric_limits<size_t>().max();
	const TreeGroup* g = nullptr;
	for (const auto& tg : groups)
	{
		if (tg.GetLevel() < min_level && tg.GetGroup().HasRank())
		{
			min_level = tg.GetLevel();
			g = &tg;
		}
	}
	return *g;
}

const TreeGroup& GetTreeGroupOfLevel(const vector<TreeGroup>& groups, size_t level)
{
	for (const auto& tg : groups)
	{
		if (tg.GetLevel() == level && tg.GetGroup().HasRank())
		{
			return tg;
		}
	}
}

bool HasRootGroup(const vector<TreeGroup>& groups)
{
	for (const auto& tg : groups)
	{
		if (tg.GetGroup().Rank() == 0)
		{
			return true;
		}
	}
	return false;
}

bool HasLevel(const vector<TreeGroup>& groups, size_t level)
{
	for (const auto& tg : groups)
	{
		if (tg.GetLevel() == level && tg.GetGroup().HasRank())
		{
			return true;
		}
	}
	return false;
}

vector<TreeGroup> FormTreeGroups(int proc_num, int proc_id)
{
	vector<TreeGroup> res;
	DivideOnGroups(res, 0, 0, proc_num - 1);
	return res;
}



template<typename T>
void SplitVec(const vector<T>& v, vector<T>& v1, vector<T>& v2)
{
	v1 = vector(v.begin(), v.begin() + v.size() / 2);
	v2 = vector(v.begin() + v.size() / 2, v.end());
}




const size_t N = 10;

void Process(const vector<TreeGroup>& groups, vector<double>& v, size_t level, int proc_id)
{

	auto tg = GetTreeGroupOfLevel(groups, level);
	cout << proc_id << " TreeGroup of " << tg.GetLeft() << " and " << tg.GetRight() << endl;
	MPI::Comm c = tg.GetGroup().GetComm();

	if (HasRootGroup(groups))
	{
		vector<double> left;
		vector<double> right;
		SplitVec(v, left, right);

		cout << proc_id << " sending vec of len " << right.size() << endl;

		MPI::Send(right.size(), 1, 0, c);
		MPI::Send(right, 1, 0, c);

		if (HasLevel(groups, level + 1))
		{
			Process(groups, left, level + 1, proc_id);
		}
		else
		{
			cout << proc_id << " sorting left" << endl;
			//sort
			std::sort(left.begin(), left.end());

			// recv right
			MPI::Recv(right, 1, MPI_ANY_TAG, c);
			// merge
			Merge(v, left, right);
		}
	}
	else
	{
		cout << proc_id << " sorting right" << endl;
		//sort and send back
		std::sort(v.begin(), v.end());
		MPI::Send(v, 0, 0, c);
	}
}


void Calc(int proc_num, int proc_id)
{
	auto groups = FormTreeGroups(proc_num, proc_id);
	auto tg = GetLowestLevelOf(groups);

	size_t level = tg.GetLevel();
	cout << "Level of " << proc_id << " is " << level << endl;

	if (proc_id == 0)
	{
		vector<double> arr(N);
		FillRandom(arr, 0, 10);
		cout << "root starting" << endl;
		Process(groups, arr, level, proc_id);
		cout << "root: " << CheckSort(arr) << endl;
	}
	else
		//receive what root sent
	{
		cout << proc_id << " TreeGroup of " << tg.GetLeft() << " and " << tg.GetRight() << " rank " << tg.GetGroup().Rank() << endl;
		MPI::Comm c = tg.GetGroup().GetComm();
		size_t len = MPI::Recv<size_t>(MPI_ANY_SOURCE, MPI_ANY_TAG, c);
		cout << proc_id << " recv len " << len << endl;
		vector<double> v(len);
		MPI::Recv(v, 0, 0, c);
		if (HasLevel(groups, level + 1))
		{
			Process(groups, v, level + 1, proc_id);
		}
		else
		{
			cout << proc_id << " sorting right" << endl;
			//sort and send back
			std::sort(v.begin(), v.end());
			MPI::Send(v, 0, 0, c);
		}
	}



}


int main(int argc, char** argv)
{
	int proc_num, proc_id;

	MPI::Init(argc, argv);

	proc_num = MPI::CommSize();
	proc_id = MPI::CommRank();


	Calc(proc_num, proc_id);




	return 0;
}









/*proc_num = MPI::CommSize();
	proc_id = MPI::CommRank();

	int dims[]{ 1 };
	MPI::CreateCart<1>(MPI::COMM_WORLD, dims);*/


	/*vector<double> a = { 1,3,5,2,4,8 };
	Merge(a, 3);
	cout << a;*/