// dialogapproximationstatistics.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\structuredialogs\dialogapproximationstatistics.h>

#include <kernel\structures\approximation.h>
#include <kernel\structures\equivalenceclasses.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogApproximationStatistics dialog


CDialogApproximationStatistics::CDialogApproximationStatistics(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogApproximationStatistics::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogApproximationStatistics)
	m_CardBn = _T("");
	m_CardLower = _T("");
	m_CardOut = _T("");
	m_CardUniv = _T("");
	m_CardUpper = _T("");
	m_NoEqBn = _T("");
	m_NoEqLower = _T("");
	m_NoEqOut = _T("");
	m_NoEqUniv = _T("");
	m_NoEqUpper = _T("");
	m_RatioBn = _T("");
	m_RatioLower = _T("");
	m_RatioOut = _T("");
	m_RatioUniv = _T("");
	m_RatioUpper = _T("");
	m_Name = _T("");
	m_FractSens = _T("");
	m_RatioSens = _T("");
	m_FractSpec = _T("");
	m_RatioSpec = _T("");
	m_FractAcc = _T("");
	m_RatioAcc = _T("");
	//}}AFX_DATA_INIT
}


void CDialogApproximationStatistics::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogApproximationStatistics)
	DDX_Control(pDX, IDC_COMBO_SET, m_ComboSet);
	DDX_Text(pDX, IDC_STATIC_CARD_BN, m_CardBn);
	DDX_Text(pDX, IDC_STATIC_CARD_LOWER, m_CardLower);
	DDX_Text(pDX, IDC_STATIC_CARD_OUT, m_CardOut);
	DDX_Text(pDX, IDC_STATIC_CARD_UNIV, m_CardUniv);
	DDX_Text(pDX, IDC_STATIC_CARD_UPPER, m_CardUpper);
	DDX_Text(pDX, IDC_STATIC_NO_EQ_BN, m_NoEqBn);
	DDX_Text(pDX, IDC_STATIC_NO_EQ_LOWER, m_NoEqLower);
	DDX_Text(pDX, IDC_STATIC_NO_EQ_OUT, m_NoEqOut);
	DDX_Text(pDX, IDC_STATIC_NO_EQ_UNIV, m_NoEqUniv);
	DDX_Text(pDX, IDC_STATIC_NO_EQ_UPPER, m_NoEqUpper);
	DDX_Text(pDX, IDC_STATIC_RATIO_BN, m_RatioBn);
	DDX_Text(pDX, IDC_STATIC_RATIO_LOWER, m_RatioLower);
	DDX_Text(pDX, IDC_STATIC_RATIO_OUT, m_RatioOut);
	DDX_Text(pDX, IDC_STATIC_RATIO_UNIV, m_RatioUniv);
	DDX_Text(pDX, IDC_STATIC_RATIO_UPPER, m_RatioUpper);
	DDX_Text(pDX, IDC_STATIC_NAME, m_Name);
	DDX_Text(pDX, IDC_STATIC_SENSITIVITY_FRACT, m_FractSens);
	DDX_Text(pDX, IDC_STATIC_SENSITIVITY_RATIO, m_RatioSens);
	DDX_Text(pDX, IDC_STATIC_SPECIFICITY_FRACT, m_FractSpec);
	DDX_Text(pDX, IDC_STATIC_SPECIFICITY_RATIO, m_RatioSpec);
	DDX_Text(pDX, IDC_STATIC_ACCURACY_FRACT, m_FractAcc);
	DDX_Text(pDX, IDC_STATIC_ACCURACY_RATIO, m_RatioAcc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogApproximationStatistics, CDialog)
	//{{AFX_MSG_MAP(CDialogApproximationStatistics)
	ON_CBN_SELCHANGE(IDC_COMBO_SET, OnSelchangeComboSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogApproximationStatistics message handlers

//-------------------------------------------------------------------
// Method........: OnSelchangeComboSet.
// Author........: 
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogApproximationStatistics::OnSelchangeComboSet() 
{

	CWaitCursor wait;

	// Get equivalence classes.
	Handle<EquivalenceClasses> universe = const_cast(EquivalenceClasses *, GetApproximation()->GetUniverse());
	Handle<EquivalenceClasses> upper    = const_cast(EquivalenceClasses *, GetApproximation()->GetUpperApproximation());
	Handle<EquivalenceClasses> lower    = const_cast(EquivalenceClasses *, GetApproximation()->GetLowerApproximation());
	Handle<EquivalenceClasses> boundary = const_cast(EquivalenceClasses *, GetApproximation()->GetBoundaryRegion());
	Handle<EquivalenceClasses> outside  = const_cast(EquivalenceClasses *, GetApproximation()->GetOutsideRegion());

	// Get current selection.
	int cur_sel = m_ComboSet.GetCurSel();

	Handle<EquivalenceClasses> selected;

	switch (cur_sel) {
		case 0:  selected = universe; break;
		case 1:  selected = upper;    break;
		case 2:  selected = lower;    break;
		case 3:  selected = boundary; break;
		case 4:  selected = outside;  break;
		default: selected = NULL;     break;
	}

	if (selected == NULL) {
		Message::Error("No counter selected.");
		m_RatioUniv  = Undefined::String().GetBuffer();
		m_RatioUpper = Undefined::String().GetBuffer();
		m_RatioLower = Undefined::String().GetBuffer();
		m_RatioBn    = Undefined::String().GetBuffer();
		m_RatioOut   = Undefined::String().GetBuffer();
		UpdateData(FALSE);
		return;
	}

	// Calculate ratios.
	float ratio_univ  = selected->GetRatio(*universe);
	float ratio_upper = selected->GetRatio(*upper);
	float ratio_lower = selected->GetRatio(*lower);
	float ratio_bn    = selected->GetRatio(*boundary);
	float ratio_out   = selected->GetRatio(*outside);

	if (ratio_univ  == Undefined::Float())
		m_RatioUniv   =  Undefined::String().GetBuffer();
	else
		m_RatioUniv   =  String::Format(ratio_univ).GetBuffer();

	if (ratio_upper == Undefined::Float())
		m_RatioUpper  =  Undefined::String().GetBuffer();
	else
		m_RatioUpper   = String::Format(ratio_upper).GetBuffer();

	if (ratio_lower == Undefined::Float())
		m_RatioLower  =  Undefined::String().GetBuffer();
	else
		m_RatioLower  =  String::Format(ratio_lower).GetBuffer();

	if (ratio_bn    == Undefined::Float())
		m_RatioBn     =  Undefined::String().GetBuffer();
	else
		m_RatioBn     =  String::Format(ratio_bn).GetBuffer();

	if (ratio_out   == Undefined::Float())
		m_RatioOut    =  Undefined::String().GetBuffer();
	else
		m_RatioOut    =  String::Format(ratio_out).GetBuffer();

	UpdateData(FALSE);
	
}

//-------------------------------------------------------------------
// Method........: OnInitDialog.
// Author........: 
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

BOOL CDialogApproximationStatistics::OnInitDialog() 
{

	CDialog::OnInitDialog();
	
	if (GetApproximation() == NULL) {
		Message::Error("No approximation structure set.");
		EndDialog(IDABORT);
		return FALSE;
	}

	CWaitCursor wait;

	// Set the name.
	m_Name = (GetApproximation()->GetName()).GetBuffer();

	// Fill combo box list.  Note the order (important elsewhere)!
	m_ComboSet.AddString("Universe");
	m_ComboSet.AddString("Upper approximation");
	m_ComboSet.AddString("Lower approximation");
	m_ComboSet.AddString("Boundary region");
	m_ComboSet.AddString("Outside region");

	m_ComboSet.SetCurSel(0);

	// Get equivalence classes.
	Handle<EquivalenceClasses> universe = const_cast(EquivalenceClasses *, GetApproximation()->GetUniverse());
	Handle<EquivalenceClasses> upper    = const_cast(EquivalenceClasses *, GetApproximation()->GetUpperApproximation());
	Handle<EquivalenceClasses> lower    = const_cast(EquivalenceClasses *, GetApproximation()->GetLowerApproximation());
	Handle<EquivalenceClasses> boundary = const_cast(EquivalenceClasses *, GetApproximation()->GetBoundaryRegion());
	Handle<EquivalenceClasses> outside  = const_cast(EquivalenceClasses *, GetApproximation()->GetOutsideRegion());

	if (universe == NULL ||
		  upper    == NULL ||
		  lower    == NULL ||
		  boundary == NULL ||
		  outside  == NULL) {
		Message::Error("One or more sets are NULL.");
		EndDialog(IDABORT);
		return FALSE;
	}

	// Get number of equivalence classes and cardinalities.
	int no_eq_univ  = universe->GetNoEquivalenceClasses();
	int no_eq_upper = upper->GetNoEquivalenceClasses();
	int no_eq_lower = lower->GetNoEquivalenceClasses();
	int no_eq_bn    = boundary->GetNoEquivalenceClasses();
	int no_eq_out   = outside->GetNoEquivalenceClasses();

	int card_univ   = universe->GetNoObjects();
	int card_upper  = upper->GetNoObjects();
	int card_lower  = lower->GetNoObjects();
	int card_bn     = boundary->GetNoObjects();
	int card_out    = outside->GetNoObjects();

	m_NoEqUniv      = String::Format(no_eq_univ).GetBuffer();
	m_NoEqUpper     = String::Format(no_eq_upper).GetBuffer();
	m_NoEqLower     = String::Format(no_eq_lower).GetBuffer();
	m_NoEqBn        = String::Format(no_eq_bn).GetBuffer();
	m_NoEqOut       = String::Format(no_eq_out).GetBuffer();

	m_CardUniv      = String::Format(card_univ).GetBuffer();
	m_CardUpper     = String::Format(card_upper).GetBuffer();
	m_CardLower     = String::Format(card_lower).GetBuffer();
	m_CardBn        = String::Format(card_bn).GetBuffer();
	m_CardOut       = String::Format(card_out).GetBuffer();

	// Calculate ratios.
	OnSelchangeComboSet();

	// Get performance measures.
	int no_sens_correct = 0;
	int no_sens_total   = 0;
	int no_spec_correct = 0;
	int no_spec_total   = 0;
	int no_acc_correct  = 0;
	int no_acc_total    = 0;

	float sens = GetApproximation()->GetSensitivity(&no_sens_correct, &no_sens_total);
	float spec = GetApproximation()->GetSpecificity(&no_spec_correct, &no_spec_total);
	float acc  = GetApproximation()->GetAccuracy(&no_acc_correct,     &no_acc_total);

	if (sens == Undefined::Float()) {
		m_RatioSens = Undefined::String().GetBuffer();
		m_FractSens = Undefined::String().GetBuffer();
	}
	else {
		m_RatioSens = String::Format(sens).GetBuffer();
		m_FractSens = (String::Format(no_sens_correct) + " out of " + String::Format(no_sens_total)).GetBuffer();
	}

	if (spec == Undefined::Float()) {
		m_RatioSpec = Undefined::String().GetBuffer();
		m_FractSpec = Undefined::String().GetBuffer();
	}
	else {
		m_RatioSpec = String::Format(spec).GetBuffer();
		m_FractSpec = (String::Format(no_spec_correct) + " out of " + String::Format(no_spec_total)).GetBuffer();
	}

	if (acc == Undefined::Float()) {
		m_RatioAcc  = Undefined::String().GetBuffer();
		m_FractAcc  = Undefined::String().GetBuffer();
	}
	else {
		m_RatioAcc  = String::Format(acc).GetBuffer();
		m_FractAcc  = (String::Format(no_acc_correct) + " out of " + String::Format(no_acc_total)).GetBuffer();
	}

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
 
