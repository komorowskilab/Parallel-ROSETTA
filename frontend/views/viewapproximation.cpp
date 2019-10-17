#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\views\viewapproximation.h>
#include <frontend\views\hint.h>

#include <kernel\structures\approximation.h>
#include <kernel\structures\equivalenceclass.h>

#include <kernel\basic\message.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewApproximation

IMPLEMENT_DYNCREATE(CViewApproximation, CViewGrid)

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

CViewApproximation::CViewApproximation()
{

	m_UseOffset = TRUE;

}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

CViewApproximation::~CViewApproximation()
{
}

BEGIN_MESSAGE_MAP(CViewApproximation, CViewGrid)
	//{{AFX_MSG_MAP(CViewApproximation)
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

void CViewApproximation::OnDraw(CDC* pDC)
{
  CViewGrid::OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CViewApproximation diagnostics

#ifdef _DEBUG
void CViewApproximation::AssertValid() const
{
	CViewGrid::AssertValid();
}

void CViewApproximation::Dump(CDumpContext& dc) const
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

void CViewApproximation::OnInitialUpdate() 
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

void CViewApproximation::SetStructure(Structure *structure)
{

	m_Approximation = dynamic_cast(Approximation *, structure);
	
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

void CViewApproximation::InitializeGridAppearance()
{

	CViewGrid::InitializeGridAppearance();

	// Is an approximation attached to the view?
  if (m_Approximation.IsNULL())
		return;

	// Get grid control parameters.
	CGXGridParam *params = GetParam();

	// Disable some stuff.
  params->EnableMoveRows(FALSE);
  params->EnableMoveCols(FALSE);

	// Set window text.
  GetParentFrame()->SetWindowText((m_Approximation->GetName()).GetBuffer());

	BOOL bLockOld = LockUpdate(TRUE);

	// Set the number of rows and columns.
  SetColCount(5);
  SetRowCount(m_Approximation->GetUniverse()->GetNoEquivalenceClasses());

	// Set column headers.
  SetStyleRange(CGXRange(0, 1), CGXStyle().SetValue(String("Universe").GetBuffer()));
  SetStyleRange(CGXRange(0, 2), CGXStyle().SetValue(String("Upper").GetBuffer()));
  SetStyleRange(CGXRange(0, 3), CGXStyle().SetValue(String("Lower").GetBuffer()));
  SetStyleRange(CGXRange(0, 4), CGXStyle().SetValue(String("Boundary").GetBuffer()));
  SetStyleRange(CGXRange(0, 5), CGXStyle().SetValue(String("Outside").GetBuffer()));

  LockUpdate(bLockOld);
	
}

//-------------------------------------------------------------------
// Method........: LoadDataIntoGrid
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....: 
//===================================================================

void CViewApproximation::LoadDataIntoGrid()
{

	// Is an approximation attached to the view?
	if (m_Approximation.IsNULL())
		return;

	// Get the approximated sets.
	Handle<EquivalenceClasses> universe = const_cast(EquivalenceClasses *, m_Approximation->GetUniverse());
	Handle<EquivalenceClasses> upper    = const_cast(EquivalenceClasses *, m_Approximation->GetUpperApproximation());
	Handle<EquivalenceClasses> lower    = const_cast(EquivalenceClasses *, m_Approximation->GetLowerApproximation());
	Handle<EquivalenceClasses> boundary = const_cast(EquivalenceClasses *, m_Approximation->GetBoundaryRegion());
	Handle<EquivalenceClasses> outside  = const_cast(EquivalenceClasses *, m_Approximation->GetOutsideRegion());

	// Get the number of eq. classes.
	int no_universe = universe->GetNoEquivalenceClasses();
	int no_upper    = upper->GetNoEquivalenceClasses();
	int no_lower    = lower->GetNoEquivalenceClasses();
	int no_boundary = boundary->GetNoEquivalenceClasses();
	int no_outside  = outside->GetNoEquivalenceClasses();

	// Alter front-end dimensions if the grid's size is too small/large.
	if ((GetRowCount() != no_universe) || (GetColCount() != 5))
		InitializeGridAppearance();

	BOOL bLockOld = LockUpdate(TRUE);

	CWaitCursor wait;
	Message     message;

	// Determine offset value.
	int offset = m_UseOffset ? 1 : 0;

	int i;

	// Fill the front-end grid with data from kernel.
	for (i = 0; i < no_universe; i++) {

		// Cancel operation?
		if ((i == 0) || ((i + 1) % 50 == 0) || (i == no_universe - 1)) {
			if (!message.Progress("Preparing approximation for display...", i, no_universe)) {
				Message::Warning("Equivalence classes " + String::Format(i + 1) + " through " + String::Format(no_universe + 1) + " will not be displayed properly.");
        break;
      }
		}

		Handle<EquivalenceClass> eqclass;
		String                   formatted;

		// Fill universe column.
		eqclass = universe->GetEquivalenceClass(i);
		if (!eqclass->Format(formatted, offset))
			formatted = "Formatting error.";
    SetValueRange(CGXRange(i + 1, 1), formatted.GetBuffer());

		// Fill upper column.
		if (i < no_upper) {
			eqclass = upper->GetEquivalenceClass(i);
			if (!eqclass->Format(formatted, offset))
				formatted = "Formatting error.";
		}
		else {
			formatted = "";
		}
    SetValueRange(CGXRange(i + 1, 2), formatted.GetBuffer());

		// Fill lower column.
		if (i < no_lower) {
			eqclass = lower->GetEquivalenceClass(i);
			if (!eqclass->Format(formatted, offset))
				formatted = "Formatting error.";
		}
		else {
			formatted = "";
		}
    SetValueRange(CGXRange(i + 1, 3), formatted.GetBuffer());

		// Fill boundary column.
		if (i < no_boundary) {
			eqclass = boundary->GetEquivalenceClass(i);
			if (!eqclass->Format(formatted, offset))
				formatted = "Formatting error.";
		}
		else {
			formatted = "";
		}
    SetValueRange(CGXRange(i + 1, 4), formatted.GetBuffer());

		// Fill outside column.
		if (i < no_outside) {
			eqclass = outside->GetEquivalenceClass(i);
			if (!eqclass->Format(formatted, offset))
				formatted = "Formatting error.";
		}
		else {
			formatted = "";
		}
    SetValueRange(CGXRange(i + 1, 5), formatted.GetBuffer());
	}

	// Set the row count if the operation was aborted.
	if (i < no_universe)
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

BOOL CViewApproximation::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{

	if (pHandlerInfo != NULL)
		return CViewGrid::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
    
  if ((nID > CVIEWAPPROXIMATION_FIRSTVALUE) && (nID < CVIEWAPPROXIMATION_LASTVALUE)) {
		if (nCode == CN_COMMAND) {
      switch (nID) {
				case CVIEWAPPROXIMATION_GRID_USEOFFSET: OnUseOffset(); return TRUE;
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

void CViewApproximation::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
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

CMenu *CViewApproximation::CreateGridMenu(CMenu *menu)
{

	if (menu == NULL)
		return NULL;

	UINT IS_CHECKED = MF_CHECKED;

	if (!m_UseOffset)
		IS_CHECKED = MF_UNCHECKED;

  menu->CreatePopupMenu();

	menu->AppendMenu(MF_STRING | MF_ENABLED | IS_CHECKED, CVIEWAPPROXIMATION_GRID_USEOFFSET, "Use offset");
  menu->AppendMenu(MF_SEPARATOR);
  menu->AppendMenu(MF_STRING | MF_ENABLED,              CVIEWGRID_GRID_RESIZETOFIT,        "Resize to fit");
  menu->AppendMenu(MF_STRING | MF_ENABLED,              CVIEWGRID_GRID_DISPLAYSETTINGS,    "Properties...");

	return menu;
 
}

//-------------------------------------------------------------------
// Method........: OnUseOffset
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewApproximation::OnUseOffset()
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
 
