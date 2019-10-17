#if !defined(AFX_DIALOGFUNCTIONALCLASSIFIERADVANCEDPARAMS_H__31862882_4726_40E5_AB77_43ABC0265CE8__INCLUDED_)
#define AFX_DIALOGFUNCTIONALCLASSIFIERADVANCEDPARAMS_H__31862882_4726_40E5_AB77_43ABC0265CE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dialogfunctionalclassifieradvancedparams.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogFunctionalClassifierAdvancedParams dialog

class CDialogFunctionalClassifierAdvancedParams : public CDialogAlgorithm
{
// Construction
public:
	CDialogFunctionalClassifierAdvancedParams(CWnd* pParent = NULL);   // standard constructor

	Algorithm::Handles m_AvailableRed;   // Available reducers.
	int                m_SelectedRed;    // Index of selected reducer.

// Dialog Data
	//{{AFX_DATA(CDialogFunctionalClassifierAdvancedParams)
	enum { IDD = IDD_FUNCTIONAL_CLASSIFIER_ADVANCED_PARAMS };
	CComboBox	m_CReducer;
	CSliderCtrl	m_SliderPruneCov;
	CSliderCtrl	m_SliderPruneAcc;
	CButton	m_Parameters;
	CStatic	m_Acc;
	CStatic	m_Cov;
	CEdit	m_CEditPruneCov;
	CEdit	m_CEditPruneAcc;
	float	m_Weight;
	BOOL	m_CheckPruning;
	float	m_EditPruneAcc;
	float	m_EditPruneCov;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogFunctionalClassifierAdvancedParams)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogFunctionalClassifierAdvancedParams)
	afx_msg void OnCheckPruning();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCustomdrawSliderPruneAcc(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSliderPruneCov(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnParameters();
	afx_msg void OnSelchangeComboReducer();
	afx_msg void OnUpdateEditPruneAcc();
	afx_msg void OnUpdateEditPruneCov();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGFUNCTIONALCLASSIFIERADVANCEDPARAMS_H__31862882_4726_40E5_AB77_43ABC0265CE8__INCLUDED_)
 
