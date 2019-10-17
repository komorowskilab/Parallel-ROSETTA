// viewrules.cpp : implementation file
//
// Comments:
// This view is read-only. If the programmer wants to change the grid, (i.e remove 
// or insert rows and columns, remember to use SetLockReadOnly(FALSE) before modifying,
// and SetLockReadOnly(TRUE) afterwards. 

#include <stdafx.h>

#include <frontend\rosetta.h>
#include <frontend\rosettadoc.h>

#include <frontend\views\viewrules.h>
#include <frontend\views\hint.h>

#include <kernel\structures\rule.h>
#include <kernel\structures\rules.h>
#include <kernel\structures\reduct.h>
#include <kernel\structures\reducts.h>
#include <kernel\structures\decisiontable.h>
#include <kernel\structures\dictionary.h>
#include <kernel\structures\project.h>

#include <kernel\utilities\mathkit.h>

#include <kernel\basic\message.h>

#include <common\messagehelper.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewRules

IMPLEMENT_DYNCREATE(CViewRules, CViewGrid)

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

CViewRules::CViewRules()
{
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

CViewRules::~CViewRules()
{
}

BEGIN_MESSAGE_MAP(CViewRules, CViewGrid)
	//{{AFX_MSG_MAP(CViewRules)
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

void CViewRules::OnDraw(CDC* pDC)
{
  CViewGrid::OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CViewRules diagnostics

#ifdef _DEBUG
void CViewRules::AssertValid() const
{
	CViewGrid::AssertValid();
}

void CViewRules::Dump(CDumpContext& dc) const
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

void CViewRules::OnInitialUpdate() 
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

void CViewRules::SetStructure(Structure *structure)
{

	m_Rules = dynamic_cast(Rules *, structure);
	
	CViewGrid::SetStructure(m_Rules.GetPointer());
	
	SetReadOnly(TRUE);
	
}

//-------------------------------------------------------------------
// Method........: GetNoColumns
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....: 
//===================================================================

int CViewRules::GetNoColumns() const {
	return 9;
}

//-------------------------------------------------------------------
// Method........: SetColumnHeaders
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....: 
//===================================================================

void CViewRules::SetColumnHeaders() {

	const int no_headers = 9;
	CString   header[no_headers];

	// Define strings.
	header[0] = CString("Rule");            // + "\n" + "LHS => RHS";
	header[1] = CString("LHS Support");     // + "\n" + "n * Pr(LHS)";
	header[2] = CString("RHS Support");     // + "\n" + "n * Pr(LHS and RHS)";
	header[3] = CString("RHS Accuracy");    // + "\n" + "Pr(RHS | LHS)";
	header[4] = CString("LHS Coverage");    // + "\n" + "Pr(LHS)";
	header[5] = CString("RHS Coverage");    // + "\n" + "Pr(LHS | RHS)";
	header[6] = CString("RHS Stability");   // + "\n" + "RHS";
	header[7] = CString("LHS Length");      // + "\n" + "LHS";
	header[8] = CString("RHS Length");      // + "\n" + "RHS";

	// Set column headers.
	for (int i = 0; i < no_headers; i++)
		SetStyleRange(CGXRange(0, i + 1),  CGXStyle().SetValue(header[i]));

	/*
	// Resize 'em.
	for (i = 0; i < no_headers; i++)
		SetColWidth(i + 1, i + 1, Width_LPtoDP(header[i].GetLength()));
	*/

}

//-------------------------------------------------------------------
// Method........: InitializeGridAppearance
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....: 
//===================================================================

void CViewRules::InitializeGridAppearance()
{

	CViewGrid::InitializeGridAppearance();

	// Is a set of rules attached to the view?
  if (m_Rules.IsNULL())
		return;

	// Get grid control parameters.
	CGXGridParam *params = GetParam();

	// Disable some stuff.
  params->EnableMoveRows(FALSE);
  params->EnableMoveCols(FALSE);

	// Set window text.
  GetParentFrame()->SetWindowText((m_Rules->GetName()).GetBuffer());

	BOOL bLockOld = LockUpdate(TRUE);

	// Set the number of rows and columns.
	SetColCount(GetNoColumns());
  SetRowCount(m_Rules->GetNoRules());

	SetColumnHeaders();

	// Make sure column headers are visible.
	//ResizeRowHeightsToFit(CGXRange().SetRows(0));
	//ResizeColWidthsToFit(CGXRange().SetCols(1, GetColCount()));

  LockUpdate(bLockOld);
	
}

//-------------------------------------------------------------------
// Method........: LoadRuleIntoGrid
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....: 
//===================================================================

void CViewRules::LoadRuleIntoGrid(const Rule &rule, int i, const DecisionTable *table, bool masked, const Vector(int) &decisions, const Vector(int) &cardinalities, int no_objects) {

	String formatted, lhs_support, rhs_support, rhs_accuracy, lhs_coverage, rhs_coverage, lhs_stability, rhs_stability, lhs_length, rhs_length;

	// Format rule..
	if (!rule.Format(formatted, m_UseDictionary ? table : NULL, masked))
		formatted = "Formatting error.";

	int no_decision_values = rule.GetNoDecisionValues();

	// Format quantitative information.
	for (int j = 0; j < no_decision_values; j++) {

		rhs_support   += String::Format(rule.GetSupport(j));
		rhs_accuracy  += String::Format(rule.GetAccuracy(j));
		rhs_stability += String::Format(rule.GetStability(j));
		rhs_coverage  += String::Format(rule.GetCoverage(j, decisions, cardinalities));

		if (j < (no_decision_values - 1)) {
			rhs_support   += ", ";
			rhs_accuracy  += ", ";
			rhs_stability += ", ";
			rhs_coverage  += ", ";
		}

	}

	rhs_length   = String::Format(rule.GetNoDecisionValues());
	lhs_length   = String::Format(rule.GetNoConditionDescriptors());
	lhs_coverage = String::Format(rule.GetCoverage(no_objects));
	lhs_support  = String::Format(rule.GetSupport());

	// Fill grid row.
	SetValueRange(CGXRange(i + 1, 1), formatted.GetBuffer());
	SetValueRange(CGXRange(i + 1, 2), lhs_support.GetBuffer());
	SetValueRange(CGXRange(i + 1, 3), rhs_support.GetBuffer());
	SetValueRange(CGXRange(i + 1, 4), rhs_accuracy.GetBuffer());
	SetValueRange(CGXRange(i + 1, 5), lhs_coverage.GetBuffer());
	SetValueRange(CGXRange(i + 1, 6), rhs_coverage.GetBuffer());
	SetValueRange(CGXRange(i + 1, 7), rhs_stability.GetBuffer());
	SetValueRange(CGXRange(i + 1, 8), lhs_length.GetBuffer());
	SetValueRange(CGXRange(i + 1, 9), rhs_length.GetBuffer());

}

//-------------------------------------------------------------------
// Method........: LoadDataIntoGrid
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....: 
//===================================================================

void CViewRules::LoadDataIntoGrid()
{

	// Is a set of rules attached to the view?
	if (m_Rules.IsNULL())
		return;

	// Get the number of rules.
	int no_rules = m_Rules->GetNoRules();

	bool masked = true;

	// Get the originating decision table.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, m_Rules->FindParent(DECISIONTABLE, *GetTopProject()));

	if (table == NULL)
		return;

	// Get size of table.
	int no_objects = table->GetNoObjects(masked);

	// Compute set of possible decision values.
	Vector(int) decisions;
	Vector(int) cardinalities;

	if (!table->GetValueSet(decisions, cardinalities, table->GetDecisionAttribute(masked), masked))
		return;

	// Alter front-end dimensions if the grid's size is too small/large.
	if (GetRowCount() != no_rules)
		InitializeGridAppearance();

	BOOL bLockOld = LockUpdate(TRUE);

	// How often should we poll for cancellations? How many rules do, say, 5 percent correspond to?
	int pollsize = no_rules / 20, pollcount = 0;

	CWaitCursor wait;
	Message     message;

	// Fill the front-end grid with data from kernel.
	for (int i = 0; i < no_rules; i++) {

		// Cancel operation?
		if (pollsize == pollcount || i == no_rules - 1) {
			if (!message.Progress("Preparing rules/patterns for display...", i, no_rules)) {
				Message::Warning("Rules " + String::Format(i + 1) + " through " + String::Format(no_rules + 1) + " will not be displayed properly.");
        break;
      }
			pollcount = 0;
		}
		else {
			pollcount++;
		}

		Handle<Rule> rule = m_Rules->GetRule(i);

		LoadRuleIntoGrid(*rule, i, table.GetPointer(), masked, decisions, cardinalities, no_objects);

	}

	// Set the row count if the operation was aborted.
	if (i < no_rules)
		SetRowCount(i);

	// Unless there are very many rules, resize column widths.
	if (no_rules < 1000)
		ResizeColWidthsToFit(CGXRange().SetCols(1, GetColCount()));
			
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

CMenu *CViewRules::CreateRowMenu(CMenu *menu, ROWCOL /*nRow*/)
{

	if (menu == NULL)
		return NULL;

  menu->CreatePopupMenu();

  menu->AppendMenu(MF_STRING | MF_ENABLED, CVIEWRULES_ROW_REMOVE, "Remove");

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

CMenu *CViewRules::CreateColumnMenu(CMenu *menu, ROWCOL nCol)
{

	if (menu == NULL)
		return NULL;

	// If it's the rule column...
	if (nCol == 1) {
	  menu->CreatePopupMenu();
	  menu->AppendMenu(MF_STRING | MF_ENABLED, CVIEWRULES_COL_SORT_DECISION, "Sort");
	  return menu;
	}

	// If it's the LHS support column...
	if (nCol == 2) {
		menu->CreatePopupMenu();
		menu->AppendMenu(MF_STRING | MF_ENABLED, CVIEWRULES_COL_SORT_LHSSUPPORT, "Sort");
		return menu;
	}

	// If it's the RHS support column...
	if (nCol == 3) {
		menu->CreatePopupMenu();
		menu->AppendMenu(MF_STRING | MF_ENABLED, CVIEWRULES_COL_SORT_RHSSUPPORT, "Sort");
		return menu;
	}

	// If it's the RHS accuracy column...
	if (nCol == 4) {
	  menu->CreatePopupMenu();
	  menu->AppendMenu(MF_STRING | MF_ENABLED, CVIEWRULES_COL_SORT_RHSACCURACY, "Sort");
	  return menu;
	}

	// If it's the LHS coverage column...
	if (nCol == 5) {
		menu->CreatePopupMenu();
		menu->AppendMenu(MF_STRING | MF_ENABLED, CVIEWRULES_COL_SORT_LHSCOVERAGE, "Sort");
		return menu;
	}

	// If it's the RHS coverage column...
	if (nCol == 6) {
		menu->CreatePopupMenu();
		menu->AppendMenu(MF_STRING | MF_ENABLED, CVIEWRULES_COL_SORT_RHSCOVERAGE, "Sort");
		return menu;
	}

	// If it's the RHS stability column...
	if (nCol == 7) {
		menu->CreatePopupMenu();
		menu->AppendMenu(MF_STRING | MF_ENABLED, CVIEWRULES_COL_SORT_RHSSTABILITY, "Sort");
		return menu;
	}

	// If it's the LHS length column...
	if (nCol == 8) {
		menu->CreatePopupMenu();
		menu->AppendMenu(MF_STRING | MF_ENABLED, CVIEWRULES_COL_SORT_LHSLENGTH, "Sort");
		return menu;
	}

	// If it's the RHS length column...
	if (nCol == 9) {
		menu->CreatePopupMenu();
		menu->AppendMenu(MF_STRING | MF_ENABLED, CVIEWRULES_COL_SORT_RHSLENGTH, "Sort");
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

BOOL CViewRules::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{

	if (pHandlerInfo != NULL)
		return CViewGrid::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
    
  if ((nID > CVIEWRULES_FIRSTVALUE) && (nID < CVIEWRULES_LASTVALUE)) { 
		if (nCode == CN_COMMAND) {
      switch (nID) {
				case CVIEWRULES_ROW_REMOVE:              OnRowRemove();        return TRUE;
				case CVIEWRULES_COL_SORT_DECISION:       OnColSort(0,  TRUE);  return TRUE;
				case CVIEWRULES_COL_SORT_LHSSUPPORT:     OnColSort(1,  FALSE); return TRUE;
				case CVIEWRULES_COL_SORT_RHSSUPPORT:     OnColSort(2,  FALSE); return TRUE;
				case CVIEWRULES_COL_SORT_RHSACCURACY:    OnColSort(4,  FALSE); return TRUE;
				case CVIEWRULES_COL_SORT_LHSCOVERAGE:    OnColSort(5,  FALSE); return TRUE;
				case CVIEWRULES_COL_SORT_RHSCOVERAGE:    OnColSort(6,  FALSE); return TRUE;
				case CVIEWRULES_COL_SORT_LHSSTABILITY:   OnColSort(7,  FALSE); return TRUE;
				case CVIEWRULES_COL_SORT_RHSSTABILITY:   OnColSort(8,  FALSE); return TRUE;
				case CVIEWRULES_COL_SORT_LHSLENGTH:      OnColSort(9,  TRUE);  return TRUE;
				case CVIEWRULES_COL_SORT_RHSLENGTH:      OnColSort(10, TRUE);  return TRUE;
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

void CViewRules::OnRowRemove()
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
		if (!m_Rules->RemoveStructure(kernel_index)) {              // Remove object from kernel.
			Message::Error("Failed to remove rule from kernel.");
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

	// Touch rules.
	if (no_selected == 1) 
		m_Rules->Touch("Removed rule " + String::Format(static_cast(int, selected[0] - 1)) + ".");
	else
		m_Rules->Touch("Removed " + String::Format(static_cast(int, no_selected)) + " rules.");

	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);

	// Update other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_Rules.GetPointer());
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

void CViewRules::OnColSort(int property, BOOL ascending)
{

	// Get current read-only lock status.
	BOOL oldLockReadOnly = GetParam()->IsLockReadOnly();

	// We may have to alter read-only cells.
	GetParam()->SetLockReadOnly(FALSE);

	// Sort the rules.
	if (!m_Rules->Sort(property, ascending, dynamic_cast(DecisionTable *, m_Rules->FindParent(DECISIONTABLE)))) {
		Message::Error("Sorting failed.");
		return;
	}

	String action;

	// Touch with suitable action description.
	action = "Sorted rules by ";

	if (property == 0)
		action += "decision";
	else if (property == 1)
		action += "LHS support";
	else if (property == 2)
		action += "RHS support";
	else if (property == 4)
		action += "RHS accuracy";
	else if (property == 5)
		action += "LHS coverage";
	else if (property == 6)
		action += "RHS coverage";
	else if (property == 7)
		action += "LHS stability";
	else if (property == 8)
		action += "RHS stability";
	else if (property == 9)
		action += "LHS length";
	else if (property == 10)
		action += "RHS length";
	else
		action += "some undefined property";

	action += " (" + String((ascending) ? "ascending" : "descending") + ").";

	m_Rules->Touch(action);

	// Reload kernel data into front-end grid, now that they have been sorted.
  LoadDataIntoGrid();

	// Revert to old read-only lock status.
	GetParam()->SetLockReadOnly(oldLockReadOnly);

	// Flag document as modified.
	GetDocument()->SetModifiedFlag(TRUE);

	// Update other views.
	LPARAM lHint = reinterpret_cast(LPARAM, m_Rules.GetPointer());
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

void CViewRules::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{

	// Is there anything to update?
	if (m_Rules.IsNULL()) {
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

	// Check relation between identifier and current rule set.
	if (m_Rules.GetPointer() != identifier.GetPointer()) {
		if (m_Rules->FindParent(identifier->GetId()) != identifier.GetPointer()) {
			CViewGrid::OnUpdate(pSender, lHint, pHint);
			return;
		}
	}

	CWaitCursor wait;

	// Allow mod. of read-only cells.
	GetParam()->SetLockReadOnly(FALSE);

	// Check for hint type.
	switch (hint->m_Hint) {
		case CHint::HINT_GRID_COL_NAME: { // Rule set's ancestor table/dictionary has changed an attribute name.
			hint->m_Hint = CHint::HINT_MODIFIED;  // Make the hint more general before passing it on upwards.
			break;
		}
		case CHint::HINT_GRID_ROW_DEL: {  // Rules have been delted from the rule set.
			Message::Debug("View not updated after row del., not implemented yet.");
			break;
		}
		case CHint::HINT_GRID_ROW_INS: {  // Rules have been inserted/appended to the rule set.
			Message::Debug("View not updated after row ins./app., not implemented yet.");
			break;
		}
	}

	// Disallow mod. of read-only cells.
	GetParam()->SetLockReadOnly(TRUE);

	// The update hint may be caught higher up in the hierarchy.
	CViewGrid::OnUpdate(pSender, lHint, pHint);
	
}
 
