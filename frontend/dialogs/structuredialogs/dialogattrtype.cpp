// dialogattrtype.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\structuredialogs\dialogattrtype.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogAttributeType dialog


CDialogAttributeType::CDialogAttributeType(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogAttributeType::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogAttributeType)
	m_Exponent = 0;
	//}}AFX_DATA_INIT

	m_Type = Attribute::TYPE_INTEGER;

}


void CDialogAttributeType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogAttributeType)
	DDX_Control(pDX, IDC_EDIT_SCALING_EXPONENT, m_EditExponent);
	DDX_Control(pDX, IDC_RADIO_STRING, m_RadioString);
	DDX_Control(pDX, IDC_RADIO_INTEGER, m_RadioInteger);
	DDX_Control(pDX, IDC_RADIO_FLOAT, m_RadioFloat);
	DDX_Text(pDX, IDC_EDIT_SCALING_EXPONENT, m_Exponent);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogAttributeType, CDialog)
	//{{AFX_MSG_MAP(CDialogAttributeType)
	ON_BN_CLICKED(IDC_RADIO_FLOAT, OnRadioFloat)
	ON_BN_CLICKED(IDC_RADIO_INTEGER, OnRadioInteger)
	ON_BN_CLICKED(IDC_RADIO_STRING, OnRadioString)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogAttributeType message handlers

void CDialogAttributeType::OnRadioFloat() 
{
	m_EditExponent.EnableWindow(TRUE);
	m_Type = Attribute::TYPE_FLOAT;
	
}

void CDialogAttributeType::OnRadioInteger() 
{
	m_EditExponent.EnableWindow(FALSE);
	m_Type = Attribute::TYPE_INTEGER;
	
}

void CDialogAttributeType::OnRadioString() 
{
	m_EditExponent.EnableWindow(FALSE);
	m_Type = Attribute::TYPE_STRING;
	
}

BOOL CDialogAttributeType::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_RadioInteger.SetCheck(m_Type == Attribute::TYPE_INTEGER);
	m_RadioFloat.SetCheck(m_Type == Attribute::TYPE_FLOAT);
	m_RadioString.SetCheck(m_Type == Attribute::TYPE_STRING);
	
	m_EditExponent.EnableWindow(m_Type == Attribute::TYPE_FLOAT);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
 
