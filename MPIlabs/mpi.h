#include <mpi.h>
#include <exception>
#include <string>
namespace MPI
{

	typedef ::MPI_Comm Comm;
	typedef ::MPI_Status Status;
	enum  ReturnCode :int
	{
		Success = MPI_SUCCESS,

	};

	class Exception : public ::std::exception
	{
		using ::std::exception::exception;
	};

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

		auto r = ::MPI_Init(&argc, &argv);
		if (r != ReturnCode::Success)
		{
			::MPI_Abort(MPI_COMM_WORLD, r);
			throw Exception(("Error code: " + ::std::to_string(r)).c_str());
		}
	}

	void Finalize()
	{

		if (!IsInitialized())
			throw Exception("MPI is not initialized yet");

		auto r = ::MPI_Finalize();
		if (r != ReturnCode::Success)
		{
			::MPI_Abort(MPI_COMM_WORLD, r);
			throw Exception(("Error code: " + ::std::to_string(r)).c_str());
		}
	}


	int CommRank(Comm c = MPI_COMM_WORLD)
	{
		int rank;
		auto r = ::MPI_Comm_rank(c, &rank);
		if (r != ReturnCode::Success)
		{
			::MPI_Abort(c, r);
			throw Exception(("Error code: " + ::std::to_string(r)).c_str());
		}
		return rank;
	}


	int CommSize(Comm c = MPI_COMM_WORLD)
	{
		int size;
		auto r = ::MPI_Comm_size(c, &size);
		if (r != ReturnCode::Success)
		{
			::MPI_Abort(c, r);
			throw Exception(("Error code: " + ::std::to_string(r)).c_str());
		}
		return size;
	}

	template<typename T>
	void Send(const T& val, int dest = 0, int tag = 0, Comm c = MPI_COMM_WORLD)
	{
		auto r = ::MPI_Send(reinterpret_cast<const void*>(&val), sizeof(T), MPI_BYTE, dest, tag, c);
		if (r != ReturnCode::Success)
		{
			::MPI_Abort(c, r);
			throw Exception(("Error code: " + ::std::to_string(r)).c_str());
		}
	}

	template<>
	void Send(const int& val, int dest, int tag, Comm c)
	{
		auto r = ::MPI_Send(&val, 1, MPI_INT, dest, tag, c);
		if (r != ReturnCode::Success)
		{
			::MPI_Abort(c, r);
			throw Exception(("Error code: " + ::std::to_string(r)).c_str());
		}
	}

	template<typename T>
	T Recv(int source = MPI_ANY_SOURCE, int tag = MPI_ANY_TAG, Comm c = MPI_COMM_WORLD, MPI_Status& status = _dummy_status)
	{
		T val{};
		auto r = ::MPI_Recv(reinterpret_cast<void*>(&val), sizeof(T), MPI_BYTE, source, tag, c, &status);
		if (r != ReturnCode::Success)
		{
			::MPI_Abort(c, r);
			throw Exception(("Error code: " + ::std::to_string(r)).c_str());
		}
		return val;
	}

#define RECV_TYPE_WRAPPER(T, MPI_T) template<>\
	T Recv(int source, int tag, Comm c, MPI_Status& status)\
	{\
		T val;\
		auto r = ::MPI_Recv(&val, 1, (MPI_T), source, tag, c, &status);\
		if (r != ReturnCode::Success)\
		{\
			::MPI_Abort(c, r);\
			throw Exception(("Error code: " + ::std::to_string(r)).c_str());\
		}\
		return val;\
	}

	RECV_TYPE_WRAPPER(int, MPI_INT)
		RECV_TYPE_WRAPPER(double, MPI_DOUBLE)
		RECV_TYPE_WRAPPER(float, MPI_FLOAT)
		RECV_TYPE_WRAPPER(unsigned int, MPI_UNSIGNED)

#undef RECV_TYPE_WRAPPER


		class _Dummy
	{
	public:
		_Dummy() {}
		~_Dummy() { if (!IsFinalized()) Finalize(); }

	} _dummy;
}

