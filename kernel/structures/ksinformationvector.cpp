//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/ksinformationvector.h>

//-------------------------------------------------------------------
// Methods for class KSInformationVector.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Copy constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

KSInformationVector::KSInformationVector(const KSInformationVector &in) : InformationVector(in) {
	inf_ = in.inf_;
}

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

KSInformationVector::KSInformationVector() {
}

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...: Constructs an inf. vector of the given size.
//                 All entries are set undefined.
// Comments......:
// Revisions.....:
//===================================================================

KSInformationVector::KSInformationVector(int size) {

	if (size < 0)
		size = -size;

	inf_.reserve(size);

	int i;

	for (i = 0; i < size; i++)
		inf_.push_back(Undefined::Integer());

}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

KSInformationVector::~KSInformationVector() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(KSInformationVector, KSINFORMATIONVECTOR, InformationVector)

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

Structure *
KSInformationVector::Duplicate() const {
	return new KSInformationVector(*this);
}

//-------------------------------------------------------------------
// Methods inherited from InformationVector.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetNoAttributes
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
KSInformationVector::GetNoAttributes() const {
	return inf_.size();
}

//-------------------------------------------------------------------
// Method........: GetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
KSInformationVector::GetEntry(int attribute_no) const {

#ifdef _DEBUG
	if ((attribute_no < 0) || (attribute_no >= GetNoAttributes())) {
		Message::Error("Index out of range.");
		return Undefined::Integer();
	}
#endif

	return inf_[attribute_no];

}

//-------------------------------------------------------------------
// Method........: SetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
KSInformationVector::SetEntry(int attribute_no, int value) {

#ifdef _DEBUG
	if ((attribute_no < 0) || (attribute_no >= GetNoAttributes())) {
		Message::Error("Index out of range.");
		return false;
	}
#endif

	inf_[attribute_no] = value;

	return true;

}

//-------------------------------------------------------------------
// Method........: InsertAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
KSInformationVector::InsertAttribute(int attribute_no) {

#ifdef _DEBUG
	if ((attribute_no < 0) || (attribute_no > GetNoAttributes())) {
		Message::Error("Index out of range.");
		return false;
	}
#endif

	inf_.insert(inf_.begin() + attribute_no, Undefined::Integer());

	return true;

}

//-------------------------------------------------------------------
// Method........: RemoveAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
KSInformationVector::RemoveAttribute(int attribute_no) {

#ifdef _DEBUG
	if ((attribute_no < 0) || (attribute_no >= GetNoAttributes())) {
		Message::Error("Index out of range.");
		return false;
	}
#endif

	inf_.erase(inf_.begin() + attribute_no);

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
KSInformationVector::Reserve(int no_attributes) {

	inf_.reserve(no_attributes);

	return true;

}
 
