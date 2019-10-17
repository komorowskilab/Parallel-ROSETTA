// dialogbrorthogonalscaler.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogorthogonalscaler.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogBROrthogonalScaler dialog

class CDialogBROrthogonalScaler : public CDialogOrthogonalScaler
{
// Construction
public:
	CDialogBROrthogonalScaler(CWnd* pParent = NULL);   // standard constructor

// Overrides
protected:
	virtual bool HasAdvancedParameters() const;
	virtual afx_msg void OnButtonAdvancedparameters();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogBROrthogonalScaler)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

};
 
