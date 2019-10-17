// viewgrid.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>
#include <frontend\rosettadoc.h>
#include <frontend\mainfrm.h>

#include <frontend\views\viewgrid.h>
#include <frontend\views\hint.h>

#include <common\configuration.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewGrid

IMPLEMENT_DYNCREATE(CViewGrid, CGXGridView)

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

CViewGrid::CViewGrid()
{

	m_UseDictionary = TRUE;

	m_Masked        = TRUE;

	m_RowOffset     = 1;  // Default: Row 1 in front-end (grid) means "row" 0 in kernel.
	m_ColOffset     = 1;  // Default: Column 1 in front-end (grid) means "column" 0 in kernel.

}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

CViewGrid::~CViewGrid()
{
}

//-------------------------------------------------------------------
// Method  ......: GetDocument
// Author........: Aleksander šhrn
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

CRosettaDoc * 
CViewGrid::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRosettaDoc)));
	return (CRosettaDoc *) m_pDocument;
}

//-------------------------------------------------------------------
// Method  ......: GetTopProject
// Author........: Aleksander šhrn
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

Project * 
CViewGrid::GetTopProject()
{

	if (GetDocument() == NULL)
		return NULL;

	return GetDocument()->GetProject();

}

BEGIN_MESSAGE_MAP(CViewGrid, CGXGridView)
	//{{AFX_MSG_MAP(CViewGrid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewGrid diagnostics

#ifdef _DEBUG
void CViewGrid::AssertValid() const
{
	CGXGridView::AssertValid();
}

void CViewGrid::Dump(CDumpContext& dc) const
{
	CGXGridView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CViewGrid message handlers

//-------------------------------------------------------------------
// Method........: OnDraw
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewGrid::OnDraw(CDC* pDC)
{
	CGXGridView::OnDraw(pDC);
}

//-------------------------------------------------------------------
// Method........: OnInitialUpdate
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewGrid::OnInitialUpdate() 
{
	CGXGridView::OnInitialUpdate();
}

//-------------------------------------------------------------------
// Method........: OnUseDictionary
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....: 
//===================================================================

void CViewGrid::OnUseDictionary()
{

	// Get current read-only lock status.
	BOOL oldLockReadOnly = GetParam()->IsLockReadOnly();

	// We may have to alter read-only cells.
	GetParam()->SetLockReadOnly(FALSE);

	// Invert current dictionary mode.
  m_UseDictionary = !m_UseDictionary;

	// Reload kernel data into front-end grid, using new dictionary mode.
  LoadDataIntoGrid();

	// Revert to old read-only lock status.
	GetParam()->SetLockReadOnly(oldLockReadOnly);

	Redraw();

}

//-------------------------------------------------------------------
// Method........: OnDisplaySettings
// Author........: 
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CViewGrid::OnDisplaySettings()
{

  BOOL bSaveDefault = FALSE;

	CGXProperties* pPropertyObj = GetParam( )->GetProperties();
	ASSERT(pPropertyObj->IsKindOf(RUNTIME_CLASS(CGXProperties)));

	// CGXProperties* pNewSettings = new CGXProperties;
	// *pNewSettings = *pPropertyObj;

	CGXProperties pNewSettings;
	pNewSettings = *pPropertyObj;

	// CGXDisplayPropertiesDialog dlg(pNewSettings, GetParam()->GetStylesMap(), &bSaveDefault, FALSE);

	CGXDisplayPropertiesDialog dlg(&pNewSettings, GetParam()->GetStylesMap(), &bSaveDefault, FALSE);

	int result = dlg.DoModal();
	if (result == IDOK) {
    // *pPropertyObj = *pNewSettings;
		*pPropertyObj = pNewSettings;
		if (bSaveDefault)
			pPropertyObj->WriteProfile();
		SetModifiedFlag();
		Redraw();
	}

	// delete pNewSettings;

}

//-------------------------------------------------------------------
// Method........: OnCmdMsg
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

BOOL CViewGrid::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	
	if (pHandlerInfo != NULL) 
		return CGXGridView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo); 
	
	if (nID > CVIEWGRID_FIRSTVALUE && nID < CVIEWGRID_LASTVALUE) {
	  if (nCode == CN_COMMAND) {
      switch (nID) {
			  case CVIEWGRID_GRID_USEDICTIONARY:      OnUseDictionary();   return TRUE;
        case CVIEWGRID_GRID_RESIZETOFIT:        ResizeToFit();       return TRUE;
        case CVIEWGRID_GRID_DISPLAYSETTINGS:    OnDisplaySettings(); return TRUE;
      }
    }
    else if (nCode == CN_UPDATE_COMMAND_UI) {
    }		
  }
  
	return CGXGridView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

}

//-------------------------------------------------------------------
// Method........: OnBeginPrinting
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewGrid::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CGXGridView::OnBeginPrinting(pDC, pInfo);
}

//-------------------------------------------------------------------
// Method........: OnEndPrinting
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewGrid::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CGXGridView::OnEndPrinting(pDC, pInfo);
}

//-------------------------------------------------------------------
// Method........: OnPreparePrinting
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

BOOL CViewGrid::OnPreparePrinting(CPrintInfo* pInfo) 
{
	return CGXGridView::OnPreparePrinting(pInfo);
}

//-------------------------------------------------------------------
// Method........: PreCreateWindow
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

BOOL CViewGrid::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CGXGridView::PreCreateWindow(cs);
}
	
//-------------------------------------------------------------------
// Method........: OnUpdate
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewGrid::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{

	// Is there anything to update?
	if (m_Structure.IsNULL()) {
		CGXGridView::OnUpdate(pSender, lHint, pHint);
		return;
	}

	// Re-cast passed hints.  
	Handle<Identifier> identifier = reinterpret_cast(Identifier *, lHint);
	CHint             *hint       = dynamic_cast(CHint *, pHint);

  // Verify hints.
	if ((hint == NULL) || (identifier == NULL)) {
		CGXGridView::OnUpdate(pSender, lHint, pHint);
		return;
	}

	// Check relation between identifier and current structure.
	if (m_Structure.GetPointer() != identifier.GetPointer()) {
		if (m_Structure->FindParent(identifier->GetId()) != identifier.GetPointer()) {
			CGXGridView::OnUpdate(pSender, lHint, pHint);
			return;
		}
	}

	CWaitCursor wait;

	// Allow mod. of read-only cells.
	GetParam()->SetLockReadOnly(FALSE);

	// Check for hint type.
	switch (hint->m_Hint) {
		case CHint::HINT_NAMECHANGE: { // Structure's name has changed.
			String name = m_Structure->GetName();
			GetParentFrame()->SetWindowText(name.GetBuffer());
			break;
		}
		case CHint::HINT_MODIFIED: {   // Structure or structure's ancestor has been modified in some unspecified way.
			InitializeGridAppearance();
			LoadDataIntoGrid();
			break;
		}
	}

	// Disallow mod. of read-only cells.
	GetParam()->SetLockReadOnly(FALSE);
	
	CGXGridView::OnUpdate(pSender, lHint, pHint);	

}

//-------------------------------------------------------------------
// Method........: OnRButtonClickedRowCol
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

BOOL CViewGrid::OnRButtonClickedRowCol(ROWCOL nRow, ROWCOL nCol, UINT nFlags, CPoint pt)
{

  CGXGridView::OnRButtonClickedRowCol(nRow, nCol, nFlags, pt);

  ClientToScreen(&pt);

	// Define an empty menu...
	CMenu empty, *filled = NULL;

	// ...fill it with its contents...
  if ((nRow == 0) && (nCol == 0)) {  // Was the grid table button hit?
		filled = CreateGridMenu(&empty);
	}
	else if (nRow == 0) {              // Was a column button hit?
		filled = CreateColumnMenu(&empty, nCol);
	}
	else if (nCol == 0) {              // Was a row button hit?
		filled = CreateRowMenu(&empty, nRow);
	}

	// ...and display it and track it...
	if (filled != NULL)
	  filled->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
  
  return TRUE;

}

//-------------------------------------------------------------------
// Method........: OnPasteFromClipboard
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

BOOL CViewGrid::OnPasteFromClipboard(const CGXRange& range) {

	// Paste into the grid.
	if (!CGXGridCore::OnPasteFromClipboard(range))
		return FALSE;

	// Message::Information("OnPasteFromClipboard called.");

	return TRUE;

}

//-------------------------------------------------------------------
// Method........: CreateGridMenu
// Author........: 
// Date..........: 
// Description...: Typically overloaded by subclasses.
// Revisions.....:
//===================================================================

CMenu *CViewGrid::CreateGridMenu(CMenu *menu)
{

	if (menu == NULL)
		return NULL;

	UINT IS_CHECKED = MF_CHECKED;

	if (!m_UseDictionary)
		IS_CHECKED = MF_UNCHECKED;

  menu->CreatePopupMenu();

	menu->AppendMenu(MF_STRING | MF_ENABLED | IS_CHECKED, CVIEWGRID_GRID_USEDICTIONARY,       "Use dictionary");
  menu->AppendMenu(MF_SEPARATOR);
  menu->AppendMenu(MF_STRING | MF_ENABLED,              CVIEWGRID_GRID_RESIZETOFIT,         "Resize to fit");
  menu->AppendMenu(MF_STRING | MF_ENABLED,              CVIEWGRID_GRID_DISPLAYSETTINGS,     "Properties...");
	
	return menu;
 
}

//-------------------------------------------------------------------
// Method........: CreateRowMenu
// Author........: 
// Date..........: 
// Description...: Typically overloaded by subclasses.
// Revisions.....:
//===================================================================

CMenu *CViewGrid::CreateRowMenu(CMenu * /*menu*/, ROWCOL /*nRow*/)
{
	Message::Debug("CViewGrid::CreateRowMenu() not overloaded.");
	return NULL;
}

//-------------------------------------------------------------------
// Method........: CreateColumnMenu
// Author........: 
// Date..........: 
// Description...: Typically overloaded by subclasses.
// Revisions.....:
//===================================================================

CMenu *CViewGrid::CreateColumnMenu(CMenu * /*menu*/, ROWCOL /*nCol*/)
{
	Message::Debug("CViewGrid::CreateColumnMenu() not overloaded.");
	return NULL;
}

//-------------------------------------------------------------------
// Method........: SetStructure
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewGrid::SetStructure(Structure *structure) 
{
	
	m_Structure = structure;

	if (structure == NULL)
		return;

	// Set window text.
	String name = structure->GetName();
	GetParentFrame()->SetWindowText(name.GetBuffer());

	CWaitCursor wait;

	GetParam()->EnableUndo(FALSE);

	// Initialize appearance of grid.
  InitializeGridAppearance();

	// Move data from kernel into front-end grid.
  LoadDataIntoGrid(); 
  	
	// Size the window properly.
	ResizeToFit();

}

//-------------------------------------------------------------------
// Method........: ResizeToFit
// Author........: 
// Date..........: 
// Description...: Resizes view to fit embedded grid.
// Comments......: Implementation lifted from OG 1.2 FAQ.
// Revisions.....:
//===================================================================

void CViewGrid::ResizeToFit() 
{	

	UpdateFontMetrics();

	CRect oClientRect  = GetGridRect();
	
	int	colTotalWidth  = CalcSumOfColWidths(0, GetColCount(), oClientRect.Width());
	int colTotalHeight = CalcSumOfRowHeights(0, GetRowCount(), oClientRect.Height());

	// Check if the calculated value is more than the default client area of the grid.
  if (colTotalHeight >= oClientRect.Height())
		colTotalWidth += ::GetSystemMetrics(SM_CXVSCROLL);

	colTotalWidth += 7 * (::GetSystemMetrics(SM_CXEDGE));
	
  if (colTotalWidth >= oClientRect.Width())
		colTotalHeight += ::GetSystemMetrics( SM_CYHSCROLL);
		
	colTotalHeight += 7 * (::GetSystemMetrics(SM_CYEDGE));
	
	// Add the window title bar to the height.
	colTotalHeight += ::GetSystemMetrics(SM_CYCAPTION);

	GetParentFrame()->SetWindowPos(NULL, 0, 0, colTotalWidth, colTotalHeight, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);

}

//-------------------------------------------------------------------
// Method........: InitializeGridAppearance
// Author........: 
// Date..........: 
// Description...: Should normally be overloaded by subclasses.
// Revisions.....:
//===================================================================

void CViewGrid::InitializeGridAppearance() 
{

	// Set standard font size.
	ChangeStandardStyle(CGXStyle().SetFont(CGXFont().SetSize(Configuration::GUI::GetGXFontSize())));

}

//-------------------------------------------------------------------
// Method........: LoadDataIntoGrid
// Author........: 
// Date..........: 
// Description...: Should normally be overloaded by subclasses.
// Revisions.....:
//===================================================================

void CViewGrid::LoadDataIntoGrid() 
{
	Message::Debug("CViewGrid::LoadDataIntoGrid() not overloaded.");
}
 
