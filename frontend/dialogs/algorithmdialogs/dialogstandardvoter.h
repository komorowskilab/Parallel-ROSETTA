// dialogstandardvoter.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogvoter.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogStandardVoter dialog

class CDialogStandardVoter : public CDialogVoter
{
// Construction
public:
	CDialogStandardVoter(CWnd* pParent = NULL);   // standard constructor

	BOOL m_EnableGeneralization;   // Enable "use most specific rule only" option?

// Overrides
protected:
	virtual bool HasAdvancedParameters() const;
	virtual afx_msg void OnButtonAdvancedparameters();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogStandardVoter)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

};
 
