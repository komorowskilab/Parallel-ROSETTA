// dialogattributeselector.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogloadfeatureselection.h>

#include <kernel\algorithms\keyword.h>

#include <kernel\structures\decisiontable.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDialogLoadFeatureSelection dialog


CDialogLoadFeatureSelection::CDialogLoadFeatureSelection(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogLoadFeatureSelection::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogLoadFeatureSelection)

	m_File="nej.txt";
	//}}AFX_DATA_INIT
}

void CDialogLoadFeatureSelection::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogLoadFeatureSelection)
	DDX_Control(pDX, IDC_FILE, m_EditFile);
	DDX_Control(pDX, IDC_BROWSE, m_Browse);
	DDX_Text(pDX, IDC_FILE, m_File);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogLoadFeatureSelection, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogLoadFeatureSelection)
	ON_BN_CLICKED(IDC_BROWSE, OnButtonBrowse)
	ON_EN_CHANGE(IDC_FILE, OnFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogManualReducer message handlers

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogLoadFeatureSelection::OnOK() 
{

	if (m_Algorithm.IsNULL()) {
		Message::Error("Algorithm not present or of unexpected type.");
		return;
	}

 	String attributes;
// 	Vector (bool) attributes; // attributes.reserve(m_ListAttributes.
	int no_added = 0;
	int i;

	// attributes += '{';

	// Build description of selected attributes.


	// attributes += '}';

		// Set parameters.
	CString cfilename;
	m_EditFile.GetWindowText(cfilename);

	
	String filename = cfilename;
	// m_Algorithm->SetParameter(Keyword::Attributes(), attributes);
		m_Algorithm->SetParameter(Keyword::Filename(), filename);
	
	CDialogAlgorithm::OnOK();

}

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogLoadFeatureSelection::OnInitDialog() 
{

	int i;

	CDialogAlgorithm::OnInitDialog();
	

	m_EditFile.EnableWindow(true);
	m_Browse.EnableWindow(true);

	if (m_Algorithm.IsNULL()) {
		Message::Error("Algorithm not present or of unexpected type.");
		EndDialog(IDABORT);
		return FALSE;
	}

	if (m_Structure.IsNULL() || !m_Structure->IsA(DECISIONTABLE)) {
		Message::Error("Structure not present or of unexpected type.");
		EndDialog(IDABORT);
		return FALSE;
	}

	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, m_Structure.GetPointer());

	String  attributes = m_Algorithm->GetParameters();
	

	// Does the parameter string contain a "ATTRIBUTES = ...;" substring? If so, extract its value.
	if (attributes.Contains(Keyword::Attributes())) {
		attributes = attributes.After(Keyword::Attributes());
		attributes = attributes.After(Keyword::Assignment());
		if (attributes.Contains(Keyword::Separator()))
			attributes = attributes.Before(Keyword::Separator());
		attributes.Trim("\t ");
	}
	else {
		attributes = "";
	}

// 	m_ListAttributes.ResetContent();

	bool masked = true;
	bool checked = TRUE;

	// Fill list box.
	for (i = 0; i < table->GetNoAttributes(masked); i++) {
		String name = table->GetAttributeName(i, masked);
	//	m_ListAttributes.AddString(name.GetBuffer());
	 	
		// Well, to avoid errors.
		//if (attributes[i]=='1') { F
		// 	m_ListAttributes.SetCheck(i, !checked);
		// }
	}

	int dcolumn= table->GetDecisionAttribute(masked);
	// 	m_ListAttributes.SetCheck(dcolumn, checked);
	

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
// Method........: OnButtonInvert
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================



void CDialogLoadFeatureSelection::OnButtonBrowse() 
{
	CFileDialog fileDlg(TRUE, NULL, "*.*");
	if (fileDlg.DoModal() == IDOK)
		m_File  = fileDlg.GetPathName();
	UpdateData(FALSE);
}


void CDialogLoadFeatureSelection::OnFile() 
{
//	CDialog::OnUpdate(); 
}
 
