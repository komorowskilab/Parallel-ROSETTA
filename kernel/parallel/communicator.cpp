#include <kernel/parallel/communicator.h>

Communicator::Communicator(){
	
	my_rank = MPI::COMM_WORLD.Get_rank();
	proc_size = MPI::COMM_WORLD.Get_size();

};
