#if !defined(AFX_DIALOGFUNCTIONALCLASSIFIERTEMPLATEDEFS_H__C61306FE_5FFC_4573_8E67_844840B0B34D__INCLUDED_)
#define AFX_DIALOGFUNCTIONALCLASSIFIERTEMPLATEDEFS_H__C61306FE_5FFC_4573_8E67_844840B0B34D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dialogfunctionalclassifiertemplatedefs.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogFunctionalClassifierTemplateDefs dialog

class CDialogFunctionalClassifierTemplateDefs : public CDialogAlgorithm
{
// Construction
public:
	CDialogFunctionalClassifierTemplateDefs(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogFunctionalClassifierTemplateDefs)
	enum { IDD = IDD_FUNCTIONAL_CLASSIFIER_TEMPLATEDEFS };
	CStatic	m_StaticTo1;
	CStatic	m_StaticTo2;
	CStatic	m_StaticTo3;
	CStatic	m_StaticTo4;
	CStatic	m_StaticTo5;
	CStatic	m_StaticTo6;
	CStatic	m_StaticStep1;
	CStatic	m_StaticStep2;
	CStatic	m_StaticStep3;
	CStatic	m_StaticStep4;
	CStatic	m_StaticStep5;
	CStatic	m_StaticStep6;
	CStatic	m_StaticParamsFile;
	CEdit	m_EditFilenameParams;
	CEdit	m_EditConstantThrFrom;
	CEdit	m_EditConstantThrTo;
	CEdit	m_EditConstantThrStep;
	float	m_ConstantThrFrom;
	float	m_ConstantThrTo;
	float	m_ConstantThrStep;
	CSliderCtrl	m_SliderConstantThrFrom;
	CSliderCtrl	m_SliderConstantThrTo;
	CSpinButtonCtrl	m_SpinConstantThrStep;
	CEdit	m_EditToleranceThrFrom;
	CEdit	m_EditToleranceThrTo;
	CEdit	m_EditToleranceThrStep;
	float	m_ToleranceThrFrom;
	float	m_ToleranceThrTo;
	float	m_ToleranceThrStep;
	CSliderCtrl	m_SliderToleranceThrFrom;
	CSliderCtrl	m_SliderToleranceThrTo;
	CSpinButtonCtrl	m_SpinToleranceThrStep;
	CEdit	m_EditCreaseThrFrom;
	CEdit	m_EditCreaseThrTo;
	CEdit	m_EditCreaseThrStep;
	float	m_CreaseThrFrom;
	float	m_CreaseThrTo;
	float	m_CreaseThrStep;
	CSliderCtrl	m_SliderCreaseThrFrom;
	CSliderCtrl	m_SliderCreaseThrTo;
	CSpinButtonCtrl	m_SpinCreaseThrStep;
	CEdit	m_EditStartEndThrFrom;
	CEdit	m_EditStartEndThrTo;
	CEdit	m_EditStartEndThrStep;
	float	m_StartEndThrFrom;
	float	m_StartEndThrTo;
	float	m_StartEndThrStep;
	CSliderCtrl	m_SliderStartEndThrFrom;
	CSliderCtrl	m_SliderStartEndThrTo;
	CSpinButtonCtrl	m_SpinStartEndThrStep;
	CEdit	m_EditTemplateLengthFrom;
	CEdit	m_EditTemplateLengthTo;
	CEdit	m_EditTemplateLengthStep;
	int		m_TemplateLengthFrom;
	int		m_TemplateLengthTo;
	int		m_TemplateLengthStep;
	CSliderCtrl	m_SliderTemplateLengthFrom;
	CSliderCtrl	m_SliderTemplateLengthTo;
	CSpinButtonCtrl	m_SpinTemplateLengthStep;
	CEdit	m_EditConstantTemplateLengthFrom;
	CEdit	m_EditConstantTemplateLengthTo;
	CEdit	m_EditConstantTemplateLengthStep;
	int		m_ConstantTemplateLengthFrom;
	int		m_ConstantTemplateLengthTo;
	int		m_ConstantTemplateLengthStep;
	CSliderCtrl	m_SliderConstantTemplateLengthFrom;
	CSliderCtrl	m_SliderConstantTemplateLengthTo;
	CSpinButtonCtrl	m_SpinConstantTemplateLengthStep;
	CButton	m_ButtonBrowseParams;
	BOOL	m_CheckIterate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogFunctionalClassifierTemplateDefs)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogFunctionalClassifierTemplateDefs)
	afx_msg void OnButtonBrowseParams();
	afx_msg void OnCheckIterate();
	afx_msg void OnUpdateEditConstantTemplateLengthFrom(){OnUpdateEditField(m_ConstantTemplateLengthFrom, m_SliderConstantTemplateLengthFrom);};
	afx_msg void OnUpdateEditConstantTemplateLengthTo(){OnUpdateEditField(m_ConstantTemplateLengthTo, m_SliderConstantTemplateLengthTo);};
	afx_msg void OnUpdateEditConstantThrFrom(){OnUpdateEditField(m_ConstantThrFrom, m_SliderConstantThrFrom);};
	afx_msg void OnUpdateEditConstantThrTo(){OnUpdateEditField(m_ConstantThrTo, m_SliderConstantThrTo);};
	afx_msg void OnUpdateEditCreasethrFrom(){OnUpdateEditField(m_CreaseThrFrom, m_SliderCreaseThrFrom);};
	afx_msg void OnUpdateEditCreasethrTo(){OnUpdateEditField(m_CreaseThrTo, m_SliderCreaseThrTo);};
	afx_msg void OnUpdateEditStartendThrFrom(){OnUpdateEditField(m_StartEndThrFrom, m_SliderStartEndThrFrom);};
	afx_msg void OnUpdateEditStartendThrTo(){OnUpdateEditField(m_StartEndThrTo, m_SliderStartEndThrTo);};
	afx_msg void OnUpdateEditTemplateLengthFrom(){OnUpdateEditField(m_TemplateLengthFrom, m_SliderTemplateLengthFrom);};
	afx_msg void OnUpdateEditTemplateLengthTo(){OnUpdateEditField(m_TemplateLengthTo, m_SliderTemplateLengthTo);};
	afx_msg void OnUpdateEditToleranceThrFrom(){OnUpdateEditField(m_ToleranceThrFrom, m_SliderToleranceThrFrom);};
	afx_msg void OnUpdateEditToleranceThrTo(){OnUpdateEditField(m_ToleranceThrTo, m_SliderToleranceThrTo);};
	afx_msg void OnCustomdrawSliderConstantTemplateLengthFrom(NMHDR* pNMHDR, LRESULT* pResult) { OnCustomdrawSlider(m_SliderConstantTemplateLengthFrom, m_SliderConstantTemplateLengthTo, true, m_ConstantTemplateLengthFrom); *pResult = 0; };
	afx_msg void OnCustomdrawSliderConstantTemplateLengthTo(NMHDR* pNMHDR, LRESULT* pResult) { OnCustomdrawSlider(m_SliderConstantTemplateLengthTo, m_SliderConstantTemplateLengthFrom, false, m_ConstantTemplateLengthTo); *pResult = 0; };
	afx_msg void OnCustomdrawSliderConstantThrFrom(NMHDR* pNMHDR, LRESULT* pResult) { OnCustomdrawSlider(m_SliderConstantThrFrom, m_SliderConstantThrTo, true, m_ConstantThrFrom);	*pResult = 0; };
	afx_msg void OnCustomdrawSliderConstantThrTo(NMHDR* pNMHDR, LRESULT* pResult){ OnCustomdrawSlider(m_SliderConstantThrTo, m_SliderConstantThrFrom, false, m_ConstantThrTo); *pResult = 0; };
	afx_msg void OnCustomdrawSliderCreasethrFrom(NMHDR* pNMHDR, LRESULT* pResult) {	OnCustomdrawSlider(m_SliderCreaseThrFrom, m_SliderCreaseThrTo, true, m_CreaseThrFrom);	*pResult = 0; };
	afx_msg void OnCustomdrawSliderCreasethrTo(NMHDR* pNMHDR, LRESULT* pResult) { OnCustomdrawSlider(m_SliderCreaseThrTo, m_SliderCreaseThrFrom, false, m_CreaseThrTo); *pResult = 0; };
	afx_msg void OnCustomdrawSliderStartendThrFrom(NMHDR* pNMHDR, LRESULT* pResult) { OnCustomdrawSlider(m_SliderStartEndThrFrom, m_SliderStartEndThrTo, true, m_StartEndThrFrom);	*pResult = 0; };
	afx_msg void OnCustomdrawSliderStartendThrTo(NMHDR* pNMHDR, LRESULT* pResult) {	OnCustomdrawSlider(m_SliderStartEndThrTo, m_SliderStartEndThrFrom, false, m_StartEndThrTo); *pResult = 0; };
	afx_msg void OnCustomdrawSliderTemplateLengthFrom(NMHDR* pNMHDR, LRESULT* pResult) { OnCustomdrawSlider(m_SliderTemplateLengthFrom, m_SliderTemplateLengthTo, true, m_TemplateLengthFrom);	*pResult = 0; };
	afx_msg void OnCustomdrawSliderTemplateLengthTo(NMHDR* pNMHDR, LRESULT* pResult) { OnCustomdrawSlider(m_SliderTemplateLengthTo, m_SliderTemplateLengthFrom, false, m_TemplateLengthTo); *pResult = 0; };
	afx_msg void OnCustomdrawSliderToleranceThrFrom(NMHDR* pNMHDR, LRESULT* pResult) { OnCustomdrawSlider(m_SliderToleranceThrFrom, m_SliderToleranceThrTo, true, m_ToleranceThrFrom);	*pResult = 0; };
	afx_msg void OnCustomdrawSliderToleranceThrTo(NMHDR* pNMHDR, LRESULT* pResult) { OnCustomdrawSlider(m_SliderToleranceThrTo, m_SliderToleranceThrFrom, false, m_ToleranceThrTo); *pResult = 0; };
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinCreasethrStep(NMHDR* pNMHDR, LRESULT* pResult){OnDeltaposSpinStep(pNMHDR, m_CreaseThrStep); *pResult = 0;};
	afx_msg void OnDeltaposSpinStartendThrStep(NMHDR* pNMHDR, LRESULT* pResult){OnDeltaposSpinStep(pNMHDR, m_StartEndThrStep); *pResult = 0;};
	afx_msg void OnDeltaposSpinToleranceThrStep(NMHDR* pNMHDR, LRESULT* pResult){OnDeltaposSpinStep(pNMHDR, m_ToleranceThrStep); *pResult = 0;};
	afx_msg void OnDeltaposSpinConstantThrStep(NMHDR* pNMHDR, LRESULT* pResult){OnDeltaposSpinStep(pNMHDR, m_ConstantThrStep); *pResult = 0;};
	afx_msg void OnUpdateEditToleranceThrStep(){OnUpdateEditStep(m_SpinToleranceThrStep, m_ToleranceThrStep);};
	afx_msg void OnUpdateEditStartendThrStep(){OnUpdateEditStep(m_SpinStartEndThrStep, m_StartEndThrStep);};
	afx_msg void OnUpdateEditCreasethrStep(){OnUpdateEditStep(m_SpinCreaseThrStep, m_CreaseThrStep);};
	afx_msg void OnUpdateEditConstantThrStep(){OnUpdateEditStep(m_SpinConstantThrStep, m_ConstantThrStep);};
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//general methods
	void OnUpdateEditField(int &edit, CSliderCtrl &slider);
	void OnUpdateEditField(float &edit, CSliderCtrl &slider, float resolution =100.0);
	void OnCustomdrawSlider(CSliderCtrl &slider1, CSliderCtrl &slider2, bool low, float &edit, float resolution = 100.0);
	void OnCustomdrawSlider(CSliderCtrl &slider1, CSliderCtrl &slider2, bool low, int &edit);
	void OnUpdateEditStep(CSpinButtonCtrl &spin, float &edit);
	void OnDeltaposSpinStep(NMHDR* pNMHDR, float &edit, float resolution = 100.0);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGFUNCTIONALCLASSIFIERTEMPLATEDEFS_H__C61306FE_5FFC_4573_8E67_844840B0B34D__INCLUDED_)
 
