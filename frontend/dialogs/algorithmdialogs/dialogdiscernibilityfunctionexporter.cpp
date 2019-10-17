// dialogdiscernibilityfunctionexporter.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogdiscernibilityfunctionexporter.h>
#include <frontend\dialogs\algorithmdialogs\dialogobjectselector.h>

#include <kernel\algorithms\discernibilityfunctionexporter.h>
#include <kernel\structures\decisiontable.h>

#include <common\messagehelper.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogDiscernibilityFunctionExporter dialog


CDialogDiscernibilityFunctionExporter::CDialogDiscernibilityFunctionExporter(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogDiscernibilityFunctionExporter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDiscernibilityFunctionExporter)
	m_FilenameIDG = _T("");
	m_FilenameOutput = _T("");
	//}}AFX_DATA_INIT
}


void CDialogDiscernibilityFunctionExporter::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDiscernibilityFunctionExporter)
	DDX_Control(pDX, IDC_RADIO_FULL, m_RadioDiscernibilityFull);
	DDX_Control(pDX, IDC_RADIO_OBJECT, m_RadioDiscernibilityObject);
	DDX_Control(pDX, IDC_EDIT_FILENAME_IDG, m_EditFilenameIDG);
	DDX_Control(pDX, IDC_COMBO_NAMES, m_ComboNames);
	DDX_Control(pDX, IDC_CHECK_USENAMES, m_CheckUseNames);
	DDX_Control(pDX, IDC_CHECK_IDG, m_CheckUseIDG);
	DDX_Control(pDX, IDC_CHECK_MODULODECISION, m_CheckModuloDecision);
	DDX_Control(pDX, IDC_CHECK_SIMPLIFY, m_CheckSimplify);
	DDX_Control(pDX, IDC_BUTTON_IDGBROWSE, m_ButtonBrowseIDG);
	DDX_Text(pDX, IDC_EDIT_FILENAME_IDG, m_FilenameIDG);
	DDX_Text(pDX, IDC_EDIT_FILENAME_OUTPUT, m_FilenameOutput);
	DDX_Control(pDX, IDC_BUTTON_OBJECTSELECTION, m_ButtonObjectSelection);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDiscernibilityFunctionExporter, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogDiscernibilityFunctionExporter)
	ON_BN_CLICKED(IDC_RADIO_FULL, OnRadioDiscernibilityFull)
	ON_BN_CLICKED(IDC_RADIO_OBJECT, OnRadioDiscernibilityObject)
	ON_BN_CLICKED(IDC_BUTTON_OBJECTSELECTION, OnButtonObjectselection)
	ON_BN_CLICKED(IDC_BUTTON_OUTPUTBROWSE, OnButtonOutputbrowse)
	ON_BN_CLICKED(IDC_BUTTON_IDGBROWSE, OnButtonIdgbrowse)
	ON_BN_CLICKED(IDC_CHECK_IDG, OnCheckIdg)
	ON_BN_CLICKED(IDC_CHECK_USENAMES, OnCheckUsenames)
	ON_BN_CLICKED(IDC_CHECK_SIMPLIFY, OnCheckSimplify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDiscernibilityFunctionExporter message handlers

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogDiscernibilityFunctionExporter::OnInitDialog() 
{
	CDialogAlgorithm::OnInitDialog();
	
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(DISCERNIBILITYFUNCTIONEXPORTER)) {
		Message::Error("Algorithm not set or of incorrect type.");
		EndDialog(IDABORT);
		return FALSE;
	}

	if (m_Structure.IsNULL() || !m_Structure->IsA(DECISIONTABLE)) {
		Message::Error("Structure not set or of incorrect type.");
		EndDialog(IDABORT);
		return FALSE;
	}

	Handle<DiscernibilityFunctionExporter> exporter = dynamic_cast(DiscernibilityFunctionExporter *, m_Algorithm.GetPointer());
	Handle<DecisionTable>                  table    = dynamic_cast(DecisionTable *, m_Structure.GetPointer());

	// Initialize values.
	m_RadioDiscernibilityFull.SetCheck(exporter->GetDiscernibilityType() == Reduct::DISCERNIBILITY_FULL);
	m_RadioDiscernibilityObject.SetCheck(exporter->GetDiscernibilityType() == Reduct::DISCERNIBILITY_OBJECT);

	m_CheckUseIDG.SetCheck(exporter->UseIDG());
	m_FilenameIDG = exporter->GetIDGFilename().GetBuffer();

	int initial_selection = 0;
	int i;

	m_CheckUseNames.SetCheck(exporter->UseIDAttribute());
	for (i = 0; i < table->GetNoAttributes(false); i++) {
		String name = table->GetAttributeName(i, false);
		m_ComboNames.AddString(name.GetBuffer());
		if (name == exporter->GetIDAttributeName())
			initial_selection = i;
	}
	m_ComboNames.SetCurSel(initial_selection);

	m_FilenameOutput = exporter->GetFilename().GetBuffer();

	m_CheckModuloDecision.SetCheck(exporter->ModuloDecision());
	m_CheckSimplify.SetCheck(exporter->Simplify());

	// Initialize states.
	m_ButtonObjectSelection.EnableWindow(m_RadioDiscernibilityObject.GetCheck());
	m_CheckUseNames.EnableWindow(m_RadioDiscernibilityObject.GetCheck());
	m_ComboNames.EnableWindow(m_RadioDiscernibilityObject.GetCheck() && m_CheckUseNames.GetCheck());
	m_EditFilenameIDG.EnableWindow(m_CheckUseIDG.GetCheck());
	m_ButtonBrowseIDG.EnableWindow(m_CheckUseIDG.GetCheck());

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogDiscernibilityFunctionExporter::OnButtonObjectselection() 
{

	Handle<DiscernibilityFunctionExporter> exporter = dynamic_cast(DiscernibilityFunctionExporter *, m_Algorithm.GetPointer());

	CDialogObjectSelector dialog;

	dialog.m_Selector      = exporter->GetObjectSelector();
	dialog.m_Table         = dynamic_cast(DecisionTable *, m_Structure.GetPointer());

	if (dialog.DoModal() == IDOK)
		exporter->SetObjectSelector(dialog.m_Selector);

}

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogDiscernibilityFunctionExporter::OnButtonOutputbrowse() 
{

	CFileDialog fileDlg(TRUE, NULL, "*.*");
	if (fileDlg.DoModal() != IDOK)
		return;

  m_FilenameOutput = fileDlg.GetPathName();

	UpdateData(FALSE);
	
}

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogDiscernibilityFunctionExporter::OnButtonIdgbrowse() 
{

	CFileDialog fileDlg(TRUE, NULL, "*.*");
	if (fileDlg.DoModal() != IDOK)
		return;

  m_FilenameIDG = fileDlg.GetPathName();

	UpdateData(FALSE);
	
}

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogDiscernibilityFunctionExporter::OnCheckIdg() 
{
	UpdateData(TRUE);
	m_EditFilenameIDG.EnableWindow(m_CheckUseIDG.GetCheck());
	m_ButtonBrowseIDG.EnableWindow(m_CheckUseIDG.GetCheck());
}

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogDiscernibilityFunctionExporter::OnCheckUsenames() 
{
	UpdateData(TRUE);
	m_ComboNames.EnableWindow(m_CheckUseNames.GetCheck());
}

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogDiscernibilityFunctionExporter::OnCheckSimplify() 
{
	UpdateData(TRUE);
	if (!m_CheckSimplify.GetCheck()) {
		String text = "For other than small toy tables, unsimplified functions can\n"
			            "get very large and unwieldy. It is therefore recommended that\n"
							    "the simplification option is checked, unless you know what\n"
								  "you are doing.\n\n"
								  "Are you really sure you want to leave the functions unsimplified?";
		if (MessageHelper::Query(text, "Simplify", MessageHelper::BUTTONS_YESNO) == MessageHelper::RESPONSE_NO) {
			m_CheckSimplify.SetCheck(TRUE);
			UpdateData(FALSE);
		}
	}
}

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogDiscernibilityFunctionExporter::OnOK() 
{
	UpdateData(TRUE);

	Handle<DiscernibilityFunctionExporter> exporter = dynamic_cast(DiscernibilityFunctionExporter *, m_Algorithm.GetPointer());

	if (m_RadioDiscernibilityFull.GetCheck())
		exporter->SetDiscernibilityType(Reduct::DISCERNIBILITY_FULL);
	if (m_RadioDiscernibilityObject.GetCheck())
		exporter->SetDiscernibilityType(Reduct::DISCERNIBILITY_OBJECT);

	exporter->UseIDG(m_CheckUseIDG.GetCheck());
	exporter->SetIDGFilename(m_FilenameIDG.GetBuffer(0));

	exporter->UseIDAttribute(m_CheckUseNames.GetCheck());
	CString name;
	m_ComboNames.GetLBText(m_ComboNames.GetCurSel(), name);
	exporter->SetIDAttributeName(name.GetBuffer(0));

	exporter->SetFilename(m_FilenameOutput.GetBuffer(0));

	exporter->ModuloDecision(m_CheckModuloDecision.GetCheck());
	exporter->Simplify(m_CheckSimplify.GetCheck());
	
	CDialogAlgorithm::OnOK();
}

//------------------------------------------------------------------
// Method........: OnRadioDiscernibilityFull
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogDiscernibilityFunctionExporter::OnRadioDiscernibilityFull() 
{

	m_ButtonObjectSelection.EnableWindow(FALSE);
	m_CheckUseNames.EnableWindow(FALSE);
	m_ComboNames.EnableWindow(FALSE);
	
	// Hack: The !#&%"%± resource should do this...
	m_RadioDiscernibilityFull.SetCheck(TRUE);
	m_RadioDiscernibilityObject.SetCheck(FALSE);

}

//------------------------------------------------------------------
// Method........: OnRadioDiscernibilityObject
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogDiscernibilityFunctionExporter::OnRadioDiscernibilityObject() 
{

	m_ButtonObjectSelection.EnableWindow(TRUE);
	m_CheckUseNames.EnableWindow(TRUE);
	m_ComboNames.EnableWindow(m_CheckUseNames.GetCheck());
	
	// Hack: The !#&%"%± resource should do this...
	m_RadioDiscernibilityFull.SetCheck(FALSE);
	m_RadioDiscernibilityObject.SetCheck(TRUE);

}
 
