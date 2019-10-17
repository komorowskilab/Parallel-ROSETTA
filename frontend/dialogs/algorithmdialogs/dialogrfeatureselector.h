// DialogTTest.h : header file
//
#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

#include <kernel\genomics\algorithms\rfeatureselector.h>


/////////////////////////////////////////////////////////////////////////////
// CDialogRFeatureSelector dialog

class CDialogRFeatureSelector : public CDialogAlgorithm
{
// Construction
public:
	CDialogRFeatureSelector(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogRFeatureSelector)
	enum { IDD = IDD_RFEATURESELECTOR };
	CButton	m_Append;
	
	CButton m_RBrowse;
	CButton	m_TempBrowse;
	CButton m_RScriptBrowse;

	
	CEdit	m_EditMaxAttr;
	CEdit	m_EditRPath;
	CEdit	m_EditTempPath;
//	CEdit m_EditRScriptPath;
	CComboBox m_EditRScriptPath;
	
	int		m_Attributes;

	CString m_RPath;
	CString m_TempPath;
	// CString m_RScriptPath;
	
	
	//}}AFX_DATA

private:
	
	Handle<RFeatureSelector> m_alg;

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogRFeatureSelector)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogRFeatureSelector)
	virtual BOOL OnInitDialog();
	virtual void OnRBrowse();
	virtual void OnRScriptBrowse();
	virtual void OnTempBrowse();
	virtual void OnOK();
	virtual void ListAllFiles ();
	afx_msg void OnSelchange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
