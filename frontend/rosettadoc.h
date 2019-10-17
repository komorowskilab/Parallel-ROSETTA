
#ifndef __ROSETTADOC_H__
#define __ROSETTADOC_H__

#include <kernel\basic\handle.h>

#include <kernel\structures\parentstructure.h>
#include <kernel\structures\decisiontable.h>
#include <kernel\structures\project.h>

#include <kernel\algorithms\algorithm.h>

#include <common\objectmanager.h>

// Class prototypes.
class CProjectTreeCtrl;

// RosettaDoc.h : interface of the CRosettaDoc class
//
/////////////////////////////////////////////////////////////////////////////

class CRosettaDoc : public CDocument
{
	
  //- Private data members...........................................
private:

 	Handle<Project> m_Project;            // The (top level) project tree.
  
	//- Private functions..............................................
private:

  void            InitializeNewProject();

public:

	Project        *GetProject();
	void            RemoveViewsForStructure(const Handle<Structure> &structure, bool including_children);

	//- Common code for calling Apply..................................
	Structure      *ApplyAlgorithm(Algorithm *algorithm, Structure *structure);
	BOOL            AppendOutputFiles(Algorithm *algorithm, Structure *structure);
	BOOL            NameResult(Algorithm *algorithm, Structure *structure, Structure *result);

	//- I/O helpers....................................................
	BOOL            LoadStructures(const String& pathname, Handle<ParentStructure> &parent);
	BOOL            SaveStructures(const String& pathname, const Handle<Structure> &structure) const;
	Structure      *ImportStructure(Id id, const String &filename);
	
protected: // create from serialization only
	CRosettaDoc();
	DECLARE_DYNCREATE(CRosettaDoc)

// Attributes
public:

// Operations
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRosettaDoc)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void DeleteContents();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	virtual void PreCloseFrame(CFrameWnd* pFrame);
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRosettaDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CRosettaDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
/////////////////////////////////////////////////////////////////////////////
 
