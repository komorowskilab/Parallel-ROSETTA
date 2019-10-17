// dialogattributestatus.h : header file
//

#include <kernel\basic\vector.h>

#include <kernel\structures\decisiontable.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogAttributeStatus dialog

class CDialogAttributeStatus : public CDialog
{
// Construction
public:
	CDialogAttributeStatus(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogAttributeStatus)
	enum { IDD = IDD_ATTRIBUTE_STATUS };
	CButton	m_RadioCondition;
	CButton	m_RadioDecision;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	Handle<DecisionTable> m_Table;       // The table operated on.
	Vector(int)           m_Attributes;  // Unmasked indices to operate on.

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogAttributeStatus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogAttributeStatus)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
