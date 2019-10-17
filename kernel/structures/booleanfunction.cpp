//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/booleanfunction.h>
#include <kernel/structures/decisiontable.h>

#include <kernel/utilities/iokit.h>

#include <kernel/basic/algorithm.h>
#include <kernel/basic/set.h>
#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>

#include <common/configuration.h>

// -N 10/07/13
#include <xmmintrin.h>

//-------------------------------------------------------------------
// Methods for class BooleanFunction.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...: Copy constructor
// Comments......: Does a shallow copy. Consider deep copying.
// Revisions.....:
//===================================================================

BooleanFunction::BooleanFunction(const BooleanFunction &in) : Structure(in) {

	int i;

  name_       = in.name_;
  components_ = in.components_;

	// Shallow copy, do not take ownership.
  for (i = 0; i < in.components_.size(); i++)
		components_[i].is_owner_ = false;

}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...: Empty constructor
// Comments......:
// Revisions.....:
//===================================================================

BooleanFunction::BooleanFunction() {
  name_ = Undefined::String();
}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Deletion of Bits not done in ~BFComponent since we
//                 then get unwanted deletions when temporaries etc. go
//                 out of scope.
// Revisions.....:
//===================================================================

BooleanFunction::~BooleanFunction() {

	int i;

  // Delete those components that it "owns".
  for (i = 0; i < components_.size(); i++) {
    if (components_[i].is_owner_)
      delete components_[i].bits_;
  }

  components_.erase(components_.begin(), components_.end());

}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(BooleanFunction, BOOLEANFUNCTION, Structure)

//-------------------------------------------------------------------
// Methods inherited from Persistent.
//===================================================================

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
BooleanFunction::Load(ifstream &stream) {
  return Load(stream, NULL, false);
}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
BooleanFunction::Save(ofstream &stream) const {
  return Save(stream, NULL, false);
}

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

//-------------------------------------------------------------------
// Method........: Clear
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: See ~BooleanFunction comment.
// Revisions.....:
//===================================================================

void
BooleanFunction::Clear() {

	int i;

  name_ = Undefined::String();

  // Delete those components that it "owns".
  for (i = 0; i < components_.size(); i++) {
    if (components_[i].is_owner_)
      delete components_[i].bits_;
  }

  components_.erase(components_.begin(), components_.end());

}

//-------------------------------------------------------------------
// New methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetName/SetName
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

const String &
BooleanFunction::GetName() const {
  return name_;
}

bool
BooleanFunction::SetName(const String &name) {
  name_ = name;
  return true;
}

//-------------------------------------------------------------------
// Method........: GetArity
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the number of variables in the function.
//
//                 Example: f = 0x + 0y + 1z
//
//                          f.GetArity(true)  = 1
//                          f.GetArity(false) = 3
//
// Comments......: Assumes all bit sets have same length.
// Revisions.....: Aš 980615: Added minimize option.
//===================================================================

int
BooleanFunction::GetArity(bool minimize) const {

  int i, no_components = GetNoComponents();

  // Empty function?
  if (no_components == 0)
    return 0;

  // Don't minimize?
  if (!minimize)
    return GetComponent(0)->GetSize();

  Bits component = *GetComponent(0);

  for (i = 1; i < no_components; i++)
    component |= *GetComponent(i);

  return component.Count(true);

}

//-------------------------------------------------------------------
// Method........: IsValid
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the function is currently valid.
// Comments......:
// Revisions.....:
//===================================================================

bool
BooleanFunction::IsValid() const {

	int i;

  // All bit sets must have the same length.
  for (i = components_.size() - 1; i > 0; i--) {
    if (components_[i].bits_ == NULL || components_[0].bits_ == NULL)
      return false;
    if (components_[i].bits_->GetSize() != components_[0].bits_->GetSize())
      return false;
  }

  return true;

}

//-------------------------------------------------------------------
// Method........: GetNoComponents
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
BooleanFunction::GetNoComponents() const {
  return components_.size();
}

//-------------------------------------------------------------------
// Method........: GetComponent
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

const Bits *
BooleanFunction::GetComponent(int i) const {
  return components_[i].bits_;
}

//-------------------------------------------------------------------
// Method........: SetComponent
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================
/*
bool
BooleanFunction::SetComponent(int i, const Bits *component, bool copy, float weight) {

  if (component == NULL)
    return false;

#ifdef _DEBUG
  if (i < 0 || i >= GetNoComponents())
    return false;
#endif

	// Set component weight.
  components_[i].weight_ = weight;

  // Check against self-assignment.
  if (components_[i].bits_ == component)
    return true;

  // Assign new component, and determine fate of old component.
  if (!components_[i].is_owner_ && !copy) {
    components_[i].bits_ = const_cast(Bits *, component);
  }
  else if (!components_[i].is_owner_ && copy) {
    components_[i].bits_ = new Bits(*component);
  }
  else if (components_[i].is_owner_ && copy) {
    *(components_[i].bits_) = *component;
  }
  else if (components_[i].is_owner_ && !copy) {
    delete components_[i].bits_;
    components_[i].bits_ = const_cast(Bits *, component);
  }

  // Set component ownership flag.
  components_[i].is_owner_ = copy;

  return true;

}
*/
//-------------------------------------------------------------------
// Method........: SetComponent
// Author........: Nicholas Baltzer
// Date..........: 23/07/13
// Description...: Same as the version above, except for switching the
//			weight assignment one step down to avoid
//			unnecessary work, and the inclusion of the bitline
//			update to facilitate SSE computations in
//			the AppendSimplified crunch.
// Comments......:
// Revisions.....:
//===================================================================
bool
BooleanFunction::SetComponent(int i, const Bits *component, bool copy, float weight) {

  	if (component == NULL)
    		return false;

#ifdef _DEBUG
	if (i < 0 || i >= GetNoComponents())
		return false;
#endif


  	// Check against self-assignment.
  	if (components_[i].bits_ == component)
		return true;
	
		// Set component weight.
	  components_[i].weight_ = weight;
	
	  // Assign new component, and determine fate of old component.
	if (!components_[i].is_owner_ && !copy) {
	  	components_[i].bits_ = const_cast(Bits *, component);
	}
	else if (!components_[i].is_owner_ && copy) {
		components_[i].bits_ = new Bits(*component);
	}
	else if (components_[i].is_owner_ && copy) {
    		*(components_[i].bits_) = *component;
  	}
  	else if (components_[i].is_owner_ && !copy) {
    	delete components_[i].bits_;
    	components_[i].bits_ = const_cast(Bits *, component);
  	}

  	// Set component ownership flag.
  	components_[i].is_owner_ = copy;
/*
	//Add the component to the bitline.
	int j = i*9;
	int k = 0;
	for (; j < i * 9 + 9; j++, k++) {
		*(blockline_ + j) = *(component + k);
	}  
*/
  	return true;

}

//-------------------------------------------------------------------
// Method........: GetWeight
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

float
BooleanFunction::GetWeight(int i) const {
  return components_[i].weight_;
}

//-------------------------------------------------------------------
// Method........: SetWeight
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
BooleanFunction::SetWeight(int i, float weight) {
  components_[i].weight_ = weight;
	return true;
}

//-------------------------------------------------------------------
// Method........: InsertComponent/AppendComponent/RemoveComponent
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
BooleanFunction::InsertComponent(const Bits *component, int i, bool copy, float weight) {

  if (component == NULL)
    return false;

#ifdef _DEBUG
  if (i < 0 || i > GetNoComponents())
    return false;
#endif

  // Insert new component.
  components_.insert(components_.begin() + i, BFComponent());

	// Set component data.
	if (copy)
		components_[i].bits_ = new Bits(*component);
	else
		components_[i].bits_ = const_cast(Bits *, component);

	components_[i].is_owner_ = copy;
	components_[i].weight_   = weight;

  return true;

}

bool
BooleanFunction::AppendComponent(const Bits *component, bool copy, float weight) {
  return InsertComponent(component, GetNoComponents(), copy, weight);
}

bool
BooleanFunction::RemoveComponent(int i) {

#ifdef _DEBUG
  if (i < 0 || i >= GetNoComponents())
    return false;
#endif

  // Delete the component if it is owned by this function.
  if (components_[i].is_owner_)
    delete components_[i].bits_;

  // Update array.
  components_.erase(components_.begin() + i);

  return true;

}

bool
BooleanFunction::RemoveAllComponents() {

  // Remember name.
  String name = name_;

  // Clear everything.
  Clear();

  // Set the name back again.
  name_ = name;

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
BooleanFunction::Reserve(int no_components) {
  components_.reserve(no_components);
  return true;
}

//-------------------------------------------------------------------
// Method........: RemoveComponents
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 9809: Introduced weight vector, one weight
//                          per component.
//                 Aš 9901: Fixed potential memory leak, forgot to
//                          delete owned components.
//                 Aš 9907: Weight vector is now held and managed
//                          internally instead of being supplied.
//===================================================================

bool
BooleanFunction::RemoveComponents(const Bits &remove) {

  int no_components = components_.size();
  int no_removed    = remove.Count(true);

  if (no_removed == 0)
    return true;

#ifdef _DEBUG
  if (no_components != remove.GetSize())
    return false;
#endif

  int i, last = no_components - 1;

  // Compact arrays.
  for (i = 0; i < no_components; i++) {
    if (!remove.GetState(i))
      continue;
    while (remove.GetState(last) && last > i)
      last--;
    if (last <= i)
      break;
    if (components_[i].is_owner_)
      delete components_[i].bits_;
    components_[i] = components_[last];
    components_[last].is_owner_ = false;
    last--;
  }

  // Erase array tail.
  components_.erase(components_.begin() + no_components - no_removed, components_.end());

  return true;

}

//-------------------------------------------------------------------
// Method........: Simplify
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Simplifies the function. A parameter is required
//                 that specifies whether the function should be
//                 treated as a "weighted" function, i.e., a
//                 function with a weight associated with each
//                 component. Optimizations are possible if
//                 weights are ignored.
//
// Revisions.....: Aš 9809:
//                    Introduced weight vector, one weight per
//                    component.
//                 SAV 990321:
//                    Calls RemoveRSupersets() instead of
//                    RemoveSupersets().
//                 Aš 990726:
//                    Weight vector is now held and managed
//                    internally instead of being supplied.
//===================================================================

bool
BooleanFunction::Simplify(bool weighted) {

	if (Configuration::Kernel::IsVerbose()) {
		Message message;
		message.Notify("Simplifying Boolean function...");
	}

  return RemoveDuplicates() && RemoveRSupersets(weighted);

}

//-------------------------------------------------------------------
// Method........: AppendSimplified
// Author........: Aleksander šhrn
// Date..........:
// Description...: Appends (a duplicate of) the candidate component to
//                 the already simplified function only if it belongs in
//                 the simplified version of "the existing components plus
//                 the candidate component". Assures that the function
//                 remains simplified if an append takes place.
//
//                 Assumes that the function is already simplified prior
//                 to calling this method. Note that a function with less
//                 than two components, in particular the empty function,
//                 is necessarily simplified.
//
//                 Returns true if the candidate component was
//                 appended, false otherwise.
//
//                 A parameter specifies if the function is a
//                 "weighted" function, i.e., a function with a weight
//                 associated with each component. Optimizations are
//                 possible if weights are ignored.
//
// Comments......: Removal by replacing with the last component saves
//                 a lot of vector reallocations.
//
//                 The way the weight vector is updated should be the
//                 same as in the RemoveSupersets/RemoveDuplicates
//                 methods.
//
// Revisions.....: Aš 990726: Weight vector is now held and managed
//                            internally instead of being supplied.
//		   NB 31/07/13: Replaced with faster version
//===================================================================
/*
bool
BooleanFunction::AppendSimplified(const Bits &component, bool weighted, float weight) {

  int  no_components  = components_.size();                            // Size of current function.
  int  last           = no_components - 1;                             // Array index of last function component.
  int  no_removed     = 0;                                             // Counts how many components that have been removed.
  bool check_superset = true;                                          // Optimization: Test for superset relation?
  bool append         = true;                                          // Schedule candidate component for append?

	int i;

//	cout << "|" << no_components << "|" << endl;
  // Check against existing components.
	//The number of components (no_components) range from low (around 20) to high (around 500).
	//This is an excellent place to optimize.
  for (i = no_components - 1; i >= 0; i--) {
    if (component == *(components_[i].bits_)) {                        // The candidate component is equal to an existing component.
			if (weighted)                                                    //    Is this function to be treated as "weighted"?
				components_[i].weight_ += weight;                              //       Update weight of existing component.
      return false;                                                    //    We're done, since we assume that the function is already simplified.
    }
    else if (check_superset && component > *(components_[i].bits_)) {  // The candidate component is a superset of an existing component.
			if (weighted)                                                    //    Is this function to be treated as "weighted"?
				components_[i].weight_ += weight;                              //       Update weight of existing component.
      append = false;                                                  //    Don't append candidate component,
      if (!weighted)                                                   //    We may be done, since we assume that the function is already simplified...
        return false;                                                  //        ...unless the function is "weighted", in which case the weights must be properly updated, so we cannot abort/return just yet.
    }
    else if (component < *(components_[i].bits_)) {                    // The candidate component is a subset of an existing component.
      if (components_[i].is_owner_)                                    //    Delete existing component, if responsible for that.
	        delete components_[i].bits_;
			if (i != last) {
				components_[i].bits_ = components_[last].bits_;                //    Replace existing component by the last, and already checked, component.
				components_[i].is_owner_ = components_[last].is_owner_;
				components_[last].is_owner_ = false;
			}
      if (weighted) {                                                  //    Is this function to be treated as "weighted"?
        weight += components_[i].weight_;                              //       Update weight to assign to new component.
				components_[i].weight_ = components_[last].weight_;
      }
      check_superset = false;                                          //    Since we assume that the function is already simplified, superset events cannot occur since a subset event occurred.
      no_removed++;                                                    //    Update counter so that array tail can be properly erased,
      last--;                                                          //    Update index of the component at the end of the array.
    }
  }

  // Erase tail, if needed.
  if (no_removed > 0) {
    components_.erase(components_.begin() + no_components - no_removed, components_.end());
  }

  // Append new component.
  if (append) {
		last = components_.size();
    components_.push_back(BFComponent());
    components_[last].bits_     = new Bits(component);
    components_[last].is_owner_ = true;
    components_[last].weight_   = weight;
  }

  return append;

}
*/
//-------------------------------------------------------------------
// Method........: IsSubtree
// Author........: Nicholas Baltzer
// Date..........: 10/07/13
// Description...: Replacement for Bits::operator< and operator>. Instead of comparing member variables
//			of a Bits variable, looks directly at the Block vector. These vectors are 32bit long
//			unsigned integer arrays.
//			
//
// Comments......: The short Block vectors are unlikely to benefit from SIMD conversions, but you are welcome to try.
//
// Revisions.....:
//===================================================================
bool
BooleanFunction::IsSubtree(const Block* tptr, const Block* xptr, int blockCount) const {

	int i;
        bool equal = false;

        for (i = 0; i < blockCount; i++, tptr++, xptr++) {
                if ((*tptr & *xptr) != *tptr)
                        return false;
                if (*tptr != *xptr)
                        equal = true;
        }
/*
        for (i = 0; i < 9; i +=3, tptr +=3, xptr +=3) {
                if ((*tptr & *xptr) != *tptr || (*(tptr + 1) & *(xptr + 1)) != *(tptr + 1) || (*(tptr + 2) & *(xptr + 2)) != *(tptr + 2
))
                        return false;
                if (*tptr != *xptr || *(tptr + 1) != *(xptr + 1) || *(tptr + 2) != *(xptr + 2))
                        equal = true;
        }
*/
        //Size of Block is 4 bytes == 32 bits. 4 of these can fit in a register.
	return equal;
}
//-------------------------------------------------------------------
// Method........: IsEqual
// Author........: Nicholas Baltzer
// Date..........: 10/07/13
// Description...: Replacement for Bits::operator==. Works on a direct Block* to Block* comparison. This is faster
//			than the Bits version, and makes for less loading and copying, as the Bits structure is no longer
//			included in the scope.
//
// Comments......: The short Block vectors are unlikely to benefit from SIMD conversions, but you are welcome to try.
//
// Revisions.....:
//===================================================================
bool
BooleanFunction::IsEqual(const Block* tptr, const Block* xptr, int blockCount) const {

                                                                    
        int i;                                                                                                       
                                                                                                             
        for (i = 0; i < blockCount; i++, tptr++, xptr++) {                                                            
                //Comparing.                                                                                         
                if (*tptr != *xptr)                      
                        return false;                                                                                
        }                                                                                                            
/*
        for (i = 0; i < 9; i+=3, tptr +=3, xptr += 3) {                                                            
                //Comparing.                                                                                         
                if (*tptr != *xptr || *(tptr + 1) != *(xptr + 1) || *(tptr + 2) != *(xptr + 2))                      
                        return false;                                                                                
        }                                                                                                            
*/                                                                                                                     
        return true;                        

}
//-------------------------------------------------------------------
// Method........: GetSingleRelation
// Author........: Nicholas Baltzer
// Date..........: 31/07/13
// Description...: Performs the equivalent of IsEqual, IsSubtree(x,y), and IsSubTree(y,x) in one go.
//			The intention is that the relevant variables are all available in L1, and thus running them 
//			in this fashion provides similar benefits to a tiled matrix multiplication. The current
//			AppendSimplified does not take advantage of this, as a simple inclusion did not provide an
//			increase of speed. Instead, it is left here (and also in AppendSimplified as commented
//			if conditionals with index == ).
//
// Comments......: The short Block vectors are unlikely to benefit from SIMD conversions, but you are welcome to try.
//
// Revisions.....:
//===================================================================
int
BooleanFunction::GetSingleRelation(Block* tptr, Block* xptr, bool superset) {
		
	
	
	//Check for equality. Returns 0 on equal.
	if (*tptr == *xptr \
		&& *(tptr + 1) == *(xptr + 1) \
		&& *(tptr + 2) == *(xptr + 2) \
		&& *(tptr + 3) == *(xptr + 3) \
		&& *(tptr + 4) == *(xptr + 4) \
		&& *(tptr + 5) == *(xptr + 5) \
		&& *(tptr + 6) == *(xptr + 6) \
		&& *(tptr + 7) == *(xptr + 7) \
		&& *(tptr + 8) == *(xptr + 8))
       		return 0;          
		
		//Checking for superset. Returns - 1 if match. 
	else if (superset) { 
		if (*xptr & *tptr == *xptr \
			&& *(xptr + 1) & *(tptr + 1) == *(xptr + 1) \
			&& *(xptr + 2) & *(tptr + 2) == *(xptr + 2) \
			&& *(xptr + 3) & *(tptr + 3) == *(xptr + 3) \
			&& *(xptr + 4) & *(tptr + 4) == *(xptr + 4) \
			&& *(xptr + 5) & *(tptr + 5) == *(xptr + 5) \
			&& *(xptr + 6) & *(tptr + 6) == *(xptr + 6) \
			&& *(xptr + 7) & *(tptr + 7) == *(xptr + 7) \
			&& *(xptr + 8) & *(tptr + 8) == *(xptr + 8)) {
        	        return -1;
			}
	}	
	//Checking for subset. Returns 1.
        else if (*tptr & *xptr == *tptr \
		&& *(tptr + 1) & *(xptr + 1) == *(tptr + 1) \
		&& *(tptr + 2) & *(xptr + 2) == *(tptr + 2) \
		&& *(tptr + 3) & *(xptr + 3) == *(tptr + 3) \
		&& *(tptr + 4) & *(xptr + 4) == *(tptr + 4) \
		&& *(tptr + 5) & *(xptr + 5) == *(tptr + 5) \
		&& *(tptr + 6) & *(xptr + 6) == *(tptr + 6) \
		&& *(tptr + 7) & *(xptr + 7) == *(tptr + 7) \
		&& *(tptr + 8) & *(xptr + 8) == *(tptr + 8)) {
              	return 1;
	}
	
	return -5;
}
//-------------------------------------------------------------------
// Method........: AppendSimplified
// Author........: Nicholas Baltzer
// Date..........: 08/07/13
// Description...: Fork of Alexanders version. Functions in the same way,
//			but handles things differently. First, it is significantly faster,
//			skipping all checks for infrequent events and instead computing these.
//			Given the low rate of branching on these, less time is spent computing
//			these events than checking every possible iteration for these conditions.
//			Second, the Bits to Bits comparisons (==, <, >) have been removed and replaced
//			with Block* to Block* comparisons. This is faster. Furthermore, the functions
//			managing these comparisons are fairly optimized to make the most of close proximity 
//			data (spatial locality). As such,
//			the Bits to Bits functions may now be considered deprecated. If there is need to use
//			these in the future, please convert to the Bits to Block* comparators, as these are
//			at least faster than the original.
//
// Comments......: Might still be room for more optimizations. The variable length vectors is a prime candidate,
//			though hard to pipeline properly. Also, any reduction in the branching predictor will be helpful. 
//
// Revisions.....:
//===================================================================
bool
BooleanFunction::AppendSimplified(const Bits &component, bool weighted, float weight) {

  	int  no_components  = components_.size();   // Size of current function.
  	int  last           = no_components - 1;    // Array index of last function component.
  	int  no_removed     = 0;                    // Counts how many components that have been removed.
  	bool check_superset = true;                 // Optimization: Test for superset relation?
  	bool append         = true;                 // Schedule candidate component for append?

	int i;
	int relation;

	Block* cblock;
	Block* iblock;
	int cBlockCount;
	int iBlockCount;
  	// Check against existing components.
	//The number of components (no_components) range from low (around 20) to high (around 500).
	//This is an excellent place to optimize.
	cblock = component.GetBlocks();
	cBlockCount = component.GetNoBlocks();

	for (i = no_components - 1; i >= 0; i--) {
		iblock = components_[i].bits_->GetBlocks();
		iBlockCount = components_[i].bits_->GetNoBlocks();
//		int index = GetSingleRelation(cblock, iblock, check_superset);
//		if (index == 0) {
		if (IsEqual(cblock, iblock, cBlockCount)) {      // The candidate component is equal to an existing component.
			if (weighted)                            //    Is this function to be treated as "weighted"?
				components_[i].weight_ += weight;            //       Update weight of existing component.
      			return false;                   //    We're done, since we assume that the function is already simplified.
    		}

//		if (index == -1) {
    		if (check_superset && IsSubtree(iblock, cblock, iBlockCount)) {// The candidate component is a superset of an existing component.
			if (weighted) {       //    Is this function to be treated as "weighted"?
				components_[i].weight_ += weight;      //       Update weight of existing component.
      				append = false;       //    Don't append candidate component,
			}
      			else {        //    We may be done, since we assume that the function is already simplified...
        			return false;
			}//...unless the function is "weighted", in which case the weights must be properly updated, so we cannot abort/return just yet.
    		}
//		else if (index == 1) {
    		else if (IsSubtree(cblock, iblock, cBlockCount)) {   // The candidate component is a subset of an existing component.
      			if (components_[i].is_owner_)                //    Delete existing component, if responsible for that.
	        		delete components_[i].bits_;

			if (i != last) {

				components_[i].bits_ = components_[last].bits_;//    Replace existing component by the last, and already checked, component.  
				components_[i].is_owner_ = components_[last].is_owner_;
				components_[last].is_owner_ = false;
			}
      			if (weighted) {               //    Is this function to be treated as "weighted"?
        			weight += components_[i].weight_;           //       Update weight to assign to new component.
				components_[i].weight_ = components_[last].weight_;
      			}
      			check_superset = false;   //    Since we assume that the function is already simplified, superset events cannot occur since a subset event occurred.
      			no_removed++;                    //    Update counter so that array tail can be properly erased,
      			last--;                          //    Update index of the component at the end of the array.
    		}
  	}

  	// Erase tail, if needed.
  	if (no_removed > 0) {
    		components_.erase(components_.begin() + no_components - no_removed, components_.end());

  	}

  	// Append new component.
  	if (append) {
		last = components_.size();
    		components_.push_back(BFComponent());
    		components_[last].bits_     = new Bits(component);
   		components_[last].is_owner_ = true;
   		components_[last].weight_   = weight;
		
  	}
  	return append;
}

//-------------------------------------------------------------------
// Method........: RemoveDuplicates
// Author........: Aleksander šhrn
// Date..........:
// Description...: Eliminates duplicate components.
// Comments......:
// Revisions.....: Aš 9809: Introduced weight vector, one weight per
//                          component.
//                 Aš 9907: Weight vector is now held and managed
//                          internally instead of being supplied.
//===================================================================

bool
BooleanFunction::RemoveDuplicates() {

  int no_components = components_.size();

#ifdef _DEBUG
  Message::Debug("Eliminating duplicates...");
#endif

  Vector(int) indices;
  indices.reserve(no_components);

  int i;

  // Prepare index vector to sort.
  for (i = 0; i < no_components; i++)
    indices.push_back(i);

  LCompare compare(&components_);

  // "Virtually" sort the components.
  std::sort(indices.begin(), indices.end(), compare);

  Bits remove(no_components, false);

  // Schedule duplicates for removal.
  for (i = indices.size() - 1; i > 0; i--) {
    if (*(components_[indices[i]].bits_) == *(components_[indices[i - 1]].bits_)) {
      remove.SetState(indices[i], true);
			components_[indices[i - 1]].weight_ += components_[indices[i]].weight_;
    }
  }

  return RemoveComponents(remove);

}

//-------------------------------------------------------------------
// Method........: RemoveSupersets
// Author........: Aleksander šhrn/Staal Vinterbo
// Date..........:
// Description...: Eliminates components that are supersets
//                 of other components.
//
//                 Eliminates duplicates as well, although the
//                 RemoveDuplicates method is quicker for
//                 duplicate removal alone.
//
//                 The overall running time can be reduced if
//                 RemoveDuplicates is called prior to this method.
//
//                 A parameter specifies if the function is a
//                 "weighted" function, i.e., a function with a weight
//                 associated with each component. Optimizations are
//                 possible if weights are ignored.
//
// Comments......: Superset sums can be removed from POS functions
//                 without altering the function's semantics since
//                 in Boolean algebras (a + b)a = a. Also, for
//                 SOP functions we can do a similar thing, since
//                 a + ab = a.
//
//                 This is called absorption.
//
//                 Sub-quadratic algorithms exist, see tech. report
//                 by Pritchard.
//
//                 The way the count vector is updated should be the
//                 same as in the AppendSimplified method.
//
// Revisions.....: Aš 9809: Introduced weight vector, one weight per
//                          component.
//                 Aš 9907: Weight vector is now held and managed
//                          internally instead of being supplied.
//===================================================================

bool
BooleanFunction::RemoveSupersets(bool weighted) {

	int i, j;

#ifdef _DEBUG
  Message::Debug("Eliminating supersets...");
#endif

  Vector(int) indices;
  indices.reserve(components_.size());

  // Prepare index vector to sort.
  for (i = 0; i < components_.size(); i++)
    indices.push_back(i);

  CCompare compare(&components_);

  // "Virtually" sort the components by cardinality, in hope that the for-loop can be aborted early.
  std::sort(indices.begin(), indices.end(), compare);

  Bits remove(components_.size(), false);

  // Schedule supersets for removal. If weights are to considered, we cannot break the for-loop
  // since the weights would then not be correctly computed.
  if (weighted) {
    for (i = 0; i < indices.size() - 1; i++) {
      if (remove.GetState(indices[i]))
        continue;
      float accumulated = components_[indices[i]].weight_;
      for (j = i + 1; j < indices.size(); j++) {
        if (*(components_[indices[i]].bits_) <= *(components_[indices[j]].bits_)) {
          remove.SetState(indices[j], true);
          accumulated += components_[indices[j]].weight_;
        }
      }
			components_[indices[i]].weight_ = accumulated;
    }
  }
  else {
    for (i = indices.size() - 1; i >= 0; i--) {
      for (j = 0; j < i; j++) {
        if (*(components_[indices[j]].bits_) <= *(components_[indices[i]].bits_)) {
          remove.SetState(indices[i], true);
          break;
        }
      }
    }
  }

#ifdef _DEBUG
  Message::Debug("Done eliminating supersets");
#endif

  return RemoveComponents(remove);

}

//-------------------------------------------------------------------
// Method........: RemoveRSupersets
// Author........: Staal Vinterbo/Aleksander šhrn
// Date..........:
// Description...: Eliminates components that are supersets
//                 of other components. Similar to RemoveSupersets,
//                 except for that the weight vector is updated
//                 in a slightly different manner.
//
//                 The method RemoveDuplicates must be called prior
//                 to this method.
//
//                 Example (slightly out-of-sync with the current
//                 implementation):
//
//                    The algorithm operates on the indices of the
//                    components.
//
//                       f = {{p}, {b, p, a}, {b, p, a, c}, {b, a}, {c}}
//                             0    1          2             3       4
//
//                    The partial order (PO) representation for each
//                    component is the set of components immediately
//                    above the component in the Hasse diagram.
//
//                       PO(0) = {1}
//                       PO(1) = {2}
//                       PO(2) = {}
//                       PO(3) = {1}
//                       PO(4) = {2}
//
//                    The "transitive closures" of each PO are captured
//                    in the "Above" sets.
//
//                       Above(0) = {1, 2}
//                       Above(1) = {2}
//                       Above(2) = {}
//                       Above(3) = {1, 2}
//                       Above(4) = {2}
//
//                    The set of "Minimals" is the set of minimal elements
//                    in the PO.
//
//                       Minimals = {0, 3, 4}
//
//                    The "Factors" number for each component captures how many
//                    of the minimal elements that are subsets of
//                    the component.
//
//                       Factors(0) = 1
//                       Factors(1) = 2
//                       Factors(2) = 3
//                       Factors(3) = 1
//                       Factors(4) = 1
//
//                    Only the minimal elements survive the absorption.
//                    The weights for the minimal elements are adjusted as
//                    follows:
//
//                       for all m in Minimals:
//                          for all a in Above(m):
//                             Weight(m) += Weight(a) / Factors(a)
//
// Comments......: See SAV/Aš paper.
//
//
// Revisions.....:
//===================================================================


bool
BooleanFunction::RemoveRSupersets(bool weighted) {

  // Revert to "old" implementation?
  if (!weighted)
    return RemoveSupersets(false);

#ifdef _DEBUG
  Message::Debug("R-eliminating supersets...");
#endif

  Vector(int) indices;
  indices.reserve(components_.size());

  int i, j;

  // Prepare index vector to sort.
  for (i = 0; i < components_.size(); i++)
    indices.push_back(i);

  CCompare compare(&components_);

  // "Virtually" sort the components by cardinality so that we can halve the for-loop below.
  std::sort(indices.begin(), indices.end(), compare);

  Vector(int) minimals;

	// The minimals array holds indices of minimal components. Reserve a guesstimate.
  minimals.reserve(500);

  Vector(int) factors;

	// The factors array holds the "normalization coefficients" for each of the minimals.
  for (i = 0; i < indices.size(); i++)
    factors.push_back(0);

  IntegerHashSetHashMap aboves;

#ifdef _DEBUG
  Message::Debug("Computing above elements in Hasse diagram (" + String::Format(indices.size()) + ")...");
#endif

  Bits remove(components_.size(), false);

	// Compute elements above in the Hasse diagram.
  for (i = 0; i < indices.size(); i++) {

		// Skip non-minimals.
    if (remove.GetState(indices[i]))
      continue;

    minimals.push_back(indices[i]);

		// Insert an empty hash set into the hash map.
    aboves[indices[i]] = IntegerHashSet();

		// Compare function components.
    for (j = i + 1; j < indices.size(); j++){
      if (*(components_[indices[j]].bits_) > *(components_[indices[i]].bits_)) {
        remove.SetState(indices[j], true);
        aboves[indices[i]].insert(indices[j]);
        factors[indices[j]]++;
      }
    }

  }

#ifdef _DEBUG
  Message::Debug("Adjusting weights (" + String::Format(minimals.size()) + ")...");
#endif

	IntegerHashSet::const_iterator it;

  // Adjust weights.
  for (i = 0; i < minimals.size(); i++) {
    for (it = aboves[minimals[i]].begin(); it != aboves[minimals[i]].end(); it++) {
			components_[minimals[i]].weight_ += components_[(*it)].weight_ / factors[(*it)];
		}
  }

#ifdef _DEBUG
  Message::Debug("R-elimination completed...");
#endif

  return RemoveComponents(remove);

}

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
BooleanFunction::Load(const String &filename, const DecisionTable *table, bool masked) {

  ifstream stream;

  if (!IOKit::Open(stream, filename))
    return false;

  return Load(stream, table, masked);

}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
BooleanFunction::Save(const String &filename, const DecisionTable *table, bool masked) const {

  ofstream stream;

  if (!IOKit::Open(stream, filename))
    return false;

  return Save(stream, table, masked);

}

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: String appends are very costly, resolve later.
//                 Currently not usable for large functions.
// Revisions.....:
//===================================================================

bool
BooleanFunction::Load(ifstream &stream, char inter, char intra, const DecisionTable *table, bool masked) {

  String function, line;

  // Load function definition.
  while (IOKit::LoadLine(stream, line)) {
    if (line == "end")
      break;
    function += line;
  }

  Map(String, int) names;

  // Parse function.
  if (table != NULL)
    return Create(function, inter, intra, *table, masked);
  else
    return Create(function, inter, intra, names);

}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Previously, simply used to call:
//
//                    stream << Format(inter, intra, table, masked) << endl;
//
//                 However, if the function is very large, creating a
//                 string representation before dumping it to the stream
//                 may be a memory hog (many, many megabytes), in addition
//                 to being very slow. Hence, a specialized formatting
//                 and saving method is provided.
//
//                 Unify Save/Format code later.
// Revisions.....:
//===================================================================

bool
BooleanFunction::Save(ofstream &stream, char inter, char intra, const DecisionTable *table, bool masked) const {

	int i, j;

  int no_components = GetNoComponents();
  int no_attributes = GetArity(false);

  Vector(String) names;
  names.reserve(no_attributes);

  // Collect attribute names, so we don't need to look them up every time.
  for (j = 0; j < no_attributes; j++)
    names.push_back((table == NULL) ? String::Format(j) : table->GetAttributeName(j, masked));

  // Function name.
  stream << GetName() << " =" << endl;

  String indent(' ', 2);
  String intersep(' ' + String(inter, 1) + '\n');
  String intrasep(' ' + String(intra, 1) + ' ');

  // Function body.
  for (i = 0; i < no_components; i++) {
    const Bits *component = GetComponent(i);
    stream << indent;
    stream << '(';
    int count = 0;
    for (j = 0; j < no_attributes; j++) {
      if ((*component)[j]) {
        count++;
        if (count > 1)
          stream << intrasep;
        stream << names[j];
      }
    }
    stream << ')';
    if (i < no_components - 1)
      stream << intersep;
  }

  stream << endl << "end" << endl;

  return true;

}

//-------------------------------------------------------------------
// Method........: Format
// Author........: Aleksander šhrn
// Date..........:
// Description...: Builds a formatted string of the function.
// Comments......:
// Revisions.....: Aš 980704: Disallow formatting of long functions.
//===================================================================

bool
BooleanFunction::Format(String &formatted, char inter, char intra, const DecisionTable *table, bool masked) const {

	int i, j;

  int no_components = GetNoComponents();
  int no_attributes = GetArity(false);

  // Don't allow formatting of long functions.
  if (no_components > 5000) {
    Message::Error("Function too large to format. Consider using Save instead.");
    return false;
  }

  // Make a crude attempt to slightly overestimate its final length.
  if (!formatted.Reserve(10 * no_components * no_attributes))
    return false;

  Vector(String) names;
  names.reserve(no_attributes);

  // Collect attribute names, so we don't need to look them up every time.
  for (j = 0; j < no_attributes; j++)
    names.push_back((table == NULL) ? String::Format(j) : table->GetAttributeName(j, masked));

  // Function name.
  formatted += GetName();
  formatted += " =\n";

  String indent(' ', 2);
  String intersep(' ' + String(inter, 1) + '\n');
  String intrasep(' ' + String(intra, 1) + ' ');

  // Function body.
  for (i = 0; i < no_components; i++) {
    const Bits *component = GetComponent(i);
    formatted += indent;
    formatted += '(';
    int count = 0;
    for (j = 0; j < no_attributes; j++) {
      if ((*component)[j]) {
        count++;
        if (count > 1)
          formatted += intrasep;
        formatted += names[j];
      }
    }
    formatted += ')';
    if (i < no_components - 1)
      formatted += intersep;
  }

  formatted += "\nend";

  // Reclaim memory.
  if (!formatted.Shrink())
    return false;

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
BooleanFunction::Format(char inter, char intra, const DecisionTable *table, bool masked) const {

  String formatted;

  if (!Format(formatted, inter, intra, table, masked))
    formatted = Undefined::String();

  return formatted;

}

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...: Initializes the function object on the basis of
//                 a string description.
// Comments......:
// Revisions.....:
//===================================================================

bool
BooleanFunction::Create(const String &function, char inter, char intra, const DecisionTable &table, bool masked) {

	int i;

  // Reset current object state.
  Clear();

  String name, body;

  // Parse name.
  if (!ParseName(function, name, body) || !SetName(name))
    return false;

  Vector(String) components;

  // Parse body.
  if (!ParseBody(body, inter, components))
    return false;

  // Parse components.
  for (i = 0; i < components.size(); i++) {
    Vector(int) variables;
    if (!ParseComponent(components[i], intra, variables, &table, masked))
      return false;
    Bits bits(variables, table.GetNoAttributes(masked));
    if (!AppendComponent(&bits, true, 1.0))
      return false;
  }

  return IsValid();

}

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...: Initializes the function object on the basis of
//                 a string description.
// Comments......:
// Revisions.....:
//===================================================================

bool
BooleanFunction::Create(const String &function, char inter, char intra, Map(String, int) &names) {

  // Reset current object state.
  Clear();

  if (function.IsEmpty())
    return false;

  String name, body;

  // Parse name.
  if (!ParseName(function, name, body) || !SetName(name))
    return false;

  Vector(String) components;

  // Parse body.
  if (!ParseBody(body, inter, components))
    return false;

  Vector(Vector(int)) variables;
  variables.reserve(components.size());

  int i;

  // Parse components.
  for (i = 0; i < components.size(); i++) {
    Vector(int) vars;
    if (!ParseComponent(components[i], intra, vars, NULL, true, &names))
      return false;
    variables.push_back(vars);
  }

  // Append components, now that we know how many variables there are.
  for (i = 0; i < variables.size(); i++) {
    Bits bits(variables[i], names.size());
    if (!AppendComponent(&bits, true, 1.0))
      return false;
  }

  return IsValid();

}

//-------------------------------------------------------------------
// Method........: ParseName
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Called from Create method.
// Revisions.....:
//===================================================================

bool
BooleanFunction::ParseName(const String &function, String &name, String &body) const {

  char separator = '=';

  // Split.
  if (!function.Contains(separator)) {
    name = Undefined::String();
    body = function;
  }
  else {
    name = function.Before(separator);
    body = function.After(separator);
  }

  // Trim.
  name.Trim(" /t/n");
  body.Trim(" /t/n");

  return true;

}

//-------------------------------------------------------------------
// Method........: ParseBody
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Called from the Create method.
// Revisions.....:
//===================================================================

bool
BooleanFunction::ParseBody(const String &body, char separator, Vector(String) &components) const {

  // Initialize vector.
  components.erase(components.begin(), components.end());
  components.reserve(body.Count(separator) + 1);

  String component;
  String sepstring(separator, 1);

  // Extract trimmed components.
  while (body.GetToken(component, sepstring.GetBuffer())) {
    component.Trim("() \t\n");
    components.push_back(component);
  }

  return true;

}

//-------------------------------------------------------------------
// Method........: ParseComponent
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Called from the Create method.
//
//                 If no table is provided (but a name map is), then
//                 variables are indexed as 0, 1, etc., with the name
//                 map mapping from names (as they appear in the
//                 string) to their assigned integer values.
//
// Revisions.....:
//===================================================================

bool
BooleanFunction::ParseComponent(const String &component, char separator, Vector(int) &variables, const DecisionTable *table, bool masked, Map(String, int) *names) const {

  // Initialize vector.
  variables.erase(variables.begin(), variables.end());
  variables.reserve(component.Count(separator) + 1);

  String variable;
  String sepstring(separator, 1);

  // Extract variables.
  while (component.GetToken(variable, sepstring.GetBuffer())) {

    // Trim spurious whitespace.
    variable.Trim(" \t\n");

    int index = Undefined::Integer();

    // Convert variable name to integer.
    if (table != NULL) {
      index = table->GetAttributeIndex(variable, true, masked);
      if (index == Undefined::Integer())
        index = table->GetAttributeIndex(variable, false, masked);
    }

    // Lookup in name map, and possibly update it?
    if (index == Undefined::Integer() && names != NULL) {
      Map(String, int)::iterator iterator = names->find(variable);
      if (!(iterator == names->end())) {
        index = (*iterator).second;
      }
      else {
        index = names->size();
        names->insert(Pair(const String, int)(variable, index));
      }
    }

    // Try direct translation?
    if (index == Undefined::Integer() && variable.IsInteger())
      index = variable.GetInteger();

    // Did conversion fail?
    if (index == Undefined::Integer()) {
      Message::Error("Failed to parse \"" + variable +"\".");
      return false;
    }

    variables.push_back(index);

  }

  return true;

}
 
