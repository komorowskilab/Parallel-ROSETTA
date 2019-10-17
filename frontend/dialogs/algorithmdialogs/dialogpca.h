// #if !defined(AFX_ATTRIBUTESELECTOR_H__086B0409_1639_11D2_954C_006008A066CE__INCLUDED_)
// #define AFX_ATTRIBUTESELECTOR_H__086B0409_1639_11D2_954C_006008A066CE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dialogpca.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>
#include <kernel\genomics\frontend\HMXChart.h>
#include <kernel\genomics\algorithms\pca.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogPrincipalComponentAnalysis dialog

class CDialogPrincipalComponentAnalysis : public CDialogAlgorithm
{
// Construction
public:
	CDialogPrincipalComponentAnalysis(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogPrincipalComponentAnalysis)
	enum { IDD = IDD_PCA };
	CHMXChart m_Chart;
	CEdit	m_EditMaxComp;
	int		m_MaxComp;
	//}}AFX_DATA

private:
	
	Handle<PrincipalComponentAnalysis> m_alg;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogPrincipalComponentAnalysis)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogPrincipalComponentAnalysis)
	virtual void OnOK();
	virtual BOOL OnInitDialog();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

// #endif // !defined(AFX_ATTRIBUTESELECTOR_H__086B0409_1639_11D2_954C_006008A066CE__INCLUDED_)
 
