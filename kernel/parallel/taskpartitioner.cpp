#include <kernel/parallel/taskpartitioner.h>

using namespace std;


/** Not ready yet.
 */
void TaskPartitioner::split_even(int* rangemin, int* rangemax){

	int howmany = *rangemax - *rangemin + 1;
	int min = *rangemin;
	*rangemin = 0;
	*rangemax = 0;

	if (howmany >= proc_size){

		int base = howmany/proc_size;
		int remainder = howmany % proc_size;

		if (my_rank < remainder){

			base +=1;
			remainder = 0;

		} else *rangemin += remainder;

		*rangemin += my_rank*base + min;
		*rangemax += *rangemin+base;

	} else {
// temporary message
		cout << "Too many processors." << endl;
		return ;
	};

};

int TaskPartitioner::DistributeRandomSeeds(int* seeds){


};

int TaskPartitioner::ComputeRandomSeed(int procmax, int myproc){


};

