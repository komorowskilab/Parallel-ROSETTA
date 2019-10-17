#ifndef __VIEWDECISIONTABLE_H__
#define __VIEWDECISIONTABLE_H__

#include <frontend\views\viewgrid.h>

#include <kernel\structures\decisiontable.h>

class CViewDecisionTable : public CViewGrid
{
// Attributes
protected:

  enum {
    CVIEWDECISIONTABLE_FIRSTVALUE = CViewGrid::CVIEWGRID_LASTVALUE + 1,
    CVIEWDECISIONTABLE_COL_MASK,
    CVIEWDECISIONTABLE_COL_STATUS,
    CVIEWDECISIONTABLE_COL_INSERT,
    CVIEWDECISIONTABLE_COL_REMOVE,
		CVIEWDECISIONTABLE_COL_SWAP,
    CVIEWDECISIONTABLE_COL_INFORMATION,
    CVIEWDECISIONTABLE_COL_STATISTICS,
		CVIEWDECISIONTABLE_COL_SORT,
		CVIEWDECISIONTABLE_COL_JOIN,
    CVIEWDECISIONTABLE_ROW_INSERT,
    CVIEWDECISIONTABLE_ROW_REMOVE,
		CVIEWDECISIONTABLE_GRID_APPENDROW,
		CVIEWDECISIONTABLE_GRID_APPENDCOL,
    CVIEWDECISIONTABLE_LASTVALUE
  };

// Attributes
protected:

  Handle<DecisionTable> m_DecisionTable; 
	BOOL                  m_Masked;

// Operations
private:

  void           OnColDisable();
  void           OnColStatus();
  void           OnColInsert();
  void           OnColAppend();
  void           OnColRemove();
	void           OnColSwap();
  void           OnColInformation();
  void           OnColStatistics();
  void           OnColSort();
  void           OnColJoin();
  void           OnRowInsert();
  void           OnRowAppend();
  void           OnRowRemove();
  void           OnAttributeName(int attribute_no);

  void           InitializeColumnAppearance(int first, int last);
  void           InitializeColumnHeaders(int first, int last);

// Operations
protected:	
	
	CViewDecisionTable();

	DECLARE_DYNCREATE(CViewDecisionTable)

	virtual CMenu *CreateGridMenu(CMenu *menu);
	virtual CMenu *CreateRowMenu(CMenu *menu, ROWCOL nRow);
	virtual CMenu *CreateColumnMenu(CMenu *menu, ROWCOL nCol);

  virtual void   InitializeGridAppearance();
  virtual void   LoadDataIntoGrid();

// Operations
protected:

	virtual BOOL   OnRButtonClickedRowCol(ROWCOL nRow, ROWCOL nCol, UINT nFlags, CPoint pt);
  virtual BOOL   OnLButtonClickedRowCol(ROWCOL nRow, ROWCOL nCol, UINT nFlags, CPoint pt);
  virtual BOOL   OnStartEditing(ROWCOL nRow, ROWCOL nCol);
  virtual BOOL   OnEndEditing(ROWCOL nRow, ROWCOL nCol);
  virtual BOOL   OnValidateCell(ROWCOL nRow, ROWCOL nCol);
  virtual BOOL   OnLButtonDblClkRowCol(ROWCOL nRow, ROWCOL nCol, UINT nFlags, CPoint pt);
	virtual BOOL   OnDeleteCell(ROWCOL nRow, ROWCOL nCol);
	virtual BOOL   ProcessKeys(CWnd* pSender, UINT nMessage, UINT nChar, UINT nRepCnt, UINT flags);
	virtual BOOL   OnPasteFromClipboard(const CGXRange& range);

// Operations
public:

  virtual void   SetStructure(Structure *structure);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewDecisionTable)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CViewDecisionTable();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CViewDecisionTable)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
/////////////////////////////////////////////////////////////////////////////
 
