#if !defined(AFX_DIALOGDYNAMICREDUCER_H__C140AA8C_7A8A_43B7_BE6A_5A47158372C1__INCLUDED_)
#define AFX_DIALOGDYNAMICREDUCER_H__C140AA8C_7A8A_43B7_BE6A_5A47158372C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dialogdynamicreducer.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogDynamicReducer dialog

class CDialogDynamicReducer : public CDialogAlgorithm
{
// Construction
public:
	CDialogDynamicReducer(CWnd* pParent = NULL);   // standard constructor

	Algorithm::Handles m_AvailableRed;   // Available reducers.
	int                m_SelectedRed;    // Index of selected reducer.

// Dialog Data
	//{{AFX_DATA(CDialogDynamicReducer)
	enum { IDD = IDD_DYNAMIC_REDUCER };
	CSpinButtonCtrl	m_SpinSeed;
	CSpinButtonCtrl	m_SpinSamples;
	CSpinButtonCtrl	m_SpinLevels;
	CSliderCtrl	m_CSliderThreshold;
	CSliderCtrl	m_CSliderUpperPercentage;
	CSliderCtrl	m_CSliderLowerPercentage;
	CButton	m_Parameters;
	CComboBox	m_CReducer;
	int		m_Levels;
	int		m_LowerPercentage;
	int		m_Samples;
	int		m_UpperPercentage;
	float	m_Threshold;
	BOOL	m_CheckGeneralized;
	long	m_Seed;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDynamicReducer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDynamicReducer)
	virtual void OnOK();
	afx_msg void OnParameters();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboReducer();
	afx_msg void OnReleasedcaptureSliderLowestLevel(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSliderLowestLevel(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSliderHighestLevel(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSliderHighestLevel(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSliderLowerPercentage(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSliderUpperPercentage(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateEditLowerPercentage();
	afx_msg void OnUpdateEditUpperPercentage();
	afx_msg void OnCustomdrawSliderThreshold(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateEditThreshold();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDYNAMICREDUCER_H__C140AA8C_7A8A_43B7_BE6A_5A47158372C1__INCLUDED_)
 
