// EABitset.h: interface for the EABitset class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EABITSET_H__8A931B51_3C08_11D2_AF8B_000000000000__INCLUDED_)
#define AFX_EABITSET_H__8A931B51_3C08_11D2_AF8B_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Individual.h"
#include <kernel/basic/bits.h>
#ifdef _MSC_VER
#include <typeinfo.h>
#endif


class EABitset  : public Individual
{
	Vector(int) index; // for inversion: keep track of org. pos
	Bits bits;
	Bits nbits;
	bool renorm;

public:
	ostream &print(ostream& out);

	Individual * clone();
	int size() const;
	EABitset(const EABitset &b);
	EABitset & operator=(const EABitset & b);
	void transpose(int bita, int bitb);
	void setP(int bit, bool state);
	void set(int bit, bool state);
	bool getP(int bit) const;
	bool get(int bit) const;
	void copyInto(Individual &p) const;
	EABitset(int size);
	EABitset();
	virtual ~EABitset();
	virtual bool operator==(Individual & b){
		return *this == *(EABitset *)&b;
	}
	virtual bool operator<(Individual & b){
		return *this < *(EABitset*)&b;
	}
	Bits &getBits() { return bits; }


	friend bool operator==(EABitset &a, EABitset &b);
	friend bool operator!=(EABitset &a, EABitset &b);
	friend bool operator>(EABitset &a, EABitset &b);
	friend bool operator<(EABitset &a, EABitset &b);
	friend bool operator<=(EABitset &a, EABitset &b);
	friend bool operator>=(EABitset &a, EABitset &b);

private:
	void checkngrow(int bit);
public:
	Bits & normalize();
};

#endif // !defined(AFX_EABITSET_H__8A931B51_3C08_11D2_AF8B_000000000000__INCLUDED_)
 
