// dialogreyreducer.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogreducer.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogReyReducer dialog

class CDialogReyReducer : public CDialogReducer
{
// Construction
public:
	CDialogReyReducer(CWnd* pParent = NULL);   // standard constructor

// Overrides
protected:
	virtual bool HasAdvancedParameters() const;
	//virtual afx_msg void OnButtonAdvancedparameters();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogJohnsonReducer)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

};
 
