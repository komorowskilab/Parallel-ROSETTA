// dialogapproximationstatistics.h : header file
//

#include <kernel\structures\approximation.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogApproximationStatistics dialog

class CDialogApproximationStatistics : public CDialog
{
private:

	Handle<Approximation> m_Approximation;

// Construction
public:
	CDialogApproximationStatistics(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogApproximationStatistics)
	enum { IDD = IDD_APPROXIMATION_STATISTICS };
	CComboBox	m_ComboSet;
	CString	m_CardBn;
	CString	m_CardLower;
	CString	m_CardOut;
	CString	m_CardUniv;
	CString	m_CardUpper;
	CString	m_NoEqBn;
	CString	m_NoEqLower;
	CString	m_NoEqOut;
	CString	m_NoEqUniv;
	CString	m_NoEqUpper;
	CString	m_RatioBn;
	CString	m_RatioLower;
	CString	m_RatioOut;
	CString	m_RatioUniv;
	CString	m_RatioUpper;
	CString	m_Name;
	CString	m_FractSens;
	CString	m_RatioSens;
	CString	m_FractSpec;
	CString	m_RatioSpec;
	CString	m_FractAcc;
	CString	m_RatioAcc;
	//}}AFX_DATA

	const Approximation *GetApproximation() const {return m_Approximation.GetPointer();}
	void                 SetApproximation(const Approximation *approximation) {m_Approximation = approximation;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogApproximationStatistics)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogApproximationStatistics)
	afx_msg void OnSelchangeComboSet();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
