#if !defined(AFX_DIALOGEQUALFREQUENCYSCALERPARAMETERS_H__71AE7B6F_8B6B_11D2_9581_006008A066CE__INCLUDED_)
#define AFX_DIALOGEQUALFREQUENCYSCALERPARAMETERS_H__71AE7B6F_8B6B_11D2_9581_006008A066CE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogEqualFrequencyScalerParameters.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>
#include <kernel\algorithms\equalfrequencyscaler.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogEqualFrequencyScalerParameters dialog

class CDialogEqualFrequencyScalerParameters : public CDialogAlgorithm
{
private:
	
	Handle<EqualFrequencyScaler> m_Scaler;

// Construction
public:
	CDialogEqualFrequencyScalerParameters(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogEqualFrequencyScalerParameters)
	enum { IDD = IDD_EQFREQSCALER_PARAMS };
	int		m_Intervals;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogEqualFrequencyScalerParameters)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogEqualFrequencyScalerParameters)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGEQUALFREQUENCYSCALERPARAMETERS_H__71AE7B6F_8B6B_11D2_9581_006008A066CE__INCLUDED_)
 
