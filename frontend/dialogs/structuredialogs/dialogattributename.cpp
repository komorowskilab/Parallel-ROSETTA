// dialogattributename.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\structuredialogs\dialogattributename.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogAttributeName dialog


CDialogAttributeName::CDialogAttributeName(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogAttributeName::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogAttributeName)
	//}}AFX_DATA_INIT
}


void CDialogAttributeName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogAttributeName)
	DDX_Control(pDX, IDC_EDITUNIT, m_EditUnit);
	DDX_Control(pDX, IDC_EDITNAME, m_EditName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogAttributeName, CDialog)
	//{{AFX_MSG_MAP(CDialogAttributeName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogAttributeName message handlers

BOOL CDialogAttributeName::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_EditName.SetWindowText(m_Name);
  m_EditUnit.SetWindowText(m_Unit);

	m_EditName.SetFocus();
	
	return FALSE; // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogAttributeName::OnOK() 
{
  CString text;
  
  m_EditName.GetWindowText(text);
  m_Name = text;

	m_EditUnit.GetWindowText(text);
  m_Unit = text;

	CDialog::OnOK();
}
 
