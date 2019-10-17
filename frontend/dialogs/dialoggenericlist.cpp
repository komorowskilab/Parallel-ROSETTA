// dialoggenericlist.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\dialoggenericlist.h>

#include <kernel\basic\undefined.h>
#include <kernel\basic\macros.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogGenericList dialog


CDialogGenericList::CDialogGenericList(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogGenericList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogGenericList)
	m_StaticHeader = _T("");
	//}}AFX_DATA_INIT

	m_Header    = "Select an entry:";
	m_Title     = "";
	m_Selection = Undefined::Integer();
	m_CanEdit   = false;
	m_CanSelect = true;

}


void CDialogGenericList::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogGenericList)
	DDX_Control(pDX, IDC_LIST_ENTRIES, m_ListEntries);
	DDX_Text(pDX, IDC_STATIC_HEADER, m_StaticHeader);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogGenericList, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogGenericList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ENTRIES, OnDblclkListEntries)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LIST_ENTRIES, OnBeginlabeleditListEntries)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_ENTRIES, OnEndlabeleditListEntries)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogGenericList message handlers

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogGenericList::OnOK() 
{

	m_Selection = 0;

	int i;

	// Determine index of selected item.
	for (i = 0; i < m_ListEntries.GetItemCount(); i++) {
		if (m_ListEntries.GetItemState(i, LVIS_SELECTED)) {
			m_Selection = i;
			break;
		}
	}
	
	CDialogAlgorithm::OnOK();

}

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogGenericList::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();

	// Set dialog title.
	SetWindowText(m_Title.GetBuffer());

	// Set header above list control.
	m_StaticHeader = m_Header.GetBuffer();

	CRect listrect;

	// Initialize the list control.
  m_ListEntries.GetClientRect(listrect);
  m_ListEntries.InsertColumn(0, "", LVCFMT_LEFT, 9 * listrect.Width() / 10, 0);
	m_ListEntries.DeleteAllItems();

	LV_ITEM	insertstruct;

	int i;

	// Fill list control.
	for (i = 0; i < m_Entries.size(); i++) {
		insertstruct.pszText    = const_cast(char *, m_Entries[i].GetBuffer());
		insertstruct.mask       = LVIF_TEXT;
		insertstruct.iItem      = INT_MAX;
		insertstruct.iSubItem   = 0;
		insertstruct.lParam     = reinterpret_cast(LPARAM, insertstruct.pszText);
		m_ListEntries.InsertItem(&insertstruct);
	}

  m_Selection = 0;

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
// Method........: OnDblclkListEntries
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogGenericList::OnDblclkListEntries(NMHDR* pNMHDR, LRESULT* pResult) 
{

	int i;

	bool is_selected = false;

	// Is an item selected?
	for (i = 0; i < m_ListEntries.GetItemCount(); i++) {
		if (m_ListEntries.GetItemState(i, LVIS_SELECTED)) {
			is_selected = true;
			break;
		}
	}

	if (m_CanSelect && is_selected)
		OnOK();
	
	*pResult = 0;

}

//-------------------------------------------------------------------
// Method........: OnBeginlabeleditListEntries
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogGenericList::OnBeginlabeleditListEntries(NMHDR* pNMHDR, LRESULT* pResult) 
{

	LV_DISPINFO *pDispInfo = (LV_DISPINFO *) pNMHDR;

	if (pDispInfo == NULL)
		return;
	
	*pResult = !m_CanEdit;

}

//-------------------------------------------------------------------
// Method........: OnEndlabeleditListEntries
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogGenericList::OnEndlabeleditListEntries(NMHDR* pNMHDR, LRESULT* pResult) 
{

	int i;

	LV_DISPINFO *pDispInfo = (LV_DISPINFO *) pNMHDR;

	if (pDispInfo == NULL)
		return;

	if (pDispInfo->item.pszText == NULL)
		return;

	bool is_selected = false;

	// Is an item selected?
	for (i = 0; i < m_ListEntries.GetItemCount(); i++) {
		if (m_ListEntries.GetItemState(i, LVIS_SELECTED)) {
			is_selected = true;
			break;
		}
	}

	if (!is_selected)
		return;

	// Update.
	if (m_CanEdit && is_selected) {
		m_Entries[i] = pDispInfo->item.pszText;
		pDispInfo->item.mask = TVIF_TEXT;
		m_ListEntries.SetItem(&pDispInfo->item);
	}
	
	*pResult = 0;

}
 
