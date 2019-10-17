// DialogBinarySplitter.h : header file
//
#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

#include <kernel\algorithms\binarysplitter.h>


/////////////////////////////////////////////////////////////////////////////
// CDialogBinarySplitter dialog

class CDialogBinarySplitter : public CDialogAlgorithm
{
// Construction
public:
	CDialogBinarySplitter(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogBinarySplitter)
	enum { IDD = IDD_BINARYSPLITTER };
	CButton	m_Append;
	CEdit	m_EditFactor;
	CEdit	m_EditSeed;
	int		m_Seed;
	float	m_Factor;
	//}}AFX_DATA

private:
	
	Handle<BinarySplitter> m_alg;

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogBinarySplitter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogBinarySplitter)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
