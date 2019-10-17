#ifndef __VIEWREDUCTS_H__
#define __VIEWREDUCTS_H__

#include <frontend\views\viewgrid.h>

#include <kernel\structures\reducts.h>

/////////////////////////////////////////////////////////////////////////////
// CViewReducts view

class CViewReducts : public CViewGrid
{
// Attributes
protected:

  enum
  {
    CVIEWREDUCTS_FIRSTVALUE = CViewGrid::CVIEWGRID_LASTVALUE + 1,
    CVIEWREDUCTS_ROW_REMOVE,
    CVIEWREDUCTS_COL_SORT_LENGTH,
    CVIEWREDUCTS_COL_SORT_SUPPORT,
    CVIEWREDUCTS_LASTVALUE
  };

// Attributes
protected:

  Handle<Reducts> m_Reducts;

// Operations
private:

	void OnRowRemove();
	void OnColSort(int property, BOOL ascending);

// Operations
protected:

	CViewReducts();

	DECLARE_DYNCREATE(CViewReducts)

	virtual CMenu *CreateRowMenu(CMenu *menu, ROWCOL nRow);
	virtual CMenu *CreateColumnMenu(CMenu *menu, ROWCOL nCol);
	virtual void   InitializeGridAppearance();
	virtual void   LoadDataIntoGrid();

// Attributes
public:

// Operations
public:

	virtual void   SetStructure(Structure *structure);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewReducts)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CViewReducts();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewReducts)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
/////////////////////////////////////////////////////////////////////////////
 
