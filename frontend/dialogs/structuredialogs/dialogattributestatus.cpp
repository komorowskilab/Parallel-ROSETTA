// dialogattributestatus.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\structuredialogs\dialogattributestatus.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogAttributeStatus dialog


CDialogAttributeStatus::CDialogAttributeStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogAttributeStatus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogAttributeStatus)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogAttributeStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogAttributeStatus)
	DDX_Control(pDX, IDC_RADIO_CONDITION, m_RadioCondition);
	DDX_Control(pDX, IDC_RADIO_DECISION, m_RadioDecision);
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogAttributeStatus, CDialog)
	//{{AFX_MSG_MAP(CDialogAttributeStatus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogAttributeStatus message handlers

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: 
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogAttributeStatus::OnOK() 
{

	DecisionTable::Status status;

	if (m_RadioCondition.GetCheck())
		status = DecisionTable::STATUS_CONDITION;
	else
		status = DecisionTable::STATUS_DECISION;

	bool ok     = true;
	bool masked = false;
	int  i;

	// Set status fields for all selected attributes.
	for (i = 0; i < m_Attributes.size(); i++) {
		int attribute_no = m_Attributes[i];
		if (!m_Table->SetStatus(attribute_no, status, masked))
			ok = false;
	}

	if (!ok)
		Message::Error("Failed to set attribute status for all attributes.");
	
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

BOOL CDialogAttributeStatus::OnInitDialog() 
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
		Message::Debug("Selected attribute " + String::Format(attribute_no) + " for status.");
	}

	int condition_count = 0;
	int decision_count  = 0;

	bool masked = false;

	// Of the selected attributes, count which status field that has the majority.
	for (i = 0; i < m_Attributes.size(); i++) {
		if (m_Table->IsCondition(m_Attributes[i], masked))
			condition_count++;
		else
			decision_count++;
	}

	bool condition = (condition_count >= decision_count);

	// Set opposite as default value in dialog.
	m_RadioCondition.SetCheck(!condition);
	m_RadioDecision.SetCheck(condition);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
 
