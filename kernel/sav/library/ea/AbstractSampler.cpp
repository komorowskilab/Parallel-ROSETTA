/* -*-C++-*-
********************************************************************************
*
* File:         AbstractSampler.cpp
* RCS:          $Header: /cvsroot/rosetta/rosetta/src/kernel/sav/library/ea/AbstractSampler.cpp,v 1.1.1.1 2000/11/01 21:33:51 aleksander Exp $
* Description:  
* Author:       Staal Vinterbo
* Created:      Tue Aug 18 15:58:52 1998
* Modified:     Thu Aug 20 13:35:13 1998 (Staal Vinterbo) staalv@or.idi.ntnu.no
*               Thu Sep 9  15:37    2004 (Robin Andersson) robin.andersson@lcb.uu.se
*                                        sample modified to correct a bug
* Language:     C++
* Package:      N/A
* Status:       Experimental (Do Not Distribute)
*
* (c) Copyright 1998, Staal Vinterbo, all rights reserved.
*
********************************************************************************
*/

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "macros.h"
#include "AbstractSampler.h"
#include "SampleVector.h"

int AbstractSampler::cumdistToDist(Vector(double) &cumdist,
                                     Vector(double) &dist)
{
	dist.erase(dist.begin(), dist.end());
  dist.reserve(cumdist.size());

  double last = 0.0;
  for (unsigned int i = 0; i < cumdist.size(); i++) {
    dist.push_back(cumdist[i] - last);
    last = cumdist[i];
  }

  return dist.size();
}

int AbstractSampler::distToCumdist(Vector(double) &dist,
                                     Vector(double) &cumdist)
{
	cumdist.erase(cumdist.begin(), cumdist.end());
  cumdist.reserve(dist.size());

  double sum = 0.0;
	unsigned int i;
  for (i = 0; i < dist.size(); i++) {
    cumdist.push_back(dist[i]);
    sum += cumdist[i];
  }
  double nsum = 0.0;
  for (i = 0; i < cumdist.size(); i++) {
    cumdist[i] /= sum;
    double tmp = cumdist[i];
    cumdist[i] += nsum;
    nsum += tmp;
  }
  
  return cumdist.size();
}

int AbstractSampler::sample(Vector(double) &cumdist,
                                       Vector(int) &indices,
                                       int elements)
{
	if(elements == 0)
		return 0;
	double dx = 1.0/((double)elements);	
  double smark = Rand::d(); // spin wheel

  int cm = (int)floor(smark/dx);
  double rest = smark - cm * dx;  // lowest marker

	//cerr << "Sampler: dx " << dx << " smark " << smark << " cm " << cm << " rest " << rest << " cumdist.size() " << cumdist.size() << "\n";

  int j = 0;    // count elements sampled so far
  unsigned int i = 0;    // index into cumulative distribution
  double cmark; // current marker
    
  while(j < elements){
    cmark = rest + cm * dx;
		//cerr << "element " << j << " cmark " << cmark << "\n";
		if(cmark > 1.0 || i >= cumdist.size()){ // wrap around wheel
			//cerr << "wrapping round\n";
      i = 0;
      cm = 0;
      cmark = rest;
    }

    while(cumdist[i] < cmark){ // find element
			//cerr << "cumdist[" << i << "] " << cumdist[i] << " < " << cmark << "\n";
      i++;

	  // RA 040909: 
	  // Added if-clause to correct a BUG that occurs if i gets out of cumdist's range
	  if (i == cumdist.size()) {
		  i = 0;
		  cm = 0;
		  cmark = rest;
	  }
		  //if(i == cumDist.size())
			//SDebug.p(i + " " + cumDist[i - 1] + " " + cmark);
    }
    //cerr << "picked cumdist[" << i << "] : " << cumdist[i] << "\n";
    indices.push_back(i);
    j++;     // increment sampled element number
    cm++;    // increment marker number
  }

  //if(sample.size() != elements)
  //SDebug.p("size error: " + sample.size() + " != " + elements);
    
   return indices.size();
}

int AbstractSampler::sampleNR(Vector(double) &cumdist,
                                       Vector(int) &indices,
                                       int elements)
{
	if(elements <= 0){
		cerr << "Sampler error: sample size: " << elements << "\n";
		return 0;
	}

  if (cumdist.size() < (unsigned)elements)
      return 0;

  Vector(int) avail;
  avail.reserve(cumdist.size());
	unsigned int i;
  for (i = 0; i < cumdist.size(); i++)
    avail.push_back(i);


  // recreate distribution
  Vector(double) dist;
  cumdistToDist(cumdist,dist);
 
  int sampled = 0;
  Vector(double) cdist = cumdist;

  while(sampled < elements){

    Vector(int) newavail;
		sampled = sampleVector(*this, elements - sampled, cdist, avail, indices, newavail);
    avail = newavail;

    Vector(double) newdist;
    newdist.reserve(avail.size());
    for(i = 0; i < avail.size(); i++){
      newdist.push_back(dist[avail[i]]);
    }
    Vector(double) newcdist;
    distToCumdist(newdist, newcdist);
    cdist = newcdist;
  }

  return indices.size();
}

    
      
      

    
  
  
  
  

 
