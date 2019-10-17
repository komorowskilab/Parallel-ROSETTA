// EABitset.cpp: implementation of the EABitset class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "EABitset.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EABitset::EABitset() : renorm(true)
{

}

EABitset::~EABitset()
{

}

EABitset::EABitset(int size) : bits(size, false), nbits(size, false), renorm(true)
{
	checkngrow(size - 1);
}

void EABitset::copyInto(Individual &b) const
{
	(*(EABitset *)&b).index = index;
	(*(EABitset *)&b).bits = bits;
	(*(EABitset *)&b).nbits = nbits;
	(*(EABitset *)&b).renorm = renorm;

	Individual::copyInto(b);
}

bool EABitset::get(int bit) const
{
	if(bit >= index.size()){
		cerr << "bit >= index.size()\n";
		return false;           // delay reallocation
	}
	return bits.GetState(index[bit]);
}

bool EABitset::getP(int bit) const
{
	if(bit >= index.size())
		return false;
	return bits.GetState(bit);
}

void EABitset::set(int bit, bool state)
{
	checkngrow(bit);
	bits.SetState(index[bit], state);
	renorm = true;
}

void EABitset::setP(int bit, bool state)
{
	checkngrow(bit);
	bits.SetState(bit, state);
	renorm = true;
}

void EABitset::checkngrow(int bit)
{
	if(index.size() < (bit + 1)){
		index.reserve(bit + 1);
		for(unsigned int i = index.size(); i <= bit; i++){
			index.push_back(i);
		}
	}
	//cerr << index.size() << " is new index.size()\n";
	// no need to do the same for bits
	renorm = true;
}

void EABitset::transpose(int bita, int bitb)
{
	bool tmp = get(bitb);
	set(bitb, get(bita));
	set(bita, tmp);
	int tmpi = index[bitb];
	index[bitb] = index[bita];
	index[bita] = tmpi;
	renorm = true;
}


EABitset & EABitset::operator =(const EABitset & b)
{
	b.copyInto(*this);
	return *this;
}

EABitset::EABitset(const EABitset & b)
{
	b.copyInto(*this);
}


bool operator==(EABitset &a, EABitset &b)
{

	return a.normalize() == b.normalize();

/*
	if(&a == &b)
		return true;

	if(a.size() != b.size())
		return false;
	// Correct?? consider:
	//EABitset a, b;
	//a[1] = b[1] = true;
	//a[2] = false;
	//a == b ?

	for(int i = 0; i < a.size(); i++){
		if(a.get(i) != b.get(i))
			return false;
	}

	return true;
*/
}


bool operator!=(EABitset &a, EABitset &b)
{
	return !(a == b);
}

bool operator<(EABitset &a, EABitset &b)
{

	return a.normalize() < b.normalize();

/*
	if(&a == &b)
		return false;

	if(a.size() > b.size())
		return false;

	bool proper = false;

	for(int i = 0; i < a.size(); i++) {
		if(a.get(i) && !b.get(i))
			return false;
		if(!a.get(i) && b.get(i))
			proper = true;
	}
	return proper;
*/
}

bool operator>(EABitset &a, EABitset &b)
{
	return (b < a);
}

bool operator<=(EABitset &a, EABitset &b)
{
	return a.normalize() <= b.normalize();
/*
	if(&a == &b)
		return false;

	if(a.size() > b.size())
		return false;

	for(int i = 0; i < a.size(); i++) {
		if(a.get(i) && !b.get(i))
			return false;
	}
	return true;
*/
}

bool operator>=(EABitset &a, EABitset &b)
{
	return (b <= a);
}

int EABitset::size() const
{
	return bits.GetSize();
}

Individual * EABitset::clone()
{
	EABitset * c = new EABitset();
	copyInto(*c);
	return c;
}


ostream &EABitset::print(ostream & out)
{
	normalize();
	return out << nbits << "\n";
//	<< "[" << term1 << "," << term2 << "] (";
//	for(unsigned int i = 0; i < index.size(); i++)
//		out << index[i] << " ";
//	return out << "\n";
}

Bits & EABitset::normalize()
{
	if(!renorm)
		return nbits;
	for(int i = 0; i < size(); i++){
		nbits.SetState(i, bits.GetState(index[i]));
	}
	renorm = false;
	return nbits;
}
 
