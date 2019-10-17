#ifndef __COMMUNICATOR_H__
#define __COMMUNICATOR_H__
/** Wrapper around desired communication library.
 * All methods should be inline so it does not affect performance and just only 
 * makes life easier. For now it uses mpi only. Methods only because it will be  
 * inherited in orderd to use it.
 */

#include <kernel/parallel/parallel.h>
#include <kernel/system/string.h>

using namespace std;

class Communicator {
	protected:
		int my_rank;
		int proc_size;
		const int root_rank = 0;

	public:
		Communicator();
		
		inline int Get_rank() { return my_rank; };
		inline int Get_size() { return proc_size; };
		static inline int Get_size_static() { return MPI::COMM_WORLD.Get_size(); };
		static inline int Get_rank_static() { return MPI::COMM_WORLD.Get_rank(); };

		static inline void Init(int argc, char *argv[]) { MPI::Init(argc, argv); };
		static inline void End() { MPI::Finalize(); };
		inline bool Root() {return my_rank == root_rank; };

		
		/** a source, b destination */
#define ile 1
#define tag 0

		inline void OneToOne(int a, int b, int* what) 
			{
				if (my_rank == a) MPI::COMM_WORLD.Send(what, ile, MPI_INT, b, tag);
				if (my_rank == b) MPI::COMM_WORLD.Recv(what, ile, MPI_INT, a, tag);
			};

		/** it it will be used more than few times it will be improved */
		static inline string OneToOne(int a, int b, string what)
			{
				if (Get_rank_static() == a) {
					
					int temp = what.length();
					MPI::COMM_WORLD.Send(&temp, 1, MPI_INT, b, 0);
					MPI::COMM_WORLD.Send(what.c_str(), what.length(), MPI_CHAR, b, 0);
					return 0;


				} else if (Get_rank_static() == b) {
					int len;
					
					MPI::COMM_WORLD.Recv(&len, 1, MPI_INT, a, 0);

					char* temp = new char[len];
					
					MPI::COMM_WORLD.Recv(temp, len, MPI_CHAR, a, 0);

					string str(temp);

					return str;

				} else {

					return 0;

				};

			};

		inline void OneToAll(int a, int* what)
			{
				MPI::COMM_WORLD.Bcast(what, ile, MPI_INT, a);
			};

		inline void AllToOne(int b, int* what, int* wynik)
			{
				MPI::COMM_WORLD.Gather(what, ile, MPI_INT, wynik , ile, MPI_INT, b);
			};

		inline void AllToAll(int* what) 
			{
			// allgather?
			// alltoall?
				MPI::COMM_WORLD.Alltoall(what, ile, MPI_INT, what, ile, MPI_INT);
			};

};

#undef ile
#undef tag


#endif
