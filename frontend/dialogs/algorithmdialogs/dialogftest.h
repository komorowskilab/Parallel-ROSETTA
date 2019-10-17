// DialogFTest.h : header file
//
#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

#include <kernel/genomics/algorithms/ftest.h>


/////////////////////////////////////////////////////////////////////////////
// CDialogFTest dialog

class CDialogFTest : public CDialogAlgorithm
{
// Construction
public:
	CDialogFTest(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogFTest)
	enum { IDD = IDD_FTEST };
	CButton	m_Append;
	CButton m_DoBoot;
	CButton m_SeedCheck;
	CEdit	m_EditFThreshold;
	CEdit	m_EditSeed;
	CEdit	m_EditMaxAttr;
	CEdit	m_EditNBoot;
	CEdit	m_EditPThres;
	int		m_Attributes;
	float	m_FThreshold;
	int m_Seed;
	int	m_NBoot;
	float m_PThres;
	//}}AFX_DATA

private:
	
	Handle<FTest> m_alg;

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogFTest)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogFTest)
	virtual BOOL OnInitDialog();
	virtual void OnCheckBoot();
	virtual void OnCheckSeed();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
