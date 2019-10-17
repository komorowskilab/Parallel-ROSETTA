#if !defined(AFX_DIALOGSAVGENETICPARAMETERS_H__215414DD_4307_11D2_9575_006008A066CE__INCLUDED_)
#define AFX_DIALOGSAVGENETICPARAMETERS_H__215414DD_4307_11D2_9575_006008A066CE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dialogsavgeneticparameters.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>
#include <kernel\sav\algorithms\savgeneticreducer.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogSAVGeneticParameters dialog

class CDialogSAVGeneticParameters : public CDialogAlgorithm
{
private:
	
	Handle<SAVGeneticReducer> m_Reducer;

// Construction
public:
	CDialogSAVGeneticParameters(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogSAVGeneticParameters)
	enum { IDD = IDD_SAVGENETICREDUCER_PARAMS };
	CButton	m_CheckApproximateSolutions;
	CEdit	m_EditKeepSteps;
	CEdit	m_EditTemperatureStop;
	CEdit	m_EditTemperatureStart;
	CEdit	m_EditTemperatureDelta;
	CEdit	m_EditLifetimeMin;
	CEdit	m_EditLifetimeMax;
	CEdit	m_EditHitFraction;
	CEdit	m_EditCostFile;
	CButton	m_CheckCosts;
	CButton	m_ButtonBrowse;
	BOOL	m_BoltzmannScaling;
	BOOL	m_Costs;
	CString	m_CostFile;
	BOOL	m_Elitism;
	BOOL	m_Replacement;
	BOOL	m_StopFitness;
	BOOL	m_StopKeep;
	float	m_Bias;
	int		m_GenerationGap;
	float	m_HitFraction;
	int		m_LifetimeMax;
	int		m_LifetimeMin;
	int		m_NCrossover;
	int		m_NInversion;
	int		m_NMutation;
	float	m_PCrossover;
	float	m_PInversion;
	float	m_PMutation;
	long	m_Seed;
	int		m_SizeKeep;
	int		m_SizePopulation;
	float	m_TempDelta;
	float	m_TempStart;
	float	m_TempStop;
	CButton m_RadioVariationSimple;
	CButton m_RadioVariationModified;
	CButton m_RadioVariationVariable;
	int		m_KeepSteps;
	BOOL	m_ApproximateSolutions;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogSAVGeneticParameters)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogSAVGeneticParameters)
	afx_msg void OnButtonBrowse();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckBoltzmannscaling();
	afx_msg void OnCheckCosts();
	afx_msg void OnRadioVariationModified();
	afx_msg void OnRadioVariationSimple();
	afx_msg void OnRadioVariationVariable();
	virtual void OnOK();
	afx_msg void OnCheckApproximate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGSAVGENETICPARAMETERS_H__215414DD_4307_11D2_9575_006008A066CE__INCLUDED_)
 
