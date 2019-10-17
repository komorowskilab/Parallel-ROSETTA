#if !defined(AFX_DIALOGREYPARAMETERS_H__C0F28484_94E6_4FEB_8064_813EC8693ADF__INCLUDED_)
#define AFX_DIALOGREYPARAMETERS_H__C0F28484_94E6_4FEB_8064_813EC8693ADF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dialogreyparameters.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>
#include <kernel\algorithms\reyreducer.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogReyParameters dialog

class CDialogReyParameters : public CDialogAlgorithm
{
private:
	Handle<ReyReducer> m_Reducer;

// Construction
public:
	CDialogReyParameters(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogReyParameters)
	enum { IDD = IDD_REY_REDUCER_PARAMS };
	CEdit	m_CEditApproximate;
	CSliderCtrl	m_SliderApproximate;
	BOOL	m_CheckApproximate;
	float	m_EditApproximate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogReyParameters)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogReyParameters)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCustomdrawSliderApproximate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateEditApproximate();
	afx_msg void OnCheckApproximate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGREYPARAMETERS_H__C0F28484_94E6_4FEB_8064_813EC8693ADF__INCLUDED_)
 
