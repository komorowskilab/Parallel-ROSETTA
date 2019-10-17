// Hint.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHint
//
// Passed when views are updated so that a view can decide if/how it
// should be updated.  Also passed is a pointer value, pointing to the 
// identifier object that has been changed.

class CHint : public CObject {
public:

	// Enumerated hint types.
  enum HintType {        

		HINT_NAMECHANGE,       // Name of identifier has changed.

		HINT_NEW,              // A new identfier has been created, typically as the result of an Apply operation.
		HINT_MODIFIED,         // An identifier has been modified, typically as the result of an Apply operation
		                       // or as the result of some user action.

		HINT_GRID_ROW_DEL,     // A grid's row was deleted.
		HINT_GRID_ROW_INS,     // A grid's row was inserted/appended.
		HINT_GRID_ROW_SORT,    // A grid's rows were sorted.

		HINT_GRID_COL_MASK,    // A grid's column was disabled or enabled.
		HINT_GRID_COL_STAT,    // A grid's status was changed.
		HINT_GRID_COL_DEL,     // A grid's column was deleted.
		HINT_GRID_COL_INS,     // A grid's column was inserted/appended.
		HINT_GRID_COL_SWAP,    // A grid's column pair was swapped.
		HINT_GRID_COL_NAME,    // A grid's column was renamed.

		HINT_GRID_CELLCHANGE   // A grid's cell was changed.

	};

public:

	// Constructors/destructor.
	CHint();
	CHint(HintType hint, int row = 0, int column = 0) : m_Hint(hint) , m_Row(row), m_Column(column) {};
	virtual ~CHint();

public:

	HintType m_Hint;       // The hint itself.

	int m_Row;             // Further hint data, grid row index.
	int m_Column;          // Further hint data, grid column index.

};

/////////////////////////////////////////////////////////////////////////////
 
