#include <kernel/utilities/threadcontrol.h>

#include <iostream>
#include "omp.h"
#include <algorithm> //For std::min
#include <cmath> //For ceil()

/*
ThreadControl::ThreadControl() {

}

ThreadControl::~ThreadControl() {

}
*/

namespace ThreadControl {

//Needs definition outside the header.
//unsigned int ThreadControl::maxThreads_;
//unsigned int ThreadControl::currentThreads_;
//unsigned int ThreadControl::reservedThreads_;

/*----------------------------------------------------------
// Name..........: ThreadControl
// Author........: Nicholas Baltzer
// Date..........: 09/09/13
// Description...: A collection of functions to supplement the current threadcontrol
//			offered by OpenMP (3.0). OpenMP functions can be called via this
//			interface, and certain runtime values are managed by the static class
//			itself, such as the number of current threads in operation, and the
//			user specified maximum value of threads. 
// Comments......:
// Revisions.....:
//==========================================================*/



/*----------------------------------------------------------
// Name..........: SetMaxThreads
// Author........: Nicholas Baltzer
// Date..........: 09/09/13
// Description...: Sets the maximum amount of threads that the program can use.
//			With dynamic threading, this is the max that may be used.
//			Without dynamic threading, this is the max that will be used (unless other limitations are set elsewhere).
// Comments......:
// Revisions.....:
//==========================================================*/

bool
//ThreadControl::SetMaxThreads(unsigned int value) {
SetMaxThreads(unsigned int value) {
	
	//omp_set_num_threads(value);
	maxThreads_ = value;
	return true;
}

/*----------------------------------------------------------
// Name..........: GetMaxThreads
// Author........: Nicholas Baltzer
// Date..........: 09/09/13
// Description...: Returns the value of maxThreads_.
// Comments......:
// Revisions.....:
//==========================================================*/

unsigned int
//ThreadControl::GetMaxThreads() {
GetMaxThreads() {
	
	return maxThreads_;

}

/*----------------------------------------------------------
// Name..........: AddThreadToCount
// Author........: Nicholas Baltzer
// Date..........: 09/09/13
// Description...: Adds the current thread (unless it's the master thread) to the global threadcounter. This should
//			be called everytime a parallel section is started.
// Comments......:
// Revisions.....:
//==========================================================*/
bool
//ThreadControl::AddThreadToCount() {
AddThreadToCount() {

	if (omp_get_thread_num() != 0) {
		#pragma omp atomic
		++currentThreads_;
//		if (reservedThreads_ > 0) {
			#pragma omp atomic
			--reservedThreads_;
//		}
	}
	return true;
}

/*----------------------------------------------------------
// Name..........: RemoveThreadFromCount
// Author........: Nicholas Baltzer
// Date..........: 09/09/13
// Description...: Removes the calling thread from the global thread counter. Should be called every time a parallel section
//			is ending. Does not count master threads.
// Comments......:
// Revisions.....:
//==========================================================*/
bool
//ThreadControl::RemoveThreadFromCount() {
RemoveThreadFromCount() {

	if (omp_get_thread_num() != 0) {
		#pragma omp atomic
		--currentThreads_;
	}
	return true;
}
/*----------------------------------------------------------
// Name..........: GetCurrentThreadCount
// Author........: Nicholas Baltzer
// Date..........: 09/09/13
// Description...: Returns the number of threads currently active across all teams (does not include master threads).
// Comments......:
// Revisions.....:
//==========================================================*/

int
//ThreadControl::GetCurrentThreadCount() {
GetCurrentThreadCount() {

	return currentThreads_ + reservedThreads_;

}
/*----------------------------------------------------------
// Name..........: SetCurrentThreadCount
// Author........: Nicholas Baltzer
// Date..........: 09/09/13
// Description...: Sets the number of current threads active. Do not use this function unless you know exactly
//			what you are doing, as it is very likely to screw up the threadcontrol system.
// Comments......:
// Revisions.....:
//==========================================================*/

bool
//ThreadControl::SetCurrentThreadCount(unsigned int value) {
SetCurrentThreadCount(unsigned int value) {

	#pragma omp critical
	{
		currentThreads_ = value;
	}
	return true;
}

/*----------------------------------------------------------
// Name..........: SetDynamicThreads
// Author........: Nicholas Baltzer
// Date..........: 09/09/13
// Description...: Interface function for omp_set_dynamic(). Sets dynamic threading mode.
// Comments......:
// Revisions.....:
//==========================================================*/
bool
//ThreadControl::SetDynamicThreads(bool value) {
SetDynamicThreads(bool value) {

	omp_set_dynamic(value);
	return true;
}
/*----------------------------------------------------------
// Name..........: GetDynamicThreads
// Author........: Nicholas Baltzer
// Date..........: 09/09/13
// Description...: Interface function for omp_get_dynamic(). Returns true if dynamic threading is on.
// Comments......:
// Revisions.....:
//==========================================================*/

bool
//ThreadControl::GetDynamicThreads() {
GetDynamicThreads() {

	return omp_get_dynamic();

}
/*----------------------------------------------------------
// Name..........: SetNestedThreads
// Author........: Nicholas Baltzer
// Date..........: 09/09/13
// Description...: Interface function for omp_set_nested(). Sets the nested mode to true or false.
// Comments......:
// Revisions.....:
//==========================================================*/

bool
//ThreadControl::SetNestedThreads(bool value) {
SetNestedThreads(bool value) {

	omp_set_nested(value);
	return true;
}
/*----------------------------------------------------------
// Name..........: GetNestedThreads
// Author........: Nicholas Baltzer
// Date..........: 09/09/13
// Description...: Interface function for omp_get_nested(). Returns true if nested threading is on.
// Comments......:
// Revisions.....:
//==========================================================*/

bool
//ThreadControl::GetNestedThreads() {
GetNestedThreads() {

	return omp_get_nested();

}
/*----------------------------------------------------------
// Name..........: GetInParallel
// Author........: Nicholas Baltzer
// Date..........: 09/09/13
// Description...: Interface function for omp_in_parallel(). Returns true if it is called from a parallel section.
// Comments......:
// Revisions.....:
//==========================================================*/

bool
//ThreadControl::GetInParallel() {
GetInParallel() {

	return omp_in_parallel();

}

/*----------------------------------------------------------
// Name..........: GetCurrentTeamSize
// Author........: Nicholas Baltzer
// Date..........: 09/09/13
// Description...: Interface function for omp_get_num_threads(). Returns the number of threads in the calling thread's team.
// Comments......:
// Revisions.....:
//==========================================================*/
unsigned int
//ThreadControl::GetCurrentTeamSize() {
GetCurrentTeamSize() {
	
	return omp_get_num_threads();

}
/*----------------------------------------------------------
// Name..........: SetTeamSize
// Author........: Nicholas Baltzer
// Date..........: 09/09/13
// Description...: Calls omp_set_num_threads() with the highest number of threads available.
// Comments......:
// Revisions.....:
//==========================================================*/

int
//ThreadControl::SetTeamSize(int value) {
SetTeamSize(int value) {
	#pragma omp critical(SetTeamSize)
	{
		omp_set_num_threads(std::max(std::min(value, GetAvailableThreads()), 1));
	}
	return std::max(std::min(value, GetAvailableThreads()), 1);
}
/*----------------------------------------------------------
// Name..........: GetSplitSize
// Author........: Nicholas Baltzer
// Date..........: 09/09/13
// Description...: Returns the number of threads available to a master thread for the next parallel section
//			when nesting is on each master thread should have an even amount of worker threads assigned
//			to it. Parameter reserve (defaults to true) reserves the assigned threads until they have been added to
//			the pool. With reserve, Setting the teamsize is automatic.
// Comments......:
// Revisions.....: 16/12/13 -N Bug fixes. Threads should no longer decrease in-process.
//==========================================================*/

int
//ThreadControl::GetSplitSize(bool reserve) {
GetSplitSize(bool reserve) {

	int teamsize;	
	#pragma omp critical(GetSplitSize)
	{
		float threads = GetAvailableThreads();
		teamsize = (int) (ceil(threads / GetCurrentTeamSize()));

		if (reserve) {
			SetTeamSize(teamsize); 
			reservedThreads_ += (teamsize - 1);
		}
	}
//	return std::max(std::min(teamsize, GetAvailableThreads()), 0);
	return teamsize;

}

/*----------------------------------------------------------
// Name..........: GetReservedThreads
// Author........: Nicholas Baltzer
// Date..........: 13/09/13
// Description...: Returns the number of reserved threads.
// Comments......:
// Revisions.....:
//==========================================================*/
int
//ThreadControl::GetReservedThreads() {
GetReservedThreads() {
	return reservedThreads_;
}

/*----------------------------------------------------------
// Name..........: GetAvailableThreads
// Author........: Nicholas Baltzer
// Date..........: 1/09/13
// Description...: Returns the number of free threads as the difference between
//			maxThreads_ and currentThreads_.
// Comments......:
// Revisions.....:
//==========================================================*/
int
//ThreadControl::GetAvailableThreads() {
GetAvailableThreads() {

	return GetMaxThreads() - GetCurrentThreadCount();
}

/*----------------------------------------------------------
// Name..........: SetThreadingEnvironment
// Author........: Nicholas Baltzer
// Date..........: 16/09/13
// Description...: Automates the standard threading parameters dependent on available threads
//			and environment variables.
// Comments......:
// Revisions.....:
//==========================================================*/
int
//ThreadControl::SetThreadingEnvironment() {
SetThreadingEnvironment() {

	if (!GetInParallel) 
		return SetTeamSize(GetAvailableThreads());
	else if (GetNestedThreads())
		return SetTeamSize(GetSplitSize());
	
	return true;
}
}
