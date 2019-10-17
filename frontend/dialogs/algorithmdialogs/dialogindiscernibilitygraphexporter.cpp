// dialogindiscernibilitygraphexporter.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogindiscernibilitygraphexporter.h>

#include <kernel\algorithms\indiscernibilitygraphexporter.h>
#include <kernel\structures\decisiontable.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogIndiscernibilityGraphExporter dialog


CDialogIndiscernibilityGraphExporter::CDialogIndiscernibilityGraphExporter(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogIndiscernibilityGraphExporter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogIndiscernibilityGraphExporter)
	m_FilenameIDG = _T("");
	m_FilenameOutput = _T("");
	m_Card = 0;
	//}}AFX_DATA_INIT
}


void CDialogIndiscernibilityGraphExporter::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogIndiscernibilityGraphExporter)
	DDX_Control(pDX, IDC_CHECK_INCLUDE_REFLEXIVE, m_CheckReflexive);
	DDX_Control(pDX, IDC_STATIC_NAMETEXT, m_NameText);
	DDX_Control(pDX, IDC_STATIC_IDGTEXT, m_IDGText);
	DDX_Control(pDX, IDC_STATIC_CARDTEXT2, m_CardText2);
	DDX_Control(pDX, IDC_STATIC_CARDTEXT1, m_CardText1);
	DDX_Control(pDX, IDC_EDIT_CARDINALITY, m_EditCard);
	DDX_Control(pDX, IDC_CHECK_CARD, m_CheckCard);
	DDX_Control(pDX, IDC_CHECK_INCLUDE_DEGREE, m_CheckDegree);
	DDX_Control(pDX, IDC_CHECK_INCLUDE_APSP, m_CheckAPSP);
	DDX_Control(pDX, IDC_EDIT_FILENAME_IDG, m_EditFilenameIDG);
	DDX_Control(pDX, IDC_COMBO_NAMES, m_ComboNames);
	DDX_Control(pDX, IDC_CHECK_USENAMES, m_CheckUseNames);
	DDX_Control(pDX, IDC_CHECK_IDG, m_CheckUseIDG);
	DDX_Control(pDX, IDC_CHECK_MODULODECISION, m_CheckModuloDecision);
	DDX_Control(pDX, IDC_BUTTON_IDGBROWSE, m_ButtonBrowseIDG);
	DDX_Text(pDX, IDC_EDIT_FILENAME_IDG, m_FilenameIDG);
	DDX_Text(pDX, IDC_EDIT_FILENAME_OUTPUT, m_FilenameOutput);
	DDX_Text(pDX, IDC_EDIT_CARDINALITY, m_Card);
	DDV_MinMaxInt(pDX, m_Card, 0, 2147483647);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogIndiscernibilityGraphExporter, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogIndiscernibilityGraphExporter)
	ON_BN_CLICKED(IDC_BUTTON_OUTPUTBROWSE, OnButtonOutputbrowse)
	ON_BN_CLICKED(IDC_BUTTON_IDGBROWSE, OnButtonIdgbrowse)
	ON_BN_CLICKED(IDC_CHECK_IDG, OnCheckIdg)
	ON_BN_CLICKED(IDC_CHECK_USENAMES, OnCheckUsenames)
	ON_BN_CLICKED(IDC_CHECK_CARD, OnCheckCard)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogIndiscernibilityGraphExporter message handlers

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogIndiscernibilityGraphExporter::OnInitDialog() 
{
	CDialogAlgorithm::OnInitDialog();
	
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(INDISCERNIBILITYGRAPHEXPORTER)) {
		Message::Error("Algorithm not set or of incorrect type.");
		EndDialog(IDABORT);
		return FALSE;
	}

	if (m_Structure.IsNULL() || !m_Structure->IsA(DECISIONTABLE)) {
		Message::Error("Structure not set or of incorrect type.");
		EndDialog(IDABORT);
		return FALSE;
	}

	Handle<IndiscernibilityGraphExporter> exporter = dynamic_cast(IndiscernibilityGraphExporter *, m_Algorithm.GetPointer());
	Handle<DecisionTable>                 table    = dynamic_cast(DecisionTable *, m_Structure.GetPointer());

	// Initialize values.
	m_CheckUseIDG.SetCheck(exporter->UseIDG());
	m_FilenameIDG = exporter->GetIDGFilename().GetBuffer();

	m_CheckCard.SetCheck(exporter->IgnoreLowCardinalityEntries());
	m_Card = exporter->GetMaximumCardinality();

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

	m_CheckDegree.SetCheck(exporter->IncludeDegreeData());
	m_CheckAPSP.SetCheck(exporter->IncludeAPSPData());
	m_CheckReflexive.SetCheck(exporter->IncludeReflexiveData());

	// Hack: Everything is not implemented yet..
	m_CheckModuloDecision.EnableWindow(FALSE);
	exporter->ModuloDecision(false);
	m_CheckModuloDecision.SetCheck(exporter->ModuloDecision());

	UpdateData(FALSE);

	// Initialize states.
	OnCheckIdg();
	OnCheckCard();
	OnCheckUsenames();
	
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

void CDialogIndiscernibilityGraphExporter::OnButtonOutputbrowse() 
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

void CDialogIndiscernibilityGraphExporter::OnButtonIdgbrowse() 
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

void CDialogIndiscernibilityGraphExporter::OnCheckIdg() 
{
	UpdateData(TRUE);
	//m_IDGText.EnableWindow(m_CheckUseIDG.GetCheck());
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

void CDialogIndiscernibilityGraphExporter::OnCheckCard() 
{
	UpdateData(TRUE);
	m_EditCard.EnableWindow(m_CheckCard.GetCheck());
	//m_CardText1.EnableWindow(m_CheckCard.GetCheck());
	m_CardText2.EnableWindow(m_CheckCard.GetCheck());
}

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogIndiscernibilityGraphExporter::OnCheckUsenames() 
{
	UpdateData(TRUE);
	//m_NameText.EnableWindow(m_CheckUseNames.GetCheck());
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

void CDialogIndiscernibilityGraphExporter::OnOK() 
{

	UpdateData(TRUE);

	Handle<IndiscernibilityGraphExporter> exporter = dynamic_cast(IndiscernibilityGraphExporter *, m_Algorithm.GetPointer());

	exporter->UseIDG(m_CheckUseIDG.GetCheck());
	exporter->SetIDGFilename(m_FilenameIDG.GetBuffer(0));

	exporter->IgnoreLowCardinalityEntries(m_CheckCard.GetCheck());
	exporter->SetMaximumCardinality(m_Card);

	exporter->UseIDAttribute(m_CheckUseNames.GetCheck());
	CString name;
	m_ComboNames.GetLBText(m_ComboNames.GetCurSel(), name);
	exporter->SetIDAttributeName(name.GetBuffer(0));

	exporter->SetFilename(m_FilenameOutput.GetBuffer(0));

	exporter->IncludeReflexiveData(m_CheckReflexive.GetCheck());
	exporter->IncludeDegreeData(m_CheckDegree.GetCheck());
	exporter->IncludeAPSPData(m_CheckAPSP.GetCheck());

	exporter->ModuloDecision(m_CheckModuloDecision.GetCheck());

	CDialogAlgorithm::OnOK();
}
 
