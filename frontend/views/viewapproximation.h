#ifndef __VIEWAPPROXIMATION_H__
#define __VIEWAPPROXIMATION_H__

#include <frontend\views\viewgrid.h>

#include <kernel\structures\approximation.h>

/////////////////////////////////////////////////////////////////////////////
// CViewApproximation view

class CViewApproximation : public CViewGrid
{
// Attributes
protected:

  enum
  {
    CVIEWAPPROXIMATION_FIRSTVALUE = CViewGrid::CVIEWGRID_LASTVALUE + 1,
    CVIEWAPPROXIMATION_GRID_USEOFFSET,
    CVIEWAPPROXIMATION_LASTVALUE
  };

// Attributes
protected:

  Handle<Approximation> m_Approximation;
	BOOL                  m_UseOffset;

// Operations
private:

	void           OnUseOffset();

// Operations
protected:

	CViewApproximation();

	DECLARE_DYNCREATE(CViewApproximation)

	virtual CMenu *CreateGridMenu(CMenu *menu);
	virtual void   InitializeGridAppearance();
	virtual void   LoadDataIntoGrid();

// Attributes
public:

// Operations
public:

	virtual void   SetStructure(Structure *structure);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewApproximation)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CViewApproximation();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewApproximation)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
/////////////////////////////////////////////////////////////////////////////
 
