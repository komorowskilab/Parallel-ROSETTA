// dialogrulepruner.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogrulepruner.h>

#include <kernel\algorithms\myrulefilter.h>

#include <kernel\structures\rule.h>
#include <kernel\structures\rules.h>
#include <kernel\structures\decisiontable.h>

#include <kernel\utilities\mathkit.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogRulePruner dialog


CDialogRulePruner::CDialogRulePruner(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogRulePruner::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogRulePruner)
	m_InvertLogic = FALSE;
	m_ProbLower = 0.0f;
	m_ProbUpper = 0.0f;
	m_SuppLower = 0;
	m_SuppUpper = 0;
	m_LengthLower = 0;
	m_LengthUpper = 0;
	m_StabLower = 0.0;
	m_StabUpper = 0.0;
	m_CovLower = 0.0f;
	m_CovUpper = 0.0f;
	//}}AFX_DATA_INIT

	m_Masked = TRUE;

}


void CDialogRulePruner::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogRulePruner)
	DDX_Control(pDX, IDC_EDIT_COV_UPPER, m_EditCovUpper);
	DDX_Control(pDX, IDC_EDIT_COV_LOWER, m_EditCovLower);
	DDX_Control(pDX, IDC_CHECK_COVERAGE, m_CheckCoverage);
	DDX_Control(pDX, IDC_EDIT_STAB_UPPER, m_EditStabUpper);
	DDX_Control(pDX, IDC_EDIT_STAB_LOWER, m_EditStabLower);
	DDX_Control(pDX, IDC_CHECK_STABILITY, m_CheckStab);
	DDX_Control(pDX, IDC_COMBO_VALUE, m_ComboValue);
	DDX_Control(pDX, IDC_COMBO_ATTRIBUTE, m_ComboAttribute);
	DDX_Control(pDX, IDC_CHECK_VALUE, m_CheckValue);
	DDX_Control(pDX, IDC_CHECK_DOMINATING, m_CheckDominating);
	DDX_Control(pDX, IDC_EDIT_LENGTH_UPPER, m_EditLengthUpper);
	DDX_Control(pDX, IDC_EDIT_LENGTH_LOWER, m_EditLengthLower);
	DDX_Control(pDX, IDC_CHECK_LENGTH, m_CheckLength);
	DDX_Control(pDX, IDC_EDIT_SUPPORT_UPPER, m_EditSuppUpper);
	DDX_Control(pDX, IDC_EDIT_SUPPORT_LOWER, m_EditSuppLower);
	DDX_Control(pDX, IDC_EDIT_PROB_UPPER, m_EditProbUpper);
	DDX_Control(pDX, IDC_EDIT_PROB_LOWER, m_EditProbLower);
	DDX_Control(pDX, IDC_CHECK_SUPPORT, m_CheckSupp);
	DDX_Control(pDX, IDC_CHECK_PROBABILITY, m_CheckProb);
	DDX_Control(pDX, IDC_CHECK_DECISION, m_CheckDec);
	DDX_Control(pDX, IDC_COMBO_DECISION, m_ComboDecision);
	DDX_Check(pDX, IDC_CHECK_INVERTLOGIC, m_InvertLogic);
	DDX_Control(pDX, IDC_RADIO_AND, m_RadioAnd);
	DDX_Control(pDX, IDC_RADIO_OR, m_RadioOr);
	DDX_Text(pDX, IDC_EDIT_PROB_LOWER, m_ProbLower);
	DDV_MinMaxFloat(pDX, m_ProbLower, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_PROB_UPPER, m_ProbUpper);
	DDV_MinMaxFloat(pDX, m_ProbUpper, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_SUPPORT_LOWER, m_SuppLower);
	DDX_Text(pDX, IDC_EDIT_SUPPORT_UPPER, m_SuppUpper);
	DDX_Text(pDX, IDC_EDIT_LENGTH_LOWER, m_LengthLower);
	DDX_Text(pDX, IDC_EDIT_LENGTH_UPPER, m_LengthUpper);
	DDX_Text(pDX, IDC_EDIT_STAB_LOWER, m_StabLower);
	DDV_MinMaxDouble(pDX, m_StabLower, 0., 1.);
	DDX_Text(pDX, IDC_EDIT_STAB_UPPER, m_StabUpper);
	DDV_MinMaxDouble(pDX, m_StabUpper, 0., 1.);
	DDX_Text(pDX, IDC_EDIT_COV_LOWER, m_CovLower);
	DDV_MinMaxFloat(pDX, m_CovLower, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_COV_UPPER, m_CovUpper);
	DDV_MinMaxFloat(pDX, m_CovUpper, 0.f, 1.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogRulePruner, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogRulePruner)
	ON_BN_CLICKED(IDC_CHECK_DECISION, OnCheckDecision)
	ON_BN_CLICKED(IDC_CHECK_PROBABILITY, OnCheckAccuracy)
	ON_BN_CLICKED(IDC_CHECK_SUPPORT, OnCheckSupport)
	ON_BN_CLICKED(IDC_CHECK_LENGTH, OnCheckLength)
	ON_BN_CLICKED(IDC_CHECK_VALUE, OnCheckValue)
	ON_CBN_SELCHANGE(IDC_COMBO_ATTRIBUTE, OnSelchangeComboAttribute)
	ON_BN_CLICKED(IDC_CHECK_STABILITY, OnCheckStability)
	ON_BN_CLICKED(IDC_CHECK_COVERAGE, OnCheckCoverage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogRulePruner message handlers

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

BOOL CDialogRulePruner::OnInitDialog() 
{
	CDialogAlgorithm::OnInitDialog();
	
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(MYRULEFILTER)) {
		Message::Error("Algorithm not set or of incorrect type.");
		EndDialog(IDABORT);
		return FALSE;
	}

	if (m_Structure.IsNULL() || !m_Structure->IsA(RULES)) {
		Message::Error("Structure not set or of incorrect type.");
		EndDialog(IDABORT);
		return FALSE;
	}

	// Get algorithm to apply.
	Handle<MyRuleFilter> pruner = dynamic_cast(MyRuleFilter *, m_Algorithm.GetPointer());

	// Get the rule set to prune.
	Handle<Rules> rules = dynamic_cast(Rules *, m_Structure.GetPointer());

	if (rules->GetNoRules() == 0) {
		Message::Error("The rule set is empty.");
		EndDialog(IDABORT);
		return FALSE;
	}

	Handle<Rule> rule = rules->GetRule(0);

	// Determine decision attribute.  Assume it's the same for all rules.
	int decision_attribute = rule->GetDecisionAttribute();

	// Does a decision attribute exist?
	bool has_decision_attribute = (decision_attribute != Undefined::Integer());

	if (!has_decision_attribute) {

		Message::Warning("The first rule has no decision attribute set.");

		// Unset and disable all RHS options.
		pruner->SetFilteringStrategy(pruner->GetFilteringStrategy() & ~MyRuleFilter::FILTERING_RHS_SUPPORT);
		pruner->SetFilteringStrategy(pruner->GetFilteringStrategy() & ~MyRuleFilter::FILTERING_RHS_ACCURACY);
		pruner->SetFilteringStrategy(pruner->GetFilteringStrategy() & ~MyRuleFilter::FILTERING_RHS_COVERAGE);
		pruner->SetFilteringStrategy(pruner->GetFilteringStrategy() & ~MyRuleFilter::FILTERING_RHS_STABILITY);
		pruner->SetFilteringStrategy(pruner->GetFilteringStrategy() & ~MyRuleFilter::FILTERING_DECISION);

		m_CheckSupp.EnableWindow(FALSE);
		m_CheckProb.EnableWindow(FALSE);
		m_CheckCoverage.EnableWindow(FALSE);
		m_CheckStab.EnableWindow(FALSE);
		m_CheckDec.EnableWindow(FALSE);

	}

	// Get the originating decision table.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, rules->FindParent(DECISIONTABLE, *m_Project));

	if (table == NULL) {
		Message::Error("Cannot trace back to originating decision table.");
		EndDialog(IDABORT);
		return FALSE;
	}

	Vector(int) decisions;
	Vector(int) frequencies; // Dummy, not in use.

	// Extract all possible decision values.
	if (has_decision_attribute) {

		table->GetValueSet(decisions, frequencies, decision_attribute, m_Masked);

		int initial_selection = 0;
		int i;

		// Build decision combo box options.
		for (i = 0; i < decisions.size(); i++) {
			if (decisions[i] == pruner->GetDecision())
				initial_selection = i;
			String name = table->GetDictionaryEntry(decision_attribute, decisions[i], m_Masked);
			m_ComboDecision.AddString(name.GetBuffer());
		}

		// Set default initial selection.
		m_ComboDecision.SetCurSel(initial_selection);

	}

	int initial_selection = 0;
	int i;

	// Build (condition) attribute combo box options.
	for (i = 0; i < table->GetNoAttributes(m_Masked); i++) {
		if (!table->IsCondition(i, m_Masked))
			continue;
		if (i == pruner->GetAttribute())
			initial_selection = i;
    String name = table->GetAttributeName(i, m_Masked);
		m_ComboAttribute.AddString(name.GetBuffer());
	}

	// Set default initial selection.
	m_ComboAttribute.SetCurSel(initial_selection);

	Vector(int) values;

	// Extract all possible values for condition attribute.
	table->GetValueSet(values, frequencies, initial_selection, m_Masked);

	int initial_value_selection = 0;

	// Build (condition) value combo box options.
	for (i = 0; i < values.size(); i++) {
		if (values[i] == pruner->GetAttributeValue())
			initial_value_selection = i;
    String name = table->GetDictionaryEntry(initial_selection, values[i], m_Masked);
		m_ComboValue.AddString(name.GetBuffer());
	}

	// Set default initial selection.
	m_ComboValue.SetCurSel(initial_value_selection);

	// Set connective stuff.
	m_RadioAnd.SetCheck(pruner->GetFilteringConnective() == MyRuleFilter::CONNECTIVE_AND);
	m_RadioOr.SetCheck(pruner->GetFilteringConnective() == MyRuleFilter::CONNECTIVE_OR);

	// Set default check button states.
	m_CheckSupp.SetCheck(pruner->GetFilteringStrategy() & MyRuleFilter::FILTERING_RHS_SUPPORT);
	m_CheckProb.SetCheck(pruner->GetFilteringStrategy() & MyRuleFilter::FILTERING_RHS_ACCURACY);
	m_CheckCoverage.SetCheck(pruner->GetFilteringStrategy() & MyRuleFilter::FILTERING_RHS_COVERAGE);
	m_CheckStab.SetCheck(pruner->GetFilteringStrategy() & MyRuleFilter::FILTERING_RHS_STABILITY);
	m_CheckDec.SetCheck(pruner->GetFilteringStrategy() & MyRuleFilter::FILTERING_DECISION);
	m_CheckDominating.SetCheck(pruner->IsDominatingDecision());
	m_CheckLength.SetCheck(pruner->GetFilteringStrategy() & MyRuleFilter::FILTERING_LHS_LENGTH);
	m_CheckValue.SetCheck(pruner->GetFilteringStrategy() & MyRuleFilter::FILTERING_CONDITION);

	// Enable fields.
	OnCheckSupport();
	OnCheckAccuracy();
	OnCheckCoverage();
	OnCheckStability();
	OnCheckDecision();
	OnCheckLength();
	OnCheckValue();

	// Set default invert logic state.
	m_InvertLogic = pruner->InvertLogic();

	// Set default supports.
	m_SuppLower = pruner->GetLowerRHSSupportThreshold();
	m_SuppUpper = pruner->GetUpperRHSSupportThreshold();

	// Set default probabilities.
	m_ProbLower = pruner->GetLowerRHSAccuracyThreshold();
	m_ProbUpper = pruner->GetUpperRHSAccuracyThreshold();

	// Set default coverages.
	m_CovLower = pruner->GetLowerRHSCoverageThreshold();
	m_CovUpper = pruner->GetUpperRHSCoverageThreshold();

	// Set default stabilities.
	m_StabLower = pruner->GetLowerRHSStabilityThreshold();
	m_StabUpper = pruner->GetUpperRHSStabilityThreshold();

	// Set default lengths.
	m_LengthLower = pruner->GetLowerLHSLengthThreshold();
	m_LengthUpper = pruner->GetUpperLHSLengthThreshold();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Assumes all safety checks done in OnInitDialog.
// Revisions.....:
//===================================================================

void CDialogRulePruner::OnOK() 
{

	UpdateData(TRUE);

	// Get algorithm/structure.
	Handle<MyRuleFilter> pruner = dynamic_cast(MyRuleFilter *, m_Algorithm.GetPointer());
	Handle<Rules>        rules  = dynamic_cast(Rules *,        m_Structure.GetPointer());

	// Get originating table.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, rules->FindParent(DECISIONTABLE, *m_Project));

	int strategy = 0;

	// Set pruning strategy.
	if (m_CheckSupp.GetCheck())
		strategy |= MyRuleFilter::FILTERING_RHS_SUPPORT;
	if (m_CheckProb.GetCheck())
		strategy |= MyRuleFilter::FILTERING_RHS_ACCURACY;
	if (m_CheckCoverage.GetCheck())
		strategy |= MyRuleFilter::FILTERING_RHS_COVERAGE;
	if (m_CheckStab.GetCheck())
		strategy |= MyRuleFilter::FILTERING_RHS_STABILITY;
	if (m_CheckDec.GetCheck())
		strategy |= MyRuleFilter::FILTERING_DECISION;
	if (m_CheckLength.GetCheck())
		strategy |= MyRuleFilter::FILTERING_LHS_LENGTH;
	if (m_CheckValue.GetCheck())
		strategy |= MyRuleFilter::FILTERING_CONDITION;

	pruner->SetFilteringStrategy(strategy);

	// Set domination flag.
	pruner->IsDominatingDecision(m_CheckDominating.GetCheck());

	// Set thresholds.
	pruner->SetLowerRHSSupportThreshold(m_SuppLower);
	pruner->SetUpperRHSSupportThreshold(m_SuppUpper);
	pruner->SetLowerRHSAccuracyThreshold(m_ProbLower);
	pruner->SetUpperRHSAccuracyThreshold(m_ProbUpper);
	pruner->SetLowerRHSCoverageThreshold(m_CovLower);
	pruner->SetUpperRHSCoverageThreshold(m_CovUpper);
	pruner->SetLowerRHSStabilityThreshold(m_StabLower);
	pruner->SetUpperRHSStabilityThreshold(m_StabUpper);
	pruner->SetLowerLHSLengthThreshold(m_LengthLower);
	pruner->SetUpperLHSLengthThreshold(m_LengthUpper);

	// Set invert logic flag.
	pruner->InvertLogic(m_InvertLogic);

	// Set connective.
	if (m_RadioAnd.GetCheck())
		pruner->SetFilteringConnective(MyRuleFilter::CONNECTIVE_AND);
	else if (m_RadioOr.GetCheck())
		pruner->SetFilteringConnective(MyRuleFilter::CONNECTIVE_OR);
	else
		Message::Warning("Filtering connective not set.");

	// Set decision value.
	CString decision_text;
	m_ComboDecision.GetWindowText(decision_text);
	int decision_attr  = rules->GetRule(0)->GetDecisionAttribute();
	if (decision_attr != Undefined::Integer()) {
		int decision_value = table->GetDictionaryEntry(decision_attr, decision_text.GetBuffer(0), m_Masked);
		pruner->SetDecision(decision_value);
	}

	// Set attribute.
	CString attribute_text;
	m_ComboAttribute.GetWindowText(attribute_text);
	int attribute = table->GetAttributeIndex(attribute_text.GetBuffer(0), true, m_Masked);

	pruner->SetAttribute(attribute);

	// Set attribute value
	CString value_text;
	m_ComboValue.GetWindowText(value_text);
	int value = table->GetDictionaryEntry(attribute, value_text.GetBuffer(0), m_Masked);

	pruner->SetAttributeValue(value);
	
	CDialogAlgorithm::OnOK();

}

void CDialogRulePruner::OnCheckDecision() 
{

	m_ComboDecision.EnableWindow(m_CheckDec.GetCheck());
	m_CheckDominating.EnableWindow(m_CheckDec.GetCheck());
	
}

void CDialogRulePruner::OnCheckAccuracy() 
{

	m_EditProbLower.EnableWindow(m_CheckProb.GetCheck());
	m_EditProbUpper.EnableWindow(m_CheckProb.GetCheck());

}

void CDialogRulePruner::OnCheckCoverage() 
{

	m_EditCovLower.EnableWindow(m_CheckCoverage.GetCheck());
	m_EditCovUpper.EnableWindow(m_CheckCoverage.GetCheck());
	
}

void CDialogRulePruner::OnCheckStability() 
{

	m_EditStabLower.EnableWindow(m_CheckStab.GetCheck());
	m_EditStabUpper.EnableWindow(m_CheckStab.GetCheck());

}

void CDialogRulePruner::OnCheckSupport() 
{

	m_EditSuppLower.EnableWindow(m_CheckSupp.GetCheck());
	m_EditSuppUpper.EnableWindow(m_CheckSupp.GetCheck());
	
}

void CDialogRulePruner::OnCheckLength() 
{

	m_EditLengthLower.EnableWindow(m_CheckLength.GetCheck());
	m_EditLengthUpper.EnableWindow(m_CheckLength.GetCheck());
	
}

void CDialogRulePruner::OnCheckValue() 
{

	m_ComboAttribute.EnableWindow(m_CheckValue.GetCheck());
	m_ComboValue.EnableWindow(m_CheckValue.GetCheck());
	
}


void CDialogRulePruner::OnSelchangeComboAttribute() 
{

	UpdateData(TRUE);

	// Get algorithm/structure.
	Handle<MyRuleFilter> pruner = dynamic_cast(MyRuleFilter *, m_Algorithm.GetPointer());
	Handle<Rules>        rules  = dynamic_cast(Rules *,        m_Structure.GetPointer());

	// Get originating table.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, rules->FindParent(DECISIONTABLE, *m_Project));

	// Get attribute index.
	CString attribute_text;
	m_ComboAttribute.GetWindowText(attribute_text);
	int attribute = table->GetAttributeIndex(attribute_text.GetBuffer(0), true, m_Masked);

	// Clear current contents of value combo box.
	m_ComboValue.ResetContent();

	Vector(int) values;
	Vector(int) frequencies; // Dummy, not in use.

	// Extract all possible values for selected condition attribute.
	table->GetValueSet(values, frequencies, attribute, m_Masked);

	int initial_selection = 0;
	int i;

	// Build (condition) value combo box options.
	for (i = 0; i < values.size(); i++) {
		if (values[i] == pruner->GetAttributeValue())
			initial_selection = i;
    String name = table->GetDictionaryEntry(attribute, values[i], m_Masked);
		m_ComboValue.AddString(name.GetBuffer());
	}

	// Set default initial selection.
	m_ComboValue.SetCurSel(initial_selection);
	
	UpdateData(FALSE);

}
 
