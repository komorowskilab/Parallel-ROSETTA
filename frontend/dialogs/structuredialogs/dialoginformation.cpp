#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\structuredialogs\dialoginformation.h>

#include <kernel\structures\structure.h>
#include <kernel\structures\annotatedstructure.h>

#include <common\messagehelper.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogInformation dialog

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: J—rn E. Nygjerd
// Date..........: 960419
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

CDialogInformation::CDialogInformation(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogInformation::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogInformation)
	//}}AFX_DATA_INIT
  m_Structure = NULL;
}


void CDialogInformation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogInformation)
	DDX_Control(pDX, IDC_EDIT_COMMENTS, m_Comments);
	DDX_Control(pDX, IDC_EDIT_HISTORY, m_History);
	DDX_Control(pDX, IDC_NAME, m_Name);
	DDX_Control(pDX, IDC_FILENAME, m_Filename);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CDialogInformation, CDialog)
	//{{AFX_MSG_MAP(CDialogInformation)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogInformation message handlers

//-------------------------------------------------------------------
// Method........: SetStructure
// Author........: J—rn E. Nygjerd
// Date..........: 960419
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogInformation::SetAnnotatedStructure(AnnotatedStructure *structure)
{
  m_Structure = structure;
}

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: J—rn E. Nygjerd
// Date..........: 960419
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

BOOL CDialogInformation::OnInitDialog()
{

  BOOL ret = CDialog::OnInitDialog();

  if (m_Structure.IsNULL())
		return false;

	bool has_annotation = m_Structure->IsAnnotated();
	
  // Get the informationvalues needed.			
	CString name         = has_annotation ? (m_Structure->GetAnnotation()->GetName()).GetBuffer() : "No name";
  CString filename     = has_annotation ? (m_Structure->GetAnnotation()->GetFilename()).GetBuffer() : " No filename";
  CString desc         = (IdHolder::GetDescription(m_Structure->GetId())).GetBuffer();
  CString comment      = has_annotation ? (m_Structure->GetAnnotation()->GetComments()).GetBuffer() : "";
  
	// Insert the values into the dialogbox. 
	m_Name.SetWindowText(name + " (" + desc + ")");
	m_Filename.SetWindowText(filename);
	m_Comments.SetWindowText(comment);

	CString history;
	int     no_entries = 0;

	if (has_annotation && m_Structure->GetAnnotation()->GetHistory() != NULL)
		no_entries = m_Structure->GetAnnotation()->GetHistory()->GetNoEntries();

	int i;

	// Initialize history window.
	for (i = 0; i < no_entries; i++) {
		history += m_Structure->GetAnnotation()->GetHistory()->GetEntry(i)->GetTimestamp().GetBuffer();
		history += " ";
		history += m_Structure->GetAnnotation()->GetHistory()->GetEntry(i)->GetUser().GetBuffer();
		history += "\r\n";
		history += m_Structure->GetAnnotation()->GetHistory()->GetEntry(i)->GetAction().GetBuffer();
		if (i < no_entries - 1)
			history += "\r\n\r\n";
	}

	m_History.SetWindowText(history);

	UpdateData(FALSE);
 
  return ret;

}



BOOL CDialogInformation::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}

void CDialogInformation::OnOK() 
{
	
	CString text;
	m_Comments.GetWindowText(text);
	m_Structure->GetAnnotation()->SetComments(text.GetBuffer(0));
	
	CDialog::OnOK();
}

void CDialogInformation::OnButtonClear() 
{

	// Query for confirmation.
	if (MessageHelper::Query("Are you sure you want to clear the history list?", "Clear", MessageHelper::BUTTONS_YESNO) != MessageHelper::RESPONSE_YES)
		return;

	if (m_Structure->IsAnnotated())
		m_Structure->GetAnnotation()->ClearHistory();

	m_History.SetWindowText("");

	UpdateData(FALSE);
	
}
 
