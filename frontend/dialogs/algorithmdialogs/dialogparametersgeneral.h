// dialogparametersgeneral.h : header file
//

#include <frontend/dialogs/algorithmdialogs/dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogParametersGeneral dialog

class CDialogParametersGeneral : public CDialogAlgorithm
{
// Construction
public:
	CDialogParametersGeneral(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogParametersGeneral)
	enum { IDD = IDD_PARAMS_GENERAL };
	CEdit	m_EditParams;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogParametersGeneral)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogParametersGeneral)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
