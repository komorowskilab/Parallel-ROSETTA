#include <stdafx.h>

#include <frontend\rosettadoc.h>
#include <frontend\mainfrm.h>

#include <frontend\views\projecttreectrl.h>
#include <frontend\views\hint.h>

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

#include <kernel\structures\structure.h>
#include <kernel\structures\annotatedstructure.h>
#include <kernel\structures\parentstructure.h>
#include <kernel\structures\project.h>

#include <kernel\algorithms\algorithm.h>

#include <kernel\basic\message.h>

#include <common\objectmanager.h>

#include <frontend\views\viewproject.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//-------------------------------------------------------------------
// Method........: Constructor
// Author........: J—rn E. Nygjerd
// Date..........: 960407
// Description...: 
// Comments......: 
// Revisions.....:
//-------------------------------------------------------------------

CProjectTreeCtrl::CProjectTreeCtrl()
{
	m_bDragging = FALSE;
	m_pimagelist = NULL;
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: J—rn E. Nygjerd
// Date..........: 960407
// Description...: 
// Comments......: 
// Revisions.....:
//-------------------------------------------------------------------

CProjectTreeCtrl::~CProjectTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CProjectTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CProjectTreeCtrl)
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(TVN_BEGINRDRAG, OnBegindrag)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginlabeledit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//-------------------------------------------------------------------
// Method........: OnMouseMove
// Author........: J—rn E. Nygjerd
// Date..........: 960407
// Description...: 
// Comments......: TODO: Drag and drop funcionality
// Revisions.....:
//-------------------------------------------------------------------

void CProjectTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	HTREEITEM		hitem;
	UINT				flags;

	if (m_bDragging)
	{
		m_pimagelist->DragMove(point);
		if ((hitem = HitTest(point, &flags)) != NULL)
		{
			m_pimagelist->DragLeave(this);
			SelectDropTarget(hitem);
			m_hitemDrop = hitem;
			m_pimagelist->DragEnter(this, point);
		}
	}

	
	CTreeCtrl::OnMouseMove(nFlags, point);
}

//-------------------------------------------------------------------
// Method........: OnButtonUp
// Author........: Aleksander šhrn/J—rn E. Nygjerd
// Date..........: 960407
// Description...: Handles drag and drop functionality.
// Comments......: 
// Revisions.....:
//-------------------------------------------------------------------

void CProjectTreeCtrl::OnButtonUp() 
{

  Handle<Structure>  structure;
  Handle<Algorithm>  algorithm;
  Handle<Identifier> drag;
  Handle<Identifier> drop;

	if (!m_bDragging)
		return;

	m_pimagelist->DragLeave(this);
	m_pimagelist->EndDrag();

	if ((m_hitemDrag == NULL) || (m_hitemDrop == NULL)) {
		Message::Error("One of the drag-and-drop objects are invalid.");
	}
	else if (m_hitemDrag == m_hitemDrop) {
		Message::Error("Drag and drop objects cannot be equal.");
	}
	else if (IsChildNodeOf(m_hitemDrop, m_hitemDrag)) {
		Message::Error("Drop object cannot be a child of drag object.");
	}
	else if (GetParentItem(m_hitemDrag) == m_hitemDrop) {
		Message::Error("Drop object cannot be a parent of drag object.");
	}
	else {

		// Get drag and drop objects.
		drag = reinterpret_cast(Identifier *, GetItemData(m_hitemDrag));
		drop = reinterpret_cast(Identifier *, GetItemData(m_hitemDrop));

		// Verify and assign objects.
		if (drag.IsNULL() || drop.IsNULL()) {
			Message::Error("One or both objects are invalid.");
		}
		else if (drag->IsA(ALGORITHM) && drop->IsA(ALGORITHM)) {
			Message::Error("Both objects cannot be algorithms.");
		}
		else if (drag->IsA(STRUCTURE) && drop->IsA(ALGORITHM)) {
			structure = dynamic_cast(Structure *, drag.GetPointer());
			algorithm = dynamic_cast(Algorithm *, drop.GetPointer());
		}
		else if (drag->IsA(ALGORITHM) && drop->IsA(STRUCTURE)) {
			structure = dynamic_cast(Structure *, drop.GetPointer());
			algorithm = dynamic_cast(Algorithm *, drag.GetPointer());
		}
		else if (drag->IsA(STRUCTURE) && drop->IsA(STRUCTURE)) {
			// This might imply something special, e.g. some implicit algorithm.
			Message::Warning("Both objects cannot be structures.");
		}

		// So that control is transferred to the dialog (popped in ApplyAlgorithm). 
		ReleaseCapture();
			
    // Get the view this control is a part of.
		CViewProject *view = dynamic_cast(CViewProject *, GetParent());

		if (view == NULL) {
			Message::Error("Failed to access parent view containing document.");
			return;
		}

		// Get the document attached to the view.
		CRosettaDoc *document = dynamic_cast(CRosettaDoc *, view->GetDocument());

		if (document == NULL) {
			Message::Error("Failed to access document.");
			return;
		}

		// Apply algorithm to structure.
		if (!algorithm.IsNULL() && !structure.IsNULL()) {
			if (document->ApplyAlgorithm(const_cast(Algorithm *, algorithm.GetPointer()), const_cast(Structure *, structure.GetPointer())) == NULL) {
				Message().Notify("Operation not performed.");
			}
			else {
				view->UpdateTree();
			}
		}
	}

	ReleaseCapture();

	m_bDragging = FALSE;

	SelectDropTarget(NULL);
	
}

//-------------------------------------------------------------------
// Method........: OnRButtonUp
// Author........: J—rn E. Nygjerd
// Date..........: 960407
// Description...: 
// Comments......: 
// Revisions.....:
//-------------------------------------------------------------------

void CProjectTreeCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	OnButtonUp();
  CTreeCtrl::OnRButtonUp(nFlags, point);
}

//-------------------------------------------------------------------
// Method........: OnLButtonUp
// Author........: J—rn E. Nygjerd
// Date..........: 960407
// Description...: 
// Comments......: 
// Revisions.....:
//-------------------------------------------------------------------

void CProjectTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	OnButtonUp();
	CTreeCtrl::OnLButtonUp(nFlags, point);
}

//-------------------------------------------------------------------
// Method........: OnBeginDrag
// Author........: J—rn E. Nygjerd
// Date..........: 960407
// Description...: 
// Comments......: 
// Revisions.....:
//-------------------------------------------------------------------

void CProjectTreeCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
  CPoint		ptAction;
	UINT		  nFlags;

	GetCursorPos(&ptAction);
	ScreenToClient(&ptAction);

	//ASSERT(!m_bDragging);
	if (m_bDragging) {
		Message::Debug("Hmm, aren't I already dragging an object...?");
		return;
	}

	m_bDragging = TRUE;
	m_hitemDrag = HitTest(ptAction, &nFlags);
	m_hitemDrop = NULL;
	m_pimagelist = CreateDragImage(m_hitemDrag);  // get the image list for dragging
	m_pimagelist->DragShowNolock(TRUE);
	m_pimagelist->SetDragCursorImage(0, CPoint(0, 0));
	m_pimagelist->BeginDrag(0, CPoint(0,0));
	m_pimagelist->DragMove(ptAction);
	m_pimagelist->DragEnter(this, ptAction);
	SetCapture();
	
	*pResult = 0;
}

//-------------------------------------------------------------------
// Method........: SetNewStyle
// Author........: J—rn E. Nygjerd
// Date..........: 960407
// Description...: 
// Comments......: 
// Revisions.....:
//-------------------------------------------------------------------

void CProjectTreeCtrl::SetNewStyle(long lStyleMask, BOOL bSetBits)
{
	long		lStyleOld;

	lStyleOld = GetWindowLong(m_hWnd, GWL_STYLE);
	lStyleOld &= ~lStyleMask;
	if (bSetBits)
		lStyleOld |= lStyleMask;

	SetWindowLong(m_hWnd, GWL_STYLE, lStyleOld);
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
}

//-------------------------------------------------------------------
// Method........: IsChildNodeOf
// Author........: J—rn E. Nygjerd
// Date..........: 960407
// Description...: 
// Comments......: 
// Revisions.....:
//-------------------------------------------------------------------

BOOL CProjectTreeCtrl::IsChildNodeOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent)
{
	do
	{
		if (hitemChild == hitemSuspectedParent)
			break;
	}
	while ((hitemChild = GetParentItem(hitemChild)) != NULL);

	return (hitemChild != NULL);
}

//-------------------------------------------------------------------
// Method........: TransferItem
// Author........: J—rn E. Nygjerd
// Date..........: 960407
// Description...: Used if one wants to move a node from one place
//                 the tree to another. Not in use by this application
//                 but included for (easy) later expansions.
// Comments......: 
// Revisions.....:
//-------------------------------------------------------------------

BOOL CProjectTreeCtrl::TransferItem(HTREEITEM hitemDrag, HTREEITEM hitemDrop)
{
	TV_INSERTSTRUCT		tvstruct;
	TCHAR				sztBuffer[50];
	HTREEITEM			hNewItem, hFirstChild;

		// avoid an infinite recursion situation
	tvstruct.item.hItem = hitemDrag;
	tvstruct.item.cchTextMax = 49;
	tvstruct.item.pszText = sztBuffer;
	tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	GetItem(&tvstruct.item);  // get information of the dragged element
	tvstruct.hParent = hitemDrop;
	tvstruct.hInsertAfter = TVI_SORT;
	tvstruct.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	hNewItem = InsertItem(&tvstruct);

	while ((hFirstChild = GetChildItem(hitemDrag)) != NULL)
	{
		TransferItem(hFirstChild, hNewItem);  // recursively transfer all the items
		DeleteItem(hFirstChild);		// delete the first child and all its children
	}
	return TRUE;
}

//-------------------------------------------------------------------
// Method........: OnBeginlabeledit
// Author........: Daniel Remmem
// Date..........: 960715
// Description...: Allows only the label of annotated structures to be
//                 edited.
// Comments......: 
// Revisions.....: 
//-------------------------------------------------------------------
void CProjectTreeCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO        *pTVDispInfo;
	Handle<Identifier>  identifier;
	
	// Default: Not allow editing.
	*pResult = TRUE;

	// Get the item.
	pTVDispInfo = reinterpret_cast(TV_DISPINFO *, pNMHDR);
	
	if (pTVDispInfo == NULL)
		return;

	identifier = reinterpret_cast(Identifier *, GetItemData(pTVDispInfo->item.hItem));
			
	// Can the structure be renamed?
	if (!identifier.IsNULL() && identifier->IsA(ANNOTATEDSTRUCTURE)) {
		*pResult = 0;
	}
	
	// This is not an annotated structure.
	if (*pResult == TRUE)
		Message::Error("This item cannot be renamed.");
		
}

//-------------------------------------------------------------------
// Method........: OnEndlabeledit
// Author........: J—rn E. Nygjerd/Aleksander šhrn
// Date..........: 960407
// Description...: Changes the name of a tree node and then transfers
//                 the name to the object in the kernel.
// Comments......: 
// Revisions.....: Aš 960704 Full rewrite.
//-------------------------------------------------------------------

void CProjectTreeCtrl::OnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult) 
{

	TV_DISPINFO        *pTVDispInfo;
	Handle<Identifier>  identifier;
	Handle<Structure>   structure;

	// Get the item.
	pTVDispInfo = reinterpret_cast(TV_DISPINFO *, pNMHDR);

	*pResult = TRUE;

	if ((pTVDispInfo == NULL) || (pTVDispInfo->item.pszText == NULL))
		return;

	identifier = reinterpret_cast(Identifier *, GetItemData(pTVDispInfo->item.hItem));

	if (identifier.IsNULL())
		return;

	if (!identifier->IsA(STRUCTURE))
		return;

	structure = dynamic_cast(Structure *, identifier.GetPointer());

	// Can the structure be renamed?
	if (!structure->SetName(pTVDispInfo->item.pszText)) {
		// Revert to old kernel name.
		pTVDispInfo->item.pszText = const_cast(char *, (structure->GetName()).GetBuffer());
		Message::Error("This structure cannot be renamed.");
	}

	pTVDispInfo->item.mask = TVIF_TEXT;
	SetItem(&pTVDispInfo->item);

	// Update window text of view of renamed object, if such a view exists.
	CViewProject *view = dynamic_cast(CViewProject *, GetParent());            // Get the view this control is a part of.
	if (view == NULL) {
		Message::Error("Cannot update views (failed to access parent view containing document).");
		return;
	}
  CRosettaDoc *document = dynamic_cast(CRosettaDoc *, view->GetDocument());  // Get the document attached to the view.
	if (document == NULL) {
		Message::Error("Cannot update views (failed to access document).");
		return;
	}
	document->SetModifiedFlag(TRUE);                                           // Flag that the document has been modified.
	LPARAM lHint = reinterpret_cast(LPARAM, identifier.GetPointer());
	CHint pHint(CHint::HINT_NAMECHANGE);
	Message::Debug("Updating views..");
	document->UpdateAllViews(view, lHint, &pHint);                             // Update all views that take the hints.

}

//-------------------------------------------------------------------
// Method........: OnDblclk
// Author........: Aleksander šhrn
// Date..........: 
// Description...: Responds to a double click in the tree.
// Comments......: 
// Revisions.....:
//-------------------------------------------------------------------

void CProjectTreeCtrl::OnDblclk(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{

	// Get the view this control is a part of.
	CViewProject *view = dynamic_cast(CViewProject *, GetParent());

	if (view == NULL) {
		Message::Error("Failed to access parent view.");
		return;
	}

	*pResult = TRUE;

	// Get the pointer to the currently selected object.
	long item_data = GetItemData(GetSelectedItem());

	Identifier *identifier = reinterpret_cast(Identifier *, item_data);

	if (identifier == NULL)
		return;

	// View the structure...
	if (identifier->IsA(STRUCTURE))
		view->DoView(dynamic_cast(Structure *, identifier));

	// .. or apply the algorithm.
	if (identifier->IsA(ALGORITHM))
		view->DoApply(dynamic_cast(Algorithm *, identifier));

}

 
