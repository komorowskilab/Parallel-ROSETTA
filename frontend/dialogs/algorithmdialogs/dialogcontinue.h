
#include <resource.h>

#ifndef __CONTINUEDLG_H__
#define __CONTINUEDLG_H__

class CDialogContinue : public CDialog
{
// Construction / Destruction
public:
    CDialogContinue(UINT nCaptionID = 0);   // standard constructor
    ~CDialogContinue();

    BOOL Create(CWnd *pParent=NULL);

    // Checking for Cancel button
    BOOL CheckCancelButton();

    // Getting/setting status text.
		CString GetStatus() const;
    void    SetStatus(LPCTSTR lpszMessage);
        
// Dialog Data
    //{{AFX_DATA(CDialogContinue)
    enum { IDD = IDD_SIMPLEPROGRESSDIALOG };
    //}}AFX_DATA

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDialogContinue)
    public:
    virtual BOOL DestroyWindow();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
	UINT m_nCaptionID;
    
  BOOL m_bCancel;
  BOOL m_bParentDisabled;

  void ReEnableParent();

  virtual void OnCancel();
  virtual void OnOK() {}; 

  void PumpMessages();

    // Generated message map functions
    //{{AFX_MSG(CDialogContinue)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // __SIMPLEPROGDLG_H__
 
