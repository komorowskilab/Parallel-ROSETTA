// dialogattributemask.h : header file
//

#include <kernel\basic\vector.h>

#include <kernel\structures\decisiontable.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogAttributeMask dialog

class CDialogAttributeMask : public CDialog
{
// Construction
public:
	CDialogAttributeMask(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogAttributeMask)
	enum { IDD = IDD_ATTRIBUTE_MASK };
	CButton	m_RadioEnabled;
	CButton	m_RadioDisabled;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	Handle<DecisionTable> m_Table;       // The decision table to mask.
	Vector(int)           m_Attributes;  // Indices (unmasked) the masking is set for.

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogAttributeMask)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogAttributeMask)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
