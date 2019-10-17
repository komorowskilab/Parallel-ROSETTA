#ifndef __VIEWGRID_H__
#define __VIEWGRID_H__

#include <kernel/structures/structure.h>

class CRosettaDoc;
class Project;

/////////////////////////////////////////////////////////////////////////////
// CViewGrid view

class CViewGrid : public CGXGridView
{

// Attributes
protected: 

	enum {
    CVIEWGRID_FIRSTVALUE,	
		CVIEWGRID_GRID_USEDICTIONARY, 
    CVIEWGRID_GRID_RESIZETOFIT,
    CVIEWGRID_GRID_DISPLAYSETTINGS,
		CVIEWGRID_LASTVALUE
	};

protected:

	Handle<Structure>      m_Structure;     // The structure displayed by this view.
	BOOL                   m_Masked;        // Access the table as if it were masked?
	BOOL                   m_UseDictionary; // Display stuff mapped through the dictionary?
	int                    m_RowOffset;     // Currently not used, for future generalization.
	int                    m_ColOffset;     // Currently not used, for future generalization.

// Operations
protected:

	CViewGrid();

	DECLARE_DYNCREATE(CViewGrid)
	
	virtual BOOL   OnRButtonClickedRowCol(ROWCOL nRow, ROWCOL nCol, UINT nFlags, CPoint pt);
  virtual BOOL   OnPasteFromClipboard(const CGXRange& range);

	virtual CMenu *CreateGridMenu(CMenu *menu);
	virtual CMenu *CreateRowMenu(CMenu *menu, ROWCOL nRow);
	virtual CMenu *CreateColumnMenu(CMenu *menu, ROWCOL nCol);

	virtual void   ResizeToFit();
	virtual void   InitializeGridAppearance();
	virtual void   LoadDataIntoGrid();

	void           OnUseDictionary();
	void           OnDisplaySettings();

// Operations
public:

	CRosettaDoc             *GetDocument();

	virtual void             SetStructure(Structure *structure);
  const Structure         *GetStructure() const {return m_Structure.GetPointer();}

  Project                 *GetTopProject();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewGrid)
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

	virtual ~CViewGrid();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewGrid)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
/////////////////////////////////////////////////////////////////////////////
 
