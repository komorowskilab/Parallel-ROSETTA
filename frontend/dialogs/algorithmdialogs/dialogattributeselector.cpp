// dialogattributeselector.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogattributeselector.h>

#include <kernel\algorithms\keyword.h>

#include <kernel\structures\decisiontable.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDialogAttributeSelector dialog


CDialogAttributeSelector::CDialogAttributeSelector(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogAttributeSelector::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogAttributeSelector)
	//}}AFX_DATA_INIT
}

void CDialogAttributeSelector::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogAttributeSelector)
	DDX_Control(pDX, IDC_LIST_ATTRIBUTES, m_ListAttributes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogAttributeSelector, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogAttributeSelector)
	ON_BN_CLICKED(IDC_BUTTON_INVERT, OnButtonInvert)
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

void CDialogAttributeSelector::OnOK() 
{

	if (m_Algorithm.IsNULL()) {
		Message::Error("Algorithm not present or of unexpected type.");
		return;
	}

	String attributes;

	int no_added = 0;
	int i;

	attributes += '{';

	// Build description of selected attributes.
	for (i = 0; i < m_ListAttributes.GetCount(); i++) {
		if (m_ListAttributes.GetCheck(i)) {
			CString name;
			m_ListAttributes.GetText(i, name);
			if (no_added > 0)
				attributes += ", ";
			attributes += name.GetBuffer(0);
			no_added++;
		}
	}

	attributes += '}';

	// Set parameters.
	m_Algorithm->SetParameter(Keyword::Attributes(), attributes);
	
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

BOOL CDialogAttributeSelector::OnInitDialog() 
{

	int i;

	CDialogAlgorithm::OnInitDialog();
	
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

	String attributes = m_Algorithm->GetParameters();

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

	m_ListAttributes.ResetContent();

	bool masked = true;

	// Fill list box.
	for (i = 0; i < table->GetNoAttributes(masked); i++) {
		String name = table->GetAttributeName(i, masked);
		m_ListAttributes.AddString(name.GetBuffer());
		bool checked = attributes.Contains(name);
		m_ListAttributes.SetCheck(i, checked);
	}

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

void CDialogAttributeSelector::OnButtonInvert() 
{

	int i;

	for (i = 0; i < m_ListAttributes.GetCount(); i++)
		m_ListAttributes.SetCheck(i, !m_ListAttributes.GetCheck(i));

}
 
