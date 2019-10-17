#include "omp.h"

//class ThreadControl {
namespace ThreadControl {

//protected:

	static unsigned int maxThreads_;
	static unsigned int currentThreads_;
	static unsigned int reservedThreads_;

//public:

	bool 		SetMaxThreads(unsigned int value);
	unsigned int 	GetMaxThreads();

	bool		AddThreadToCount();
	bool		RemoveThreadFromCount();
	int 		GetCurrentThreadCount();	
	
	bool		SetCurrentThreadCount(unsigned int value);

	bool		SetDynamicThreads(bool value);
	bool		GetDynamicThreads();

	bool		SetNestedThreads(bool value);
	bool		GetNestedThreads();

	bool		GetInParallel();

	unsigned int	GetCurrentTeamSize();
	int		SetTeamSize(int value=10000);

	int		GetSplitSize(bool reserve=true);
	int		GetAvailableThreads();
	int 		GetReservedThreads();

	int		SetThreadingEnvironment();
}
//};
