//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....: -N 17/16/13
//			After profiling, added lots and lots of inlines to speed things
//			up.
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/basic/bits.h>
#include <kernel/basic/undefined.h>

#include <kernel/system/stdlib.h>
#include <kernel/system/memory.h>
#include <kernel/system/iostream.h>

//-N 20/06/13
#include <xmmintrin.h>
//-------------------------------------------------------------------
// Static helpers (file scope).
//===================================================================

// Used to efficiently determine the number of bits set for a specific
// byte value. This is useful for quickly counting the number of elements
// in a bit set object.

static int static_counts_[256] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
                                  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	                              1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	                              2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	                              1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	                              2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	                              2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	                              3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	                              1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	                              2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	                              2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	                              3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	                              2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	                              3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	                              3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	                              4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};

//-------------------------------------------------------------------
// Methods for class Bits.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Bits::Bits() {

  blocks_    = NULL;
	no_blocks_ = 0;
	no_bits_   = 0;

}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Bits::Bits(int no_bits, bool state) {

	blocks_    = NULL;
	no_blocks_ = 0;
	no_bits_   = 0;

	// Allocate buffer. Bits are assumed initialized as 0.
	Resize(no_bits, false);

	// Initialize contents, if needed.
	if (state)
		Invert();

}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Bits::Bits(const String &text, char on, const String &delimiters) {

	blocks_    = NULL;
	no_blocks_ = 0;
	no_bits_   = 0;

	Create(text, on, delimiters);

}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Bits::Bits(const Bits &bits) {

	no_blocks_ = bits.no_blocks_;
	no_bits_   = bits.no_bits_;

	if (bits.blocks_ != NULL) {
		blocks_ = new Block[no_blocks_];
		memcpy(blocks_, bits.blocks_, no_blocks_ * BYTES_PER_BLOCK);
	}
	else {
		blocks_ = NULL;
	}

}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Bits::Bits(const Vector(bool) &v) {

	blocks_    = NULL;
	no_blocks_ = 0;
	no_bits_   = 0;

	Create(v);

}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Bits::Bits(const Vector(int) &v, int size) {

	blocks_    = NULL;
	no_blocks_ = 0;
	no_bits_   = 0;

	// Allocate space.
	Resize(size, false);

	// Initialize buffer.
	memset(blocks_, 0, no_blocks_ * BYTES_PER_BLOCK);

	int i;

	// Set bits.
	for (i = v.size() - 1; i >= 0; i--) {
		if (v[i] >= 0 && v[i] < no_bits_)
			SetState(v[i], true);
	}

}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Bits::~Bits() {

	if (blocks_ != NULL)
		delete [] blocks_;

}

//-------------------------------------------------------------------
// Operators.
//===================================================================

//-------------------------------------------------------------------
// Method........: operator =
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Consider more optimal reallocation.
// Revisions.....: Aš 980615: Reuse current block buffer, if possible.
//===================================================================

Bits &
Bits::operator=(const Bits &in) {

	if (&in == this)
		return *this;

	// Can we reuse the current block buffer?
	if (no_blocks_ == in.no_blocks_) {
	}
	else {
		if (blocks_ != NULL)
			delete [] blocks_;
		no_blocks_ = in.no_blocks_;
		blocks_ = new Block[no_blocks_];
	}

	no_bits_ = in.no_bits_;

	// Copy block buffer.
	if (blocks_ != NULL && in.blocks_ != NULL)
		memcpy(blocks_, in.blocks_, in.no_blocks_ * BYTES_PER_BLOCK);

	return *this;

}

//-------------------------------------------------------------------
// Method........: operator =
// Author........: Aleksander šhrn
// Date..........:
// Description...: Sets all bits to the given state.
// Comments......:
// Revisions.....:
//===================================================================

Bits &
Bits::operator=(bool state) {

	if (state)
		memset(blocks_, ~((Block) 0), no_blocks_ * BYTES_PER_BLOCK);
	else
		memset(blocks_, 0, no_blocks_ * BYTES_PER_BLOCK);

	UpdateLastBlock();

	return *this;

}

//-------------------------------------------------------------------
// Method........: operator |
// Author........: Aleksander šhrn
// Date..........:
// Description...: Bitwise OR.
// Comments......:
// Revisions.....:
//===================================================================

Bits
Bits::operator|(const Bits &x) const {
	Bits z(*this);
	z |= x;
	return z;
}

//-------------------------------------------------------------------
// Method........: operator &
// Author........: Aleksander šhrn
// Date..........:
// Description...: Bitwise AND.
// Comments......:
// Revisions.....:
//===================================================================

Bits
Bits::operator&(const Bits &x) const {
	Bits z(*this);
	z &= x;
	return z;
}

//-------------------------------------------------------------------
// Method........: operator -
// Author........: Aleksander šhrn
// Date..........:
// Description...: Set difference.
// Comments......:
// Revisions.....:
//===================================================================

Bits
Bits::operator-(const Bits &x) const {
	Bits z(*this);
	z -= x;
	return z;
}

//-------------------------------------------------------------------
// Method........: operator ^
// Author........: Aleksander šhrn
// Date..........:
// Description...: Bitwise XOR.
// Comments......:
// Revisions.....:
//===================================================================

inline Bits
Bits::operator^(const Bits &x) const {
	Bits z(*this);
	z ^= x;
	return z;
}

//-------------------------------------------------------------------
// Method........: operator ==
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Assumes that unused portions of the last blocks
//                 are identical.
//
//                 Consider using memcmp instead.
//
// Revisions.....: Aš 980903: Compare complete blocks.
//                 Aš 981126: Pointer arithmetic.
//===================================================================

bool
Bits::operator==(const Bits &x) const {

	if (this == &x)
		return true;

	if (no_bits_ != x.no_bits_)
		return false;

	const Bits::Block *tptr = blocks_;
	const Bits::Block *xptr = x.blocks_;

	int i;

	// Compare complete blocks, even the last one.
	for (i = 0; i < no_blocks_; i++, tptr++, xptr++) {
		if (*tptr != *xptr)
			return false;
	}

	return true;

}







//-------------------------------------------------------------------
// Method........: operator ==
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 980903: Compare complete blocks.
//                 Aš 981126: Pointer arithmetic.
//===================================================================

bool
Bits::operator==(bool state) {

	Block goal = state ? ~((Block) 0) : ((Block) 0);

	// How many of the blocks are completely used?
	int no_complete = no_bits_ / BITS_PER_BLOCK;

	const Block *ptr = blocks_;

	int i;

	// Compare complete blocks.
	for (i = 0; i < no_complete; i++, ptr++) {
		if (*ptr != goal)
			return false;
	}

	// Compute logical index of first bit in the last, incomplete block.
	int start = no_complete * BITS_PER_BLOCK;

	// Since the last block may be incompletely used, compare relevant bits only.
	for (i = start; i < no_bits_; i++) {
		if (GetState(i) != state)
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: operator !=
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

inline bool
Bits::operator!=(const Bits &x) const {
	return !((*this) == x);
}

//-------------------------------------------------------------------
// Method........: operator !=
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

inline bool
Bits::operator!=(bool state) {
	return !((*this) == state);
}

//-------------------------------------------------------------------
// Method........: operator <
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if this is a proper subset of x.
//
// Comments......: Assumes that unused portions of the last blocks
//                 are identical.
//
//                 This bitset and x should really be of the same
//                 length.
//
// Revisions.....: Aš 980903: Compare complete blocks.
//                 Aš 981126: Pointer arithmetic.
//		   -N 20/06/13: Optimisation. This is the most
//			time-consuming operation in the program.
//===================================================================

bool
Bits::operator<(const Bits &x) const {

	if (this == &x)
		return false;

	if (no_bits_ > x.no_bits_)
		return false;

	bool equal = true;

	const Bits::Block *tptr = blocks_;
	const Bits::Block *xptr = x.blocks_;

	int i;

	// Compare complete blocks, even the last one.
	for (i = 0; i < no_blocks_; i++, tptr++, xptr++) {
		if ((*tptr & *xptr) != *tptr)
			return false;
		if (*tptr != *xptr)
			equal = false;
	}


	return !equal;

}

//-------------------------------------------------------------------
// Method........: operator <=
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if this is a subset of x.
//
// Comments......: Assumes that unused portions of the last blocks
//                 are identical.
//
//                 This bitset and x should really be of the same
//                 length.
//
// Revisions.....: Aš 980903: Compare complete blocks.
//                 Aš 981126: Pointer arithmetic.
//===================================================================

bool
Bits::operator<=(const Bits &x) const {

	if (this == &x)
		return true;

	if (no_bits_ > x.no_bits_)
		return false;

	const Bits::Block *tptr = blocks_;
	const Bits::Block *xptr = x.blocks_;

	int i;

	// Compare complete blocks, even the last one.
	for (i = 0; i < no_blocks_; i++, tptr++, xptr++) {
		if ((*tptr & *xptr) != *tptr)
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: operator >
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Bits::operator>(const Bits &x) const {
	return (x < (*this));
}




//-------------------------------------------------------------------
// Method........: operator >=
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Bits::operator>=(const Bits &x) const {
	return (x <= (*this));
}

//-------------------------------------------------------------------
// Method........: operator |=
// Author........: Aleksander šhrn
// Date..........:
// Description...: Bitwise OR.
// Comments......: Assumes that unused portions of the last blocks
//                 are identical.
//
//                 This bitset and x should really be of the same
//                 length.
//
// Revisions.....: Aš 981126: Pointer arithmetic.
//===================================================================

Bits &
Bits::operator|=(const Bits &x) {

	int no_blocks = (x.no_blocks_ < no_blocks_) ? x.no_blocks_ : no_blocks_;

	      Block *tptr = blocks_;
	const Block *xptr = x.blocks_;

	int i;

	for (i = 0; i < no_blocks; i++, tptr++, xptr++)
		*tptr |= *xptr;

	return *this;

}

//-------------------------------------------------------------------
// Method........: operator &=
// Author........: Aleksander šhrn
// Date..........:
// Description...: Bitwise AND.
// Comments......: Assumes that unused portions of the last blocks
//                 are identical.
//
//                 This bitsets and x should really be of the same
//                 length.
//
// Revisions.....: Aš 981126: Pointer arithmetic.
//===================================================================

Bits &
Bits::operator&=(const Bits &x) {

	int no_blocks = (x.no_blocks_ < no_blocks_) ? x.no_blocks_ : no_blocks_;

	      Block *tptr = blocks_;
	const Block *xptr = x.blocks_;

	int i;

	for (i = 0; i < no_blocks; i++, tptr++, xptr++)
		*tptr &= *xptr;

	return *this;

}

//-------------------------------------------------------------------
// Method........: operator -=
// Author........: Aleksander šhrn
// Date..........:
// Description...: Set difference.
// Comments......: Assumes that unused portions of the last blocks
//                 are identical.
//
//                 This bitset and x should really be of the same
//                 length.
//
// Revisions.....: Aš 981126: Pointer arithmetic.
//===================================================================

Bits &
Bits::operator-=(const Bits &x) {

	int no_blocks = (x.no_blocks_ < no_blocks_) ? x.no_blocks_ : no_blocks_;

	      Block *tptr = blocks_;
	const Block *xptr = x.blocks_;

	int i;

	for (i = 0; i < no_blocks; i++, tptr++, xptr++)
		*tptr &= ~(*xptr);

	return *this;

}

//-------------------------------------------------------------------
// Method........: operator ^=
// Author........: Aleksander šhrn
// Date..........:
// Description...: Bitwise XOR.
// Comments......: Assumes that unused portions of the last blocks
//                 are identical.
//
//                 This bitset and x should really be of the same
//                 length.
//
// Revisions.....: Aš 981126: Pointer arithmetic.
//===================================================================

Bits &
Bits::operator^=(const Bits &x) {

	int no_blocks = (x.no_blocks_ < no_blocks_) ? x.no_blocks_ : no_blocks_;

	      Block *tptr = blocks_;
	const Block *xptr = x.blocks_;

	int i;

	for (i = 0; i < no_blocks; i++, tptr++, xptr++)
		*tptr ^= *xptr;

	return *this;

}

//-------------------------------------------------------------------
// Method........: operator >>
// Author........: Aleksander šhrn
// Date..........:
// Description...: Stream input.
// Comments......:
// Revisions.....:
//===================================================================

istream &
operator>>(istream &stream, Bits &x) {
  static char buffer[1000];
  stream >> buffer;
  x.Create(String(buffer));
  return stream;
}

//-------------------------------------------------------------------
// Method........: operator <<
// Author........: Aleksander šhrn
// Date..........:
// Description...: Stream output.
// Comments......:
// Revisions.....:
//===================================================================

ostream &
operator<<(ostream &stream, const Bits &x) {
	stream << x.Format();
  return stream;
}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: UpdateLastBlock
// Author........: Aleksander šhrn
// Date..........:
// Description...: If bit counting is to be done on a per byte level,
//                 the unused bits in the last block must be set to
//                 0.
// Comments......:
// Revisions.....:
//===================================================================

bool
Bits::UpdateLastBlock() {

	// Is there a last block to process?
	if (blocks_ == NULL || no_blocks_ == 0)
		return true;

	// How many of the blocks are completely used?
	int no_complete = no_bits_ / BITS_PER_BLOCK;

	// How many of the bits of the last block are in use?
	int in_use = no_bits_ - (no_complete * BITS_PER_BLOCK);

	Block block(0);

	int i;

	// Make a copy of the last block where the unused bits are set to 0.
	for (i = 0; i < in_use; i++) {
		if ((blocks_[no_blocks_ - 1] & (((Block) 1) << i)) != 0)
			block |= (((Block) 1) << i);
	}

	// Replace the last block.
	blocks_[no_blocks_ - 1] = block;

	return true;

}

//-------------------------------------------------------------------
// Method........: Resize
// Author........: Aleksander šhrn
// Date..........:
// Description...: Resizes blocks in order to fit the specified
//                 number of bits. Preserves contents of old buffer,
//                 if specified.
//
//                 Any new bits are initialized to 0. This so that
//                 bit operations can be safely carried out on
//                 a per block level, and so that bit counting can
//                 be done on a per byte level.
//
// Comments......: Consider more optimal reallocation.
// Revisions.....:
//===================================================================

bool
Bits::Resize(int no_bits, bool preserve) {

	const int overhead = 1;

	// Determine new buffer size.
	int no_blocks = overhead + (no_bits / BITS_PER_BLOCK);

	// Can we reuse current buffer?
	if (no_blocks == no_blocks_) {
		no_bits_ = no_bits;
		return true;
	}

	// Allocate new buffer.
	Block *blocks = new Block[no_blocks];

	// Initialize new buffer.
	memset(blocks, 0, no_blocks * BYTES_PER_BLOCK);

	// Copy contents of old buffer into new buffer, if specified and possible.
	if (preserve && blocks_ != NULL) {
		int n = (no_blocks_ < no_blocks) ? no_blocks_ : no_blocks;
		memcpy(blocks, blocks_, n * BYTES_PER_BLOCK);
	}

	// Delete old buffer.
	if (blocks_ != NULL)
		delete [] blocks_;

	// Reassign internal representation.
	blocks_    = blocks;
	no_blocks_ = no_blocks;
	no_bits_   = no_bits;

	return true;

}

//-------------------------------------------------------------------
// Method........: Count
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Assumes that all unused bits in last block are 0.
// Revisions.....:
//===================================================================

int
Bits::Count(bool state) const {

	int count = 0;

	// Compute how many bytes the buffer is.
	int no_bytes = no_blocks_ * BYTES_PER_BLOCK;

	// Aquire a "byte pointer" to the block buffer.
	unsigned char *pointer = reinterpret_cast(unsigned char *, blocks_);

	int i;

	// Count byte per byte via a lookup table, even the last block.
	for (i = 0; i < no_bytes; i++)
		count += static_counts_[*pointer++];

	if (state)
		return count;
	else
		return no_bits_ - count;

}

//-------------------------------------------------------------------
// Method........: Invert
// Author........: Aleksander šhrn
// Date..........:
// Description...: Complement.
// Comments......:
// Revisions.....: Aš 981126: Pointer arithmetic.
//===================================================================

bool
Bits::Invert() {

	Block *ptr = blocks_;

	int i;

	for (i = 0; i < no_blocks_; i++, ptr++)
		*ptr = ~(*ptr);

	// Make sure that the last block has 0s in the right locations.
	UpdateLastBlock();

	return true;

}

bool
Bits::Invert(int i) {
	return SetState(i, !GetState(i));
}

//-------------------------------------------------------------------
// Method........: Reverse
// Author........: Aleksander šhrn
// Date..........:
// Description...: Reverses the bit order.
// Comments......: Suboptimal implementation.
// Revisions.....:
//===================================================================

bool
Bits::Reverse() {

	int i, half = no_bits_ / 2;

	for (i = 0; i <= half; i++) {
		bool tmp = GetState(i);
		SetState(i, GetState(no_bits_ - 1 - i));
		SetState(no_bits_ - 1 - i, tmp);
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Format
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Bits::Format(String &formatted, char on, char off, const String &delimiter) const {

	formatted = "";

	int i;

	for (i = 0; i < GetSize(); i++) {
		if (GetState(i))
			formatted += on;
		else
			formatted += off;
		if (i < GetSize() - 1)
			formatted += delimiter;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Format
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
Bits::Format(char on, char off, const String &delimiter) const {

	String formatted;

	if (!Format(formatted, on, off, delimiter))
		return Undefined::String();

	return formatted;

}

//-------------------------------------------------------------------
// Method........: GetVector
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Bits::GetVector(Vector(int) &v) const {

	v.erase(v.begin(), v.end());
	v.reserve(Count(true));

	int i;

	for (i = 0; i < GetSize(); i++) {
		if (GetState(i))
			v.push_back(i);
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: SetState
// Author........: Aleksander šhrn
// Date..........:
// Description...: Sets a bit to true or false.
// Comments......:
// Revisions.....: Aš 980915: Removed dynamic resizing option.
//===================================================================

bool
Bits::SetState(int i, bool state) {

#ifdef _DEBUG
	if (i < 0 || i >= no_bits_)
		return false;
#endif

	int block  = GetBlock(i);
	int offset = GetOffset(i);

	// Determine mask.
	Block mask = (((Block) 1) << offset);

	// Mask block.
	if (state)
		blocks_[block] |= mask;
	else
		blocks_[block] &= ~mask;

	return true;

}

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...: Set bits as specified by string.
// Comments......:
// Revisions.....:
//===================================================================

bool
Bits::Create(const String &text, char on, const String &delimiters) {

	int length  = text.GetLength();
	int no_bits = 0;
	int i;

	// Count bits.
	for (i = 0; i < length; i++) {
		if (!delimiters.Contains(text[i]))
			no_bits++;
	}

	// Allocate space.
	if (!Resize(no_bits, false))
		return false;

	int current_bit = 0;

	// Set bits.
	for (i = 0; i < length; i++) {
		if (!delimiters.Contains(text[i])) {
			if (!SetState(current_bit++, text[i] == on))
				return false;
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...: Set bits as specified by vector.
// Comments......:
// Revisions.....:
//===================================================================

bool
Bits::Create(const Vector(bool) &v) {

	// Allocate space.
	if (!Resize(v.size(), false))
		return false;

	int i;

	// Set bits.
	for (i = 0; i < no_bits_; i++) {
		if (!SetState(i, v[i]))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Compare
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns a raw memory comparison between the block
//                 buffer of this bitset and the buffer of the input
//                 bitset.
//
//                 The bitsets should normally be the same size.
//
//                 Return values:
//
//                 -1: This buffer is less than input buffer.
//                  0: This buffer is identical to input buffer.
//                  1: This buffer is greater than input buffer.
//
// Comments......: A raw memory comparison is not necessarily the same
//                 as a lexicographical comparison, although both yield
//                 total orders.
// Revisions.....:
//===================================================================

int
Bits::Compare(const Bits &in) const {

	if (this == &in)
		return 0;

	if (no_blocks_ != in.no_blocks_) {
		if (no_blocks_ < in.no_blocks_)
			return -1;
		else
			return 1;
	}

	if (no_blocks_ == 0)
		return 0;

	return memcmp(blocks_, in.blocks_, no_blocks_ * BYTES_PER_BLOCK);

}

//-------------------------------------------------------------------
// Method........: Intersects
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the two bitsets intersect.
// Comments......: Assumes that unused portions of the last blocks
//                 are identical.
//
//                 The bitsets should really be of the same length.
//
// Revisions.....: Aš 981126: Pointer arithmetic.
//===================================================================

bool
Bits::Intersects(const Bits &in) const {

	int no_blocks = (no_blocks_ < in.no_blocks_) ? no_blocks_ : in.no_blocks_;

	const Block *ptr1 = blocks_;
	const Block *ptr2 = in.blocks_;

	int i;

	for (i = 0; i < no_blocks; i++, ptr1++, ptr2++) {
		if (((Block) 0) != (*ptr1 & *ptr2))
			return true;
	}

	return false;

}
 
