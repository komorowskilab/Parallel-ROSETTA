
/** This is not proper file to include for parallel capabilities!!!
 * Use kernel/parallel/parallel.h instead.
 */

#ifndef __MPIROS_H__
#define __MPIROS_H__

#undef SEEK_SET
#undef SEEK_CUR
#undef SEEK_END


// LL: macros hack 
#undef Set
#undef Map
#undef MPI


#include <mpi.h>

// LL: dirty!!!! 
#include <kernel/basic/macros.h>

#endif

