//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>
#include <kernel/utilities/mathkit.h>

#include <kernel/genomics/utilities/statkit.h>
#include <kernel/genomics/utilities/nrutil.h>

StatKit::StatKit() {
	
	cof=new double [6];
	cof[0]=76.1800917294714;
	cof[1]=-86.50532032941677;
	cof[2]=24.01409824083091;
	cof[3]=-1.231739572450155;
	cof[4]=0.1208650973866179e-2;
	cof[5]=-0.5395239384953e-5;
	MAXIT=100;
	EPS=3.0e-7;
	FPMIN=1.0e-30;     
}     

StatKit::~StatKit() {delete [] cof;}

// Note this code for calculating these functions are in "Numerical Recipes in C
// The Art of Scientific Computing Second Edition"

float StatKit::Gammln(float xx) {    	// In "Numerical Recipes" Ch. 6-1, p. 214
	
	y=x=xx;
	tmp=x + 5.5;
	tmp -= (x+.5)*log(tmp);
	ser=1.000000000190015;
	for (j=0;j<=5;j++) ser += cof[j]/++y;
	
	return -tmp + log(2.5066282746210005*ser/x);
}     

float StatKit::Betai(float a,float b,float x) {    // In "Numerical Recipes" Ch. 6-4, p. 227
	
	if(x < 0.0 || x > 1.0) error("Bad x in routine betai");
	
	if(x == 0.0 || x == 1.0) {bt=0.0;}
	
	else {bt=exp(Gammln(a+b) - Gammln(a) - Gammln(b)
		+ a*log(x) + b*log(1.0 - x));}
	
	if(x < (a+1.0)/(a+b+2.0)) {return bt*Betacf(a,b,x)/a;}
	
	else {return 1.0 - bt*Betacf(b,a,1.0 - x)/b;}
}

float StatKit::Sqr(float x) {return x*x;}

float StatKit::Betacf(float a,float b,float x) {   // In "Numerical Recipes" Ch. 6-4, p. 227 - 228
	// It is used for the calculation of the
	qab=a+b;				    // above function Betai only.
	qap=a+1.0;
	qam=a-1.0;
	c=1.0;
	d=1.0-qab*x/qap;
	
	if(fabs(d) < FPMIN) d=FPMIN;
	
	d=1.0/d;
	h=d;
	for (m=1;m<=MAXIT;m++) {
		
		m2=2*m;
		aa=m*(b-m)*x/((qam+m2)*(a+m2));          
		d=1.0 + aa*d;
		if (fabs(d) < FPMIN) d=FPMIN;
		c=1.0 + aa/c;
		if (fabs(c) < FPMIN) c=FPMIN;
		d=1.0/d;
		h *= d*c;
		aa = -(a+m)*(qab+m)*x/((a+m2)*(qap+m2));
		d=1.0+aa*d;
		if (fabs(d) < FPMIN) d=FPMIN;
		c=1.0 + aa/c;
		if (fabs(c) < FPMIN) c=FPMIN;
		d=1.0/d;
		h *= d*c;
		if (fabs(d*c - 1.0) < EPS) break;
	}
	
	if (m > MAXIT) error("a or b too big, or MAXIT too small in betacf");
	
	return h;
} 

void StatKit::error(const char ErrorText[]) {
	
	cerr<<endl<<ErrorText<<endl;
	cout.flush();
	abort();
} 

float StatKit:: CoVariance(Vector(float) &x, Vector(float) &y)
{
	int i, j;
	float sum=0.0f;
	
	int size= x.size();
	
	float mean_x= MathKit::Mean(x);
	float mean_y= MathKit::Mean(y);
	
	
	for (i=0;i<size ;i++)
	{
		//sum = sum + ((x.at(i)-mean_x) *  (y.at(i)-mean_y));	
		sum = sum + ((x[i]-mean_x) *  (y[i]-mean_y));	
	}
	
	
	
	if (size==0)
	{
		return 0.0f;
	}
	else 
	{
		return (sum/(1.0f*size));
	}
	
}


void StatKit::CoVarianceMatrix(DecisionTable &hej, Matrix <float> &x)
{
	Message message;
	bool masked=true;
	int i,j;
	int n = hej.GetNoAttributes(masked)-1;
	Vector(float) entries1;
	Vector(float) entries2;
	
	
	
	for (i=0;i<n;i++)
	{
		
		for(j=0;j<n;j++)
		{	
			hej.GetEntries(entries1, entries2, i, j, masked, true, true);
			
			x[i][j] = CoVariance(entries1,entries2);
			
			//	message.Warning("x[" + String::Format(i) +"][" + String::Format(j) + "] :" + String::Format(x[i][j]));
			
		}
		
		if (!message.Progress("PCA: Calculating co-variance matrix...", i, n))
		{
			return;
		}
		
	}
}
//#include <math.h>
// Householder reduction of a real, symmetric matrix a[1..n][1..n]. On output, a is replaced
// by the orthogonal matrix Q effecting the transformation. d[1..n] returns the diagonal elements
// of the tridiagonal matrix, and e[1..n] the off-diagonal elements, with e[1]=0. Several
// statements, as noted in comments, can be omitted if only eigenvalues are to be found, in which
// case a contains no useful information on output. Otherwise they are to be included.

void StatKit:: tred2(Matrix <float> &a, int n, Vector (float) &d, Vector (float) &e)
{
	Message message;
	int l,k,j,i;
	float scale,hh,h,g,f;
	
	for (i=n-1;i>=1;i--) {
		
		if (!message.Progress("PCA: Calculating household matrix (part 1)...", n-i, n)) 
		{
			return;
		}
		
		l=i-1;
		h=scale=0.0f;
		
		if (l > 0) {
			for (k=0;k<=l;k++)
			{scale += fabs(a[i][k]);}
			if (scale == 0.0f)  // Skip transformation.
				e[i]=a[i][l];
			else {
				for (k=0;k<=l;k++) {
					a[i][k] /= scale;  // Use scaled aËs for transformation.
					h += a[i][k]*a[i][k]; // Form  in h.
				}
				f=a[i][l];
				g=(f >= 0.0f ? -sqrt(h) : sqrt(h));
				e[i]=scale*g;
				h -= f*g;  // Now h is equation (11.2.4).
				a[i][l]=f-g; // Store u in the ith row of a.
				f=0.0f;
				for (j=0;j<=l;j++) {
					// Next statement can be omitted if eigenvectors not wanted 
					a[j][i]=a[i][j]/h; //  Store u/H in ith column of a.
					// 
					g=0.0;							// Form an element of A ¹ u in g.
					for (k=0;k<=j;k++)
						g += a[j][k]*a[i][k];
					for (k=j+1;k<=l;k++)
						g += a[k][j]*a[i][k];
					e[j]=g/h;		// Form element of p in temporarily unused
					// element of e.
					
					
					
					f += e[j]*a[i][j];
				}
				hh=f/(h+h); // Form K, equation (11.2.11).
				for (j=0;j<=l;j++) { // Form q and store in e overwriting p.
					f=a[i][j];
					e[j]=g=e[j]-hh*f;
					for (k=0;k<=j;k++) // Reduce a, equation (11.2.13).
						a[j][k] -= (f*e[k]+g*a[i][k]);
				}
			}
		} else
			e[i]=a[i][l];
		d[i]=h;
	}
	// Next statement can be omitted if eigenvectors not wanted 
	d[0]=0.0f;
	e[0]=0.0f;
	// Contents of this loop can be omitted if eigenvectors not
	// wanted except for statement d[i]=a[i][i]; 
	
	for (i=0;i<=n-1;i++) { // Begin accumulation of transformation matrices.
		l=i-1;
		if (d[i]) { // This block skipped when i=1.
			for (j=0;j<=l;j++) {
				g=0.0;
				for (k=0;k<=l;k++) // Use u and u/H stored in a to form P¹Q.
					g += a[i][k]*a[k][j];
				for (k=0;k<=l;k++)
					a[k][j] -= g*a[k][i];
			}
		}
		d[i]=a[i][i]; // This statement remains.
		a[i][i]=1.0; // Reset row and column of a to identity
		// matrix for next iteration. 
		for (j=0;j<=l;j++) a[j][i]=a[i][j]=0.0;
		
		//progressbar..
		
		if (!message.Progress("PCA: Calculating household matrix (part 2)...", i, n)) 
		{
			return;
		}
		
	}
}


// #include "nrutil.h"

// QL algorithm with implicit shifts, to determine the eigenvalues and eigenvectors of a real, symmetric,
// tridiagonal matrix, or of a real, symmetric matrix previously reduced by tred2 ¦11.2. On
// input, d[1..n] contains the diagonal elements of the tridiagonal matrix. On output, it returns
// the eigenvalues. The vector e[1..n] inputs the subdiagonal elements of the tridiagonal matrix,
// with e[1] arbitrary. On output e is destroyed. When finding only the eigenvalues, several lines
// may be omitted, as noted in the comments. If the eigenvectors of a tridiagonal matrix are desired,
// the matrix z[1..n][1..n] is input as the identity matrix. If the eigenvectors of a matrix
// that has been reduced by tred2 are required, then z is input as the matrix output by tred2.
//In either case, the kth column of z returns the normalized eigenvector corresponding to d[k].

void StatKit:: tqli(Vector (float) &d,  Vector (float) &e, int n, Matrix <float> &z)
{
	
	Message message;
	//??? ef Float, not float
	// float pythag(float a, float b);
	int m,l,iter,i,k;
	float s,r,p,g,f,dd,c,b;
	for (i=1;i<=n-1;i++) {
		e[i-1]=e[i];
	} // Convenient to renumber the elements
	// of e.
	e[n-1]=0.0;
	for (l=0;l<=n-1;l++) {
		
		iter=0;
		do {
			for (m=0;m<=n-1;m++) { // Look for a single small subdiagonal
				//element to split
				// the matrix.
				dd=fabs(d[m])+fabs(d[m+1]);
				if ((float)(fabs(e[m])+dd) == dd) break;
			}
			if (m != 0) {
				// if (iter++ == 30) nrerror("Too many iterations in tqli");
				g=(d[l+1]-d[l])/(2.0*e[l]); // Form shift.
				r=pythag(g,1.0);
				g=d[m]-d[l]+e[l]/(g+SIGN(r,g));//  This is dm - ks.
				s=c=1.0;
				p=0.0;
				
				
				for (i=m-1;i>=l;i--) { //A plane rotation as in the original
					// QL, followed by Givens
					//rotations to restore tridiagonal
					//form.
					f=s*e[i]; 
					b=c*e[i];
					e[i+1]=(r=pythag(f,g));
					if (r == 0.0) {  // Recover from underflow.
						d[i+1] -= p;
						e[m]=0.0;
						break;
					}
					s=f/r;
					c=g/r;
					g=d[i+1]-p;
					r=(d[i]-g)*s+2.0*c*b;
					d[i+1]=g+(p=s*r);
					g=c*r-b;
					//  Next loop can be omitted if eigenvectors not wanted
					for (k=0;k<=n-1;k++) { // Form eigenvectors.
						f=z[k][i+1];
						z[k][i+1]=s*z[k][i]+c*f;
						z[k][i]=c*z[k][i]-s*f;
					}
				}
				if (r == 0.0 && i >= l) continue;
				d[l] -= p;
				e[l]=g;
				e[m]=0.0;
			}
		} while (m != 0);
		// progrssmeddelande
		
		if (!message.Progress("PCA: Calculating eigenvalues...", i, n)) {
			return;
		}
	}
}



float StatKit::pythag(float a, float b)
// Computes (a2 + b2)1/2 without destructive underflow or overflow.
{
	float absa,absb;
	absa=fabs(a);
	absb=fabs(b);
	if (absa > absb) return absa*sqrt(1.0+SQR(absb/absa));
	else return (absb == 0.0 ? 0.0 : absb*sqrt(1.0+SQR(absa/absb)));
}






bool StatKit::Matrix2DecisionTable(Matrix <float>  &theMatrix, int size , DecisionTable &pcaTable)
{
	Message message;
	bool masked=true;
	int i,j;
	// resiza decision täjbeln
	
	Vector(Attribute::Type) types;
	Vector(String) names;
	Vector(int) scales;
	types.reserve(size+2);
	names.reserve(size+2);
	scales.reserve(size+2);
	
	for(i=0;i<size+1;i++)
	{
		types.push_back(Attribute::TYPE_FLOAT);
		names.push_back("Eigenvector (" + String::Format(i) + ")");
		scales.push_back(5);
	}
	
	names[size] = "Eigenvalue";
	
	types.push_back(Attribute::TYPE_INTEGER);
	names.push_back("Selected");
	
	/*
	if(!(InitFeatureSelectionTable(*table, *featuretable, removes, names, types, scales))) 
	return NULL;
	*/
	int new_no_attributes = size+2;
	
	
	
	
	// Empty the input table. Let the table have one initial object.  (If, e.g., the
	// decision table is a RSES decision table, attributes can only be inserted/appended
	// if at least one object is present.)
	if (!pcaTable.Resize(1, 0, masked))
		return false;
	
	// Create an empty dictionary.
	Handle<Dictionary> dictionary = Creator::Dictionary();
	
	// Assign the empty dictionary to the table.
	if (!pcaTable.SetDictionary(dictionary.GetPointer()))
		return false;
	
	// Add attributes to the table (and dictionary).
	for (i = 0; i < new_no_attributes; i++) {
		
		// Skip non-selected columns.
		//if (removes[i])
		// continue;
		
		Handle<Attribute> attribute;
		
		switch (types[i]) {
		case Attribute::TYPE_STRING:  attribute = Creator::StringAttribute();                      break;
		case Attribute::TYPE_INTEGER: attribute = Creator::IntegerAttribute();                     break;
		case Attribute::TYPE_FLOAT:   attribute = Creator::FloatAttribute(scales[i], NULL, false); break;
		default:                      attribute = Creator::StringAttribute();                      break;
		}
		
		if (!attribute->SetName(names[i]))
		{	message.Error("2");
		return false;
		
		}
		if (!pcaTable.AppendAttribute(attribute.GetPointer(), masked))
		{	message.Error("3");
		return false;
		
		}
		
	}
	
	
	if (!pcaTable.Resize(size, size+2, masked))
	 		{	message.Error("4");
	return false;
	
	}
	
	
	
	
	
	
	
	
	
	for (i=0;i<size;i++)
	{
		for (j=0;j<size;j++)
		{
			pcaTable.SetEntry(j,i,(theMatrix[i][j])*100000,masked);
			// message.Warning("x[" + String::Format(i) +"][" + String::Format(j) + "] :" + String::Format(theMatrix[i][j]));
			// message.Warning("Tabeln [" + String::Format(i) +"][" + String::Format(j) + "] :" + (pcaTable.GetEntry(i,j,true,masked)));
			
			
		}
	}
	return true;
} 
