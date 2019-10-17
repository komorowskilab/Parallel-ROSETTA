/*  This header file contains several member functions which returns
values of some special functions, at certain argument values.  These
functions are used in Test.h, particularly for the classes 
SvarMeanTest, DvarMeanTest, and VarTest.  These functions are also used
in Generate.h, particularly for the member function poisson.
*/    

#ifndef __STATKIT_H__
#define __STATKIT_H__
#include <kernel/basic/message.h>
#include <kernel/system/math.h>
#include <kernel/system/iostream.h>
#include <kernel/genomics/basic/matrix.h>
#include <kernel/utilities/creator.h>
#include <kernel/structures/decisiontable.h>

#include <kernel/structures/dictionary.h>
#include <kernel/structures/attribute.h>

#include <kernel/structures/stringattribute.h>
#include <kernel/structures/integerattribute.h>
#include <kernel/structures/floatattribute.h>


class StatKit {
public:
	
	StatKit();
	~StatKit();
	float Gammln(float xx);			// Natural Log of gamma function.
	float Betai(float a,float b,float x);	// Incomplete Beta Function.
	float Sqr(float x);			// Square
	void tqli(Vector (float) &d, Vector (float) &e, int n, Matrix <float> &z);
	void tred2(Matrix <float> &a, int n, Vector (float) &d, Vector (float) &e);
	void CoVarianceMatrix(DecisionTable &hej, Matrix <float> &x);
	float CoVariance(Vector(float) &x, Vector(float) &y);
	
	bool Matrix2DecisionTable(Matrix <float>  &theMatrix, int size , DecisionTable &pcaTable);
	
private:
	int j;
	double x,y,tmp,ser;
	double* cof; 
	float bt;
	int MAXIT;
	double EPS;
	double FPMIN;
	void error(const char ErrorText[]);
	int m,m2;
	float aa,c,d,h,qab,qam,qap;
	float Betacf(float a,float b,float x);   	// Function needed only for Betai.
	float pythag(float a, float b);
	
};


#endif




 
