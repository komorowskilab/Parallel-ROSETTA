// viewproject.h : interface of the CViewProject class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __VIEWPROJECT__
#define __VIEWPROJECT__

#include <frontend\views\projecttreectrl.h>

class Algorithm;
class CRosettaDoc;
class DecisionTable;
class Persistent;
class Reducts;
class Rules;

class CViewProject : public CView
{
private:
  enum
  {
    FIRSTVALUE,		
		RULES_CLASSIFY,

		TESTSTUFFDEBUG,
		
		VIEW,
		REMOVE,
		DUPLICATE,

		SAVE,
		SAVE_AS,
		LOAD_CHILDREN,
		ODBC_IMPORT,

		DICTIONARY_EXPORT,
		DICTIONARY_IMPORT,

		TABLE_IMPORT,
		REDUCTS_IMPORT,

		STATISTICS,
		INFORMATION,

		APPLY,

    NO_ACTION,
		LASTVALUE   // This must be the last enum.

  };

private:
	
	// Placeholders for temporary stuff scheduled for deletion.
	CObList		 m_temporaryMenu; // List of popup menus to be deleted when menu is deselected.
	CImageList m_imagelist;     // List of bitmaps ("tree-object icons").

	// Menuflags to be used in the menus
	UINT      m_menuflag; 
	UINT      m_menugrayedflag;
	UINT      m_popupflag;
	UINT      m_popupgrayedflag;

	// The item that was last modified.
	HTREEITEM m_hLastModified;

	// Access/display tables as masked/unmasked.
	BOOL      m_Masked;

	// The tree control
	CProjectTreeCtrl	m_Tree ;

	// Gets the toplevel project.
	Project *GetTopProject();
	
	// This function builds the structures in the tree recursively
	void   BuildStructuresInTree(Structure *structure, HTREEITEM parent);

	// This function ensures that the item with the given data is visible, if present.
	void   EnsureVisible(HTREEITEM parent, LPARAM data);

	//- Deletes all temporary menu objects.
	void	 DeleteTempMenus();
	void	 DeleteImageList();

	// Functions that OnCmdMsg calls
	void   OnTestStuffDebug(Structure *structure);
	void   OnSave(Structure *structure);
	void   OnSaveAs(Structure *structure);
	void   OnLoadChildren(Structure *structure);
	void   OnView(Structure *structure);
	void   OnRemove(Structure *structure);          
  void   OnDuplicate(Structure *structure);       
	void   OnStatistics(Structure *structure);      
  void   OnInformation(Structure *structure);  
  void   OnRulesClassify(Structure *structure);    
	void   OnODBCImport(Structure *structure);
	void   OnDictionaryExport(Structure *structure);
	void   OnDictionaryImport(Structure *structure);
	void   OnDecisionTableImport(Structure *structure);
	void   OnReductsImport(Structure *structure);
	void   OnApply(Algorithm *algorithm);

protected:

  int    MakeMenu(CMenu &menu, const Identifier *identifier);

	int    MakeCommonTop(CMenu *menu, const Identifier *identifier);
	int    MakeCommonBottom(CMenu *menu, const Identifier *identifier);

	int    MakeDecisionTableMenu(CMenu &menu, const DecisionTable &table);
  int    MakeReductsMenu(CMenu &menu, const Reducts &reducts);
  int    MakeRulesMenu(CMenu &menu, const Rules &rules);
  int    MakeAlgorithmMenu(CMenu &menu, const Algorithm &algorithm);

	int    MakeDynamicMenu(CMenu &menu, const Vector(String) &labels, const Vector(Id) &ids, const Structure &structure, bool verify, Id exception);
	int    MakeDynamicMenu(CMenu &menu, const Vector(String) &labels, const Vector(Id) &ids, const Structure &structure, bool verify, const Vector(Id) *exceptions);
	int    MakeDynamicMenu(CMenu &menu, const String &label, Id id, const Structure &structure, bool verify, Id exception);
	int    MakeDynamicMenu(CMenu &menu, const String &label, Id id, const Structure &structure, bool verify, const Vector(Id) *exceptions);
	int    MakeDynamicMenu(CMenu &menu, const String &label, const Vector(Id) &exceptions, const Structure &structure, bool verify);
	
	//RA 0406: Added mode menues
	int    MakeInverseModeMenu(CMenu &menu, const Structure &structure, bool verify);
	int    MakeGenomicModeMenues(CMenu &menu, const Vector(char*) genlabels, const Structure &structure, bool verify, bool sep); 
protected: // create from serialization only

	afx_msg void OnContextMenu(CWnd*, CPoint point);
  void           InitialUpdateTree();

public:
  void           UpdateTree();

public:

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	void DoView(Structure *structure)  {OnView(structure);}
	void DoApply(Algorithm *algorithm) {OnApply(algorithm);}

// Attributes
public:

	CRosettaDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewProject)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CViewProject();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:

	DECLARE_DYNCREATE(CViewProject)
	
	CViewProject () ;

	//{{AFX_MSG(CViewProject)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in viewproject.cpp
inline CRosettaDoc* CViewProject::GetDocument()
   { return (CRosettaDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

#endif
 
