#ifndef __VIEWPATTERNS_H__
#define __VIEWPATTERNS_H__

#include <frontend\views\viewrules.h>

/////////////////////////////////////////////////////////////////////////////
// CViewPatterns view

class CViewPatterns : public CViewRules
{
// Operations
protected:

	CViewPatterns();

	DECLARE_DYNCREATE(CViewPatterns)

	virtual CMenu *CreateColumnMenu(CMenu *menu, ROWCOL nCol);
	virtual int    GetNoColumns() const;
	virtual void   SetColumnHeaders();
	virtual void   LoadRuleIntoGrid(const Rule &rule, int i, const DecisionTable *table, bool masked, const Vector(int) &decisions, const Vector(int) &cardinalities, int no_objects);

// Attributes
public:

// Operations
public:

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewRules)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CViewPatterns();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewPatterns)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
/////////////////////////////////////////////////////////////////////////////
 
