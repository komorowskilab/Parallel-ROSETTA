// viewreducts.cpp : implementation file
//
// Comments:
// This view is read-only. If the programmer wants to change the grid, (i.e remove 
// or insert rows and columns, remember to use SetLockReadOnly(FALSE) before modifying,
// and SetLockReadOnly(TRUE) afterwards. 

#include <stdafx.h>

#include <frontend\rosetta.h>
#include <frontend\rosettadoc.h>

#include <frontend\views\viewreducts.h>
#include <frontend\views\hint.h>

#include <kernel\structures\reduct.h>
#include <kernel\structures\reducts.h>
#include <kernel\structures\decisiontable.h>
#include <kernel\structures\dictionary.h>

#include <kernel\basic\message.h>

#include <common\messagehelper.h>

//#include <frontend\system\bstring.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewReducts

IMPLEMENT_DYNCREATE(CViewReducts, CViewGrid)

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

CViewReducts::CViewReducts()
{
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

CViewReducts::~CViewReducts()
{
}

BEGIN_MESSAGE_MAP(CViewReducts, CViewGrid)
	//{{AFX_MSG_MAP(CViewReducts)
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

void CViewReducts::OnDraw(CDC* pDC)
{
  CViewGrid::OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CViewReducts diagnostics

#ifdef _DEBUG
void CViewReducts::AssertValid() const
{
	CViewGrid::AssertValid();
}

void CViewReducts::Dump(CDumpContext& dc) const
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

void CViewReducts::OnInitialUpdate() 
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

void CViewReducts::SetStructure(Structure *structure)
{

	m_Reducts = dynamic_cast(Reducts *, structure);

	CViewGrid::SetStructure(m_Reducts.GetPointer());
	
	SetReadOnly(TRUE);
	
}

//-------------------------------------------------------------------
// Method........: InitializeGridAppearance
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....: 
//===================================================================

void CViewReducts::InitializeGridAppearance()
{

	CViewGrid::InitializeGridAppearance();

	// Any reducts present?
	if (m_Reducts.IsNULL())
		return;

	// Get grid control parameters.
	CGXGridParam *params = GetParam();

	// Disable some stuff.
  params->EnableMoveRows(FALSE);
  params->EnableMoveCols(FALSE);

	// Set window text.
  GetParentFrame()->SetWindowText((m_Reducts->GetName()).GetBuffer());

  BOOL bLockOld = LockUpdate(TRUE);

	// Set the number of rows and columns.
  SetColCount(3);
  SetRowCount(m_Reducts->GetNoReducts());

	// Set column headers.
  SetStyleRange(CGXRange(0, 1), CGXStyle().SetValue(String("Reduct").GetBuffer()));
  SetStyleRange(CGXRange(0, 2), CGXStyle().SetValue(String("Support").GetBuffer()));
  SetStyleRange(CGXRange(0, 3), CGXStyle().SetValue(String("Length").GetBuffer()));

  LockUpdate(bLockOld);

}

//-------------------------------------------------------------------
// Method........: LoadDataIntoGrid
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....: 
//===================================================================

void CViewReducts::LoadDataIntoGrid()
{

	// Any reducts "attached" to the view?
  if (m_Reducts.IsNULL())
		return;

	// Get some key dimensional quantities.
	int no_reducts = m_Reducts->GetNoReducts();

	// Alter front-end dimensions if the grid's size is too small/large.
	if ((GetRowCount() != no_reducts) || (GetColCount() != 2))
		InitializeGridAppearance();

	// Get the originating dec. table.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, m_Reducts->FindParent(DECISIONTABLE, *GetTopProject()));

  BOOL bLockOld = LockUpdate(TRUE);

	CWaitCursor wait;
	Message     message;

	bool masked = true;

	// Fill the front-end grid with data from the kernel.
	for (int i = 0; i < no_reducts; i++) {

		// Progress.
		if ((i == 0) || ((i + 1) % 50 == 0) || (i == no_reducts - 1)) {
			if (!message.Progress("Preparing reducts for display...", i, no_reducts)) {
				MessageHelper::Information("Reducts " + String::Format(i + 1) + " through " + String::Format(no_reducts + 1) + " will not be displayed properly.");
				break;
			}
		}

		// Get reduct.
		Handle<Reduct> reduct = m_Reducts->GetReduct(i);

		String formatted;

		// Format reduct.
		if (!reduct->Format(formatted, m_UseDictionary ? table.GetPointer() : NULL, masked))
			formatted = "Formatting error.";

		// Get length and support.
		String length  = String::Format(reduct->GetNoAttributes());
		String support = String::Format(reduct->GetSupport());

		// Fill grid.
		SetValueRange(CGXRange(i + 1, 1), formatted.GetBuffer());
		SetValueRange(CGXRange(i + 1, 2), support.GetBuffer());
		SetValueRange(CGXRange(i + 1, 3), length.GetBuffer());

	}

	// Set the row count if the operation was aborted.
	if (i < no_reducts)
		SetRowCount(i);

	ResizeColWidthsToFit(CGXRange(0, 1, GetRowCount(), GetColCount()));

	LockUpdate(bLockOld);

	Redraw();

}

//-------------------------------------------------------------------
// Method........: CreateRowMenu
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

CMenu *CViewReducts::CreateRowMenu(CMenu *menu, ROWCOL /*nRow*/)
{

	if (menu == NULL)
		return NULL;

  menu->CreatePopupMenu();

  menu->AppendMenu(MF_STRING | MF_ENABLED, CVIEWREDUCTS_ROW_REMOVE, "Remove");

	return menu;

}

//-------------------------------------------------------------------
// Method........: CreateColumnMenu
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

CMenu *CViewReducts::CreateColumnMenu(CMenu *menu, ROWCOL nCol)
{

	if (menu == NULL)
		return NULL;

	// If it's the support column...
	if (nCol == 2) {
		menu->CreatePopupMenu();
		menu->AppendMenu(MF_STRING | MF_ENABLED, CVIEWREDUCTS_COL_SORT_SUPPORT, "Sort");
		return menu;
	}

	// If it's the length column...
	if (nCol == 3) {
		menu->CreatePopupMenu();
		menu->AppendMenu(MF_STRING | MF_ENABLED, CVIEWREDUCTS_COL_SORT_LENGTH, "Sort");
		return menu;
	}

	return NULL;
 
}

//-------------------------------------------------------------------
// Method........: OnCmdMsg
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

BOOL CViewReducts::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{

	if (pHandlerInfo != NULL)
		return CViewGrid::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
    
  if ((nID > CVIEWREDUCTS_FIRSTVALUE) && (nID < CVIEWREDUCTS_LASTVALUE)) { 
		if (nCode == CN_COMMAND) {
      switch (nID) {
				case CVIEWREDUCTS_ROW_REMOVE:       OnRowRemove();       return TRUE;
				case CVIEWREDUCTS_COL_SORT_LENGTH:  OnColSort(1, TRUE);  return TRUE;
				case CVIEWREDUCTS_COL_SORT_SUPPORT: OnColSort(0, FALSE); return TRUE;
      }
    }
    else if (nCode == CN_UPDATE_COMMAND_UI) {
    }
  }

	// The command message may be handled higher up in the hierarchy.
	return CViewGrid::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

}

//-------------------------------------------------------------------
// Method........: OnRowRemove
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

void CViewReducts::OnRowRemove()
{

	String       text;
  CRowColArray selected;
  ROWCOL       no_selected = GetSelectedRows(selected, TRUE);

	// Are any rows selected?
	if (no_selected == 0) {
		Message::Error("No rows selected.");
		return;
	}

	// Query the user.
	if (no_selected == 1)
		text = "Are you sure you want to remove row " + String::Format(static_cast(int, selected[0])) + "?";
	else
		text = String::Format(static_cast(int, no_selected)) + " rows selected.  Are you sure you want to remove them?";
	if (MessageHelper::Query(text, "Remove", MessageHelper::BUTTONS_OKCANCEL) != MessageHelper::RESPONSE_OK)
		return;

  // Allows modifying of the grid.
	GetParam()->SetLockReadOnly(FALSE);

	int range_begin = selected[no_selected - 1];
	int range_end   = selected[no_selected - 1];

	// Remove objects in kernel and front-end.
	for (int i = no_selected - 1; i >=0; i--) {                   // Loop backwards to minimize front-end updates.
		int kernel_index = selected[i] - 1;
		if (!m_Reducts->RemoveStructure(kernel_index)) {            // Remove object from kernel.
			Message::Error("Failed to remove reduct from kernel.");
			return;
		}
		if ((i > 0) && (selected[i - 1] == range_begin - 1)) {      // Can we delay updating the front-end?
			range_begin--;
		}
		else {                                                      // Update the front-end.
			RemoveRows(range_begin, range_end);
			if (i > 0)
				range_begin = range_end = selected[i - 1];
		}
	}

	// Reset the LockReadOnly state. 
	GetParam()->SetLockReadOnly(TRUE);

	// Touch reducts.
	if (no_selected == 1) 
		m_Reducts->Touch("Removed reduct " + String::Format(static_cast(int, selected[0] - 1)) + ".");
	else
		m_Reducts->Touch("Removed " + String::Format(static_cast(int, no_selected)) + " reducts.");

	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);

	// Update other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_Reducts.GetPointer());
	CHint  pHint(CHint::HINT_MODIFIED, 0, 0);  // Should be more specific...

	GetDocument()->UpdateAllViews(this, lHint, &pHint);

}

//-------------------------------------------------------------------
// Method........: OnColSort
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

void CViewReducts::OnColSort(int property, BOOL ascending)
{
	// Get current read-only lock status.
	BOOL oldLockReadOnly = GetParam()->IsLockReadOnly();

	// We may have to alter read-only cells.
	GetParam()->SetLockReadOnly(FALSE);

	// Sort the equivalence classes.
	m_Reducts->Sort(property, ascending);

	String action;

	// Touch with suitable action description.
	action = "Sorted reducts by ";

	if (property == 0)
		action += "support";
	else if (property == 1)
		action += "length";
	else
		action += "some undefined property";

	action += " (" + String((ascending) ? "ascending" : "descending") + ").";

	m_Reducts->Touch(action);

	// Reload kernel data into front-end grid, now that they have been sorted.
  LoadDataIntoGrid();

	// Revert to old read-only lock status.
	GetParam()->SetLockReadOnly(oldLockReadOnly);

	Redraw();

	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);

	// Update other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_Reducts.GetPointer());
	CHint  pHint(CHint::HINT_MODIFIED, 0, 0);  // Should be more specific...

	GetDocument()->UpdateAllViews(this, lHint, &pHint);

}

//-------------------------------------------------------------------
// Method........: OnUpdate
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewReducts::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{

	// Is there anything to update?
	if (m_Reducts.IsNULL()) {
		CViewGrid::OnUpdate(pSender, lHint, pHint);
		return;
	}

	// Re-cast passed hints.  
	Handle<Identifier> identifier = reinterpret_cast(Identifier *, lHint);
	CHint             *hint       = dynamic_cast(CHint *, pHint);

  // Verify hints.
	if ((hint == NULL) || (identifier == NULL)) {
		CViewGrid::OnUpdate(pSender, lHint, pHint);
		return;
	}

	// Check relation between identifier and current reduct set.
	if (m_Reducts.GetPointer() != identifier.GetPointer()) {
		if (m_Reducts->FindParent(identifier->GetId()) != identifier.GetPointer()) {
			CViewGrid::OnUpdate(pSender, lHint, pHint);
			return;
		}
	}

	CWaitCursor wait;

	// Allow mod. of read-only cells.
	GetParam()->SetLockReadOnly(FALSE);

	// Check for hint type.
	switch (hint->m_Hint) {
		case CHint::HINT_GRID_COL_NAME: { // Reduct set's ancestor table/dictionary has changed an attribute name.
			hint->m_Hint = CHint::HINT_MODIFIED;  // Make the hint more general before passing it on upwards.
			break;
		}
		case CHint::HINT_GRID_ROW_DEL: { // Reducts have been deleted from the reduct set.
			Message::Debug("View not updated after row del., not implemented yet.");
			break;
		}
		case CHint::HINT_GRID_ROW_INS: { // Reducts have been inserted/appended to the reduct set.
			Message::Debug("View not updated after row ins./app., not implemented yet.");
			break;
		}
	}

	// Disallow mod. of read-only cells.
	GetParam()->SetLockReadOnly(TRUE);
	
	// The update hint may be caught higher up in the hierarchy.
	CViewGrid::OnUpdate(pSender, lHint, pHint);
	
}
 
