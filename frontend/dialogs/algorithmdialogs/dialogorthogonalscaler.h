// dialogorthogonalscaler.h : header file
//

#ifndef __DIALOGORTHOGONALSCALER_H__
#define __DIALOGORTHOGONALSCALER_H__

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

#include <kernel\algorithms\orthogonalscaler.h>


/////////////////////////////////////////////////////////////////////////////
// CDialogOrthogonalScaler dialog

class CDialogOrthogonalScaler : public CDialogAlgorithm
{
// Construction
public:
	CDialogOrthogonalScaler(CWnd* pParent = NULL);   // standard constructor



private:
	
	Scaler::Mode             m_Mode;
	Handle<OrthogonalScaler> m_alg;

public:
	
// Dialog Data
	//{{AFX_DATA(CDialogOrthogonalScaler)
	enum { IDD = IDD_SCALER_ORTHOGONAL };
	CButton	m_ButtonAdvancedParams;
	CButton	m_MaskSymbolic;
	CEdit	m_EditFilenameSave;
	CButton	m_BrowseButtonSave;
	CButton m_RadioDiscard;
	CButton m_RadioSave;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogOrthogonalScaler)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	virtual bool HasAdvancedParameters() const;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogOrthogonalScaler)
	afx_msg void OnRadioDiscard();
	afx_msg void OnRadioSave();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonBrowseSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual afx_msg void OnButtonAdvancedparameters();

};

#endif
 
