

#include "mpi.h"
#include <iostream>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#define __PRINT__VEC__ false

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

const int m = 10;

enum class MSGType :int
{
	Response,
	Request,
	Message
};


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
struct ReqestedVector {
	MPI::Immediate::Request request;
	std::vector<T> data;
};


bool AllComplete(const vector<ReqestedVector<double>>& rvs)
{
	for (const auto& rv : rvs)
	{
		if (rv.data.size() != 0)return false;
	}
	return true;
}

void Root(int proc_num)
{
	int n;
	int p;
	cout << "Enter poly count ";
	cin >> n;
	cout << "Enter poly power ";
	cin >> p;


	vector<vector<double>> polys(n);
	for (int i = 0; i < n; i++)
		polys[i] = MakeRandomPoly(p, 0.0, 10.0);

	vector<double> res;

	vector<ReqestedVector<double>>  requests(proc_num - 1);


	while (polys.size() > 1 || !AllComplete(requests))
	{
		for (int i = 1; i < proc_num; i++)
		{
			auto& req = requests[i - 1];
			if (req.data.size() != 0)// was a request
			{
				if (MPI::Immediate::Test(req.request))//check request
				{
					cout << "ROOT: got\n\t" << req.data << endl;
					polys.push_back(req.data);
					req.data.clear();
				}
				else
				{
					continue;
				}
			}
			if (polys.size() >= 2)
			{
				cout << "ROOT: sending to " << i << endl;
				auto p1 = polys.back();
				polys.pop_back();
				auto p2 = polys.back();
				polys.pop_back();

				MPI::Send(p1.size(), i);
				MPI::Send(p2.size(), i);

				cout << "\t" << p1 << "\n\t" << p2 << endl;
				MPI::Send(p1, i);
				MPI::Send(p2, i);

				req.data.resize(p1.size() + p2.size() - 1, 0.0);

				req.request = MPI::Immediate::Recv(req.data, i);
			}
		}
	}

	for (int i = 1; i < proc_num; i++)
	{
		MPI::Send(0, i);
	}

	cout << polys.back() << "\n";
	cout << "Root complete" << endl;



}

void Branch(int id)
{
	int p1_size;
	int p2_size;


	vector<double> p1;
	vector<double> p2;
	vector<double> p3;

	while (true)
	{
		p1_size = MPI::Recv<int>();
		if (p1_size == 0)break;
		p2_size = MPI::Recv<int>();

		p1.resize(p1_size);
		p2.resize(p2_size);

		MPI::Recv(p1);
		MPI::Recv(p2);
		cout << "Branch " << id << ": recv:\n\t" << p1 << "\n\t" << p2 << endl;

		p3 = Mult(p1, p2);

		cout << "Branch " << id << " done:\n\t" << p3 << endl;
		MPI::Send(p3);
	}
	cout << "Branch " << id << " complete" << endl;
}


int main(int argc, char** argv)
{
	int proc_num, proc_id;

	MPI::Init(argc, argv);
	proc_num = MPI::CommSize();
	proc_id = MPI::CommRank();


	switch (proc_id)
	{
	case 0:
		Root(proc_num);
		break;
	default:
		Branch(proc_id);
		break;
	}

	return 0;
}



#include <iostream>
#include <tuple>


template <class... Ts>
struct Packed;

template <size_t Index, typename _Packed>
struct _PackElem;

template <>
struct Packed<>
{
	constexpr Packed() = default;
	constexpr Packed(const Packed&) {};

	constexpr Packed& operator=(const Packed&) = default;

};


template <typename T>
struct _PackVal
{
	T _val;
};


template <size_t Index, typename _Packed>
struct _PackElem<Index, const _Packed> :_PackElem<Index, _Packed>
{
	using Base = _PackElem<Index, _Packed>;
	using type = const typename Base::type;
};




template < typename T, typename ...Ts>
struct _PackElem<0, Packed< T, Ts...>>
{
	using type = T;
	using Ptype = Packed<T, Ts...>;
};

template <size_t Index, typename T, typename ...Ts>
struct _PackElem < Index, Packed<T, Ts...>> : _PackElem< Index - 1, Packed< Ts...>> {};


template <size_t Index, class... _Ts>
constexpr  const  typename _PackElem<Index, Packed<_Ts...>>::type& Get(const Packed< _Ts...>& p) noexcept {
	using Ptype = typename _PackElem<Index, Packed< _Ts...>>::Ptype;
	return static_cast<const Ptype&>(p)._data._val;
}

template <size_t Index, class... Ts>
constexpr typename _PackElem<Index, Packed<Ts...>>::type& Get(Packed< Ts...>& p) noexcept {
	using Ptype = typename _PackElem<Index, Packed< Ts...>>::Ptype;
	return static_cast<Ptype&>(p)._data._val;
}


template <typename T, typename ... Ts>
struct Packed<T, Ts ...> : public Packed<Ts...>
{
public:
	using _Base = Packed < Ts...>;




	template <size_t Index, class... _Ts>
	friend constexpr const  typename _PackElem<Index, Packed<_Ts...>>::type& Get(const Packed< _Ts...>& p) noexcept;

	template <size_t Index, class... _Ts>
	friend constexpr typename _PackElem<Index, Packed<_Ts...>>::type& Get(const Packed< _Ts...>& p)noexcept;


	_PackVal<T> _data;
};





//int main()
//{
//	Packed<int, float>a;
//
//	Get<0>(a) = 4;
//	Get<1>(a) = 0.5;
//	std::cout << Get<0>(a) << std::endl;
//	std::cout << Get<1>(a);
//}
//
