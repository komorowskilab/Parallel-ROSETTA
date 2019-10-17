// dialoglicense.cpp : implementation file
//

#include <stdafx.h>
#include <frontend\rosetta.h>
#include <frontend\dialogs\dialoglicense.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogLicense dialog


CDialogLicense::CDialogLicense(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogLicense::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogLicense)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogLicense::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogLicense)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogLicense, CDialog)
	//{{AFX_MSG_MAP(CDialogLicense)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogLicense message handlers
 
