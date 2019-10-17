#include <stdafx.h>

#include <frontend\rosetta.h>
#include <frontend\rosettadoc.h>

#include <frontend\views\viewpartition.h>
#include <frontend\views\hint.h>

#include <kernel\structures\equivalenceclass.h>
#include <kernel\structures\equivalenceclasses.h>

#include <kernel\basic\message.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewPartition

IMPLEMENT_DYNCREATE(CViewPartition, CViewGrid)

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

CViewPartition::CViewPartition()
{

	m_UseOffset     = TRUE;

}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

CViewPartition::~CViewPartition()
{
}

BEGIN_MESSAGE_MAP(CViewPartition, CViewGrid)
	//{{AFX_MSG_MAP(CViewPartition)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------
// Method........: OnDraw
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewPartition::OnDraw(CDC* pDC)
{
  CViewGrid::OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CViewPartition diagnostics

#ifdef _DEBUG
void CViewPartition::AssertValid() const
{
	CViewGrid::AssertValid();
}

void CViewPartition::Dump(CDumpContext& dc) const
{
	CViewGrid::Dump(dc);
}
#endif //_DEBUG

//-------------------------------------------------------------------
// Method........: OnInitialUpdate
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewPartition::OnInitialUpdate() 
{
	CViewGrid::OnInitialUpdate();
}

//-------------------------------------------------------------------
// Method........: SetStructure
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewPartition::SetStructure(Structure *structure)
{

	m_Partition = dynamic_cast(EquivalenceClasses *, structure);
	
	CViewGrid::SetStructure(structure);
	
	SetReadOnly(TRUE);
	
}

//-------------------------------------------------------------------
// Method........: InitializeGridAppearance
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....: 
//===================================================================

void CViewPartition::InitializeGridAppearance()
{

	CViewGrid::InitializeGridAppearance();

	// Is a partition attached to the view?
  if (m_Partition.IsNULL())
		return;

	// Get grid control parameters.
	CGXGridParam *params = GetParam();

	// Disable some stuff.
  params->EnableMoveRows(FALSE);
  params->EnableMoveCols(FALSE);

	// Set window text.
  GetParentFrame()->SetWindowText((m_Partition->GetName()).GetBuffer());

	BOOL bLockOld = LockUpdate(TRUE);

	// Set the number of rows and columns.
  SetColCount(2);
  SetRowCount(m_Partition->GetNoEquivalenceClasses());

	// Set column headers.
  SetStyleRange(CGXRange(0, 1), CGXStyle().SetValue(String("Eq. class").GetBuffer()));
  SetStyleRange(CGXRange(0, 2), CGXStyle().SetValue(String("Cardinality").GetBuffer()));

  LockUpdate(bLockOld);
	
}

//-------------------------------------------------------------------
// Method........: LoadDataIntoGrid
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....: 
//===================================================================

void CViewPartition::LoadDataIntoGrid()
{

	// Is a partition attached to the view?
	if (m_Partition.IsNULL())
		return;

	// Get the number of eq. classes.
	int no_eqclasses = m_Partition->GetNoEquivalenceClasses();

	// Alter front-end dimensions if the grid's size is too small/large.
	if ((GetRowCount() != no_eqclasses) || (GetColCount() != 2))
		InitializeGridAppearance();

	BOOL bLockOld = LockUpdate(TRUE);

	CWaitCursor wait;
	Message     message;

	// Determine offset value.
	int offset = m_UseOffset ? 1 : 0;

	int i;

	// Fill the front-end grid with data from kernel.
	for (i = 0; i < no_eqclasses; i++) {

		// Cancel operation?
		if ((i == 0) || ((i + 1) % 50 == 0) || (i == no_eqclasses - 1)) {
			if (!message.Progress("Preparing partition for display...", i, no_eqclasses)) {
				Message::Warning("Equivalence class " + String::Format(i + 1) + " through " + String::Format(no_eqclasses + 1) + " will not be displayed properly.");
        break;
      }
		}

		Handle<EquivalenceClass> eqclass = m_Partition->GetEquivalenceClass(i);

		// Fill the formatted eq. class into the grid.
		String formatted;
		if (!eqclass->Format(formatted, offset))
			formatted = "Formatting error.";
    SetValueRange(CGXRange(i + 1, 1), formatted.GetBuffer());

		// Fill the rule's strengths into the grid.
		String cardinality = String::Format(eqclass->GetNoObjects());
    SetValueRange(CGXRange(i + 1, 2), cardinality.GetBuffer());
	}

	// Set the row count if the operation was aborted.
	if (i < no_eqclasses)
		SetRowCount(i);

	ResizeColWidthsToFit(CGXRange(0, 1, GetRowCount(), GetColCount()));
			
  LockUpdate(bLockOld);
	
	Redraw();

}

//-------------------------------------------------------------------
// Method........: OnCmdMsg
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

BOOL CViewPartition::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{

	if (pHandlerInfo != NULL)
		return CViewGrid::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
    
  if ((nID > CVIEWPARTITION_FIRSTVALUE) && (nID < CVIEWPARTITION_LASTVALUE)) {
		if (nCode == CN_COMMAND) {
      switch (nID) {
				case CVIEWPARTITION_GRID_USEOFFSET:       OnUseOffset();              return TRUE;
				case CVIEWPARTITION_COL_SORT_INDICES:     OnSortIndices(TRUE);        return TRUE;
				case CVIEWPARTITION_COL_SORT_CARDINALITY: OnSortByCardinality(FALSE); return TRUE;
      }
    }
    else if (nCode == CN_UPDATE_COMMAND_UI) {
    }
  }

	// The command message may be handled higher up in the hierarchy.
	return CViewGrid::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

}

//-------------------------------------------------------------------
// Method........: OnUpdate
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewPartition::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CViewGrid::OnUpdate(pSender, lHint, pHint);
}

//-------------------------------------------------------------------
// Method........: CreateGridMenu
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

CMenu *CViewPartition::CreateGridMenu(CMenu *menu)
{

	if (menu == NULL)
		return NULL;

	UINT IS_CHECKED = MF_CHECKED;

	if (!m_UseOffset)
		IS_CHECKED = MF_UNCHECKED;

  menu->CreatePopupMenu();

	menu->AppendMenu(MF_STRING | MF_ENABLED | IS_CHECKED, CVIEWPARTITION_GRID_USEOFFSET,  "Use offset");
  menu->AppendMenu(MF_SEPARATOR);
  menu->AppendMenu(MF_STRING | MF_ENABLED,              CVIEWGRID_GRID_RESIZETOFIT,     "Resize to fit");
  menu->AppendMenu(MF_STRING | MF_ENABLED,              CVIEWGRID_GRID_DISPLAYSETTINGS, "Properties...");

	return menu;
 
}

//-------------------------------------------------------------------
// Method........: CreateColumnMenu
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

CMenu *CViewPartition::CreateColumnMenu(CMenu *menu, ROWCOL nCol)
{

	if (menu == NULL)
		return NULL;

	// If it's the eq. class column...
	if (nCol == 1) {
		menu->CreatePopupMenu();
		menu->AppendMenu(MF_STRING | MF_ENABLED, CVIEWPARTITION_COL_SORT_INDICES, "Sort internally");
		return menu;
	}

	// If it's the cardinality column...
	if (nCol == 2) {
		menu->CreatePopupMenu();
		menu->AppendMenu(MF_STRING | MF_ENABLED, CVIEWPARTITION_COL_SORT_CARDINALITY, "Sort");
		return menu;
	}

	return NULL;
 
}

//-------------------------------------------------------------------
// Method........: OnUseOffset
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewPartition::OnUseOffset()
{

	// Get current read-only lock status.
	BOOL oldLockReadOnly = GetParam()->IsLockReadOnly();

	// We may have to alter read-only cells.
	GetParam()->SetLockReadOnly(FALSE);

	// Invert current offset mode.
  m_UseOffset = !m_UseOffset;

	// Reload kernel data into front-end grid, using new offset mode.
  LoadDataIntoGrid();

	// Revert to old read-only lock status.
	GetParam()->SetLockReadOnly(oldLockReadOnly);

	Redraw();

}

//-------------------------------------------------------------------
// Method........: OnSortIndices
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewPartition::OnSortIndices(BOOL ascending)
{

	// Get current read-only lock status.
	BOOL oldLockReadOnly = GetParam()->IsLockReadOnly();

	// We may have to alter read-only cells.
	GetParam()->SetLockReadOnly(FALSE);

	int no_eqclasses = m_Partition->GetNoEquivalenceClasses();
	int i;

	// Sort the equivalence classes.
	for (i = 0; i < no_eqclasses; i++)
		m_Partition->GetEquivalenceClass(i)->Sort(ascending);

	m_Partition->Touch("Sorted equivalence classes internally (" + String((ascending) ? "ascending" : "descending") + ").");

	// Reload kernel data into front-end grid, now that they have been sorted.
  LoadDataIntoGrid();

	// Revert to old read-only lock status.
	GetParam()->SetLockReadOnly(oldLockReadOnly);

	Redraw();

	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);

	// Update other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_Partition.GetPointer());
	CHint  pHint(CHint::HINT_MODIFIED, 0, 0);  // Should be more specific...

	GetDocument()->UpdateAllViews(this, lHint, &pHint);

}

//-------------------------------------------------------------------
// Method........: OnSortByCardinality
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewPartition::OnSortByCardinality(BOOL ascending)
{

	// Get current read-only lock status.
	BOOL oldLockReadOnly = GetParam()->IsLockReadOnly();

	// We may have to alter read-only cells.
	GetParam()->SetLockReadOnly(FALSE);

	// Sort the equivalence classes.
	m_Partition->Sort(ascending);

	m_Partition->Touch("Sorted equivalence classes by cardinality (" + String((ascending) ? "ascending" : "descending") + ").");

	// Reload kernel data into front-end grid, now that they have been sorted.
  LoadDataIntoGrid();

	// Revert to old read-only lock status.
	GetParam()->SetLockReadOnly(oldLockReadOnly);

	Redraw();

	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);

	// Update other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_Partition.GetPointer());
	CHint  pHint(CHint::HINT_MODIFIED, 0, 0);  // Should be more specific...

	GetDocument()->UpdateAllViews(this, lHint, &pHint);

}
 
