// dialogequalfrequencyscaler.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogorthogonalscaler.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogEqualFrequencyScaler dialog

class CDialogEqualFrequencyScaler : public CDialogOrthogonalScaler
{
// Construction
public:
	CDialogEqualFrequencyScaler(CWnd* pParent = NULL);   // standard constructor

// Overrides
protected:
	virtual bool HasAdvancedParameters() const;
	virtual afx_msg void OnButtonAdvancedparameters();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogEqualFrequencyScaler)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

};
 
