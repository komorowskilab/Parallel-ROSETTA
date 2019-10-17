#ifndef __VIEWRULES_H__
#define __VIEWRULES_H__

#include <frontend\views\viewgrid.h>

#include <kernel\structures\rules.h>

/////////////////////////////////////////////////////////////////////////////
// CViewRules view

class CViewRules : public CViewGrid
{
// Attributes
protected:

  enum
  {
    CVIEWRULES_FIRSTVALUE = CViewGrid::CVIEWGRID_LASTVALUE + 1,
    CVIEWRULES_ROW_REMOVE,
    CVIEWRULES_COL_SORT_DECISION,
    CVIEWRULES_COL_SORT_LHSLENGTH,
    CVIEWRULES_COL_SORT_RHSLENGTH,
    CVIEWRULES_COL_SORT_LHSSUPPORT,
    CVIEWRULES_COL_SORT_RHSSUPPORT,
    CVIEWRULES_COL_SORT_LHSPROBABILITY,
    CVIEWRULES_COL_SORT_RHSACCURACY,
    CVIEWRULES_COL_SORT_LHSCOVERAGE,
    CVIEWRULES_COL_SORT_RHSCOVERAGE,
    CVIEWRULES_COL_SORT_LHSSTABILITY,
    CVIEWRULES_COL_SORT_RHSSTABILITY,
    CVIEWRULES_LASTVALUE
  };

// Attributes
protected:

  Handle<Rules> m_Rules;

// Operations
private:

	void OnRowRemove();
	void OnColSort(int property, BOOL ascending);

// Operations
protected:

	CViewRules();

	DECLARE_DYNCREATE(CViewRules)

	virtual CMenu *CreateRowMenu(CMenu *menu, ROWCOL nRow);
	virtual CMenu *CreateColumnMenu(CMenu *menu, ROWCOL nCol);
	virtual int    GetNoColumns() const;
	virtual void   SetColumnHeaders();
	virtual void   InitializeGridAppearance();
	virtual void   LoadDataIntoGrid();
	virtual void   LoadRuleIntoGrid(const Rule &rule, int i, const DecisionTable *table, bool masked, const Vector(int) &decisions, const Vector(int) &cardinalities, int no_objects);

// Attributes
public:

// Operations
public:

	virtual void   SetStructure(Structure *structure);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewRules)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CViewRules();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewRules)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
/////////////////////////////////////////////////////////////////////////////
 
