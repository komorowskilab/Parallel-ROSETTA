//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....: This used to be a class with all static methods.
//                 I made it a namespace instead because some methods
//                 should be templatized and some not, and currently
//                 the compiler does not support member templates.
//                 A library client would use the class in an
//                 identical manner, though, by prefixing all calls
//                 with "MathKit::".
//===================================================================

#ifndef __MATHKIT_H__
#define __MATHKIT_H__

#include <copyright.h>

#include <kernel/utilities/rng.h>

#include <kernel/basic/macros.h>
#include <kernel/basic/undefined.h>
#include <kernel/basic/vector.h>
#include <kernel/basic/map.h>
#include <kernel/basic/algorithm.h>

#include <kernel/system/math.h>

//-------------------------------------------------------------------
// Begin namespace MathKit
//===================================================================

namespace MathKit {

//-------------------------------------------------------------------
// Method........: Round
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Rounds a floating point number to the nearest
//                 integer. Handles negative values.
// Comments......:
// Revisions.....: AÛ 990323: Templatized.
//===================================================================

template <class T>
inline int Round(T f) {
	return (f > 0.0) ? static_cast(int, f + 0.5) : -static_cast(int, -f + 0.5);
}

//-------------------------------------------------------------------
// Method........: Power
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Raises x to the y-th power.
// Comments......:
// Revisions.....: AÛ 990323: Templatized.
//===================================================================

template <class S, class T>
inline float Power(S x, T y) {
	return static_cast(float, pow(x, y));
}

//-------------------------------------------------------------------
// Method........: Truncate
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Clips y so that it lies in the range [x, z].
// Comments......:
// Revisions.....: AÛ 990323: Templatized.
//===================================================================

template <class S, class T, class U>
inline T Truncate(S x, T y, U z) {
	if (y < x)
		y = x;
	if (y > z)
		y = z;
	return y;
}

//-------------------------------------------------------------------
// Method........: Distribution
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Calculates and returns (in place) the histogram
//                 of the specified vector.
// Comments......:
// Revisions.....: AÛ 981208: Templatized.
//                 AÛ 990323: Use std::pair constructor instead of
//                            std::make_pair. Otherwise, the code
//                            won't compile under VC++ 6.0.
//===================================================================

template <class T>
inline bool Distribution(Map(T, int) &histogram, const Vector(T) &x) {

	int i, n = x.size();

	// Clear the input histogram.
	if (!histogram.empty())
		histogram.erase(histogram.begin(), histogram.end());

	// Build the histogram.
	for (i = 0; i < n; i++) {
		histogram.insert(Pair(const T, int)(x[i], 0));
		histogram[x[i]]++;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Sum
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Calculates and returns the sum.
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline T Sum(const Vector(T) &x) {
	return std::accumulate(x.begin(), x.end(), 0);
}

//-------------------------------------------------------------------
// Method........: Mean
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Calculates and returns the mean.
// Comments......:
// Revisions.....: 0797 AÛ: Alternative implementation to avoid
//                          overflow in sum.
//===================================================================

template <class T>
inline float Mean(const Vector(T) &x) {

	int i, n = x.size();

	if (n == 0)
		return Undefined::Float();

	typename Vector(T)::const_iterator xit = x.begin();

	double mean = 0.0;

	for (i = 0; i < n; i++, xit++)
		mean = ((*xit) + (i * mean)) / (i + 1);

	return static_cast(float, mean);

}

//-------------------------------------------------------------------
// Method........: Median
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Calculates and returns the median.
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline float Median(const Vector(T) &x) {

	if (x.empty())
		return Undefined::Float();

	// Create copy of input vector.
	Vector(T) copy(x);

	// Determine the median index.
	int position = copy.size() / 2;

	// Aquire and position an iterator.
	typename Vector(T)::iterator median = copy.begin() + position;

	// Find the median.
	std::nth_element(copy.begin(), median, copy.end());

	// Are the number of elements odd?
	if (copy.size() % 2 == 1)
		return *median;

	// Hold the found partial median.
	T m = *median;

	// Determine index of other partial median.
	median = copy.begin() + position - 1;

	// Find the other partial median.
	std::nth_element(copy.begin(), median, copy.end());

	// Return the average of the two partial medians.
	return 0.5f * (m + *median);

}

//-------------------------------------------------------------------
// Method........: Variance
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Calculates and returns the variance.
// Comments......: Consider alternative more robust implemenation.
// Revisions.....:
//===================================================================

template <class T>
inline float Variance(const Vector(T) &x) {

	int i, n = x.size();

	if (n < 2)
		return Undefined::Float();

	double s  = 0.0; // Sum.
	double s2 = 0.0; // Sum of squares.

	typename Vector(T)::const_iterator xit = x.begin();

	for (i = 0; i < n; i++, xit++) {
		s  += (*xit);
		s2 += (*xit) * (*xit);
	}

	return static_cast(float, (s2 - ((s * s) / n)) / (n - 1));

}

//-------------------------------------------------------------------
// Method........: StandardDeviation
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Calculates and returns the standard deviation.
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline float StandardDeviation(const Vector(T) &x) {

	float variance = Variance(x);

	if (variance == Undefined::Float())
		return Undefined::Float();

	return static_cast(float, sqrt(variance));

}

//-------------------------------------------------------------------
// Method........: Minimum
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns the smallest element.
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline T Minimum(const Vector(T) &x) {

	if (x.empty())
		return T(0);
	else
		return *std::min_element(x.begin(), x.end());

}

//-------------------------------------------------------------------
// Method........: Maximum
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns the largest element.
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline T Maximum(const Vector(T) &x) {

	if (x.empty())
		return T(0);
	else
		return *std::max_element(x.begin(), x.end());

}

//-------------------------------------------------------------------
// Method........: PearsonCorrelation
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Calculates and returns the cross correlation
//                 coefficient ("Pearson's r") between x and y.
// Comments......:
// Revisions.....:
//===================================================================

template <class S, class T>
inline float PearsonCorrelation(const Vector(S) &x, const Vector(T) &y) {

	// Test for quick return.
	if (&x == &y)
		return 1.0;

	// Verify dimensions.
	if (x.size() != y.size())
		return Undefined::Float();

	int i, n = x.size();

	double x_sum  = 0.0;
	double y_sum  = 0.0;
	double xx_sum = 0.0;
	double yy_sum = 0.0;
	double xy_sum = 0.0;

	typename Vector(S)::const_iterator xit = x.begin();
	typename Vector(T)::const_iterator yit = y.begin();

	// Compute statistics.
	for (i = 0; i < n; i++, xit++, yit++) {
		x_sum  += (*xit);
		y_sum  += (*yit);
		xx_sum += (*xit) * (*xit);
		yy_sum += (*yit) * (*yit);
		xy_sum += (*xit) * (*yit);
	}

	double L_xy = xy_sum - ((x_sum * y_sum) / n);
	double L_xx = xx_sum - ((x_sum * x_sum) / n);
	double L_yy = yy_sum - ((y_sum * y_sum) / n);

	if (L_xx == 0.0 || L_yy == 0.0)
		return Undefined::Float();

	// Return correlation.
	return L_xy / sqrt(L_xx * L_yy);

}

//-------------------------------------------------------------------
// Method........: KendallCorrelation
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Calculates and a non-parametric rank correlation
//                 coefficient ("Kendall's tau") between x and y.
// Comments......: See Numerical Recipes in C.
// Revisions.....:
//===================================================================

template <class S, class T>
inline float KendallCorrelation(const Vector(S) &x, const Vector(T) &y) {

	// Test for quick return.
	if (&x == &y)
		return 1.0;

	// Verify dimensions.
	if (x.size() != y.size())
		return Undefined::Float();

	S            a1;
	T            a2;
	float        aa;
	unsigned int n1 = 0;
	unsigned int n2 = 0;
	int          is = 0;

	int i, j, n = x.size();

	for (i = 0; i < n; i++) {             // Loop over first member of pair...
		for (j = i + 1; j < n; j++) {       // ...and second member.
			a1 = x[i] - x[j];
			a2 = y[i] - y[j];
			aa = a1 * a2;
			if (aa) {                         // Neither array has a tie.
				++n1;
				++n2;
				aa > 0.0 ? ++is : --is;
			}
			else {                            // One or both arrays have ties.
				if (a1)                         // An "extra x" event.
					++n1;
				if (a2)                         // An "extra y" event.
					++n2;
			}
		}
	}

	if (n1 == 0 || n2 == 0)
		return Undefined::Float();

	float tau = is / (sqrt(static_cast(double, n1)) * sqrt(static_cast(double, n2)));

	return tau;

}

//-------------------------------------------------------------------
// Method........: Mode
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Calculates and returns the most frequently occurring
//                 entry in the vector.
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline T Mode(const Vector(T) &x) {

	Map(T, int) histogram;

	// Build histogram.
	Distribution(histogram, x);

	int count = 0;
	T   mode(0);

	// Determine the mode.
	for (typename Map(T, int)::iterator it = histogram.begin(); it != histogram.end(); it++) {
		if ((*it).second > count) {
			mode  = (*it).first;
			count = (*it).second;
		}
	}

	return mode;

}

//-------------------------------------------------------------------
// Method........: LinearRegression
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Calculates the statistics for a line by using the
//                 "least squares" method to calculate a straight line
//                 that best fits the (x, y) data.
//
// Comments......: Consider also returning R^2 and F.
//
//                 R^2 can be thought of as the proportion of the
//                 variance of y that can be explained by the variable
//                 x. If R^2 = 1, then all the variation in y can be
//                 explained by the variation in x, and all the data
//                 points fall on the regression line. If R^2 = 0,
//                 then x gives no information about y, and the variance
//                 of y is the same with or without knowing x.
//
//                 Under the null hypothesis that the slope is 0, then
//                 F follows an F-distribution with 1 and n - 2
//                 degrees of freedom.
// Revisions.....:
//===================================================================

template <class S, class T>
inline bool LinearRegression(const Vector(S) &x, const Vector(T) &y, float &slope, float &intercept) {

	// Verify dimensions.
	if (x.size() != y.size())
		return false;

	int i, n = x.size();

	if (n < 3)
		return false;

	double x_sum  = 0.0;
	double y_sum  = 0.0;
	double xx_sum = 0.0;
	double xy_sum = 0.0;

	typename Vector(S)::const_iterator xit = x.begin();
	typename Vector(T)::const_iterator yit = y.begin();

	// Compute statistics.
	for (i = 0; i < n; i++, xit++, yit++) {
		x_sum  += (*xit);
		y_sum  += (*yit);
		xx_sum += (*xit) * (*xit);
		xy_sum += (*xit) * (*yit);
	}

	double L_xy = xy_sum - ((x_sum * y_sum) / n);
	double L_xx = xx_sum - ((x_sum * x_sum) / n);

	if (L_xx == 0.0)
		return false;

	slope     = L_xy / L_xx;
	intercept = (y_sum - (slope * x_sum)) / n;

	return true;

}

//-------------------------------------------------------------------
// Method........: Permute
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Reorders the contents of the vector, as specified
//                 by the permutation.
//
// Comments......: Eliminate vector copy, swap internally.
// Revisions.....:
//===================================================================

template <class T>
inline bool Permute(Vector(T) &x, const Vector(int) &permutation) {

	int i, n = x.size();

	if (n != permutation.size())
		return false;

	Vector(T) copy(x);

	for (i = 0; i < n; i++)
		x[i] = copy[permutation[i]];

	return true;

}

//-------------------------------------------------------------------
// Method........: Sample
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Creates a sample of the set of items of a
//                 specified size. Sampling can be done with or without
//                 replacement.
//
// Comments......: Sampling without replacement is currently done
//                 by creating a copy. This is potentially costly, but
//                 guarantees termination.
// Revisions.....:
//===================================================================

template <class T>
inline bool Sample(const Vector(T) &items, int size, bool replace, const RNG &rng, Vector(T) &sample) {

	if (size < 0)
		return false;

	sample.erase(sample.begin(), sample.end());
	sample.reserve(size);

	int i, j;

	if (replace) { // Sample with replacement.
		for (i = 0; i < size; i++)
			sample.push_back(items[rng.DrawInteger(0, items.size() - 1)]);
	}
	else {         // Sample without replacement.
		Vector(T) copy(items);
		for (j = 0; j < size; j++) {
			if (copy.empty())
				break;
			int index = rng.DrawInteger(0, copy.size() - 1);
			sample.push_back(copy[index]);
			copy[index] = copy[copy.size() - 1];
			copy.pop_back();
		}
	}

	return (sample.size() == static_cast(unsigned int, size));

}

//-------------------------------------------------------------------
// Method........: Sample
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Creates a sample of the index set {0, ..., n - 1} of
//                 a specified size. Sampling can be done with or without
//                 replacement.
// Comments......:
// Revisions.....:
//===================================================================

inline bool
Sample(int n, int size, bool replace, const RNG &rng, Vector(int) &sample) {

	if (n < 1 || size < 0)
		return false;

	Vector(int) items;
	items.reserve(size);

	int i;

	for (i = 0; i < n; i++)
		items.push_back(i);

	return Sample(items, size, replace, rng, sample);

}

//-------------------------------------------------------------------
// Method........: CumulativeNormal
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Computes the integral from minus infinity to x of
//                 the standard normal distribution.
//
// Comments......: http://lib.stat.cmu.edu/general/gaut.c
// Revisions.....:
//===================================================================

inline double
CumulativeNormal(double x) {

	const double SQRT2      = 1.414213562373095049;
	const double SQRTPI     = 1.772453850905516027;
  const double UPPERLIMIT = 20.0;

	const double P10 = 242.66795523053175;
	const double P11 = 21.979261618294152;
	const double P12 = 6.9963834886191355;
	const double P13 = -0.035609843701815385;
	const double Q10 = 215.05887586986120;
	const double Q11 = 91.164905404514901;
	const double Q12 = 15.082797630407787;
	const double Q13 = 1.0;

	const double P20 = 300.4592610201616005;
	const double P21 = 451.9189537118729422;
	const double P22 = 339.3208167343436870;
	const double P23 = 152.9892850469404039;
	const double P24 = 43.16222722205673530;
	const double P25 = 7.211758250883093659;
	const double P26 = 0.5641955174789739711;
	const double P27 = -0.0000001368648573827167067;
	const double Q20 = 300.4592609569832933;
	const double Q21 = 790.9509253278980272;
	const double Q22 = 931.3540948506096211;
	const double Q23 = 638.9802644656311665;
	const double Q24 = 277.5854447439876434;
	const double Q25 = 77.00015293522947295;
	const double Q26 = 12.78272731962942351;
	const double Q27 = 1.0;

	const double P30 = -0.00299610707703542174;
	const double P31 = -0.0494730910623250734;
	const double P32 = -0.226956593539686930;
	const double P33 = -0.278661308609647788;
	const double P34 = -0.0223192459734184686;
	const double Q30 = 0.0106209230528467918;
	const double Q31 = 0.191308926107829841;
	const double Q32 = 1.05167510706793207;
	const double Q33 = 1.98733201817135256;
	const double Q34 = 1.0;

  if (x < -UPPERLIMIT)
		return 0.0;

  if (x > UPPERLIMIT)
		return 1.0;

	int    sn;
  double R1, R2, y, y2, y3, y4, y5, y6, y7;
  double erf, erfc, z, z2, z3, z4;
  double phi;

  y = x / SQRT2;

  if (y < 0) {
		y  = -y;
		sn = -1;
	}
	else {
		sn = 1;
	}

	y2 = y * y;
	y4 = y2 * y2;
	y6 = y4 * y2;

	if (y < 0.46875) {
		R1  = P10 + P11 * y2 + P12 * y4 + P13 * y6;
		R2  = Q10 + Q11 * y2 + Q12 * y4 + Q13 * y6;
		erf = y * R1 / R2;
		if (sn == 1)
			phi = 0.5 + 0.5 * erf;
		else
			phi = 0.5 - 0.5 * erf;
	}
	else if (y < 4.0) {
		y3   = y2 * y;
		y5   = y4 * y;
		y7   = y6 * y;
		R1   = P20 + P21 * y + P22 * y2 + P23 * y3 + P24 * y4 + P25 * y5 + P26 * y6 + P27 * y7;
		R2   = Q20 + Q21 * y + Q22 * y2 + Q23 * y3 + Q24 * y4 + Q25 * y5 + Q26 * y6 + Q27 * y7;
		erfc = exp(-y2) * R1 / R2;
		if (sn == 1)
			phi = 1.0 - 0.5 * erfc;
		else
			phi = 0.5 * erfc;
	}
	else {
		z    = y4;
		z2   = z * z;
		z3   = z2 * z;
		z4   = z2 * z2;
		R1   = P30 + P31 * z + P32 * z2 + P33 * z3 + P34 * z4;
		R2   = Q30 + Q31 * z + Q32 * z2 + Q33 * z3 + Q34 * z4;
		erfc = (exp(-y2) / y) * (1.0 / SQRTPI + R1 / (R2 * y2));
		if (sn == 1)
			phi = 1.0 - 0.5 * erfc;
		else
			phi = 0.5 * erfc;
	}

	return phi;

}

//-------------------------------------------------------------------
// End namespace MathKit
//===================================================================

}

#endif
 
