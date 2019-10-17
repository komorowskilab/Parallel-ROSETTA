// SampleVector.h: interface for the SampleVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAMPLEVECTOR_H__773ED941_40A4_11D2_AF97_000000000000__INCLUDED_)
#define AFX_SAMPLEVECTOR_H__773ED941_40A4_11D2_AF97_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

template<class T>
int sampleVector(AbstractSampler &s,
							 int elements,
							 Vector(double) &cdist,
							 Vector(T) &in,
							 Vector(T) &out,
							 Vector(T) &rest)
{
		Vector(int) indices;
		indices.reserve(elements);
		if(s.sample(cdist, indices, elements) != elements)
			return 0;
		// std::vector<int> sampled(cdist.size(), 0); // Expanded to loop 980901 by Aleksander šhrn.
		Vector(int) sampled;
		sampled.reserve(cdist.size());
		for (int dummy = 0; dummy < cdist.size(); dummy++)
			sampled.push_back(0);
		unsigned int i;
		for(i = 0; i < indices.size(); i++)
				sampled[indices[i]] ++;
		for(i = 0; i < sampled.size(); i++){
			if(sampled[i] != 0)
				out.push_back(in[i]);
			else
				rest.push_back(in[i]);
		}
		return out.size();
}


#endif // !defined(AFX_SAMPLEVECTOR_H__773ED941_40A4_11D2_AF97_000000000000__INCLUDED_)
 
