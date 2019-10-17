//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __BITS_H__
#define __BITS_H__

#include <copyright.h>

#include <kernel/basic/types.h>
#include <kernel/basic/string.h>
#include <kernel/basic/vector.h>

#include <kernel/system/stdlib.h>
#include <kernel/system/limits.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class.........: Bits
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: A vector of bits.
//
// Comments......: Consider introducing "memory pooling" techniques
//                 to reduce memory fragmentation.
// Revisions.....:
//===================================================================

class Bits {
public:

	//- Helper class...................................................
  class BitReference {
	private:

		//- Physical representation......................................
		Bits         *bits_;  // Bit vector that contains the bit this structure refers to.
    int           index_; // Logical index of bit.

		//- Constructors.................................................
    BitReference() {bits_ = NULL; index_ = 0;}

	public:

		//- Constructors/destructor......................................
		BitReference(Bits *bits, int index) {bits_ = bits; index_ = index;}
		~BitReference() {}

		//- Conversion operators.........................................
		operator bool() const {return bits_->GetState(index_);}

		//- Assignment operators.........................................
		bool          operator=(bool state) {bits_->SetState(index_, state); return state;}
		bool          operator=(const BitReference &in) {bool state = (bool) in; *this = state; return state;}

	};

	//- Type definitions...............................................
	typedef unsigned int Block;

	//- Platform specific constants....................................
#if defined(WIN32)
	enum {BYTES_PER_BLOCK = 4,   // sizeof(Block)
				BITS_PER_BLOCK  = 32}; // CHAR_BIT * sizeof(Block)
#else
	#if defined(_MSC_VER)        // Microsoft Visual C++.
		#pragma message("Not 32-bit Windows, incurring slight overhead in Bits class.")
	#endif
	#define BYTES_PER_BLOCK sizeof(Block)
	#define BITS_PER_BLOCK  CHAR_BIT * sizeof(Block)
#endif

private:

	//- Internal representation........................................
  Block          *blocks_;     // Array of blocks that contain bits.
	int             no_blocks_;  // Length of block buffer.
	int             no_bits_;    // Logical length of bit vector.

private:

	//- Helper methods.................................................
	int             GetBlock(int i) const {return i / BITS_PER_BLOCK;}
	int             GetOffset(int i) const {return i % BITS_PER_BLOCK;}

	bool            Resize(int no_bits, bool preserve);
	bool            UpdateLastBlock();

public:

	//- Constructors/destructor........................................
  Bits();
  Bits(int no_bits, bool state);
	Bits(const String &text, char on = '1', const String &delimiters = ", \t\n");
  Bits(const Bits &bits);
	Bits(const Vector(bool) &v);
	Bits(const Vector(int) &v, int size);
  ~Bits();

	//- Dimensional methods............................................
  int             GetSize() const {return no_bits_;}
  bool            IsEmpty() const {return (GetSize() == 0);}

	//- Access methods.................................................
	bool            GetState(int i) const {return ((blocks_[GetBlock(i)] & (((Block) 1) << GetOffset(i))) != 0);}
	bool            SetState(int i, bool state);
	//08/07/13 -N
	Block*		GetBlocks() const {return blocks_;}
	//19/19/13 -N
	int		GetNoBlocks() const {return no_blocks_;}

	//- Creation methods...............................................
	bool            Create(const String &text, char on = '1', const String &delimiters = ", \t\n");
	bool            Create(const Vector(bool) &v);

	//- Assignment operators...........................................
  Bits           &operator=(const Bits &in);
  Bits           &operator=(bool state);

	//- Bracket operators..............................................
  bool            operator[](int i) const {return GetState(i);}
  BitReference    operator[](int i) {return BitReference(this, i);}

	//- Set operators..................................................
  Bits            operator|(const Bits &x) const;
  Bits            operator&(const Bits &x) const;
  Bits            operator-(const Bits &x) const;
  Bits            operator^(const Bits &x) const;

  friend Bits     operator~(const Bits &x);

	//- Equality and subset operators..................................
 
 	bool	operator==(const Bits &x) const;
  	bool	operator!=(const Bits &x) const;
  	bool	operator< (const Bits &x) const;
  	bool	operator<=(const Bits &x) const;
  	bool	operator> (const Bits &x) const;
  	bool	operator>=(const Bits &x) const;

  	bool	operator==(bool state);
  	bool	operator!=(bool state);

	//- Stream operators...............................................
  friend ostream &operator<<(ostream &stream, const Bits &x);
  friend istream &operator>>(istream &stream, Bits &x);

	//- Operations on self.............................................
  Bits           &operator|=(const Bits &x);
  Bits           &operator&=(const Bits &x);
  Bits           &operator-=(const Bits &x);
  Bits           &operator^=(const Bits &x);

	bool            Invert();
	bool            Invert(int i);

	bool            Reverse();

	//- Status methods.................................................
	int             Count(bool state) const;

	//- Query methods .................................................
	bool            Intersects(const Bits &in) const;
	int             Compare(const Bits &in) const;

	//- Formatting.....................................................
	bool            Format(String &formatted, char on = '1', char off = '0', const String &delimiter = "") const;
	String          Format(char on = '1', char off = '0', const String &delimiter = "") const;

	//- Miscellaneous..................................................
	bool            GetVector(Vector(int) &v) const;

};

//-------------------------------------------------------------------
// Global operators.
//===================================================================

//-------------------------------------------------------------------
// Method........: operator ~
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Complement
// Comments......:
// Revisions.....:
//===================================================================

inline Bits
operator~(const Bits &x) {
	Bits z(x);
	z.Invert();
	return z;
}

#endif
 
