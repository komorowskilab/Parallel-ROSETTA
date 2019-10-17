// dialogjohnsonreducer.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogreducer.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogJohnsonReducer dialog

class CDialogJohnsonReducer : public CDialogReducer
{
// Construction
public:
	CDialogJohnsonReducer(CWnd* pParent = NULL);   // standard constructor

// Overrides
protected:
	virtual bool HasAdvancedParameters() const;
	virtual afx_msg void OnButtonAdvancedparameters();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogJohnsonReducer)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

};
 
