#ifndef __VIEWPARTITION_H__
#define __VIEWPARTITION_H__

#include <frontend\views\viewgrid.h>

#include <kernel\structures\equivalenceclasses.h>

/////////////////////////////////////////////////////////////////////////////
// CViewPartition view

class CViewPartition : public CViewGrid
{
// Attributes
protected:

  enum
  {
    CVIEWPARTITION_FIRSTVALUE = CViewGrid::CVIEWGRID_LASTVALUE + 1,
    CVIEWPARTITION_GRID_USEOFFSET,
    CVIEWPARTITION_COL_SORT_INDICES,
    CVIEWPARTITION_COL_SORT_CARDINALITY,
    CVIEWPARTITION_LASTVALUE
  };

// Attributes
protected:

  Handle<EquivalenceClasses> m_Partition;
	BOOL                       m_UseOffset;

// Operations
private:

	void          OnUseOffset();
	void          OnSortIndices(BOOL ascending);
	void          OnSortByCardinality(BOOL ascending);

// Operations
protected:

	CViewPartition();

	DECLARE_DYNCREATE(CViewPartition)

	virtual CMenu *CreateGridMenu(CMenu *menu);
	virtual CMenu *CreateColumnMenu(CMenu *menu, ROWCOL nCol);
	virtual void   InitializeGridAppearance();
	virtual void   LoadDataIntoGrid();

// Attributes
public:

// Operations
public:

	virtual void   SetStructure(Structure *structure);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewPartition)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CViewPartition();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewPartition)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
/////////////////////////////////////////////////////////////////////////////
 
