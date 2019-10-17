//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/ksreduct.h>

//-------------------------------------------------------------------
// Methods for class KSReduct.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Copy constructor.
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

KSReduct::KSReduct(const KSReduct &in) : Reduct(in) {
	attributes_     = in.attributes_;
	discernibility_ = in.discernibility_;
	object_no_      = in.object_no_;
	support_        = in.support_;
	modulo_         = in.modulo_;
}

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

KSReduct::KSReduct() {
	discernibility_ = Reduct::DISCERNIBILITY_FULL;
	object_no_      = Undefined::Integer();
	support_        = 0;
	modulo_         = false;
}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

KSReduct::~KSReduct() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(KSReduct, KSREDUCT, Reduct)

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

Structure *
KSReduct::Duplicate() const {
	return new KSReduct(*this);
}

//-------------------------------------------------------------------
// Methods inherited from Reduct.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetNoAttributes
// Author........:
// Date..........:
// Description...: Returns the number of attributes (indices) defining
//                 the reduct.
//
//                 Example:  R = {3, 6, 7, 9}
//                           R has four attributes.
//
// Comments......:
// Revisions.....:
//===================================================================

int
KSReduct::GetNoAttributes() const {
	return attributes_.size();
}

//-------------------------------------------------------------------
// Method........: GetAttribute
// Author........:
// Date..........:
// Description...: Returns the attribute number in the given position.
//
//                 Example:  R = {3, 6, 7, 9}
//                           R.GetAttribute(0) will return 3.
//                           R.GetAttribute(1) will return 6.
//                           R.GetAttribute(2) will return 7.
//                           R.GetAttribute(3) will return 9.
//                           R.GetAttribute(4) will return Undefined::Integer()
//
// Comments......:
// Revisions.....:
//===================================================================

int
KSReduct::GetAttribute(int position_no) const {

#ifdef _DEBUG
	// Is the index argument legal?
	if ((position_no < 0) || (position_no >= GetNoAttributes())) {
		Message::Error("Reduct position index out of range.");
		return Undefined::Integer();
	}
#endif

	return attributes_[position_no];

}

//-------------------------------------------------------------------
// Method........: InsertAttribute
// Author........:
// Date..........:
// Description...: A reduct is conceptually defined through a list of
//                 attribute numbers (indices).  Returns true if the
//                 specified attribute was actually appended to the
//                 reduct, false otherwise.  The list of attribute
//                 indices is hence kept unique.
//
//                 Example:  R = {3, 6, 7, 9}
//                           R.AppendAttribute(5) returns true.
//                           R = {3, 6, 7, 9, 5}
//                           R.AppendAttribute(7) returns false.
//                           R = {3, 5, 6, 7, 9}
//
// Comments......: Keep the reduct sorted?
// Revisions.....:
//===================================================================

bool
KSReduct::InsertAttribute(int attribute_no) {

	// Is the attribute already a member?  If so, do not violate uniqueness.
	if (IsMember(attribute_no))
		return false;

	attributes_.push_back(attribute_no);

	return true;

}

//-------------------------------------------------------------------
// Method........: RemoveAttribute
// Author........:
// Date..........:
// Description...: Given a position index into the list of attribute
//                 indices defining the reduct, returns true if the
//                 specified attribute was removed from the reduct,
//                 false otherwise.
//
//                 Example:  R = {3, 6, 7, 9}
//                           R.RemoveAttribute(4) returns false.
//                           R = {3, 6, 7, 9}
//                           R.RemoveAttribute(1) returns true.
//                           R = {3, 7, 9}
//
// Comments......:
// Revisions.....:
//===================================================================

bool
KSReduct::RemoveAttribute(int position_no) {

#ifdef _DEBUG
  // Legal index?
  if ((position_no < 0) || (position_no >= GetNoAttributes())) {
		Message::Error("Position index in reduct out of range.");
    return false;
	}
#endif

	attributes_.erase(attributes_.begin() + position_no);

	return true;

}

//-------------------------------------------------------------------
// Method........: Reserve
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
KSReduct::Reserve(int no_attributes) {

	attributes_.reserve(no_attributes);

	return true;

}

//-------------------------------------------------------------------
// Method........: Reindex
// Author........: Aleksander šhrn
// Date..........:
// Description...: Reindexes the attribute indexes according to some
//                 supplied vector of new indices.
//
//                 Example:
//
//                    R = {0, 3, 4}
//                    R.Reindex([3, 4, 6, 7, 9]);
//                    R = {3, 7, 9}
//
// Comments......:
// Revisions.....:
//===================================================================

bool
KSReduct::Reindex(const Vector(int) &/*indices*/) {
	Message::Debug("KSReduct::Reindex not implemented yet.");
	return false;
}

//-------------------------------------------------------------------
// Method........: GetDiscernibilityType
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the reduct discernibility type.
//
//                 DISCERNIBILITY_FULL:           Among all objects.
//                 DISCERNIBILITY_OBJECTRELATED:  Each object separately (relative
//                                                to each object).
// Comments......:
// Revisions.....:
//===================================================================

Reduct::Discernibility
KSReduct::GetDiscernibilityType() const {
	return discernibility_;
}

//-------------------------------------------------------------------
// Method........: SetDiscernibilityType
// Author........: Aleksander šhrn
// Date..........:
// Description...: Cf. the GetDiscernibilityType method.
// Comments......:
// Revisions.....:
//===================================================================

bool
KSReduct::SetDiscernibilityType(Reduct::Discernibility discernibility) {
	discernibility_ = discernibility;
	return true;
}

//-------------------------------------------------------------------
// Method........: GetObject
// Author........: Aleksander šhrn
// Date..........:
// Description...: If the reduct is relative to an object, returns
//                 the object the reduct is relative to.
// Comments......:
// Revisions.....:
//===================================================================

int
KSReduct::GetObject() const {

	// Is the reduct really object-related?
	if (GetDiscernibilityType() == DISCERNIBILITY_FULL)
		return Undefined::Integer();

	return object_no_;

}

//-------------------------------------------------------------------
// Method........: SetObject
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Cf. GetObject method.
// Revisions.....:
//===================================================================

bool
KSReduct::SetObject(int object_no) {
	object_no_ = object_no;
	return true;
}

//-------------------------------------------------------------------
// Method........: GetSupport
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the "support" of the reduct.
// Comments......:
// Revisions.....:
//===================================================================

int
KSReduct::GetSupport() const {
	return support_;
}

//-------------------------------------------------------------------
// Method........: SetSupport
// Author........: Aleksander šhrn
// Date..........:
// Description...: Cf. the GetSupport method.
// Comments......:
// Revisions.....:
//===================================================================

bool
KSReduct::SetSupport(int support) {
	support_ = support;
	return true;
}

//-------------------------------------------------------------------
// Method........: IsModuloDecision
// Author........: Aleksander šhrn
// Date..........:
// Description...: Is the reduct computed modulo the decision?
// Comments......:
// Revisions.....:
//===================================================================

bool
KSReduct::IsModuloDecision() const {
	return modulo_;
}

//-------------------------------------------------------------------
// Method........: IsModuloDecision
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
KSReduct::IsModuloDecision(bool modulo) {
	modulo_ = modulo;
	return true;
}
 
