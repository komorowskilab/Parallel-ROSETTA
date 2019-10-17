#ifndef __PARTITIONER_H__
#define __PARTITIONER_H__

#include <kernel/parallel/communicator.h>
#include <kernel/system/iostream.h>

/** Task partitioner.
 * Manages process of partitioning everything among the processors.
 *
 */
class TaskPartitioner: public Communicator {
/** gdziesz trzeba pamietac wzgledne  wspolczynniki efektywnosci algorytmow */
	private:

		

	public:
	/** ma byc private */
	void split_even(int* rangemin, int* rangemax);
	
	int ComputeRandomSeed(int procmax, int myproc);
	int DistributeRandomSeeds(int* seeds);

	/** cos co wezmie pipa i go podzieli*/
//	int SplitLearningPipe(int pipehandler, int return_splitted_pipe);
//	int SplitAlgo(int algo, int splited_algo);
//	int split_Data(int data, int splited_data);
		
//	int collect_results(int myproc, int when, int where);

};

#endif
