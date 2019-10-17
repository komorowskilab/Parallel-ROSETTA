// mylistctrl.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\mylistctrl.h>
#include <frontend\dialogs\sortclass.h>

#include <kernel\basic\message.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl

CMyListCtrl::CMyListCtrl()
{
}

CMyListCtrl::~CMyListCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CMyListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnCopytoclipboard)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl message handlers

//-------------------------------------------------------------------
// Method........: OnColumnclick
// Author........: Aleksander šhrn
// Date..........: 980426
// Description...: Sorts the clicked column.
// Comments......: 
// Revisions.....:
//===================================================================

void CMyListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{

	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *) pNMHDR;
       
  // bAscending will be use to order from lower to higher or higher to lower
  bool bAscending = true;
        
  CSortClass csc(this, pNMListView->iSubItem);
	csc.Sort(bAscending);
  	
	*pResult = 0;

}

//-------------------------------------------------------------------
// Method........: OnContextMenu
// Author........: Aleksander šhrn
// Date..........: 990610
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CMyListCtrl::OnContextMenu(CWnd*, CPoint point)
{

	CMenu menu;

	// Make a menu.
	menu.CreatePopupMenu();

	// Hack: Using LoadMenu, the menu seems to get "zero" width. As an ugly hack,
	// hardcode the damn menu instead of using the menu resource.
	//menu.LoadMenu(IDR_MYLISTCTRLMENU);
	menu.AppendMenu(MF_STRING, ID_EDIT_COPY, "Copy");

	// Display it.
  menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);

	// Clean up.
	menu.DestroyMenu();
	
}

//-------------------------------------------------------------------
// Method  ......: OnCopytoclipboard
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================
                                           
void CMyListCtrl::OnCopytoclipboard() 
{

	Message message;

	// Open clipboard.
  if (!OpenClipboard()) {
		message.Error("Cannot open the Clipboard.");
    return;
  }

  // Remove the current Clipboard contents
  if (!EmptyClipboard()) {
		message.Error("Cannot empty the Clipboard.");
		CloseClipboard();
    return;
  }

	// Determine how many columns the control has. We first get a pointer to 
	// the header control and then use the header control to query the number 
	// of columns it has.
	CHeaderCtrl *header = (CHeaderCtrl *) GetDlgItem(0);
	int no_columns = header->GetItemCount();

	CString text;

  // Create a string with the current data.
	for (int i = 0; i < GetItemCount(); i++) {
		for (int j = 0; j < no_columns; j++) {
			text += GetItemText(i, j);
			text += '\t';
		}
		text += '\n';
	}

	// Prepare for copying.
	HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, text.GetLength() + 1);
	char *buffer = (char *) GlobalLock(clipbuffer);
	strcpy(buffer, LPCSTR(text));
	GlobalUnlock(clipbuffer);

	// Copy text to clipboard.
	if (!SetClipboardData(CF_TEXT, clipbuffer)) {
		message.Error("Cannot copy text to Clipboard.");
		CloseClipboard();
		return;
	}

	// Close clipboard.
  CloseClipboard();

	message.Notify("List control copied to Clipboard.");

}
 
