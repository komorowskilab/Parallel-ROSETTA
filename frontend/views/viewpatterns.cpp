// viewpatterns.cpp : implementation file
//
// Comments:
// This view is read-only. If the programmer wants to change the grid, (i.e remove 
// or insert rows and columns, remember to use SetLockReadOnly(FALSE) before modifying,
// and SetLockReadOnly(TRUE) afterwards. 

#include <stdafx.h>

#include <frontend\rosetta.h>
#include <frontend\rosettadoc.h>

#include <frontend\views\viewpatterns.h>

#include <kernel\structures\rule.h>
#include <kernel\structures\rules.h>
#include <kernel\structures\decisiontable.h>

#include <kernel\basic\message.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewPatterns

IMPLEMENT_DYNCREATE(CViewPatterns, CViewRules)

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

CViewPatterns::CViewPatterns()
{
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

CViewPatterns::~CViewPatterns()
{
}

BEGIN_MESSAGE_MAP(CViewPatterns, CViewRules)
	//{{AFX_MSG_MAP(CViewPatterns)
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

void CViewPatterns::OnDraw(CDC* pDC)
{
  CViewRules::OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CViewPatterns diagnostics

#ifdef _DEBUG
void CViewPatterns::AssertValid() const
{
	CViewRules::AssertValid();
}

void CViewPatterns::Dump(CDumpContext& dc) const
{
	CViewRules::Dump(dc);
}
#endif //_DEBUG

//-------------------------------------------------------------------
// Method........: OnInitialUpdate
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewPatterns::OnInitialUpdate() 
{
	CViewRules::OnInitialUpdate();
}

//-------------------------------------------------------------------
// Method........: GetNoColumns
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....: 
//===================================================================

int CViewPatterns::GetNoColumns() const {
	return 5;
}

//-------------------------------------------------------------------
// Method........: SetColumnHeaders
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....: 
//===================================================================

void CViewPatterns::SetColumnHeaders() {

	const int no_headers = 5;
	CString   header[no_headers];

	// Define strings.
	header[0] = CString("Pattern");
	header[1] = CString("LHS Support");
	header[2] = CString("LHS Coverage");
	header[3] = CString("LHS Stability");
	header[4] = CString("LHS Length");

	int i;

	// Set column headers.
	for (i = 0; i < no_headers; i++)
		SetStyleRange(CGXRange(0, i + 1),  CGXStyle().SetValue(header[i]));

}

//-------------------------------------------------------------------
// Method........: LoadRuleIntoGrid
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....: 
//===================================================================

void CViewPatterns::LoadRuleIntoGrid(const Rule &rule, int i, const DecisionTable *table, bool masked, const Vector(int) &/*decisions*/, const Vector(int) &/*cardinalities*/, int no_objects) {

	String formatted, lhs_support, lhs_coverage, lhs_stability, lhs_length;

	// Format pattern.
	if (!rule.Format(formatted, m_UseDictionary ? table : NULL, masked))
		formatted = "Formatting error.";

	lhs_length    = String::Format(rule.GetNoConditionDescriptors());
	lhs_coverage  = String::Format(rule.GetCoverage(no_objects));
	lhs_stability = String::Format(rule.GetStability());
	lhs_support   = String::Format(rule.GetSupport());

	// Fill grid row.
	SetValueRange(CGXRange(i + 1, 1),  formatted.GetBuffer());
	SetValueRange(CGXRange(i + 1, 2),  lhs_support.GetBuffer());
	SetValueRange(CGXRange(i + 1, 3),  lhs_coverage.GetBuffer());
	SetValueRange(CGXRange(i + 1, 4),  lhs_stability.GetBuffer());
	SetValueRange(CGXRange(i + 1, 5),  lhs_length.GetBuffer());

}

//-------------------------------------------------------------------
// Method........: CreateColumnMenu
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

CMenu *CViewPatterns::CreateColumnMenu(CMenu *menu, ROWCOL nCol)
{

	if (menu == NULL)
		return NULL;

	// If it's the LHS support column...
	if (nCol == 2) {
		menu->CreatePopupMenu();
		menu->AppendMenu(MF_STRING | MF_ENABLED, CVIEWRULES_COL_SORT_LHSSUPPORT, "Sort");
		return menu;
	}

	// If it's the LHS coverage column...
	if (nCol == 3) {
		menu->CreatePopupMenu();
		menu->AppendMenu(MF_STRING | MF_ENABLED, CVIEWRULES_COL_SORT_LHSCOVERAGE, "Sort");
		return menu;
	}

	// If it's the LHS stability column...
	if (nCol == 4) {
		menu->CreatePopupMenu();
		menu->AppendMenu(MF_STRING | MF_ENABLED, CVIEWRULES_COL_SORT_LHSSTABILITY, "Sort");
		return menu;
	}

	// If it's the LHS length column...
	if (nCol == 5) {
		menu->CreatePopupMenu();
		menu->AppendMenu(MF_STRING | MF_ENABLED, CVIEWRULES_COL_SORT_LHSLENGTH, "Sort");
		return menu;
	}

	return NULL;
 
}


 
