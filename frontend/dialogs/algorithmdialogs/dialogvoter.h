// dialogvoter.h : header file
//

#ifndef __DIALOGVOTER_H__
#define __DIALOGVOTER_H__

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

#include <kernel\basic\identifier.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogVoter dialog

class CDialogVoter : public CDialogAlgorithm
{
// Construction
public:
	CDialogVoter(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogVoter)
	enum { IDD = IDD_VOTER };
	float	m_Fraction;
	CComboBox	m_ComboRules;
	CEdit   m_EditFilename;
	CButton	m_CheckUseIDG;
	CButton	m_ButtonBrowse;
	CString	m_Filename;
	CButton	m_ButtonAdvancedParams;
	//}}AFX_DATA

	Identifier::Handles m_Available;            // Available rule sets.
	BOOL                m_EnableRules;          // Enable rule set selection?
	BOOL                m_EnableAdvanced;       // Enable advanced parameter selection, if supported?

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogObjectTrackingVoter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	virtual bool HasAdvancedParameters() const;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogObjectTrackingVoter)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboRules();
	afx_msg void OnKillfocusEditFraction();
	afx_msg void OnCheckUseIDG();
	afx_msg void OnButtonBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual afx_msg void OnButtonAdvancedparameters();

};

#endif 
