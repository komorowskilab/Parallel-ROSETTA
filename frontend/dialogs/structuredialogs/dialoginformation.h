// InformationDialog.h : header file
//

#include <kernel\structures\structure.h>
#include <kernel\structures\annotatedstructure.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogInformation dialog

class CDialogInformation : public CDialog
{
// Attributes
private:

// Operations
private:
  Handle<AnnotatedStructure> m_Structure;

public:
  void SetAnnotatedStructure(AnnotatedStructure *structure);

    // Overridables
  virtual  BOOL OnInitDialog();


// Construction
public:
	CDialogInformation(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogInformation)
	enum { IDD = IDD_INFORMATION };
	CEdit	m_Comments;
	CEdit	m_History;
	CStatic	m_Name;
	CStatic	m_Filename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogInformation)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogInformation)
	virtual void OnOK();
	afx_msg void OnButtonClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
