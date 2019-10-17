// DialogTTest.h : header file
//
#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

#include <kernel\genomics\algorithms\ttest.h>


/////////////////////////////////////////////////////////////////////////////
// CDialogTTest dialog

class CDialogTTest : public CDialogAlgorithm
{
// Construction
public:
	CDialogTTest(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogTTest)
	enum { IDD = IDD_TTEST };
	CButton	m_Append;
	CButton m_DoBoot;
	CButton m_SeedCheck;
	CEdit	m_EditTThreshold;
	CEdit	m_EditMaxAttr;
  CEdit	m_EditSeed;
	CEdit	m_EditNBoot;
	CEdit	m_EditPThres;
	int		m_Attributes;
	float	m_TThreshold;
	int	m_NBoot;
	int m_Seed;
	float m_PThres;
	//}}AFX_DATA

private:
	
	Handle<TTest> m_alg;

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogTTest)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogTTest)
	virtual BOOL OnInitDialog();
	virtual void OnCheckBoot();
	virtual void OnCheckSeed();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
