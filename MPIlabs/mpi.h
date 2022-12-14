#include <mpi.h>
#include <exception>
#include <string>
#include <vector>
namespace MPI
{
	typedef ::MPI_Comm Comm;
	typedef ::MPI_Status Status;
	typedef ::MPI_Request Request;
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

	const Comm WORLD = MPI_COMM_WORLD;

	void _CheckSuccess(int result, Comm c)
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


	int CommRank(Comm c = MPI_COMM_WORLD)
	{
		int rank;
		_CheckSuccess(::MPI_Comm_rank(c, &rank), c);
		return rank;
	}


	int CommSize(Comm c = MPI_COMM_WORLD)
	{
		int size;
		_CheckSuccess(::MPI_Comm_size(c, &size), c);
		return size;
	}

	template<typename T>
	void Send(const T& val, int dest = 0, int tag = 0, Comm c = MPI_COMM_WORLD)
	{
		_CheckSuccess(::MPI_Send(reinterpret_cast<const void*>(&val), sizeof(T), MPI_BYTE, dest, tag, c), c);
	}

	template<>
	void Send(const int& val, int dest, int tag, Comm c)
	{
		_CheckSuccess(::MPI_Send(&val, 1, MPI_INT, dest, tag, c), c);
	}

	template<typename T>
	T Recv(int source = MPI_ANY_SOURCE, int tag = MPI_ANY_TAG, Comm c = WORLD, Status& status = _dummy_status)
	{
		T val{};
		_CheckSuccess(::MPI_Recv(reinterpret_cast<void*>(&val), sizeof(T), MPI_BYTE, source, tag, c, &status), c);
		return val;
	}

	template<typename T>
	void Bcast(T* val, int size, int root = 0, Comm c = WORLD)
	{
		_CheckSuccess(::MPI_Bcast(reinterpret_cast<void*>(val), sizeof(T) * size, MPI_BYTE, root, c), c);
	}

	template<typename T>
	void Bcast(vector<T>& v, int root = 0, Comm c = WORLD)
	{
		_CheckSuccess(::MPI_Bcast(reinterpret_cast<void*>(v.data()), sizeof(T) * v.size(), MPI_BYTE, root, c), c);
	}

	template<>
	void Bcast(vector<int>& v, int root, Comm c)
	{
		_CheckSuccess(::MPI_Bcast(reinterpret_cast<void*>(v.data()), v.size(), MPI_INT, root, c), c);
	}


	template<>
	void Bcast(int* val, int size, int root, Comm c)
	{
		_CheckSuccess(::MPI_Bcast(reinterpret_cast<void*>(val), size, MPI_INT, root, c), c);
	}

	template<typename T>
	void Reduce(const T* send, T* recv, int size = 1, Operation op = Operation::Null, int root = 0, Comm c = WORLD)
	{

		_CheckSuccess(::MPI_Reduce(reinterpret_cast<const void*>(send), reinterpret_cast<void*>(recv), sizeof(T) * size, MPI_BYTE, static_cast<MPI_Op>(op), root, c), c);

	}

	template<>
	void Reduce(const int* send, int* recv, int size, Operation op, int root, Comm c)
	{
		_CheckSuccess(::MPI_Reduce(reinterpret_cast<const void*>(send), reinterpret_cast<void*>(recv), size, MPI_INT, static_cast<MPI_Op>(op), root, c), c);
	}


	template<typename T>
	void Reduce(const vector<T>& send, vector<T>& recv, Operation op = Operation::Null, int root = 0, Comm c = WORLD)
	{
		return Reduce<T>(send.data(), recv.data(), send.size(), op, root, c);
	}


	void Barrier(Comm c = WORLD)
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
	T Recv(int source, int tag, Comm c, Status& status)\
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
	void Scatter(const T* send, int send_count, T* recv, int recv_count, int root = 0, Comm c = WORLD)
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
	void Scatter(const int* send, int send_count, int* recv, int recv_count, int root, Comm c)
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
	vector<T> Scatter(const vector<T>& send, int recv_count = 0, int root = 0, Comm c = WORLD)
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
	void Gather(const T* send, int send_count, T* recv, int recv_count, int root = 0, Comm c = WORLD)
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
	void Gather(const int* send, int send_count, int* recv, int recv_count, int root, Comm c)
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
	vector<T> Gather(const vector<T>& send, int recv_count = 0, int root = 0, Comm c = WORLD)
	{
		if (recv_count == 0)
		{
			recv_count = send.size() * CommSize(c);
		}
		vector<T> res(recv_count, T{});
		Gather<T>(send.data(), send.size(), res.data(), recv_count / CommSize(c), root, c);
		return res;
	}



	class _Dummy
	{
	public:
		_Dummy() {}
		~_Dummy() { if (!IsFinalized()) Finalize(); }

	} _dummy;
}

