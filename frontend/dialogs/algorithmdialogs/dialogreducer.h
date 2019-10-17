// dialogreducer.h : header file
//

#ifndef __DIALOGREDUCER_H__
#define __DIALOGREDUCER_H__

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogReducer dialog

class CDialogReducer : public CDialogAlgorithm
{
// Construction
public:
	CDialogReducer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogReducer)
	enum { IDD = IDD_REDUCER };
	CButton	m_GroupUseIDG;
	CButton	m_GroupPrecomputeMatrix;
	CButton	m_CheckModuloDecision;
	CButton	m_CheckUseBRT;
	CEdit   m_EditBRTThreshold;
	CButton	m_ButtonObjectSelection;
	CButton	m_RadioDiscernibilityFull;
	CButton	m_RadioDiscernibilityObject;
	CButton	m_CheckUseIDG;
	CButton	m_CheckPrecomputeMatrix;
	CStatic m_StaticIDGText;
	CStatic m_StaticPrecomputeText;
	CEdit   m_EditFilename;
	CString	m_Filename;
	CButton	m_ButtonBrowse;
	CButton	m_ButtonAdvancedParameters;
	float m_BRTThreshold;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogReducer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	virtual bool HasPrecomputeSupport() const;
	virtual bool HasIDGSupport() const;
	virtual bool HasBRTSupport() const;
	virtual bool HasAdvancedParameters() const;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogReducer)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRadioDiscernibilityFull();
	afx_msg void OnRadioDiscernibilityObject();
	afx_msg void OnButtonObjectselection();
	afx_msg void OnCheckModuloDecision();
	afx_msg void OnCheckUseBRT();
	afx_msg void OnCheckUseIDG();
	afx_msg void OnButtonBrowse();
	afx_msg void OnCheckPrecomputematrix();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual afx_msg void OnButtonAdvancedparameters();

};

#endif
 
