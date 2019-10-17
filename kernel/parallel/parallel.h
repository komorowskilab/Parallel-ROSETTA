#ifndef __PARALLEL_H__
#define __PARALLEL_H__

/** Header file to hide implementation of parallel communication.
 */

#ifdef MPI
	#include <kernel/system/mpi.h>
#endif

#ifdef MPI2
	#include <kernel/system/mpi.h>
#endif

#endif
