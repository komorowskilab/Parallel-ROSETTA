// dialogreductperformancepruner.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

#include <kernel\structures\decisiontable.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogReductPerformancePruner dialog

class CDialogReductPerformancePruner : public CDialogAlgorithm
{
// Protected member variables
protected:

	Algorithm::Handles    m_AvailableRG;   // Available rule generator.
	int                   m_SelectedRG;    // Index of selected rule generator.
	Algorithm::Handles    m_AvailableBC;   // Available batch classifier.
	int                   m_SelectedBC;    // Index of selected batch classifier.
	Identifier::Handles   m_AvailableDT;   // Available decision tables.
	int                   m_SelectedDTRG;  // Index of selected decision table for RG.
	int                   m_SelectedDTBC;  // Index of selected decision table for BC.

// Construction
public:
	CDialogReductPerformancePruner(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogReductPerformancePruner)
	enum { IDD = IDD_REDUCTPERFORMANCEFILTER };
	CEdit	m_EditFilename;
	CButton	m_ButtonBrowse;
	CEdit	m_EditIndex;
	CComboBox	m_ComboTableRG;
	CComboBox	m_ComboTableBC;
	CComboBox	m_ComboRG;
	CComboBox	m_ComboBC;
	CButton	m_ButtonRGParams;
	CButton	m_ButtonBCParams;
	CButton	m_RadioDiagonal;
	CButton	m_RadioRow;
	CButton	m_RadioCol;
	BOOL	m_Invert;
	int		m_Index;
	float	m_Threshold;
	BOOL	m_Log;
	CString	m_Filename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogReductPerformancePruner)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogReductPerformancePruner)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonBcparams();
	afx_msg void OnButtonRgparams();
	afx_msg void OnSelchangeComboBc();
	afx_msg void OnSelchangeComboRg();
	afx_msg void OnRadioCol();
	afx_msg void OnRadioDiagonal();
	afx_msg void OnRadioRow();
	afx_msg void OnSelchangeComboRgTable();
	afx_msg void OnSelchangeComboBcTable();
	afx_msg void OnCheckLogfile();
	afx_msg void OnButtonBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
