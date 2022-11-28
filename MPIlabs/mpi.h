#include <mpi.h>
#include <exception>
#include <string>
#include <vector>
#include "mpi_packable.h"

namespace MPI
{
	typedef ::MPI_Comm CommId;
	typedef ::MPI_Status Status;
	typedef ::MPI_Request Request;
	typedef ::MPI_Group GroupId;
	using ::std::vector;
	enum  ReturnCode :int
	{
		Success = MPI_SUCCESS,

	};

	class Exception : public ::std::exception
	{
		using ::std::exception::exception;
	};

	enum class Operation :MPI_Op
	{
		Max = MPI_MAX,
		Min = MPI_MIN,
		Sum = MPI_SUM,
		Prod = MPI_PROD,
		NoOp = MPI_NO_OP,
		Null = MPI_OP_NULL,
	};

	enum class Types : MPI_Datatype
	{
		Byte = MPI_BYTE,
		Int = MPI_INT,
		Char = MPI_CHAR,
		UnsignedChar = MPI_UNSIGNED_CHAR,
		Short = MPI_SHORT,
		UnsignedShort = MPI_UNSIGNED_SHORT,
		Unsigned = MPI_UNSIGNED,
		UnsignedInt = MPI_UNSIGNED,
		Long = MPI_LONG,
		UnsignedLong = MPI_UNSIGNED_LONG,

		/*MPI_LONG_LONG_INT,
		MPI_LONG_LONG,
		MPI_FLOAT,
		MPI_DOUBLE,
		MPI_LONG_DOUBLE,
		MPI_BYTE,
		MPI_WCHAR,*/
	};

	const CommId COMM_WORLD = MPI_COMM_WORLD;
	const CommId COMM_EMPTY = MPI_COMM_NULL;

	void _CheckSuccess(int result, CommId c = COMM_WORLD)
	{
		if (result != ReturnCode::Success)
		{
			::MPI_Abort(c, result);
			throw Exception(("Error code: " + ::std::to_string(result)).c_str());
		}
	}



	MPI_Status _dummy_status{};

	bool IsInitialized()
	{
		int i = 0;
		::MPI_Initialized(&i);
		return i != 0;
	}

	bool IsFinalized()
	{
		int i = 0;
		::MPI_Finalized(&i);
		return i != 0;
	}



	void Init(const int argc, char** argv)
	{
		if (IsInitialized())
			throw Exception("MPI is already initialized");

		_CheckSuccess(::MPI_Init(&argc, &argv), MPI_COMM_WORLD);
	}

	void Finalize()
	{

		if (!IsInitialized())
			throw Exception("MPI is not initialized yet");

		_CheckSuccess(::MPI_Finalize(), MPI_COMM_WORLD);
	}


	int CommRank(CommId c = MPI_COMM_WORLD)
	{
		int rank;
		_CheckSuccess(::MPI_Comm_rank(c, &rank), c);
		return rank;
	}


	int CommSize(CommId c = MPI_COMM_WORLD)
	{
		int size;
		_CheckSuccess(::MPI_Comm_size(c, &size), c);
		return size;
	}

	template<typename T>
	void Send(const T* val, int size = 1, int dest = 0, int tag = 0, CommId c = MPI_COMM_WORLD)
	{
		_CheckSuccess(::MPI_Send(reinterpret_cast<const void*>(val), sizeof(T) * size, MPI_BYTE, dest, tag, c), c);

	}

	template<>
	void Send(const double* val, int size, int dest, int tag, CommId c)
	{
		_CheckSuccess(::MPI_Send(reinterpret_cast<const void*>(val), size, MPI_DOUBLE, dest, tag, c), c);
	}

	template<typename T>
	void Send(const T& val, int dest = 0, int tag = 0, CommId c = MPI_COMM_WORLD)
	{
		_CheckSuccess(::MPI_Send(reinterpret_cast<const void*>(&val), sizeof(T), MPI_BYTE, dest, tag, c), c);
	}

	template<>
	void Send(const double& val, int dest, int tag, CommId c)
	{
		_CheckSuccess(::MPI_Send(&val, 1, MPI_DOUBLE, dest, tag, c), c);
	}


	template<>
	void Send(const int& val, int dest, int tag, CommId c)
	{
		_CheckSuccess(::MPI_Send(&val, 1, MPI_INT, dest, tag, c), c);
	}

	template<typename T>
	void Send(const vector<T>& v, int dest = 0, int tag = 0, CommId c = MPI_COMM_WORLD)
	{
		Send(v.data(), v.size(), dest, tag, c);
	}

	template<>
	void Send(const vector<double>& v, int dest, int tag, CommId c)
	{
		Send(v.data(), v.size(), dest, tag, c);
	}


	template<typename T>
	T Recv(int source = MPI_ANY_SOURCE, int tag = MPI_ANY_TAG, CommId c = COMM_WORLD, Status& status = _dummy_status)
	{
		T val{};
		_CheckSuccess(::MPI_Recv(reinterpret_cast<void*>(&val), sizeof(T), MPI_BYTE, source, tag, c, &status), c);
		return val;
	}

	template<typename T>
	void Recv(vector<T>& v, int source = MPI_ANY_SOURCE, int tag = MPI_ANY_TAG, CommId c = COMM_WORLD, Status& status = _dummy_status)
	{
		_CheckSuccess(::MPI_Recv(reinterpret_cast<void*>(v.data()), sizeof(T) * v.size(), MPI_BYTE, source, tag, c, &status), c);
	}

	template<>
	void Recv(vector<double>& v, int source, int tag, CommId c, Status& status)
	{
		_CheckSuccess(::MPI_Recv(reinterpret_cast<void*>(v.data()), v.size(), MPI_DOUBLE, source, tag, c, &status), c);
	}



	template<typename T>
	void Bcast(T* val, int size = 1, int root = 0, CommId c = COMM_WORLD)
	{
		_CheckSuccess(::MPI_Bcast(reinterpret_cast<void*>(val), sizeof(T) * size, MPI_BYTE, root, c), c);
	}

	template<>
	void Bcast(int* val, int size, int root, CommId c)
	{
		_CheckSuccess(::MPI_Bcast(
			reinterpret_cast<void*>(val),
			size,
			MPI_INT,
			root,
			c
		), c);
	}

	template<typename T>
	void Bcast(vector<T>& v, int root = 0, CommId c = COMM_WORLD)
	{
		_CheckSuccess(::MPI_Bcast(reinterpret_cast<void*>(v.data()), sizeof(T) * v.size(), MPI_BYTE, root, c), c);
	}

	template<>
	void Bcast(vector<int>& v, int root, CommId c)
	{
		_CheckSuccess(::MPI_Bcast(reinterpret_cast<void*>(v.data()), v.size(), MPI_INT, root, c), c);
	}




	template<typename T>
	void Reduce(const T* send, T* recv, int size = 1, Operation op = Operation::Null, int root = 0, CommId c = COMM_WORLD)
	{

		_CheckSuccess(::MPI_Reduce(
			reinterpret_cast<const void*>(send),
			reinterpret_cast<void*>(recv),
			sizeof(T) * size,
			MPI_BYTE,
			static_cast<MPI_Op>(op),
			root,
			c
		), c);

	}

	template<>
	void Reduce(const int* send, int* recv, int size, Operation op, int root, CommId c)
	{
		_CheckSuccess(::MPI_Reduce(
			reinterpret_cast<const void*>(send),
			reinterpret_cast<void*>(recv),
			size,
			MPI_INT,
			static_cast<MPI_Op>(op),
			root,
			c
		), c);
	}


	template<typename T>
	void Reduce(const vector<T>& send, vector<T>& recv, Operation op = Operation::Null, int root = 0, CommId c = COMM_WORLD)
	{
		return Reduce<T>(send.data(), recv.data(), send.size(), op, root, c);
	}


	void Barrier(CommId c = COMM_WORLD)
	{
		_CheckSuccess(::MPI_Barrier(c), c);
	}

	class Buffer
	{
	public:
		Buffer(void* data, int* size);
		~Buffer();

	private:
		void* _data = nullptr;
		int* _size = nullptr;
	};

	Buffer::Buffer(void* data, int* size) :_data(data), _size(size)
	{
		::MPI_Buffer_attach(this->_data, *this->_size);
	}

	Buffer ::~Buffer()
	{
		::MPI_Buffer_detach(this->_data, this->_size);
	}





#define RECV_TYPE_WRAPPER(T, MPI_T) template<>\
	T Recv(int source, int tag, CommId c, Status& status)\
	{\
		T val;\
		_CheckSuccess(::MPI_Recv(&val, 1, (MPI_T), source, tag, c, &status),c);\
		return val;\
	}

	RECV_TYPE_WRAPPER(int, MPI_INT);
	RECV_TYPE_WRAPPER(double, MPI_DOUBLE);
	RECV_TYPE_WRAPPER(float, MPI_FLOAT);
	RECV_TYPE_WRAPPER(unsigned int, MPI_UNSIGNED);

#undef RECV_TYPE_WRAPPER







	template<typename T>
	void Scatter(const T* send, int send_count, T* recv, int recv_count, int root = 0, CommId c = COMM_WORLD)
	{
		_CheckSuccess(::MPI_Scatter(
			reinterpret_cast<const void*>(send),
			send_count * sizeof(T),
			MPI_BYTE,
			reinterpret_cast<void*>(recv),
			recv_count * sizeof(T),
			MPI_BYTE,
			root,
			c
		), c);
	}

	template<>
	void Scatter(const int* send, int send_count, int* recv, int recv_count, int root, CommId c)
	{
		_CheckSuccess(::MPI_Scatter(
			reinterpret_cast<const void*>(send),
			send_count,
			MPI_INT,
			reinterpret_cast<void*>(recv),
			recv_count,
			MPI_INT,
			root,
			c
		), c);
	}

	template<typename T>
	vector<T> Scatter(const vector<T>& send, int recv_count = 0, int root = 0, CommId c = COMM_WORLD)
	{
		if (recv_count == 0)
		{
			recv_count = send.size() / CommSize(c);
		}
		vector<T> res(recv_count, T{});
		Scatter<T>(send.data(), send.size() / CommSize(c), res.data(), recv_count, root, c);
		return res;
	}



	template<typename T>
	void Gather(const T* send, int send_count, T* recv, int recv_count, int root = 0, CommId c = COMM_WORLD)
	{
		_CheckSuccess(::MPI_Gather(
			reinterpret_cast<const void*>(send),
			send_count * sizeof(T),
			MPI_BYTE,
			reinterpret_cast<void*>(recv),
			recv_count * sizeof(T),
			MPI_BYTE,
			root,
			c
		), c);
	}

	template<>
	void Gather(const int* send, int send_count, int* recv, int recv_count, int root, CommId c)
	{
		_CheckSuccess(::MPI_Gather(
			reinterpret_cast<const void*>(send),
			send_count,
			MPI_INT,
			reinterpret_cast<void*>(recv),
			recv_count,
			MPI_INT,
			root,
			c
		), c);
	}


	template<typename T>
	vector<T> Gather(const vector<T>& send, int recv_count = 0, int root = 0, CommId c = COMM_WORLD)
	{
		if (recv_count == 0)
		{
			recv_count = send.size() * CommSize(c);
		}
		vector<T> res(recv_count, T{});
		Gather<T>(send.data(), send.size(), res.data(), recv_count / CommSize(c), root, c);
		return res;
	}


	template<typename T>
	void Pack(const T& data, void* dest, int dest_size, int* pos, CommId c = COMM_WORLD)
	{
		_CheckSuccess(::MPI_Pack(
			reinterpret_cast<const void*>(&data),
			sizeof(T),
			MPI_BYTE,
			dest,
			dest_size,
			pos,
			c
		), c);
	}


	template<typename T>
	T UnPack(const void* data, int size, int* pos, CommId c = COMM_WORLD)
	{
		T res{};
		_CheckSuccess(::MPI_Unpack(
			data,
			size,
			pos,
			&res,
			sizeof(T),
			MPI_BYTE,
			c
		), c);
	}


	//Immediate ops

	namespace Immediate
	{
		typedef ::MPI_Request Request;


		template<typename T>
		Request Recv(T* buff, int size, int source = MPI_ANY_SOURCE, int tag = MPI_ANY_TAG, CommId c = COMM_WORLD)
		{
			Request req;
			_CheckSuccess(::MPI_Irecv(reinterpret_cast<void*>(buff), sizeof(T) * size, MPI_BYTE, source, tag, c, &req), c);
			return req;
		}

		template<>
		Request Recv(int* buff, int size, int source, int tag, CommId c)
		{
			Request req;
			_CheckSuccess(::MPI_Irecv(reinterpret_cast<void*>(buff), size, MPI_INT, source, tag, c, &req), c);
			return req;
		}

		template<>
		Request Recv(double* buff, int size, int source, int tag, CommId c)
		{
			Request req;
			_CheckSuccess(::MPI_Irecv(reinterpret_cast<void*>(buff), size, MPI_DOUBLE, source, tag, c, &req), c);
			return req;
		}

		template<typename T>
		Request Recv(vector<T>& v, int source = MPI_ANY_SOURCE, int tag = MPI_ANY_TAG, CommId c = COMM_WORLD)
		{
			return Recv<T>(v.data(), v.size(), source, tag, c);
		}




		bool Test(Request& req, Status& status = _dummy_status)
		{
			int flag;
			_CheckSuccess(::MPI_Test(&req, &flag, &status), COMM_WORLD);
			return flag != 0;
		}

	}


	GroupId CommGroup(CommId c = COMM_WORLD)
	{
		GroupId g;
		_CheckSuccess(::MPI_Comm_group(c, &g), c);
		return g;
	}

	int GroupSize(GroupId group)
	{
		int size;
		_CheckSuccess(::MPI_Group_size(group, &size));
		return size;
	}

	void FreeGroup(GroupId& g)
	{
		_CheckSuccess(::MPI_Group_free(&g));
	}

	int GroupRank(GroupId g)
	{
		int rank;
		_CheckSuccess(::MPI_Group_rank(g, &rank));
		return rank;
	}

	const class Group
	{
	public:
		using Ranks = std::vector<int>;

		Group(CommId c, GroupId id)
		{
			_comm = c;
			_id = id;
		}

		Group(CommId c = COMM_WORLD)
		{
			_comm = c;
			_id = CommGroup(c);
		}


		~Group()
		{
			if (_comm != COMM_WORLD)
				FreeGroup(_id);
		}


		int Size()
		{
			return GroupSize(_id);
		}

		int Rank()
		{
			return GroupRank(_id);
		}

		Group Include(const Ranks& r)
		{
			GroupId g;
			_CheckSuccess(::MPI_Group_incl(_id, r.size(), r.data(), &g));
			return Group(COMM_EMPTY, g);;
		}

		Group Exclude(const Ranks& r)
		{
			GroupId g;
			_CheckSuccess(::MPI_Group_excl(_id, r.size(), r.data(), &g));
			return Group(COMM_EMPTY, g);;
		}

		Group Union(const Group& other)
		{
			GroupId g;
			_CheckSuccess(::MPI_Group_union(_id, other._id, &g));
			return Group(COMM_EMPTY, g);;
		}

		Group Intersection(const Group& other)
		{
			GroupId g;
			_CheckSuccess(::MPI_Group_intersection(_id, other._id, &g));
			return Group(COMM_EMPTY, g);;
		}

		Group Difference(const Group& other)
		{
			GroupId g;
			_CheckSuccess(::MPI_Group_difference(_id, other._id, &g));
			return Group(COMM_EMPTY, g);;
		}




	private:
		GroupId _id;
		CommId _comm;

	} global_group;





	class _Dummy
	{
	public:
		_Dummy() {}
		~_Dummy() { if (!IsFinalized()) Finalize(); }

	} _dummy;
}

