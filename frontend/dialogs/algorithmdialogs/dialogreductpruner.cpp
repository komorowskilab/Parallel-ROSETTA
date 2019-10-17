// dialogreductpruner.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogreductpruner.h>

#include <kernel\algorithms\myreductfilter.h>

#include <kernel\structures\reducts.h>
#include <kernel\structures\decisiontable.h>
#include <kernel\structures\dictionary.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogReductPruner dialog


CDialogReductPruner::CDialogReductPruner(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogReductPruner::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogReductPruner)
	m_InvertLogic = FALSE;
	m_LengthLower = 0;
	m_LengthUpper = 0;
	m_StrengthLower = 0;
	m_StrengthUpper = 0;
	//}}AFX_DATA_INIT

	m_Masked = TRUE;

}


void CDialogReductPruner::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogReductPruner)
	DDX_Control(pDX, IDC_CHECK_STRENGTH, m_CheckStrength);
	DDX_Control(pDX, IDC_CHECK_LENGTH, m_CheckLength);
	DDX_Control(pDX, IDC_CHECK_ATTRIBUTE, m_CheckAttribute);
	DDX_Control(pDX, IDC_CHECK_INVERTLOGIC, m_CheckInvertLogic);
	DDX_Control(pDX, IDC_EDIT_STRENGTH_UPPER, m_EditStrengthUpper);
	DDX_Control(pDX, IDC_EDIT_STRENGTH_LOWER, m_EditStrengthLower);
	DDX_Control(pDX, IDC_EDIT_LENGTH_UPPER, m_EditLengthUpper);
	DDX_Control(pDX, IDC_EDIT_LENGTH_LOWER, m_EditLengthLower);
	DDX_Control(pDX, IDC_COMBO_ATTRIBUTE, m_ComboAttribute);
	DDX_Control(pDX, IDC_RADIO_AND, m_RadioAnd);
	DDX_Control(pDX, IDC_RADIO_OR, m_RadioOr);
	DDX_Check(pDX, IDC_CHECK_INVERTLOGIC, m_InvertLogic);
	DDX_Text(pDX, IDC_EDIT_LENGTH_LOWER, m_LengthLower);
	DDX_Text(pDX, IDC_EDIT_LENGTH_UPPER, m_LengthUpper);
	DDX_Text(pDX, IDC_EDIT_STRENGTH_LOWER, m_StrengthLower);
	DDX_Text(pDX, IDC_EDIT_STRENGTH_UPPER, m_StrengthUpper);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogReductPruner, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogReductPruner)
	ON_BN_CLICKED(IDC_CHECK_INVERTLOGIC, OnCheckInvertlogic)
	ON_BN_CLICKED(IDC_CHECK_ATTRIBUTE, OnCheckAttribute)
	ON_BN_CLICKED(IDC_CHECK_LENGTH, OnCheckLength)
	ON_BN_CLICKED(IDC_CHECK_STRENGTH, OnCheckStrength)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogReductPruner message handlers

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: 
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

BOOL CDialogReductPruner::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();

	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(MYREDUCTFILTER)) {
		Message::Error("Algorithm of wrong type, expected a reduct filter.");
		EndDialog(IDABORT);
		return FALSE;
	}

	if (m_Structure.IsNULL() || !m_Structure->IsA(REDUCTS)) {
		Message::Error("Structure of wrong type, expected a reduct set.");
		EndDialog(IDABORT);
		return FALSE;
	}

	/*
	if (m_Structure->HasChildren()) {
		Message::Error("Cannot prune a reduct set with children.");
		EndDialog(IDABORT);
		return FALSE;
	}
	*/

	Handle<MyReductFilter> pruner = dynamic_cast(MyReductFilter *, m_Algorithm.GetPointer());

	// Set check buttons.
	m_CheckLength.SetCheck(pruner->GetFilteringStrategy() & MyReductFilter::FILTERING_LENGTH);
	m_CheckStrength.SetCheck(pruner->GetFilteringStrategy() & MyReductFilter::FILTERING_SUPPORT);
	m_CheckAttribute.SetCheck(pruner->GetFilteringStrategy() & MyReductFilter::FILTERING_ATTRIBUTE);

	// Enable windows.
	OnCheckLength();
	OnCheckStrength();
	OnCheckAttribute();

	// Set edit controls.
	m_LengthLower    = pruner->GetLowerLengthThreshold();
	m_LengthUpper    = pruner->GetUpperLengthThreshold();
	m_StrengthLower  = pruner->GetLowerSupportThreshold();
	m_StrengthUpper  = pruner->GetUpperSupportThreshold();

	// Get originating table and dictionary.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, m_Structure->FindParent(DECISIONTABLE, *m_Project));

	if (table == NULL) {
		Message::Error("Could not find parent decision table.");
		EndDialog(IDABORT);
		return FALSE;
	}

	int i;

	// Fill attribute list.
	for (i = 0; i < table->GetNoAttributes(m_Masked); i++) {
		String name = table->GetAttributeName(i, m_Masked);
		m_ComboAttribute.AddString(name.GetBuffer());
	}
	m_ComboAttribute.SetCurSel(pruner->GetAttribute());

	// Set connective stuff.
	m_RadioAnd.SetCheck(pruner->GetFilteringConnective() == MyReductFilter::CONNECTIVE_AND);
	m_RadioOr.SetCheck(pruner->GetFilteringConnective() == MyReductFilter::CONNECTIVE_OR);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
// Method  ......: OnCheckLength
// Author........: 
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogReductPruner::OnCheckLength() 
{

	m_EditLengthLower.EnableWindow(m_CheckLength.GetCheck());
	m_EditLengthUpper.EnableWindow(m_CheckLength.GetCheck());

}

//-------------------------------------------------------------------
// Method  ......: OnCheckStrength
// Author........: 
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogReductPruner::OnCheckStrength() 
{

	m_EditStrengthLower.EnableWindow(m_CheckStrength.GetCheck());
	m_EditStrengthUpper.EnableWindow(m_CheckStrength.GetCheck());

}

//-------------------------------------------------------------------
// Method  ......: OnCheckAttribute
// Author........: 
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogReductPruner::OnCheckAttribute() 
{

	m_ComboAttribute.EnableWindow(m_CheckAttribute.GetCheck());

}

//-------------------------------------------------------------------
// Method  ......: OnCheckInvertlogic
// Author........: 
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogReductPruner::OnCheckInvertlogic() 
{
}

//-------------------------------------------------------------------
// Method  ......: OnOK
// Author........: 
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogReductPruner::OnOK() 
{

	CDialogAlgorithm::OnOK();

	UpdateData(TRUE);

	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(MYREDUCTFILTER))
		return;

	Handle<MyReductFilter> pruner = dynamic_cast(MyReductFilter *, m_Algorithm.GetPointer());

	int strategy = 0;

	// Set pruning strategy.
	if (m_CheckLength.GetCheck())
		strategy |= MyReductFilter::FILTERING_LENGTH;
	if (m_CheckStrength.GetCheck())
		strategy |= MyReductFilter::FILTERING_SUPPORT;
	if (m_CheckAttribute.GetCheck())
		strategy |= MyReductFilter::FILTERING_ATTRIBUTE;

	pruner->SetFilteringStrategy(strategy);

	// Set thresholds.
	pruner->SetLowerLengthThreshold(m_LengthLower);
	pruner->SetUpperLengthThreshold(m_LengthUpper);
	pruner->SetLowerSupportThreshold(m_StrengthLower);
	pruner->SetUpperSupportThreshold(m_StrengthUpper);

	// Set attribute index.
	int attribute = m_ComboAttribute.GetCurSel();
	if (attribute == CB_ERR)
		pruner->SetAttribute(Undefined::Integer());
	else
		pruner->SetAttribute(attribute);

	// Set connective.
	if (m_RadioAnd.GetCheck())
		pruner->SetFilteringConnective(MyReductFilter::CONNECTIVE_AND);
	else if (m_RadioOr.GetCheck())
		pruner->SetFilteringConnective(MyReductFilter::CONNECTIVE_OR);
	else
		Message::Warning("Filtering connective not set.");

	// Set invert logic flag.
	pruner->InvertLogic(m_CheckInvertLogic.GetCheck());

}
 
