#if !defined(AFX_DIALOGROSETTAMODE_H__846B2172_4D60_45F0_87F7_42D86A3AF8BC__INCLUDED_)
#define AFX_DIALOGROSETTAMODE_H__846B2172_4D60_45F0_87F7_42D86A3AF8BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogRosettaMode.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CDialogRosettaMode dialog

class CDialogRosettaMode : public CDialog
{
// Construction
public:
	CDialogRosettaMode(CWnd* pParent = NULL);   // standard constructor

// Private Attributes
private:

	CBitmap m_RosBmp;
	CBitmap m_GenBmp;

public:

// Dialog Data
	//{{AFX_DATA(CDialogRosettaMode)
	enum { IDD = IDD_ROSETTA_MODE };
	CButton m_ButtonStdRosetta;
	CButton	m_ButtonGenRosetta;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogRosettaMode)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogRosettaMode)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonGenRosetta();
	afx_msg void OnButtonStdRosetta();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGROSETTAMODE_H__846B2172_4D60_45F0_87F7_42D86A3AF8BC__INCLUDED_)
 
