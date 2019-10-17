// dialogqualityrulefilter.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogqualityrulefilter.h>

#include <kernel\algorithms\qualityrulefilter.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogQualityRuleFilter dialog


CDialogQualityRuleFilter::CDialogQualityRuleFilter(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogQualityRuleFilter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogQualityRuleFilter)
	m_Invert = FALSE;
	m_UseLower = FALSE;
	m_UseUpper = FALSE;
	m_Lower = 0.0f;
	m_Upper = 0.0f;
	m_Weight = 0.0f;
	m_RemoveUndefined = FALSE;
	//}}AFX_DATA_INIT

	m_EnableOnlyQualitySelection = FALSE;
	m_EnableUndefinedSelection   = TRUE;

}


void CDialogQualityRuleFilter::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogQualityRuleFilter)
	DDX_Control(pDX, IDC_CHECK_REMOVE_UNDEFINED, m_CheckRemoveUndefined);
	DDX_Control(pDX, IDC_CHECK_USEUPPER, m_CheckUseUpper);
	DDX_Control(pDX, IDC_CHECK_USELOWER, m_CheckUseLower);
	DDX_Control(pDX, IDC_CHECK_INVERT, m_CheckInvert);
	DDX_Control(pDX, IDC_EDIT_MICHALSKI_WEIGHT, m_EditWeight);
	DDX_Control(pDX, IDC_EDIT_UPPER, m_EditUpper);
	DDX_Control(pDX, IDC_EDIT_LOWER, m_EditLower);
	DDX_Control(pDX, IDC_COMBO_COLEMAN_NORMALIZATION, m_ComboNormalization);
	DDX_Check(pDX, IDC_CHECK_INVERT, m_Invert);
	DDX_Check(pDX, IDC_CHECK_USELOWER, m_UseLower);
	DDX_Check(pDX, IDC_CHECK_USEUPPER, m_UseUpper);
	DDX_Text(pDX, IDC_EDIT_LOWER, m_Lower);
	DDX_Text(pDX, IDC_EDIT_UPPER, m_Upper);
	DDX_Text(pDX, IDC_EDIT_MICHALSKI_WEIGHT, m_Weight);
	DDV_MinMaxFloat(pDX, m_Weight, 0.f, 1.f);
	DDX_Control(pDX, IDC_RADIO_MICHALSKI, m_RadioMichalski);
	DDX_Control(pDX, IDC_RADIO_TORGO, m_RadioTorgo);
	DDX_Control(pDX, IDC_RADIO_BRAZDIL, m_RadioBrazdil);
	DDX_Control(pDX, IDC_RADIO_PEARSON, m_RadioPearson);
	DDX_Control(pDX, IDC_RADIO_G2, m_RadioG2);
	DDX_Control(pDX, IDC_RADIO_J, m_RadioJ);
	DDX_Control(pDX, IDC_RADIO_COHEN, m_RadioCohen);
	DDX_Control(pDX, IDC_RADIO_COLEMAN, m_RadioColeman);
	DDX_Control(pDX, IDC_RADIO_KONONENKO, m_RadioKononenko);
	DDX_Check(pDX, IDC_CHECK_REMOVE_UNDEFINED, m_RemoveUndefined);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogQualityRuleFilter, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogQualityRuleFilter)
	ON_BN_CLICKED(IDC_CHECK_USELOWER, OnCheckUselower)
	ON_BN_CLICKED(IDC_CHECK_USEUPPER, OnCheckUseupper)
	ON_BN_CLICKED(IDC_RADIO_BRAZDIL, OnRadioBrazdil)
	ON_BN_CLICKED(IDC_RADIO_COHEN, OnRadioCohen)
	ON_BN_CLICKED(IDC_RADIO_COLEMAN, OnRadioColeman)
	ON_BN_CLICKED(IDC_RADIO_G2, OnRadioG2)
	ON_BN_CLICKED(IDC_RADIO_J, OnRadioJ)
	ON_BN_CLICKED(IDC_RADIO_KONONENKO, OnRadioKononenko)
	ON_BN_CLICKED(IDC_RADIO_MICHALSKI, OnRadioMichalski)
	ON_BN_CLICKED(IDC_RADIO_PEARSON, OnRadioPearson)
	ON_BN_CLICKED(IDC_RADIO_TORGO, OnRadioTorgo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogQualityRuleFilter message handlers

//------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogQualityRuleFilter::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();
	
	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(QUALITYRULEFILTER)) { 
		Message::Error("Algorithm not present or not a QualityRuleFilter.");
		CDialog::OnCancel();
		return FALSE;
  }

	// Cast to verified type.
	Handle<QualityRuleFilter> filter = dynamic_cast(QualityRuleFilter *, m_Algorithm.GetPointer());

	// Initialize radio buttons.
	m_RadioMichalski.SetCheck(filter->GetFilteringStrategy() == QualityRuleFilter::FILTERING_MICHALSKI);
	m_RadioTorgo.SetCheck(filter->GetFilteringStrategy()     == QualityRuleFilter::FILTERING_TORGO);
	m_RadioBrazdil.SetCheck(filter->GetFilteringStrategy()   == QualityRuleFilter::FILTERING_BRAZDIL);
	m_RadioPearson.SetCheck(filter->GetFilteringStrategy()   == QualityRuleFilter::FILTERING_PEARSON);
	m_RadioG2.SetCheck(filter->GetFilteringStrategy()        == QualityRuleFilter::FILTERING_G2);
	m_RadioJ.SetCheck(filter->GetFilteringStrategy()         == QualityRuleFilter::FILTERING_J);
	m_RadioCohen.SetCheck(filter->GetFilteringStrategy()     == QualityRuleFilter::FILTERING_COHEN);
	m_RadioColeman.SetCheck(filter->GetFilteringStrategy()   == QualityRuleFilter::FILTERING_COLEMAN);
	m_RadioKononenko.SetCheck(filter->GetFilteringStrategy() == QualityRuleFilter::FILTERING_KONONENKO);

	// Initialize combo boxes.
	m_ComboNormalization.ResetContent();
	m_ComboNormalization.AddString("None");
	m_ComboNormalization.AddString("C1");
	m_ComboNormalization.AddString("C2");

	if (filter->GetColemanNormalization() == ColemanRuleEvaluator::NORMALIZATION_NONE)
		m_ComboNormalization.SetCurSel(0);
	else if (filter->GetColemanNormalization() == ColemanRuleEvaluator::NORMALIZATION_C1)
		m_ComboNormalization.SetCurSel(1);
	else if (filter->GetColemanNormalization() == ColemanRuleEvaluator::NORMALIZATION_C2)
		m_ComboNormalization.SetCurSel(2);

	// Initialize check boxes.
	m_Invert          = filter->InvertLogic();
	m_UseLower        = filter->UseLower();
	m_UseUpper        = filter->UseUpper();
	m_RemoveUndefined = filter->RemoveUndefinedRules();

	// Initialize edit fields.
	m_Weight = filter->GetMichalskiWeight();
	m_Lower  = filter->GetLower();
	m_Upper  = filter->GetUpper();

	// Enable/disable stuff.
	m_EditWeight.EnableWindow(m_RadioMichalski.GetCheck());
	m_ComboNormalization.EnableWindow(m_RadioColeman.GetCheck());
	m_EditLower.EnableWindow(m_UseLower && !m_EnableOnlyQualitySelection);
	m_EditUpper.EnableWindow(m_UseUpper && !m_EnableOnlyQualitySelection);
 
	m_CheckInvert.EnableWindow(!m_EnableOnlyQualitySelection);
	m_CheckUseLower.EnableWindow(!m_EnableOnlyQualitySelection);
	m_CheckUseUpper.EnableWindow(!m_EnableOnlyQualitySelection);

	m_CheckRemoveUndefined.EnableWindow(m_EnableUndefinedSelection);

	// Update data.
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//------------------------------------------------------------------
// Method........: OnCheck...
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogQualityRuleFilter::OnCheckUselower() 
{
	UpdateData(TRUE);
	m_EditLower.EnableWindow(m_UseLower);
}

void CDialogQualityRuleFilter::OnCheckUseupper() 
{
	UpdateData(TRUE);
	m_EditUpper.EnableWindow(m_UseUpper);
}

//------------------------------------------------------------------
// Method........: OnRadio...
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: The %&±&% class wizard fucks things up. Do it
//                 by hand.
// Revisions.....:
//===================================================================

void CDialogQualityRuleFilter::OnRadioBrazdil() 
{
	m_RadioMichalski.SetCheck(FALSE);
	m_RadioTorgo.SetCheck(FALSE);
	m_RadioBrazdil.SetCheck(TRUE);
	m_RadioPearson.SetCheck(FALSE);
	m_RadioG2.SetCheck(FALSE);
	m_RadioJ.SetCheck(FALSE);
	m_RadioCohen.SetCheck(FALSE);
	m_RadioColeman.SetCheck(FALSE);
	m_RadioKononenko.SetCheck(FALSE);

	m_EditWeight.EnableWindow(FALSE);
	m_ComboNormalization.EnableWindow(FALSE);
}

void CDialogQualityRuleFilter::OnRadioCohen() 
{
	m_RadioMichalski.SetCheck(FALSE);
	m_RadioTorgo.SetCheck(FALSE);
	m_RadioBrazdil.SetCheck(FALSE);
	m_RadioPearson.SetCheck(FALSE);
	m_RadioG2.SetCheck(FALSE);
	m_RadioJ.SetCheck(FALSE);
	m_RadioCohen.SetCheck(TRUE);
	m_RadioColeman.SetCheck(FALSE);
	m_RadioKononenko.SetCheck(FALSE);

	m_EditWeight.EnableWindow(FALSE);
	m_ComboNormalization.EnableWindow(FALSE);
}

void CDialogQualityRuleFilter::OnRadioColeman() 
{
	m_RadioMichalski.SetCheck(FALSE);
	m_RadioTorgo.SetCheck(FALSE);
	m_RadioBrazdil.SetCheck(FALSE);
	m_RadioPearson.SetCheck(FALSE);
	m_RadioG2.SetCheck(FALSE);
	m_RadioJ.SetCheck(FALSE);
	m_RadioCohen.SetCheck(FALSE);
	m_RadioColeman.SetCheck(TRUE);
	m_RadioKononenko.SetCheck(FALSE);

	m_EditWeight.EnableWindow(FALSE);
	m_ComboNormalization.EnableWindow(TRUE);
}

void CDialogQualityRuleFilter::OnRadioG2() 
{
	m_RadioMichalski.SetCheck(FALSE);
	m_RadioTorgo.SetCheck(FALSE);
	m_RadioBrazdil.SetCheck(FALSE);
	m_RadioPearson.SetCheck(FALSE);
	m_RadioG2.SetCheck(TRUE);
	m_RadioJ.SetCheck(FALSE);
	m_RadioCohen.SetCheck(FALSE);
	m_RadioColeman.SetCheck(FALSE);
	m_RadioKononenko.SetCheck(FALSE);

	m_EditWeight.EnableWindow(FALSE);
	m_ComboNormalization.EnableWindow(FALSE);
}

void CDialogQualityRuleFilter::OnRadioJ() 
{
	m_RadioMichalski.SetCheck(FALSE);
	m_RadioTorgo.SetCheck(FALSE);
	m_RadioBrazdil.SetCheck(FALSE);
	m_RadioPearson.SetCheck(FALSE);
	m_RadioG2.SetCheck(FALSE);
	m_RadioJ.SetCheck(TRUE);
	m_RadioCohen.SetCheck(FALSE);
	m_RadioColeman.SetCheck(FALSE);
	m_RadioKononenko.SetCheck(FALSE);

	m_EditWeight.EnableWindow(FALSE);
	m_ComboNormalization.EnableWindow(FALSE);
}

void CDialogQualityRuleFilter::OnRadioKononenko() 
{
	m_RadioMichalski.SetCheck(FALSE);
	m_RadioTorgo.SetCheck(FALSE);
	m_RadioBrazdil.SetCheck(FALSE);
	m_RadioPearson.SetCheck(FALSE);
	m_RadioG2.SetCheck(FALSE);
	m_RadioJ.SetCheck(FALSE);
	m_RadioCohen.SetCheck(FALSE);
	m_RadioColeman.SetCheck(FALSE);
	m_RadioKononenko.SetCheck(TRUE);

	m_EditWeight.EnableWindow(FALSE);
	m_ComboNormalization.EnableWindow(FALSE);
}

void CDialogQualityRuleFilter::OnRadioMichalski() 
{
	m_RadioMichalski.SetCheck(TRUE);
	m_RadioTorgo.SetCheck(FALSE);
	m_RadioBrazdil.SetCheck(FALSE);
	m_RadioPearson.SetCheck(FALSE);
	m_RadioG2.SetCheck(FALSE);
	m_RadioJ.SetCheck(FALSE);
	m_RadioCohen.SetCheck(FALSE);
	m_RadioColeman.SetCheck(FALSE);
	m_RadioKononenko.SetCheck(FALSE);

	m_EditWeight.EnableWindow(TRUE);
	m_ComboNormalization.EnableWindow(FALSE);
}

void CDialogQualityRuleFilter::OnRadioPearson() 
{
	m_RadioMichalski.SetCheck(FALSE);
	m_RadioTorgo.SetCheck(FALSE);
	m_RadioBrazdil.SetCheck(FALSE);
	m_RadioPearson.SetCheck(TRUE);
	m_RadioG2.SetCheck(FALSE);
	m_RadioJ.SetCheck(FALSE);
	m_RadioCohen.SetCheck(FALSE);
	m_RadioColeman.SetCheck(FALSE);
	m_RadioKononenko.SetCheck(FALSE);

	m_EditWeight.EnableWindow(FALSE);
	m_ComboNormalization.EnableWindow(FALSE);
}

void CDialogQualityRuleFilter::OnRadioTorgo() 
{
	m_RadioMichalski.SetCheck(FALSE);
	m_RadioTorgo.SetCheck(TRUE);
	m_RadioBrazdil.SetCheck(FALSE);
	m_RadioPearson.SetCheck(FALSE);
	m_RadioG2.SetCheck(FALSE);
	m_RadioJ.SetCheck(FALSE);
	m_RadioCohen.SetCheck(FALSE);
	m_RadioColeman.SetCheck(FALSE);
	m_RadioKononenko.SetCheck(FALSE);

	m_EditWeight.EnableWindow(FALSE);
	m_ComboNormalization.EnableWindow(FALSE);
}

//------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogQualityRuleFilter::OnOK() 
{

	UpdateData(TRUE);

	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(QUALITYRULEFILTER)) {
		Message::Error("Algorithm not present or of unexpected type.");
		EndDialog(IDABORT);
		return;
	}

	// Cast to verified type.
	Handle<QualityRuleFilter> filter = dynamic_cast(QualityRuleFilter *, m_Algorithm.GetPointer());

	// Set quality measure.
	if (m_RadioMichalski.GetCheck())
		filter->SetFilteringStrategy(QualityRuleFilter::FILTERING_MICHALSKI);
	else if (m_RadioTorgo.GetCheck())
		filter->SetFilteringStrategy(QualityRuleFilter::FILTERING_TORGO);
	else if (m_RadioBrazdil.GetCheck())
		filter->SetFilteringStrategy(QualityRuleFilter::FILTERING_BRAZDIL);
	else if (m_RadioPearson.GetCheck())
		filter->SetFilteringStrategy(QualityRuleFilter::FILTERING_PEARSON);
	else if (m_RadioG2.GetCheck())
		filter->SetFilteringStrategy(QualityRuleFilter::FILTERING_G2);
	else if (m_RadioJ.GetCheck())
		filter->SetFilteringStrategy(QualityRuleFilter::FILTERING_J);
	else if (m_RadioCohen.GetCheck())
		filter->SetFilteringStrategy(QualityRuleFilter::FILTERING_COHEN);
	else if (m_RadioColeman.GetCheck())
		filter->SetFilteringStrategy(QualityRuleFilter::FILTERING_COLEMAN);
	else if (m_RadioKononenko.GetCheck())
		filter->SetFilteringStrategy(QualityRuleFilter::FILTERING_KONONENKO);

	// Set normalization.
	if (m_ComboNormalization.GetCurSel() == 0)
		filter->SetColemanNormalization(ColemanRuleEvaluator::NORMALIZATION_NONE);
	else if (m_ComboNormalization.GetCurSel() == 1)
		filter->SetColemanNormalization(ColemanRuleEvaluator::NORMALIZATION_C1);
	else if (m_ComboNormalization.GetCurSel() == 2)
		filter->SetColemanNormalization(ColemanRuleEvaluator::NORMALIZATION_C2);

	// Set various.
	filter->SetMichalskiWeight(m_Weight);
	filter->UseLower(m_UseLower);
	filter->UseUpper(m_UseUpper);
	filter->SetLower(m_Lower);
	filter->SetUpper(m_Upper);
	filter->InvertLogic(m_Invert);
	filter->RemoveUndefinedRules(m_RemoveUndefined);
	
	CDialogAlgorithm::OnOK();

}
 
