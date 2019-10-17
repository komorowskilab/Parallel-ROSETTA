// dialognaivebayesclassifier.h : header file
//

#include <frontend\dialogs\dialoggenericlist.h>

#include <kernel\basic\identifier.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogNaiveBayesClassifier dialog

class CDialogNaiveBayesClassifier : public CDialogGenericList
{
// Construction
public:
	CDialogNaiveBayesClassifier(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogNaiveBayesClassifier)
	//}}AFX_DATA

	Identifier::Handles m_Available;            // Available tables.

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogNaiveBayesClassifier)
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogNaiveBayesClassifier)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
