#ifndef __DIALOGALGORITHM_H__
#define __DIALOGALGORITHM_H__

#include <frontend\rosetta.h>

#include <kernel\structures\project.h>
#include <kernel\algorithms\algorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogAlgorithm dialog

class CDialogAlgorithm : public CDialog
{

public:

  Handle<Algorithm> m_Algorithm;  // The algorithm to apply.
	Handle<Structure> m_Structure;  // The structure to apply the algorithm on.
	Handle<Project>   m_Project;    // The project to which the structure belongs.

  void SetAlgorithm(Algorithm *algorithm, Structure *structure, Project *project);


// Construction
public:
	CDialogAlgorithm(UINT DialogID, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogAlgorithm)
	enum { IDD = IDD_ABOUTBOX };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogAlgorithm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogAlgorithm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
 
