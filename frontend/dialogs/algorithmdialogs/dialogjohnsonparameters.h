#if !defined(AFX_DIALOGJOHNSONPARAMETERS_H__215414DD_4307_11D2_9575_006008A066CE__INCLUDED_)
#define AFX_DIALOGJOHNSONPARAMETERS_H__215414DD_4307_11D2_9575_006008A066CE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dialogjohnsonparameters.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>
#include <kernel\algorithms\johnsonreducer.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogJohnsonParameters dialog

class CDialogJohnsonParameters : public CDialogAlgorithm
{
private:
	
	Handle<JohnsonReducer> m_Reducer;

// Construction
public:
	CDialogJohnsonParameters(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogJohnsonParameters)
	enum { IDD = IDD_JOHNSONREDUCER_PARAMS };
	CButton	m_CheckApproximateSolutions;
	CEdit	m_EditHitFraction;
	float	m_HitFraction;
	BOOL	m_ApproximateSolutions;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogJohnsonParameters)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogJohnsonParameters)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCheckApproximate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGJOHNSONPARAMETERS_H__215414DD_4307_11D2_9575_006008A066CE__INCLUDED_)
 
