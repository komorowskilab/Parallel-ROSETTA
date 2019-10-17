// dialogbatchclassifier.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

#include <kernel\algorithms\algorithm.h>
#include <kernel\algorithms\classifier.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogBatchClassifier dialog

class CDialogBatchClassifier : public CDialogAlgorithm
{
// Construction
public:
	CDialogBatchClassifier(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogBatchClassifier)
	enum { IDD = IDD_BATCHCLASSIFIER };
	CEdit	m_EditFBThresh;
	CButton	m_CheckFallback;
	CEdit	m_EditPriThreshold;
	CComboBox	m_ComboPriClass;
	CComboBox	m_ComboFallback;
	CButton	m_RadioBest;
	CButton	m_RadioPrioritize;
	CButton	m_RadioRefrain;
	CButton	m_CheckVerbose;
	CEdit	m_EditFilename;
	CButton	m_CheckLog;
	CButton	m_ButtonBrowse;
	CComboBox	m_ComboClassifier;
	CButton	m_ButtonParameters;
	float	m_PriThreshold;
	float	m_FBThresh;
	//}}AFX_DATA

	Algorithm::Handles  m_Available;  // Available classifiers.
	int                 m_Selected;   // Index of selected classifier.

	BOOL                m_Masked;            // Operate on a masked table.
	int                 m_DecisionAttribute; // Index of decision attribute.
	Vector(int)         m_DecisionValues;    // Possible decision values.

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogBatchClassifier)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogBatchClassifier)
	afx_msg void OnButtonParameters();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboClassifier();
	virtual void OnOK();
	afx_msg void OnButtonBrowse();
	afx_msg void OnCheckLog();
	afx_msg void OnCheckFallback();
	afx_msg void OnRadioBest();
	afx_msg void OnRadioPrioritize();
	afx_msg void OnRadioRefrain();
	afx_msg void OnButtonCalres();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
