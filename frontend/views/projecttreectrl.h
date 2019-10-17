// NewTree.h : header file
//

// Kernel class prototypes.
class Algorithm;
class Structure;

/////////////////////////////////////////////////////////////////////////////
// CProjectTreeCtrl window
#ifndef __NEWTREE_H__
#define __NEWTREE_H__


class CProjectTreeCtrl : public CTreeCtrl
{
// Construction
public:

	CProjectTreeCtrl();

// Attributes
public:
  
  CImageList	*m_pimagelist;
  BOOL m_bDragging;
  HTREEITEM	m_rghItem[12];
	HTREEITEM	m_hitemDrag;
	HTREEITEM	m_hitemDrop;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual ~CProjectTreeCtrl();

  void       SetNewStyle(long lStyleMask, BOOL bSetBits);
  BOOL       IsChildNodeOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent);
  BOOL       TransferItem(HTREEITEM hitemDrag, HTREEITEM hitemDrop);
  void       OnButtonUp(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(CProjectTreeCtrl)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
 
