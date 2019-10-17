//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/confusionmatrix.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/creator.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Static methods (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticInteger/StaticFloat
// Author........: Aleksander šhrn
// Date..........:
// Description...: For fixed-width formatting.
// Comments......:
// Revisions.....:
//===================================================================

static String
StaticInteger(int i, int width) {

	String formatted;

	// Initialize.
	if (i == Undefined::Integer())
		formatted = Undefined::String();
	else
		formatted = String::Format(i);

	// Pad or truncate.
	formatted.Pad(' ', width);

	return formatted;

}

static String
StaticFloat(float f, int width) {

	String formatted;

	// Initialize.
	if (f == Undefined::Float())
		formatted = Undefined::String();
	else
		formatted = String::Format(100 * f) + '%';

	// Pad or truncate.
	formatted.Pad(' ', width);

	if (f == Undefined::Float())
		return formatted;

	if (formatted.Last() != ' ')
		formatted[formatted.GetLength() - 1] = '%';

	return formatted;

}

//-------------------------------------------------------------------
// Methods for class ConfusionMatrix.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Copy constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

ConfusionMatrix::ConfusionMatrix(const ConfusionMatrix &in) : AnnotatedStructure(in) {

	// Duplicate matrix.
	if (!in.matrix_.IsNULL())
		matrix_ = dynamic_cast(DecisionTable *, in.matrix_->Duplicate());
	else
		matrix_ = NULL;

	// Duplicate map.
	if (in.map_ == NULL)
		map_ = NULL;
	else
		map_ = new IMap(*(in.map_));

}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Annotation not needed, superstructure should have
//                 one.
// Revisions.....:
//===================================================================

ConfusionMatrix::ConfusionMatrix() {

	// Create the physical representation.
	matrix_    = Creator::DecisionTable();
	map_       = NULL;

	matrix_->SetDictionary(NULL);

	// No annotation needed.
	SetAnnotation(NULL);

}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Annotation not needed, superstructure should have
//                 one.
// Revisions.....:
//===================================================================

ConfusionMatrix::ConfusionMatrix(int dimension) {

	// Create the physical representation.
	matrix_    = Creator::DecisionTable();
	map_       = NULL;

	matrix_->SetDictionary(NULL);

	// Adjust dimensionality.
	SetDimension(dimension);

	// No annotation needed.
	SetAnnotation(NULL);

  // Reset entries.
	Clear();

}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

ConfusionMatrix::~ConfusionMatrix() {
	delete map_;
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ConfusionMatrix, CONFUSIONMATRIX, AnnotatedStructure)

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
ConfusionMatrix::Load(ifstream &stream) {

	// Load data higher up in the hierarchy.
	if (!AnnotatedStructure::Load(stream))
		return false;

	// Reset old physical representation.
	matrix_ = NULL;
	matrix_ = Creator::DecisionTable();
	matrix_->SetDictionary(NULL);

	delete map_;
	map_ = NULL;

	int size, mapping;

	// Load map size.
	if (!IOKit::Load(stream, size))
		return false;

	int i;

	// Load map.
	for (i = 0; i < size; i++) {
		if (!IOKit::Load(stream, mapping))
			return false;
		if (!SetMap(i, mapping))
			return false;
	}

	// Load actual matrix.
	if (!matrix_->Load(stream))
		return false;

	return true;

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
ConfusionMatrix::Save(ofstream &stream) const {

	// Save data higher up in the hierarchy.
	if (!AnnotatedStructure::Save(stream))
		return false;

	// Save size.
	if (!IOKit::Save(stream, GetDimension()))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	int i;

	// Save map.
	for (i = 0; i < GetDimension(); i++) {
		if (!IOKit::Save(stream, GetValue(i)))
			return false;
	}

	if (!IOKit::Save(stream, '\n'))
		return false;

	// Save actual matrix.
	if (!matrix_->Save(stream))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

//-------------------------------------------------------------------
// Method........: Duplicate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
ConfusionMatrix::Duplicate() const {
	return new ConfusionMatrix(*this);
}

//-------------------------------------------------------------------
// Method........: Clear
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
ConfusionMatrix::Clear() {

	int i, j;

	// Reset all entries.
	for (i = 0; i < GetDimension(); i++) {
		for (j = 0; j < GetDimension(); j++) {
			SetEntry(i, j, 0);
		}
	}

	// Clear stuff higher up.
	AnnotatedStructure::Clear();

}

//-------------------------------------------------------------------
// Operators.
//===================================================================

//-------------------------------------------------------------------
// Operator......: +=
// Author........: Susanne Bornelöv
// Date..........:
// Description...: Add one confusion matrix to another
// Comments......: 
// Revisions.....:
//===================================================================

ConfusionMatrix &
ConfusionMatrix::operator+=(const ConfusionMatrix &in) {
	*this = *this + in;
	return *this;
}

//-------------------------------------------------------------------
// Operator......: +
// Author........: Susanne Bornelöv
// Date..........:
// Description...: Merge two confusion matrices
// Comments......:
// Revisions.....:
//===================================================================

ConfusionMatrix &
ConfusionMatrix::operator+(const ConfusionMatrix &in) {

	// Define new confusion matrix
	ConfusionMatrix * sum = dynamic_cast(ConfusionMatrix*,new ConfusionMatrix());

	// Find out new dimension
	IMap dim;
	for (int i = 0; i < in.GetDimension(); i++)
		dim.insert(Pair(int,int) (in.GetValue(i),i));
	for (int i = 0; i < this->GetDimension(); i++)
		dim.insert(Pair(int,int) (this->GetValue(i),i));

	int dimension = dim.size();

	// Set new confusion matrix dimension
	sum->SetDimension(dimension);

	// Create new map out of all unique entries in both maps
	int index = 0;
	for (int i = 0; i < in.GetDimension(); i++)
		sum->SetMap(index++,in.GetValue(i));
	for (int i = 0; i < this->GetDimension(); i++)
		if (sum->GetIndex(this->GetValue(i)) == Undefined::Integer())
				sum->SetMap(index++,this->GetValue(i));

	if (index != dimension)
		Message::Warning("Error in confusion matrix dimensions",false);	

	int ii,jj;

	// Increment values	
	for (int i = 0; i < dimension; i++) {
		for (int j = 0; j < dimension; j++) {
			int value = 0; // Value of entry i,j in confusion matrix

			// Map from index to value(=name)
			int vali = sum->GetValue(i);
			int valj = sum->GetValue(j);

			// The first GetDimension() entries in sum->map_ are equal to that of in.map_ ...
			if (i < in.GetDimension() && j < in.GetDimension())
				value += in.GetEntry(i,j);
			// ... but for this->map_ we need to do mapping.
			if (this->GetIndex(vali) != Undefined::Integer() && this->GetIndex(valj) != Undefined::Integer()) {
				ii = this->GetIndex(vali);
				jj = this->GetIndex(valj);
				value += this->GetEntry(ii,jj);
			}
			sum->SetEntry(i,j,value);
		}
	}

	// This should be done elsewhere...
	sum->SetAnnotation(NULL);

  *this = *sum;
  return *this;
}


//-------------------------------------------------------------------
// Operator......: =
// Author........: Aleksander šhrn
// Date..........:
// Description...: Assignment operator.
// Comments......:
// Revisions.....:
//===================================================================

ConfusionMatrix &
ConfusionMatrix::operator=(const ConfusionMatrix &in) {

	// Protect against self-assignment.
	if (this == &in)
		return *this;

	// Duplicate physical representation.
	if (in.matrix_.IsNULL())
		matrix_ = NULL;
	else
		matrix_ = dynamic_cast(DecisionTable *, in.matrix_->Duplicate());

	if (in.map_ == NULL)
		map_ = NULL;
	else
		map_ = new IMap(*(in.map_));

	// This should be done elsewhere...
	SetAnnotation(NULL);

  return *this;

}

//-------------------------------------------------------------------
// Operator......: ==
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 990721: For map comparison, changed (.. != ..)
//                            !(.. == ..). Otherwise, the compiler
//                            complained when I upgraded to STLport
//                            3.2...
//===================================================================

bool
ConfusionMatrix::operator==(const ConfusionMatrix &in) const {

  // Are the two matrices physically equal?
  if (&in == this)
    return true;

	// Are the physical representations unequal?
	if (*matrix_ != *(in.matrix_))
		return false;

	// Are the maps unequal?
	if (map_ != NULL && in.map_ != NULL && !(*map_ == *(in.map_)))
		return false;

  return true;

}

//-------------------------------------------------------------------
// Operator......: !=
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
ConfusionMatrix::operator!=(const ConfusionMatrix &in) const {
  return !(*this == in);
}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: SetDimension
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
ConfusionMatrix::SetDimension(int dimension) {

	if (matrix_.IsNULL())
		return false;

#if 0
	// Get the current dimension.
	int current = GetDimension();
#endif
	
	// Append or remove rows/columns.
	matrix_->Resize(dimension, dimension, false);

	int i, j;

	// Set all entries to 0.
	for (i = 0; i < matrix_->GetNoObjects(false); i++) {
		for (j = 0; j < matrix_->GetNoAttributes(false); j++) {
			matrix_->SetEntry(i, j, 0, false);
		}
	}

	// Clear the current map.
	delete map_;
	map_ = NULL;

	return true;

}

//-------------------------------------------------------------------
// Method........: GetSum
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the sum of all entries.
// Comments......:
// Revisions.....:
//===================================================================

int
ConfusionMatrix::GetSum() const {

	int i, sum = 0, dimension = GetDimension();

	for (i = 0; i < dimension; i++)
		sum += GetRowSum(i);

	return sum;

}

//-------------------------------------------------------------------
// Method........: GetRowSum
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the sum of all entries in the given row.
// Comments......:
// Revisions.....:
//===================================================================

int
ConfusionMatrix::GetRowSum(int i) const {

	int j, sum = 0, dimension = GetDimension();

	for (j = 0; j < dimension; j++)
		sum += GetEntry(i, j);

	return sum;

}

//-------------------------------------------------------------------
// Method........: GetColumnSum
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the sum of all entries in the given column.
// Comments......:
// Revisions.....:
//===================================================================

int
ConfusionMatrix::GetColumnSum(int j) const {

	int i, sum = 0, dimension = GetDimension();

	for (i = 0; i < dimension; i++)
		sum += GetEntry(i, j);

	return sum;

}

//-------------------------------------------------------------------
// Method........: GetDiagonalSum
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the sum of all entries on the diagonal.
// Comments......:
// Revisions.....:
//===================================================================

int
ConfusionMatrix::GetDiagonalSum() const {

	int i, sum = 0, dimension = GetDimension();

	for (i = 0; i < dimension; i++)
		sum += GetEntry(i, i);

	return sum;

}

//-------------------------------------------------------------------
// Method........: GetRowRatio
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the ratio between element (i, i) and the
//                 sum of all entries in row i.
// Comments......:
// Revisions.....:
//===================================================================

float
ConfusionMatrix::GetRowRatio(int i) const {

	int sum = GetRowSum(i);

	if (sum == 0)
		return Undefined::Float();

	return static_cast(float, GetEntry(i, i)) / sum;

}

//-------------------------------------------------------------------
// Method........: GetColumnRatio
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the ratio between element (j, j) and the
//                 sum of all entries in column j.
// Comments......:
// Revisions.....:
//===================================================================

float
ConfusionMatrix::GetColumnRatio(int j) const {

	int sum = GetColumnSum(j);

	if (sum == 0)
		return Undefined::Float();

	return static_cast(float, GetEntry(j, j)) / sum;

}

//-------------------------------------------------------------------
// Method........: GetDiagonalRatio
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the ratio between the diagonal sum and the
//                 total sum.
// Comments......:
// Revisions.....:
//===================================================================

float
ConfusionMatrix::GetDiagonalRatio() const {

	int sum = GetSum();

	if (sum == 0)
		return Undefined::Float();

	return static_cast(float, GetDiagonalSum()) / sum;

}

//-------------------------------------------------------------------
// Method........: SetMap
// Author........: Aleksander šhrn
// Date..........:
// Description...: Assigns a pair in the index/"meaning" mapping.
//                 A mapping is needed to map between the matrix
//                 indices and their "meaning".  For instance, the
//                 matrix indices may run from 0 to 3, but their
//                 "meaning" does not necessarily have to be
//                 {0, 1, 2, 3}, but could e.g. be {-18, 3, 7, 12}.
// Comments......:
// Revisions.....:
//===================================================================

bool
ConfusionMatrix::SetMap(int index, int meaning) {

	// Instantiate the map if this is the first time around.
	if (map_ == NULL)
		map_ = new IMap;

	// Check if the index already exists in the map.
	IMap::iterator iterator = map_->find(index);

	if (!(iterator == map_->end())) {
		(*iterator).second = meaning;
		return true;
	}

	// Add a new entry to the map.
	if (!(map_->insert(Pair(const int, int)(index, meaning))).second) {
		Message::Error("Failed to add pair (" + String::Format(index) + ", " + String::Format(meaning) + ") to confusion matrix map.");
		return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: GetValue
// Author........: Aleksander šhrn
// Date..........:
// Description...: Maps from a matrix index to its integral "meaning".
// Comments......:
// Revisions.....:
//===================================================================

int
ConfusionMatrix::GetValue(int index) const {

	// If no map is present, assume the mapping is 1-1.
	if (map_ == NULL)
		return index;

	// Check if the index exists in the map.
	// This should be a const_iterator, but VC++ 6.0 won't let me...
	IMap::iterator iterator = map_->find(index);

	if (!(iterator == map_->end())) {
		return (*iterator).second;
	}

	return Undefined::Integer();

}

//-------------------------------------------------------------------
// Method........: GetIndex
// Author........: Aleksander šhrn
// Date..........:
// Description...: Maps from an integral "meaning" to a matrix index.
// Comments......:
// Revisions.....:
//===================================================================

int
ConfusionMatrix::GetIndex(int value) const {

	// If no map is present, assume the mapping is 1-1.
	if (map_ == NULL)
		return value;

	// This should be a const_iterator, but VC++ 6.0 won't let me...
	IMap::iterator iterator = map_->begin();

	// Iterate over all entries.
	while (!(iterator == map_->end())) {
		if ((*iterator).second == value)
			return (*iterator).first;
		iterator++;
	}

	return Undefined::Integer();

}

//-------------------------------------------------------------------
// Method........: Format
// Author........: Aleksander šhrn
// Date..........:
// Description...: Fills the input string with a formatted version of
//                 the matrix.
// Comments......:
// Revisions.....:
//===================================================================

bool
ConfusionMatrix::Format(String &formatted, const String &indent) const {

	formatted = "";

	// Get dimension.
	int dimension = GetDimension();

	// Set field width.
	int width = 9;

	int i, j;

	// Format header.
	formatted += indent + String(' ', width) + " | ";
	for (i = 0; i < dimension; i++)
		formatted += StaticInteger(GetValue(i), width) + " ";
	formatted += "|\n";
	formatted += indent + String('-', 3 + (dimension + 2) * (width + 1)) + "\n";

	// Format contents.
	for (i = 0; i < dimension; i++) {
		formatted += indent + StaticInteger(GetValue(i), width) + " | ";
		for (j = 0; j < dimension; j++)
			formatted += StaticInteger(GetEntry(i, j), width) + " ";
		formatted += "| " + StaticFloat(GetRowRatio(i), width) + "\n";
	}

	// Format footer.
	formatted += indent + String('-', 3 + (dimension + 2) * (width + 1)) + "\n";
	formatted += indent + String(' ', width) + " | ";
	for (i = 0; i < dimension; i++)
		formatted += StaticFloat(GetColumnRatio(i), width) + " ";
	formatted += "| " + StaticFloat(GetDiagonalRatio(), width) + "\n";

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
ConfusionMatrix::Format(const String &indent) const {

	String formatted;

	if (!Format(formatted, indent))
		formatted = Undefined::String();

	return formatted;

}

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: A temporary hack for backwards compatibility.
//                 Remove this method after some time has passed.
// Revisions.....:
//===================================================================

bool
ConfusionMatrix::Load(ifstream &stream, bool &has_extra_data, int &attribute, int &roc_class, float &roc_area, float &roc_stderr, float &roc_threshold_nw, float &roc_threshold_accuracy) {

	// Reset old physical matrix representation.
	matrix_ = NULL;
	matrix_ = Creator::DecisionTable();
	matrix_->SetDictionary(NULL);
	delete map_;
	map_ = NULL;

	String line;

	// Load newline.
	if (!IOKit::Load(stream, line, false))
		return false;

	// Does it look like an annotation is present?
	bool has_annotation = (stream.peek() == '\"');

	// Load annotation, if present.
	if (has_annotation && !AnnotatedStructure::Load(stream))
		return false;

	int size, mapping;

	// Load map size.
	if (!IOKit::Load(stream, size))
		return false;

	int i;

	// Load map.
	for (i = 0; i < size; i++) {
		if (!IOKit::Load(stream, mapping))
			return false;
		if (!SetMap(i, mapping))
			return false;
	}

	// Load newline.
	if (!IOKit::Load(stream, line, false))
		return false;

	// Does it look like extra data is present?
	has_extra_data = (stream.peek() != '\"');

	if (has_extra_data) {

		if (!IOKit::Load(stream, line, false))
			return false;

		line.Trim(' ');

		// How many tokens are there in the line?
		int no_tokens = line.GetNoTokens(" ");

		// Does it have enough tokens, indicating that ROC data is present?
		if (no_tokens >= 3 && no_tokens <= 5) {

			int   roc_c   = Undefined::Integer();
			float roc_a   = Undefined::Float();
			float roc_s   = Undefined::Float();
			float roc_t_n = Undefined::Float();
			float roc_t_a = Undefined::Float();

			String token;

			// Yes, new (or even newer) format. Extract ROC information.
			line.GetToken(token, " ");   roc_c   = token.GetInteger();
			line.GetToken(token, " ");   roc_a   = token.GetFloat();   if (roc_a   > 1 || roc_a   < 0) roc_a   = Undefined::Float();
			line.GetToken(token, " ");   roc_t_n = token.GetFloat();   if (roc_t_n > 1 || roc_t_n < 0) roc_t_n = Undefined::Float();
			if (no_tokens >= 4) {
				line.GetToken(token, " "); roc_s   = token.GetFloat();   if (roc_s   > 1 || roc_s   < 0) roc_s   = Undefined::Float();
			}
			if (no_tokens >= 5) {
				line.GetToken(token, " "); roc_t_a = token.GetFloat();   if (roc_t_a > 1 || roc_t_a < 0) roc_t_a = Undefined::Float();
			}
			while (line.GetToken(token, " ")) {
			}

			// Set ROC stuff.
			roc_class              = roc_c;
			roc_area               = roc_a;
			roc_stderr             = roc_s;
			roc_threshold_nw       = roc_t_n;
			roc_threshold_accuracy = roc_t_a;

			// Load decision attribute index.
			if (!IOKit::Load(stream, attribute))
				return false;

		}
		else {

			// No, old format.
			has_extra_data = false;

			// This format is obsolete.
			if (no_tokens == 1)
				Message::Warning("This file format for confusion matrices is obsolete and no longer supported.");

		}

	}

	// Load actual matrix.
	if (!matrix_->Load(stream))
		return false;

	return true;

}
 
