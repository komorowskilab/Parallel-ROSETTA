#ifndef __VIEWBATCHCLASSIFICATION_H__
#define __VIEWBATCHCLASSIFICATION_H__

#include <frontend\views\viewgrid.h>

#include <kernel\structures\batchclassification.h>
#include <kernel\structures\decisiontable.h>

/////////////////////////////////////////////////////////////////////////////
// CViewBatchClassification view

class CViewBatchClassification : public CViewGrid
{

// Attributes
private: 

	Handle<BatchClassification> m_BatchClassification;
	Handle<DecisionTable>       m_DecisionTable;
			
// Operations
private:

	void           SetGridStyles();

protected:

	CViewBatchClassification();
	
	DECLARE_DYNCREATE(CViewBatchClassification)

	virtual void   InitializeGridAppearance();
	virtual void   LoadDataIntoGrid();

public:

	BOOL           m_Masked; // Operate on masked table?

	virtual void   SetStructure(Structure *structure);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewBatchClassification)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CViewBatchClassification();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewBatchClassification)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
/////////////////////////////////////////////////////////////////////////////
 
