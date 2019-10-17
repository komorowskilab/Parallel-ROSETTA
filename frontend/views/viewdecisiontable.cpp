// DecisionTableView.cpp : implementation of the CViewDecisionTable class
//

#include <stdafx.h>

#include <frontend\rosetta.h>
#include <frontend\rosettadoc.h>

#include <frontend\views\viewdecisiontable.h>
#include <frontend\views\hint.h>

#include <frontend\dialogs\structuredialogs\dialogattributemask.h>
//#include <frontend\dialogs\structuredialogs\dialogattributestatus.h>
#include <frontend\dialogs\structuredialogs\dialogattributename.h>
#include <frontend\dialogs\structuredialogs\dialogattrtype.h>
#include <frontend\dialogs\structuredialogs\dialogattributestatistics.h>

#include <kernel\structures\structure.h>
#include <kernel\structures\dictionary.h>
#include <kernel\structures\stringattribute.h>
#include <kernel\structures\integerattribute.h>
#include <kernel\structures\floatattribute.h>

#include <kernel\utilities\creator.h>

#include <kernel\basic\vector.h>

#include <common\messagehelper.h>
#include <common\configuration.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewDecisionTable

IMPLEMENT_DYNCREATE(CViewDecisionTable, CViewGrid)

BEGIN_MESSAGE_MAP(CViewDecisionTable, CViewGrid)
	//{{AFX_MSG_MAP(CViewDecisionTable)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CGXGridView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CGXGridView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CGXGridView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewDecisionTable

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

CViewDecisionTable::CViewDecisionTable()
{

	m_RowOffset = 1; // Currently not used, for future grid view generalization.
	m_ColOffset = 1; // Currently not used, for future grid view generalization.

	m_Masked    = FALSE;

}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

CViewDecisionTable::~CViewDecisionTable()
{
}

//-------------------------------------------------------------------
// Method........: PreCreateWindow
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

BOOL CViewDecisionTable::PreCreateWindow(CREATESTRUCT& cs)
{
	return CViewGrid::PreCreateWindow(cs);
}

//-------------------------------------------------------------------
// Method........: OnDraw
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

void CViewDecisionTable::OnDraw(CDC* pDC)
{
  CViewGrid::OnDraw(pDC);
}

//-------------------------------------------------------------------
// Method........: OnPreparePrinting
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

BOOL CViewDecisionTable::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

//-------------------------------------------------------------------
// Method........: OnBeginPrinting
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

void CViewDecisionTable::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
  CViewGrid::OnBeginPrinting(pDC, pInfo);
}

//-------------------------------------------------------------------
// Method........: OnEndPrinting
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

void CViewDecisionTable::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CViewGrid::OnEndPrinting(pDC, pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// CViewDecisionTable diagnostics

#ifdef _DEBUG
void CViewDecisionTable::AssertValid() const
{
	CViewGrid::AssertValid();
}

void CViewDecisionTable::Dump(CDumpContext& dc) const
{
	CViewGrid::Dump(dc);
}
#endif //_DEBUG

//-------------------------------------------------------------------
// Method........: OnInitialUpdate
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

void CViewDecisionTable::OnInitialUpdate() 
{	
	CViewGrid::OnInitialUpdate();
}

//-------------------------------------------------------------------
// Method........: OnStartEditing
// Author........: 
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: DR : m_editing, TRUE when the user is editing a cell.
//====================================================================

BOOL CViewDecisionTable::OnStartEditing(ROWCOL nRow, ROWCOL nCol)
{

	// m_Editing = TRUE;

	// If dictionary is used, allow the grid to be modified.
	// if (m_UseDictionary)
	//   SetReadOnly(FALSE);

	// Do stuff higher up in the hierarchy.
  if (!CGXGridCore::OnStartEditing(nRow, nCol))
		return FALSE;
  
  return TRUE;

}

//-------------------------------------------------------------------
// Method........: OnEndEditing
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....: DR : m_editing, set to FALSE when the user is no 
//                 longer editing a cell.
//====================================================================

BOOL CViewDecisionTable::OnEndEditing(ROWCOL nRow, ROWCOL nCol)
{ 
	
	// m_Editing = FALSE;

	return CGXGridCore::OnEndEditing(nRow, nCol);	

}

//-------------------------------------------------------------------
// Method........: OnRButtonClickedRowCol
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

BOOL CViewDecisionTable::OnRButtonClickedRowCol(ROWCOL nRow, ROWCOL nCol, UINT nFlags, CPoint pt)
{

	return CViewGrid::OnRButtonClickedRowCol(nRow, nCol, nFlags, pt);

}

//-------------------------------------------------------------------
// Method........: OnValidateCell
// Author........: 
// Date..........: 
// Description...: 
// Comments......: The grid control's technical documentation specifies
//                 that AfxMessageBox should not be called from the
//                 OnValidateCell method, but that SetWarningText should
//                 be used instead.
// Revisions.....: 
//====================================================================

BOOL CViewDecisionTable::OnValidateCell(ROWCOL nRow, ROWCOL nCol)
{

	// Is a dec. table present?
	if (m_DecisionTable.IsNULL()) {
		SetWarningText("No decision table present.");
		return FALSE;
	}

  // Get stuff from the grid extension.
  const CGXStyle &style = LookupStyleRowCol(nRow, nCol);
	CGXControl *pControl  = GetRegisteredControl(style.GetControl());

  CString oldtext, newtext, inserttext;

	// Get old and new contents.
	pControl->GetCurrentText(newtext);
	oldtext = GetValueRowCol(nRow, nCol);

	// Any changes?
	if (newtext == oldtext)
		return CGXGridView::OnValidateCell(nRow, nCol);

	// Does the decision table have any children?
	if (m_DecisionTable->HasChildren()) {
		SetWarningText("Editing the table may invalidate the basis upon\n"
			             "which the children were derived.");
		pControl->SetCurrentText(oldtext);
		return FALSE;
	}

	String contents = newtext.GetBuffer(0);

	// Are the new contents of legal type (if restricted type)?
	if (m_DecisionTable->IsFloat(nCol - 1, m_Masked) && !contents.IsFloat()) {
		SetWarningText("Input must be a floating point number.");
		pControl->SetCurrentText(oldtext);
		return FALSE;
	}
	if (m_DecisionTable->IsInteger(nCol - 1, m_Masked) && !contents.IsInteger()) {
		SetWarningText("Input must be an integer.");
		pControl->SetCurrentText(oldtext);
		return FALSE;
	}

	// Determine integral value of new contents.
	int mapvalue = m_DecisionTable->SuggestDictionaryEntry(nCol - 1, contents, m_Masked);

	// Update dictionary.
	m_DecisionTable->SetDictionaryEntry(nCol - 1, mapvalue, contents, m_Masked);

	// Update table.
  m_DecisionTable->SetEntry(nRow - 1, nCol - 1, mapvalue, m_Masked);

	// Do stuff higher up in the hierarchy.
	CGXGridView::OnValidateCell(nRow, nCol);

	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);

	// Prepare hints to be passed to other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_DecisionTable.GetPointer());
	CHint  pHint(CHint::HINT_GRID_CELLCHANGE, nRow, nCol);

	// Update other views of same table.
	GetDocument()->UpdateAllViews(this, lHint, &pHint);

	return TRUE;

}

//-------------------------------------------------------------------
// Method........: ProcessKeys
// Author........: Daniel Remmem
// Date..........: 960717
// Description...: This function prevents that the contents in the marked
//                 cells are deleted from the grid when the user presses delete.
//                 It also prevents Shift+Delete from deleting things. 
// Comments......:
// Revisions.....:
//===================================================================

BOOL CViewDecisionTable::ProcessKeys(CWnd* pSender, UINT nMessage, UINT nChar, UINT nRepCnt, UINT flags)
{
	
	BOOL bShift = GetKeyState(VK_SHIFT) & 0x8000;

	// No processing should be done when delete is pressed.
	if (nChar == VK_DELETE) 
		return FALSE;

	// Hack warning... 
	// When the user presses the shift, the ReadOnly state is set to TRUE to prevent that 
	// Shift+Delete does strange things. Note that this will not happen if the user is 
	// editing a cell, so that capital letters still are allowed...
	// if ((nMessage == WM_KEYDOWN) && bShift && !m_Editing)
	//   SetReadOnly(TRUE);

	// The ReadOnly state is reset when the user releases the shift.
	// if ((flags & 0x0001) && bShift && !m_Editing)
	//   SetReadOnly(!m_UseDictionary);
	
	return CGXGridView::ProcessKeys(pSender, nMessage, nChar, nRepCnt, flags);

}

//-------------------------------------------------------------------
// Method........: OnPasteFromClipboard
// Author........: 
// Date..........: 
// Description...: 
// Comments......: Overloaded to ensure consistency between front-end
//                 and kernel.
//
//                 Some overlap with OnValidateCell -- calling that
//                 method from here seemingly had no effect...?
// Revisions.....:
//===================================================================

BOOL CViewDecisionTable::OnPasteFromClipboard(const CGXRange& range) {

	// Does the decision table have any children?
	if (m_DecisionTable->HasChildren()) {
		Message::Error("Pasting may invalidate the basis upon\n"
			             "which the children were derived.");
		return FALSE;
	}

	// Update front-end.
	if (!CViewGrid::OnPasteFromClipboard(range)) {
		Message::Error("Pasting from the clipboard failed.");
		return FALSE;
	}

	ROWCOL nRow, nCol;

	bool first  = true;
	bool done   = false;
	bool all_ok = true;

	// Update kernel.
	while (!done) {

		// Get cell to update.
		if (first) {
			first = false;
			done  = !range.GetFirstCell(nRow, nCol);
		}
		else {
			done  = !range.GetNextCell(nRow, nCol);
		}

		// Done iterating?
		if (done)
			continue;

		int kernel_row = nRow - 1;
		int kernel_col = nCol - 1;

		// Skip column/row "buttons" (headers).
		if ((kernel_row < 0) || (kernel_col < 0))
			continue;

		// Get stuff from the grid extension.
		const CGXStyle &style = LookupStyleRowCol(nRow, nCol);
		CGXControl *pControl  = GetRegisteredControl(style.GetControl());

		// Get contents of cell.
		CString contents_mfc  = GetValueRowCol(nRow, nCol);
		String  contents      = contents_mfc.GetBuffer(0);

		// Determine integral value of new contents.
		int mapvalue = m_DecisionTable->SuggestDictionaryEntry(kernel_col, contents, m_Masked);

		bool this_ok = true;

		// Deal with specially reserved values for missing entries.
		if (mapvalue == Undefined::Integer() && contents != Undefined::String()) {
			this_ok = false;
			all_ok  = false;
		}

		if (!this_ok)
			continue;

		// Update dictionary.
		m_DecisionTable->SetDictionaryEntry(kernel_col, mapvalue, contents, m_Masked);

	  // Update table.
    m_DecisionTable->SetEntry(kernel_row, kernel_col, mapvalue, m_Masked);

	}

	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);

	if (!all_ok)
		Message::Error("Failed to fully update the kernel.");

	// Prepare hints to be passed to other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_DecisionTable.GetPointer());
	CHint  pHint(CHint::HINT_MODIFIED, nRow, nCol);

	// Update other views of same table.
	if (all_ok)
		GetDocument()->UpdateAllViews(this, lHint, &pHint);
	else
		GetDocument()->UpdateAllViews(NULL, lHint, &pHint);

	if (all_ok)
		return TRUE;
	else
		return FALSE;

}

//-------------------------------------------------------------------
// Method........: OnLButtonClickedRowCol
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

BOOL CViewDecisionTable::OnLButtonClickedRowCol(ROWCOL nRow, ROWCOL nCol, UINT nFlags, CPoint pt)
{

  CGXGridView::OnLButtonClickedRowCol(nRow, nCol, nFlags, pt);

  // Did we hit the grid button?
  if ((nRow == 0) && (nCol == 0))
    return TRUE;
  
  // Did we hit a column header button?
  else if (nRow == 0)
    return TRUE;
        
  // Did we hit a row header button?
  else if (nCol == 0)
    return TRUE;

  return TRUE;

}

//-------------------------------------------------------------------
// Method........: OnLButtonDblClkRowCol
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

BOOL CViewDecisionTable::OnLButtonDblClkRowCol(ROWCOL nRow, ROWCOL nCol, UINT nFlags, CPoint pt)
{

  // Did we hit the grid button?
  if ((nRow == 0) && (nCol == 0))
    return TRUE;

  // Did we hit a column header button?
  if (nRow == 0) {
    OnAttributeName(nCol);
  }

  CGXGridView::OnLButtonDblClkRowCol(nRow, nCol, nFlags, pt);
  
  return TRUE;

}

//-------------------------------------------------------------------
// Method........: OnDeleteCell
// Author........: Daniel Remmem
// Date..........: 960716
// Description...: 
// Comments......: obsolete function? keep a while for debugging...
// Revisions.....:
//===================================================================

BOOL CViewDecisionTable::OnDeleteCell(ROWCOL /*nRow*/, ROWCOL /*nCol*/)
{
	Message::Warning("OnDeleteCell was called.");
	return TRUE;	
}

//-------------------------------------------------------------------
// Method........: CreateGridMenu
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

CMenu *CViewDecisionTable::CreateGridMenu(CMenu *menu)
{

	if (menu == NULL)
		return NULL;

	UINT IS_CHECKED = MF_CHECKED;
	UINT IS_GRAYED  = 0;

	if (m_DecisionTable->HasChildren()) // To ensure consistency of derived data.
		IS_GRAYED = MF_GRAYED;

	if (!m_UseDictionary)
		IS_CHECKED = MF_UNCHECKED;

  menu->CreatePopupMenu();

	menu->AppendMenu(MF_STRING | MF_ENABLED | IS_CHECKED, CVIEWGRID_GRID_USEDICTIONARY,              "Use dictionary");
  menu->AppendMenu(MF_SEPARATOR);
  menu->AppendMenu(MF_STRING | MF_ENABLED | IS_GRAYED,  CVIEWDECISIONTABLE_GRID_APPENDROW,         "Append object");
  menu->AppendMenu(MF_STRING | MF_ENABLED | IS_GRAYED,  CVIEWDECISIONTABLE_GRID_APPENDCOL,         "Append attribute...");
  menu->AppendMenu(MF_SEPARATOR);
  menu->AppendMenu(MF_STRING | MF_ENABLED,              CViewGrid::CVIEWGRID_GRID_RESIZETOFIT,     "Resize to fit");
  menu->AppendMenu(MF_STRING | MF_ENABLED,              CViewGrid::CVIEWGRID_GRID_DISPLAYSETTINGS, "Properties...");

	return menu;
 
}

//-------------------------------------------------------------------
// Method........: CreateColumnMenu
// Author........: 
// Date..........: 
// Description...: 
// Comments......: Creates same menu for all columns.
// Revisions.....:
//===================================================================

CMenu *CViewDecisionTable::CreateColumnMenu(CMenu *menu, ROWCOL /*nCol*/)
{

	UINT IS_GRAYED = 0;

	// Some options are unavailable if the decision table has any children.
	if (m_DecisionTable->HasChildren())
		IS_GRAYED = MF_GRAYED;

	if (menu == NULL)
		return NULL;

  menu->CreatePopupMenu();

  menu->AppendMenu(MF_STRING | MF_ENABLED | IS_GRAYED, CVIEWDECISIONTABLE_COL_MASK,        "Masking...");
#if 0
  menu->AppendMenu(MF_STRING | MF_ENABLED | IS_GRAYED, CVIEWDECISIONTABLE_COL_STATUS,      "Status...");
#endif
  menu->AppendMenu(MF_SEPARATOR);
  menu->AppendMenu(MF_STRING | MF_ENABLED | IS_GRAYED, CVIEWDECISIONTABLE_COL_INSERT,      "Insert...");
  menu->AppendMenu(MF_STRING | MF_ENABLED | IS_GRAYED, CVIEWDECISIONTABLE_COL_REMOVE,      "Remove");
  menu->AppendMenu(MF_STRING | MF_ENABLED | IS_GRAYED, CVIEWDECISIONTABLE_COL_SWAP,        "Swap");
  menu->AppendMenu(MF_STRING | MF_ENABLED | IS_GRAYED, CVIEWDECISIONTABLE_COL_SORT,        "Sort");
  menu->AppendMenu(MF_STRING | MF_ENABLED | IS_GRAYED, CVIEWDECISIONTABLE_COL_JOIN,        "Join");
  menu->AppendMenu(MF_SEPARATOR);
  menu->AppendMenu(MF_STRING | MF_ENABLED,             CVIEWDECISIONTABLE_COL_STATISTICS,  "Statistics...");
  menu->AppendMenu(MF_STRING | MF_ENABLED,             CVIEWDECISIONTABLE_COL_INFORMATION, "Properties...");

	return menu;
  
}

//-------------------------------------------------------------------
// Method........: CreateRowMenu
// Author........: 
// Date..........: 
// Description...: 
// Comments......: Creates same menu for all rows.
// Revisions.....:
//===================================================================

CMenu *CViewDecisionTable::CreateRowMenu(CMenu *menu, ROWCOL /*nRow*/)
{

	UINT IS_GRAYED = 0;

	// Some options are unavailable if the decision table has any children.
	if (m_DecisionTable->HasChildren())
		IS_GRAYED = MF_GRAYED;

	if (menu == NULL)
		return NULL;

  menu->CreatePopupMenu();

  menu->AppendMenu(MF_STRING | MF_ENABLED | IS_GRAYED, CVIEWDECISIONTABLE_ROW_INSERT, "Insert...");  
  menu->AppendMenu(MF_STRING | MF_ENABLED | IS_GRAYED, CVIEWDECISIONTABLE_ROW_REMOVE, "Remove");

	return menu;

}

//-------------------------------------------------------------------
// Method........: OnCmdMsg
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

BOOL CViewDecisionTable::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{

	if (pHandlerInfo != NULL)
		return CViewGrid::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	
  if ((nID > CVIEWDECISIONTABLE_FIRSTVALUE) && (nID < CVIEWDECISIONTABLE_LASTVALUE)) {
    if (nCode == CN_COMMAND) {
      switch (nID) {
        case CVIEWDECISIONTABLE_COL_MASK:              OnColDisable();      return TRUE;
        case CVIEWDECISIONTABLE_COL_STATUS:            OnColStatus();       return TRUE;
        case CVIEWDECISIONTABLE_COL_INSERT:            OnColInsert();       return TRUE;
        case CVIEWDECISIONTABLE_COL_REMOVE:            OnColRemove();       return TRUE;
        case CVIEWDECISIONTABLE_COL_SWAP:              OnColSwap();         return TRUE;
        case CVIEWDECISIONTABLE_COL_SORT:              OnColSort();         return TRUE;
        case CVIEWDECISIONTABLE_COL_JOIN:              OnColJoin();         return TRUE;
        case CVIEWDECISIONTABLE_COL_INFORMATION:       OnColInformation();  return TRUE;
        case CVIEWDECISIONTABLE_COL_STATISTICS:        OnColStatistics();   return TRUE;
        case CVIEWDECISIONTABLE_ROW_INSERT:            OnRowInsert();       return TRUE;
        case CVIEWDECISIONTABLE_ROW_REMOVE:            OnRowRemove();       return TRUE;
        case CVIEWDECISIONTABLE_GRID_APPENDROW:        OnRowAppend();       return TRUE;
        case CVIEWDECISIONTABLE_GRID_APPENDCOL:        OnColAppend();       return TRUE;
      }
    }
    else if (nCode == CN_UPDATE_COMMAND_UI) {
    }
  }

	// The command message may be handled higher up in the hierarchy.
	return CViewGrid::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

}


//-------------------------------------------------------------------
// Method........: OnColInsert
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

void CViewDecisionTable::OnColInsert()
{

  CRowColArray         selected;
  ROWCOL               no_selected = GetSelectedCols(selected, TRUE);
	CDialogAttributeType dialog;
	Handle<Attribute>    attribute;

	// Is a decision table "attached"?
	if (m_DecisionTable.IsNULL())
		return;

	// Does the decision table have any children?
	if (m_DecisionTable->HasChildren()) {
		Message::Error("Cannot insert column, the decision table has children.");
		return;
	}

	// Is exactly one column selected?
	if (no_selected != 1) {
		Message::Error("Only one column must be selected.");
		return;
	}

	// Pop the type dialog.
	if (dialog.DoModal() != IDOK)
		return;

	// Determine and create the type of attribute.
	if (dialog.m_Type == Attribute::TYPE_STRING) {
		attribute = Creator::StringAttribute();
	}
	else if (dialog.m_Type == Attribute::TYPE_INTEGER) {
		attribute = Creator::IntegerAttribute();
	}
	else if (dialog.m_Type == Attribute::TYPE_FLOAT) {
		attribute = Creator::FloatAttribute(dialog.m_Exponent, NULL, false);
	}
	else {
		Message::Error("Unknown attribute type.");
		return;
	}

	// Update the kernel.
	if (!m_DecisionTable->InsertAttribute(selected[0] - 1, attribute.GetPointer(), m_Masked)) {
		Message::Error("Failed to insert the attribute into the decision table.");
		return;
	}

	// Touch the decision table.
	m_DecisionTable->Touch("Inserted new attribute (" + Attribute::GetString(attribute->GetType()) + ").");
	
	// Allows modifying of the grid.
	// GetParam()->SetLockReadOnly(FALSE);

	// Update the front-end.
  InsertCols(selected[0], 1);
	InitializeColumnAppearance(selected[0], selected[0]);
    
	// Reset LockReadOnly state. 
	// GetParam()->SetLockReadOnly(TRUE);

	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);

	// Update other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_DecisionTable.GetPointer());
	CHint  pHint(CHint::HINT_GRID_COL_INS, 0, selected[0]);

	GetDocument()->UpdateAllViews(this, lHint, &pHint);

}

//-------------------------------------------------------------------
// Method........: OnColAppend
// Author........: 
// Date..........: 
// Description...: 
// Comments......: Remove this method, use same code as OnColInsert
//                 instead.
// Revisions.....:
//===================================================================

void CViewDecisionTable::OnColAppend()
{

	CDialogAttributeType dialog;
	Handle<Attribute>    attribute;

	// Is a decision table "attached"?
	if (m_DecisionTable.IsNULL())
		return;

	// Does the decision table have any children?
	if (m_DecisionTable->HasChildren()) {
		Message::Error("Cannot append column, the decision table has children.");
		return;
	}

	// Pop the type dialog.
	if (dialog.DoModal() != IDOK)
		return;

	// Determine and create the type of attribute.
	if (dialog.m_Type == Attribute::TYPE_STRING) {
		attribute = Creator::StringAttribute();
	}
	else if (dialog.m_Type == Attribute::TYPE_INTEGER) {
		attribute = Creator::IntegerAttribute();
	}
	else if (dialog.m_Type == Attribute::TYPE_FLOAT) {
		attribute = Creator::FloatAttribute(dialog.m_Exponent, NULL, false);
	}
	else {
		Message::Error("Unknown type.");
		return;
	}
	
	// Update the kernel.
	if (!m_DecisionTable->AppendAttribute(attribute.GetPointer(), m_Masked)) {
		Message::Error("Failed to append the attribute to the decision table.");
		return;
	}

	int no_columns = m_DecisionTable->GetNoAttributes(m_Masked);

	// Touch the decision table.
	m_DecisionTable->Touch("Appended new attribute (" + Attribute::GetString(attribute->GetType()) + ").");

	// Allows modifying of the grid.
	// GetParam()->SetLockReadOnly(FALSE);

	// Update the front-end.
  InsertCols(no_columns, 1);
	InitializeColumnAppearance(1, no_columns);
  
  // Reset LockReadOnly state. 
	// GetParam()->SetLockReadOnly(TRUE);

	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);

	// Update other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_DecisionTable.GetPointer());
	CHint  pHint(CHint::HINT_GRID_COL_INS, 0, no_columns);

	GetDocument()->UpdateAllViews(this, lHint, &pHint);

}

//-------------------------------------------------------------------
// Method........: OnColRemove
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....: 
//===================================================================

void CViewDecisionTable::OnColRemove()
{

	String       text;
  CRowColArray selected;
  ROWCOL       no_selected = GetSelectedCols(selected, TRUE);

	// Is a decision table "attached"?
	if (m_DecisionTable.IsNULL())
		return;

	// Does the decision table have any children?
	if (m_DecisionTable->HasChildren()) {
		Message::Error("Cannot remove column(s), the decision table has children.");
		return;
	}

	// Are any columns selected?
	if (no_selected == 0) {
		Message::Error("No columns selected.");
		return;
	}

	// Query the user.
	if (no_selected == 1)
		text = "Are you sure you want to remove column " + String::Format(static_cast(int, selected[0])) + "?";
	else
		text = String::Format(static_cast(int, no_selected)) + " columns selected.  Are you sure you want to remove them?";
	if (MessageHelper::Query(text, "Remove", MessageHelper::BUTTONS_OKCANCEL) != MessageHelper::RESPONSE_OK)
		return;

	// Allows modifying of the grid.
	// GetParam()->SetLockReadOnly(FALSE);

	BOOL oldlock = LockUpdate(TRUE);

	int range_begin = selected[no_selected - 1];
	int range_end   = selected[no_selected - 1];

	int i;

	// Remove attributes in kernel and front-end.
	for (i = no_selected - 1; i >=0; i--) {                           // Loop backwards to minimize front-end updates.
		int kernel_index = selected[i] - 1;
		String name = m_DecisionTable->GetAttributeName(selected[i] - 1, m_Masked);
		if (!m_DecisionTable->RemoveAttribute(selected[i] - 1, m_Masked)) { // Remove attribute from kernel.
			Message::Error("Failed to remove attribute " + name + " from kernel.");
			return;
		}
	  m_DecisionTable->Touch("Removed attribute " + name + ".");
		if ((i > 0) && (selected[i - 1] == range_begin - 1)) {              // Can we delay updating the front-end?
			range_begin--;
		}
		else {                                                              // Update the front-end.
			RemoveCols(range_begin, range_end);
			if (i > 0)
				range_begin = range_end = selected[i - 1];
		}
	}

	// Update looks of front-end columns.
	InitializeColumnAppearance(1, GetColCount());

	// Reset the LockReadOnly state.
	// GetParam()->SetLockReadOnly(TRUE);

	LockUpdate(oldlock);
	Redraw();
		
	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);

	// Update other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_DecisionTable.GetPointer());
	CHint  pHint(CHint::HINT_MODIFIED, 0, 0);  // Should be more specific...

	GetDocument()->UpdateAllViews(this, lHint, &pHint);

}

//-------------------------------------------------------------------
// Method........: OnColSwap
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

void CViewDecisionTable::OnColSwap()
{

	String       text;
  CRowColArray selected;
  ROWCOL       no_selected = GetSelectedCols(selected, TRUE);

	// Is a decision table "attached"?
	if (m_DecisionTable.IsNULL())
		return;

	// Does the decision table have any children?
	if (m_DecisionTable->HasChildren()) {
		Message::Error("Cannot swap columns, the decision table has children.");
		return;
	}

	// Are exactly two columns selected?
	if (no_selected != 2) {
		Message::Error("Two columns must be selected to swap.");
		return;
	}

  int i = selected[0]; // Front-end index.
  int j = selected[1]; // Front-end index.

	String name_i = m_DecisionTable->GetAttributeName(i - 1, m_Masked);
	String name_j = m_DecisionTable->GetAttributeName(j - 1, m_Masked);

	// Query the user.
	text = "Are you sure you want to swap columns " + name_i + " and " + name_j + "?";
	if (MessageHelper::Query(text, "Swap", MessageHelper::BUTTONS_OKCANCEL) != MessageHelper::RESPONSE_OK)
		return;

	// Swap attributes (columns) in kernel.
	if (!m_DecisionTable->SwapAttributes(i - 1, j - 1, m_Masked)) {
		Message::Error("Failed to swap attributes " + name_i + " and " + name_j + " in kernel.");
		return;
	}

	// Touch the dec. table.
	m_DecisionTable->Touch("Swapped attributes " + name_i + " and " + name_j + ".");
	
	// Allows modifying of the grid.
	// GetParam()->SetLockReadOnly(FALSE);

	BOOL oldlock = LockUpdate(TRUE);
	
	// Swap attributes (columns) in front-end grid.  (Note that i < j).
	MoveCols(i, i, j, GX_UPDATENOW, gxDo);
	MoveCols(j - 1, j - 1, i, GX_UPDATENOW, gxDo);

	// Update looks of front-end columns.
	InitializeColumnAppearance(i, i);
	InitializeColumnAppearance(j, j);
	
	LockUpdate(oldlock);
	Redraw();
	
	// Reset the LockReadOnly state. 
	// GetParam()->SetLockReadOnly(TRUE);

	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);

	// Update other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_DecisionTable.GetPointer());
	CHint  pHint(CHint::HINT_GRID_COL_SWAP, selected[0], selected[1]);

	GetDocument()->UpdateAllViews(this, lHint, &pHint);

}

//-------------------------------------------------------------------
// Method........: OnColJoin
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....: 
//===================================================================

void CViewDecisionTable::OnColJoin()
{

  CRowColArray selected;
  ROWCOL       no_selected = GetSelectedCols(selected, TRUE);

	// Is a decision table "attached"?
	if (m_DecisionTable.IsNULL())
		return;

	// Does the decision table have any children?
	if (m_DecisionTable->HasChildren()) {
		Message::Error("Cannot join columns, the decision table has children.");
		return;
	}

	// Are enough columns selected?
	if (no_selected < 2) {
		Message::Error("More than two columns must be selected.");
		return;
	}

	String text = "Are you sure you want to create a new, composite attribute?";

	// Query the user.
	if (MessageHelper::Query(text, "Join", MessageHelper::BUTTONS_OKCANCEL) != MessageHelper::RESPONSE_OK)
		return;

	// Create a new attribute fo the dictionary.
	Handle<StringAttribute> joined = Creator::StringAttribute();

	String composite_name;

	// Allocate a vector to temporarily hold the coded values for the composite values.
	Vector(int) composite_coded;

	composite_coded.reserve(m_DecisionTable->GetNoObjects(m_Masked));

	int i, j;

	// Create composite dictionary entries.
	for (i = 0; i < m_DecisionTable->GetNoObjects(m_Masked); i++) {

		if (i == 0)
			composite_name = "{";

		String composite = "{";

		for (j = 0; j < no_selected; j++) {

			// Translate from GUI index to kernel index.
			int kernel_j = selected[j] - 1;

			// Build name of new attribute if not done.
			if (i == 0) {
				composite_name += m_DecisionTable->GetAttributeName(kernel_j, m_Masked);
				if (j < no_selected - 1)
					composite_name += ", ";
			}

			composite += m_DecisionTable->GetEntry(i, kernel_j, true, m_Masked);

			if (j < no_selected - 1)
				composite += ", ";

		}

		if (i == 0)
			composite_name += "}";

		composite += "}";

		// Add the composite entry to the table.
		int suggested_coded = joined->SuggestEntry(composite);

		if (suggested_coded == Undefined::Integer()) {
			Message::Error("Failed to determine coding for " + composite + ".");
			return;
		}

		if (!joined->SetEntry(suggested_coded, composite)) {
			Message::Error("Failed to build joined attribute.");
			return;
		}

		composite_coded.push_back(suggested_coded);

	}

	// Baptize the attribute.
	joined->SetName(composite_name);

	// Determine where to insert the new attribute.
	int insert_loc_gui = selected[no_selected - 1];
	int insert_loc_ker = insert_loc_gui - 1;

	// Insert a new column into the kernel.
	if (!m_DecisionTable->InsertAttribute(insert_loc_ker, joined.GetPointer(), m_Masked)) {
		Message::Error("Failed to insert the attribute into the decision table.");
		return;
	}

	// Touch the decision table.
	m_DecisionTable->Touch("Inserted new, composite attribute " + composite_name + ".");

	// Update table entries.
	for (i = 0; i < m_DecisionTable->GetNoObjects(m_Masked); i++) {
		if (!m_DecisionTable->SetEntry(i, insert_loc_ker, composite_coded[i], m_Masked)) {
			Message::Error("Failed to update decision table.");
			return;
		}
	}

	// Update the front-end.
  InsertCols(insert_loc_gui, 1);
	InitializeColumnAppearance(insert_loc_gui, insert_loc_gui);

	// Load data from kernel into front-end.
	LoadDataIntoGrid();

	// Redraw front-end.
  RedrawRowCol(1, 1, GetRowCount(), GetColCount());  
		
	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);

	// Update other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_DecisionTable.GetPointer());
	CHint  pHint(CHint::HINT_GRID_COL_INS, insert_loc_gui, insert_loc_gui);

	GetDocument()->UpdateAllViews(this, lHint, &pHint);

}

//-------------------------------------------------------------------
// Method........: OnColInformation
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Temporarily synonymous with attribute naming.
// Revisions.....:
//===================================================================

void CViewDecisionTable::OnColInformation()
{  
  CRowColArray selected;
  GetSelectedCols(selected, TRUE);
	OnAttributeName(selected[0]);
}

//-------------------------------------------------------------------
// Method........: OnColStatistics
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

void CViewDecisionTable::OnColStatistics()
{ 
	
  CRowColArray selected;
  GetSelectedCols(selected, TRUE);
	
	CDialogAttributeStatistics dialog;
  
  dialog.SetDecisionTable(m_DecisionTable.GetPointer());
  dialog.m_NoAttribute = selected[0] - 1;

  dialog.DoModal();

}

//-------------------------------------------------------------------
// Method........: OnRowInsert
// Author........: Merete Hvalshagen
// Date..........: 960414
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

void CViewDecisionTable::OnRowInsert()
{

  CRowColArray selected;
  ROWCOL       no_selected = GetSelectedCols(selected, TRUE);

	// Does the decision table have any children?
	if (m_DecisionTable->HasChildren()) {
		Message::Error("Cannot insert row, the decision table has children.");
		return;
	}

	// Is exactly one column selected?
	if (no_selected != 1) {
		Message::Error("Only one row must be selected.");
		return;
	}

	// Update kernel.
	if (!m_DecisionTable->InsertObject(selected[0] - 1, m_Masked)) {
		Message::Error("Failed to insert object into kernel decision table.");
		return;
	}

  // Allows modifying of the grid.
	// GetParam()->SetLockReadOnly(FALSE);

	// Update front-end.
	InsertRows(selected[0], 1);
	InitializeColumnAppearance(1, GetColCount());

	// Reset the LockReadOnly state. 
	// GetParam()->SetLockReadOnly(FALSE);

	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);

	// Update other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_DecisionTable.GetPointer());
	CHint  pHint(CHint::HINT_GRID_ROW_INS, selected[0], 0);

	GetDocument()->UpdateAllViews(this, lHint, &pHint);

}

//-------------------------------------------------------------------
// Method........: OnRowAppend
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....: 
//===================================================================

void CViewDecisionTable::OnRowAppend()
{

	// Does the decision table have any children?
	if (m_DecisionTable->HasChildren()) {
		Message::Error("Cannot append row, the decision table has children.");
		return;
	}

	// Update kernel.
	if (!m_DecisionTable->AppendObject(m_Masked)) {
		Message::Error("Failed to append object to kernel decision table.");
		return;
	}

  // Allows modifying of the grid.
	// GetParam()->SetLockReadOnly(FALSE);

	// Update front-end.
	int no_rows = GetRowCount();
	InsertRows(no_rows + 1, 1);
	InitializeColumnAppearance(1, GetColCount());

	// Reset the LockReadOnly state. 
	// GetParam()->SetLockReadOnly(FALSE);

	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);

	// Update other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_DecisionTable.GetPointer());
	CHint  pHint(CHint::HINT_GRID_ROW_INS, no_rows + 1, 0);

	GetDocument()->UpdateAllViews(this, lHint, &pHint);

}

//-------------------------------------------------------------------
// Method........: OnRowRemove
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....: 
//===================================================================

void CViewDecisionTable::OnRowRemove()
{

	String       text;
  CRowColArray selected;
  ROWCOL       no_selected = GetSelectedRows(selected, TRUE);

	// Are any rows selected?
	if (no_selected == 0) {
		Message::Error("No rows selected.");
		return;
	}

	// Does the decision table have any children?
	if (m_DecisionTable->HasChildren()) {
		Message::Error("Cannot remove row, the decision table has children.");
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
	// GetParam()->SetLockReadOnly(FALSE);

	int range_begin = selected[no_selected - 1];
	int range_end   = selected[no_selected - 1];

	int i;

	// Remove objects in kernel and front-end.
	for (i = no_selected - 1; i >=0; i--) {                      // Loop backwards to minimize front-end updates.
		int kernel_index = selected[i] - 1;
		if (!m_DecisionTable->RemoveObject(kernel_index, m_Masked)) {  // Remove object from kernel.
			Message::Error("Failed to remove object from kernel.");
			return;
		}
		if ((i > 0) && (selected[i - 1] == range_begin - 1)) {         // Can we delay updating the front-end?
			range_begin--;
		}
		else {                                                         // Update the front-end.
			RemoveRows(range_begin, range_end);
			if (i > 0)
				range_begin = range_end = selected[i - 1];
		}
	}

	// Reset the LockReadOnly state. 
	// GetParam()->SetLockReadOnly(TRUE);

  int kernel_index = selected[0] - 1;

	// Touch dec. table.
	if (no_selected == 1) 
		m_DecisionTable->Touch("Removed object " + String::Format(static_cast(int, kernel_index)) + ".");
	else
		m_DecisionTable->Touch("Removed " + String::Format(static_cast(int, no_selected)) + " objects.");

	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);
	
	// Update other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_DecisionTable.GetPointer());
	CHint  pHint(CHint::HINT_MODIFIED, 0, 0);  // Should be more specific...

	GetDocument()->UpdateAllViews(this, lHint, &pHint);

}

//-------------------------------------------------------------------
// Method........: OnColSort
// Author........: 
// Date..........: 
// Description...: Sorts the rows according to the value of a selected
//                 column.
// Comments......:
// Revisions.....: 
//===================================================================

void CViewDecisionTable::OnColSort()
{

	String       text;
  CRowColArray selected;
  ROWCOL       no_selected = GetSelectedCols(selected, TRUE);

	// Is a decision table "attached"?
	if (m_DecisionTable.IsNULL())
		return;

	// Does the decision table have any children?
	if (m_DecisionTable->HasChildren()) {
		Message::Error("Cannot sort objects, the decision table has children.");
		return;
	}

	// Is exactly one column selected?
	if (no_selected != 1) {
		Message::Error("One column only must be selected as a sorting key.");
		return;
	}

	int sort_attribute = selected[0] - 1;

	String name = m_DecisionTable->GetAttributeName(sort_attribute, m_Masked);

	// Query the user.
	text = "Are you sure you want to sort the objects according to attribute " + name + "?";
	if (MessageHelper::Query(text, "Sort", MessageHelper::BUTTONS_OKCANCEL) != MessageHelper::RESPONSE_OK)
		return;

	CWaitCursor wait;
	Message     message;

	message.Notify("Sorting objects...");

	// Sort table in kernel.
	bool success = m_DecisionTable->SortObjects(sort_attribute, m_Masked);

	if (!success) {
		Message::Error("Sort unsuccessful.");
		m_DecisionTable->Touch("Unsuccessful sort by attribute " + name + ".");
	}
	else {
		m_DecisionTable->Touch("Sorted objects by attribute " + name + ".");
	}

	// Load data from kernel into front-end.
	LoadDataIntoGrid();

	// Redraw front-end.
  RedrawRowCol(1, 1, GetRowCount(), GetColCount());  
  //RedrawRowCol(2, 1, GetRowCount(), GetColCount());  

	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);

	// Update other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_DecisionTable.GetPointer());
	CHint  pHint(CHint::HINT_MODIFIED, 0, 0);  // Should be more specific...

	GetDocument()->UpdateAllViews(this, lHint, &pHint);

}

//-------------------------------------------------------------------
// Method........: OnAttributeName
// Author........: 
// Date..........: 
// Description...: 
// Comments......: The argument index is relative to the front-end.
// Revisions.....:
//===================================================================

void CViewDecisionTable::OnAttributeName(int attribute_no)
{

	// Is a dec. table "attached" to the view?
	if (m_DecisionTable.IsNULL())
		return;

	// Does the table have a dictionary?
	if (!m_DecisionTable->HasDictionary()) {
		Message::Error("Cannot edit attribute name, no dictionary present.");
		return;
	}

	int kernel_index = attribute_no - 1;

	// Get the current name and unit.
	CString name = (m_DecisionTable->GetAttributeName(kernel_index, m_Masked)).GetBuffer();
	CString unit = (m_DecisionTable->GetAttributeUnit(kernel_index, m_Masked)).GetBuffer();

  CDialogAttributeName dialog;

	// Update the dialog box before popping it.
	dialog.m_Name = name;
	dialog.m_Unit = unit;

	// Pop the dialog.
	if (dialog.DoModal() != IDOK)
		return;

	// Update the attribute.
	m_DecisionTable->SetAttributeName(kernel_index, String(dialog.m_Name.GetBuffer(0)), m_Masked);
	m_DecisionTable->SetAttributeUnit(kernel_index, String(dialog.m_Unit.GetBuffer(0)), m_Masked);

  // Allows modifying of the grid.
	// GetParam()->SetLockReadOnly(FALSE);

	// Update the front-end.
	InitializeColumnHeaders(attribute_no, attribute_no);

	// Make sure column headers have the same height.
	ResizeRowHeightsToFit(CGXRange().SetRows(0));

	// Reset LockReadOnly state. 
	// GetParam()->SetLockReadOnly(TRUE);

	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);

	// Update other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_DecisionTable.GetPointer());
	CHint  pHint(CHint::HINT_GRID_COL_NAME, 0, attribute_no);

	GetDocument()->UpdateAllViews(this, lHint, &pHint);

}

//-------------------------------------------------------------------
// Method........: SetStructure
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....: 
//===================================================================

void CViewDecisionTable::SetStructure(Structure *structure)
{

	m_DecisionTable = dynamic_cast(DecisionTable *, structure);

	CViewGrid::SetStructure(m_DecisionTable.GetPointer());

	// SetReadOnly(!m_UseDictionary);	
  
}

//-------------------------------------------------------------------
// Method........: InitializeGridAppearance
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....: 
//===================================================================

void CViewDecisionTable::InitializeGridAppearance()
{

	CViewGrid::InitializeGridAppearance();

	// Is there a table present?
	if (m_DecisionTable.IsNULL()) {
		Message::Error("No decision table present.");
		return;
	}

	// Get grid control parameters.
	CGXGridParam *params = GetParam();

	// Disable some stuff.
  params->EnableMoveRows(FALSE);
  params->EnableMoveCols(FALSE);

  BOOL bLockOld = LockUpdate(TRUE);

	// Set window text.
  GetParentFrame()->SetWindowText((m_DecisionTable->GetName()).GetBuffer());

	// Get dimensions.
	int no_attributes = m_DecisionTable->GetNoAttributes(m_Masked);
  int no_objects    = m_DecisionTable->GetNoObjects(m_Masked);
  
  // Set the number of rows and columns in the grid.
  SetRowCount(no_objects);
  SetColCount(no_attributes);

	InitializeColumnAppearance(1, no_attributes);

  LockUpdate(bLockOld);
	
}

//-------------------------------------------------------------------
// Method........: LoadDataIntoGrid
// Author........: 
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CViewDecisionTable::LoadDataIntoGrid()
{

	// Is there a table present?
	if (m_DecisionTable.IsNULL()) {
		Message::Error("No decision table present.");
		return;
	}

	// Get dimensions.
  int no_attributes = m_DecisionTable->GetNoAttributes(m_Masked);
  int no_objects    = m_DecisionTable->GetNoObjects(m_Masked);

	// Alter front-end dimensions if the grid's size is too small/large.
	if ((GetRowCount() != no_objects) || (GetColCount() != no_attributes))
		InitializeGridAppearance();

  BOOL bLockOld = LockUpdate(TRUE);
  
	CWaitCursor wait;
	Message     message;

	int i, j;

	// How often should we poll for cancellations? How many object do, say, 5 percent correspond to?
	int pollsize = no_objects / 20, pollcount = 0;

	// Display data from kernel in front-end.
  for (i = 0; i < no_objects; i++) {

		// Cancel?
		if (pollcount == pollsize || i == no_objects - 1) {
			if (!message.Progress("Preparing objects for display...", i, no_objects)) {
				Message::Warning("Objects " + String::Format(i + 1) + " through " + String::Format(no_objects + 1) + " will not be displayed properly.");
				break;
			}
			pollcount = 0;
		}
		else {
			pollcount++;
		}

    for (j = 0; j < no_attributes; j++) {
			String entry = m_DecisionTable->GetEntry(i, j, m_UseDictionary, m_Masked);
			SetValueRange(CGXRange(i + 1, j + 1), entry.GetBuffer());
    }

	}	

	// Set the row count if the operation was aborted.
	if (i < no_objects)
		SetRowCount(i);

	LockUpdate(bLockOld);

	Redraw();

}

//-------------------------------------------------------------------
// Method........: InitializeColumnHeaders
// Author........: Aleksander šhrn
// Date..........: 
// Description...: Initializes the headers (names etc) of the columns 
//                 in the given range.
//
// Comments......: The argument indices are relative to the front-end.
// Revisions.....: 
//===================================================================

void CViewDecisionTable::InitializeColumnHeaders(int first, int last)
{

  BOOL bLockOld = LockUpdate(TRUE);
  
	int j;

	// Loop over specified range.
	for (j = first; j <= last; j++) {

		CGXStyle style;

		int kernel_index = j - 1;

		if (kernel_index < 0)
			continue;

		// Set header name.
		String name = m_DecisionTable->GetAttributeName(kernel_index, m_Masked);

		// Show type?
		if (Configuration::GUI::ShowAttributeTypes())
			name += "\n[" + Attribute::GetString(m_DecisionTable->GetAttributeType(kernel_index, m_Masked)) + "]";

		// Show unit?
		if (Configuration::GUI::ShowAttributeUnits() && m_DecisionTable->GetAttributeUnit(kernel_index, m_Masked) != Undefined::String())
			name += "\n[" + m_DecisionTable->GetAttributeUnit(kernel_index, m_Masked) + "]";

    SetStyleRange(CGXRange(0, j), CGXStyle().SetValue(name.GetBuffer()));

	}

	LockUpdate(bLockOld);

}

//-------------------------------------------------------------------
// Method........: InitializeColumnAppearance
// Author........: Aleksander šhrn
// Date..........: 
// Description...: Initializes the appearance of the columns in the 
//                 given range.  By appearance is meant such stuff as
//                 backgrounds, fonts, justification, etc.
//
// Comments......: The argument indices are relative to the front-end.
// Revisions.....: 
//===================================================================

void CViewDecisionTable::InitializeColumnAppearance(int first, int last)
{

  BOOL bLockOld = LockUpdate(TRUE);
  
	CWaitCursor wait;

	int j;

	// Loop over specified range.
	for (j = first; j <= last; j++) {

		CGXStyle style;

		int kernel_index = j - 1;

		if (kernel_index < 0)
			continue;

		// Initialize column headers.
		InitializeColumnHeaders(j, j);

		// Set justification.
		if (!m_DecisionTable->HasDictionary() || !m_UseDictionary)
			style.SetHorizontalAlignment(DT_RIGHT);
		else if (m_DecisionTable->IsNumeric(kernel_index, m_Masked))
			style.SetHorizontalAlignment(DT_RIGHT);
		else
			style.SetHorizontalAlignment(DT_CENTER);

		// Set background.
		if (!m_Masked) {
			if (m_DecisionTable->IsAttributeDisabled(kernel_index))
				style.SetInterior(CGXBrush().SetColor(RGB(200, 200, 200)));
			else
				style.SetInterior(CGXBrush().SetColor(RGB(255, 255, 255)));
		}
  
		// Set font.
		if (m_DecisionTable->IsDecision(kernel_index, m_Masked))
			style.SetFont(CGXFont().SetBold(TRUE));
		else
			style.SetFont(CGXFont().SetBold(FALSE));

		// Do the style setting.
    SetStyleRange(CGXRange().SetCols(j), style);

	}

	// Make sure column headers have the same height.
	ResizeRowHeightsToFit(CGXRange().SetRows(0));

	LockUpdate(bLockOld);

	// Redraw updated columns.
	RedrawRowCol(CGXRange().SetCols(first, last));

}

//-------------------------------------------------------------------
// Method........: OnColDisable
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewDecisionTable::OnColDisable()
{

  CRowColArray selected;
  ROWCOL       no_selected = GetSelectedCols(selected, TRUE);

	// Is a decision table "attached"?
	if (m_DecisionTable.IsNULL())
		return;

	// Does the decision table have any children?
	if (m_DecisionTable->HasChildren()) {
		Message::Error("Cannot disable/enable attributes, the decision table has children.");
		return;
	}

	// Are any columns selected?
	if (no_selected == 0) {
		Message::Error("No columns selected.");
		return;
	}

	Vector(int) kernel_indices;

	int i;

	// Convert from (unmasked) front-end indices to (unmasked) kernel indices.
	for (i = 0; i < no_selected; i++) {
		if (selected[i] > 0)
			kernel_indices.push_back(selected[i] - 1);
	}

	CDialogAttributeMask dialog;

	// Set table data.
	dialog.m_Table      = m_DecisionTable;
	dialog.m_Attributes = kernel_indices;

	if (dialog.DoModal() != IDOK)
		return;

	// Allows modifying of the grid.
	// GetParam()->SetLockReadOnly(FALSE);

	// Mark columns in front-end as masked.
	for (i = 0; i < no_selected; i++)
		InitializeColumnAppearance(selected[i], selected[i]);

	// Reset the LockReadOnly state.
	// GetParam()->SetLockReadOnly(TRUE);

	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);
		
	// Update other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_DecisionTable.GetPointer());
	CHint  pHint(CHint::HINT_GRID_COL_MASK, 0, 0);

	GetDocument()->UpdateAllViews(this, lHint, &pHint);

}

//-------------------------------------------------------------------
// Method........: OnColStatus
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewDecisionTable::OnColStatus()
{

#if 0
  CRowColArray selected;
  ROWCOL       no_selected = GetSelectedCols(selected, TRUE);

	// Is a decision table "attached"?
	if (m_DecisionTable.IsNULL())
		return;

	// Does the decision table have any children?
	if (m_DecisionTable->HasChildren()) {
		Message::Error("Cannot alter status fields, the decision table has children.");
		return;
	}

	// Are any columns selected?
	if (no_selected == 0) {
		Message::Error("No columns selected.");
		return;
	}

	Vector(int) kernel_indices;
	int         i;

	// Convert from (unmasked) front-end indices to (unmasked) kernel indices.
	for (i = 0; i < no_selected; i++) {
		if (selected[i] > 0)
			kernel_indices.push_back(selected[i] - 1);
	}

	CDialogAttributeStatus dialog;

	// Set table data.
	dialog.m_Table      = m_DecisionTable;
	dialog.m_Attributes = kernel_indices;

	if (dialog.DoModal() != IDOK)
		return;

	// Allows modifying of the grid.
	// GetParam()->SetLockReadOnly(FALSE);

	// Change column appearance in front-end.
	for (i = 0; i < no_selected; i++)
		InitializeColumnAppearance(selected[i], selected[i]);

	// Reset the LockReadOnly state.
	// GetParam()->SetLockReadOnly(TRUE);
		
	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);

	// Update other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_DecisionTable.GetPointer());
	CHint  pHint(CHint::HINT_GRID_COL_STAT, 0, 0);

	GetDocument()->UpdateAllViews(this, lHint, &pHint);
#endif

}

//-------------------------------------------------------------------
// Method........: OnUpdate
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewDecisionTable::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{

	// Is there anything to update?
	if (m_DecisionTable.IsNULL()) {
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

	// Check relation between identifier and current table.
	if (m_DecisionTable.GetPointer() != identifier.GetPointer()) {
	  CViewGrid::OnUpdate(pSender, lHint, pHint);
		return;
	}

	CWaitCursor wait;

	// Check for hint type.
	switch (hint->m_Hint) {
		case CHint::HINT_GRID_COL_MASK:    {   // An attribute's disablement/enablement mask has changed.
			InitializeColumnAppearance(1, GetColCount());
			break;
		}
		case CHint::HINT_GRID_COL_NAME:    {   // An attribute's name has changed.
			InitializeColumnAppearance(hint->m_Column, hint->m_Column);
			break;
		}
		case CHint::HINT_GRID_COL_STAT:    {   // An attribute's status has changed.
			InitializeColumnAppearance(1, GetColCount());
			break;
		}
		case CHint::HINT_GRID_COL_SWAP:    {   // Two attributes have been swapped.
			int i = hint->m_Row;
			int j = hint->m_Column;
			MoveCols(i, i, j, GX_UPDATENOW, gxDo);
			MoveCols(j - 1, j - 1, i, GX_UPDATENOW, gxDo);
			break;
		}
		case CHint::HINT_GRID_COL_INS:     {   // An attribute has been inserted/appended.
			InsertCols(hint->m_Column, 1);
			InitializeColumnAppearance(hint->m_Column, hint->m_Column);
		}
		case CHint::HINT_GRID_COL_DEL:     {   // An attribute has been deleted
			Message::Debug("View not updated after column del., not implemented yet.");
			break;
		}
		case CHint::HINT_GRID_ROW_DEL:     {   // An object has been deleted
			Message::Debug("View not updated after object del., not implemented yet.");
			break;
		}
		case CHint::HINT_GRID_ROW_INS:     {   // An object has been inserted/appended.
			Message::Debug("View not updated after object ins./app., not implemented yet.");
			break;
		}
		case CHint::HINT_GRID_ROW_SORT:    {   // The objects have been sorted.
			Message::Debug("View not updated after object sorting, not implemented yet.");
			break;
		}
		case CHint::HINT_GRID_CELLCHANGE: {    // A single table entry has changed.
			// GetParam()->SetLockReadOnly(FALSE);
			int r = hint->m_Row;
			int c = hint->m_Column;
			SetValueRange(CGXRange(r, c), (m_DecisionTable->GetEntry(r - 1, c - 1, m_UseDictionary, m_Masked)).GetBuffer());
			// GetParam()->SetLockReadOnly(TRUE);
			break;
		}

	}
	
	CViewGrid::OnUpdate(pSender, lHint, pHint);
	
}
 
