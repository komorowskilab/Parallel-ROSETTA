// dialogsavgeneticreducer.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogreducer.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogSAVGeneticReducer dialog

class CDialogSAVGeneticReducer : public CDialogReducer
{
// Construction
public:
	CDialogSAVGeneticReducer(CWnd* pParent = NULL);   // standard constructor

// Overrides
protected:
	virtual bool HasAdvancedParameters() const;
	virtual afx_msg void OnButtonAdvancedparameters();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogSAVGeneticReducer)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

};
 
