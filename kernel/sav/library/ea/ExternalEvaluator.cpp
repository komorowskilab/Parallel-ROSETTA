// ExternalEvaluator.cpp: implementation of the ExternalEvaluator class.
//
//////////////////////////////////////////////////////////////////////

#include "ExternalEvaluator.h"
#include <kernel/system/fstream.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ExternalEvaluator::ExternalEvaluator(char * commandline, char * iofile, Stats &s)
: cline(commandline), fbase(iofile), stats(s)
{

}

ExternalEvaluator::~ExternalEvaluator()
{

}

void ExternalEvaluator::eval(Population &p)
{
//	cerr << "Entering evaluator\n";
	// output population to file
	ofstream ofs(fbase);
	if(!ofs.is_open()){
		cerr << "ExternalEvaluator error: Could not open population file for output...\n";
		return;
	}
  //	cerr << "opened file, p size:" << p.size() << "\n";
	int i;
	for(i = 0; i < p.size(); i++){
    //    cerr << "printing element " << i << "\n";
		p[i]->print(ofs);
	}
	ofs.close();
  //	cerr << "output file\n";
	// call commandline
	int rval = system(cline);
	if(rval != 0){
		cerr << "ExternalEvaluator error: system() != 0\n";
		return;
	}
  //	cerr << "called program\n";
	// read result...
	ifstream ifs(fbase);
	if(!ifs.is_open()){
		cerr << "ExternalEvaluator error: Could not open fitness value file for input...\n";
		return;
	}
//cerr << "opened infile\n";
	for(i = 0; i < p.size(); i++){
		ifs >> p[i]->fitness;
		if(ifs.fail()){
			cerr << "ExternalEvaluator error: read failed (" << i << ")\n";
			return;
		}
		p[i]->sfitness = p[i]->fitness;
	}
	ifs.close();
//	cerr << "Calling stat.apply()\n";
	p = stats.apply(p);
//	cerr << "calling hook\n";
  hook();
//	cerr << "exiting evaluator\n";
}
 
