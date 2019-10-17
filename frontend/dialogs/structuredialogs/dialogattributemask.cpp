// dialogattributemask.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\structuredialogs\dialogattributemask.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogAttributeMask dialog


CDialogAttributeMask::CDialogAttributeMask(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogAttributeMask::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogAttributeMask)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogAttributeMask::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogAttributeMask)
	DDX_Control(pDX, IDC_RADIO_DISABLE, m_RadioDisabled);
	DDX_Control(pDX, IDC_RADIO_ENABLE, m_RadioEnabled);
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogAttributeMask, CDialog)
	//{{AFX_MSG_MAP(CDialogAttributeMask)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogAttributeMask message handlers

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: 
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogAttributeMask::OnOK() 
{

	DecisionTable::Mask mask;

	if (m_RadioDisabled.GetCheck())
		mask = DecisionTable::MASK_DISABLED;
	else
		mask = DecisionTable::MASK_ENABLED;

	bool ok = true;
	int  i;

	// Disable or enable all selected attributes.
	for (i = 0; i < m_Attributes.size(); i++) {
		int attribute_no = m_Attributes[i];
		Message::Debug("Flagging attribute " + String::Format(attribute_no) + " as " + DecisionTable::GetString(mask) + ".");
		if (!m_Table->SetAttributeMask(attribute_no, mask))
			ok = false;
	}

	if (!ok)
		Message::Error("Failed to set attribute mask.");
	
	CDialog::OnOK();

}

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: 
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

BOOL CDialogAttributeMask::OnInitDialog() 
{

	CDialog::OnInitDialog();

	if (m_Table == NULL) {
		Message::Error("Decision table not set.");
		CDialog::OnCancel();
		return FALSE;
	}

	int i, j;

	for (j = 0; j < m_Attributes.size(); j++) {
		int attribute_no = m_Attributes[j];
		Message::Debug("Selected attribute " + String::Format(attribute_no) + " for masking.");
	}

	int enable_count  = 0;
	int disable_count = 0;

	// Of the selected attributes, count which masking that has the majority.
	for (i = 0; i < m_Attributes.size(); i++) {
		if (m_Table->IsAttributeEnabled(m_Attributes[i]))
			enable_count++;
		else
			disable_count++;
	}

	bool disabled = (disable_count >= enable_count);

	// Set opposite as default value in dialog.
	m_RadioDisabled.SetCheck(!disabled);
	m_RadioEnabled.SetCheck(disabled);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
 
