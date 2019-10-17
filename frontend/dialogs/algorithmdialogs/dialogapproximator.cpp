// dialogapproximator.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogapproximator.h>
#include <frontend\dialogs\algorithmdialogs\dialogattributeselector.h>

#include <kernel\structures\decisiontable.h>

#include <kernel\algorithms\approximator.h>

#include <kernel\utilities\mathkit.h>
#include <kernel\utilities\creator.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogApproximator dialog


CDialogApproximator::CDialogApproximator(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogApproximator::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogApproximator)
	m_Precision = 0.0f;
	//}}AFX_DATA_INIT

	m_Masked = TRUE;

}


void CDialogApproximator::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogApproximator)
	DDX_Control(pDX, IDC_COMBO_DECISION, m_ComboDecision);
	DDX_Control(pDX, IDC_BUTTON_PARTITION, m_ButtonAttributes);
	DDX_Text(pDX, IDC_EDIT_PRECISION, m_Precision);
	DDV_MinMaxFloat(pDX, m_Precision, 0.0f, 0.5f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogApproximator, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogApproximator)
	ON_BN_CLICKED(IDC_BUTTON_PARTITION, OnButtonPartition)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogApproximator message handlers

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

void CDialogApproximator::OnOK() 
{

	UpdateData(TRUE);

	// Get algorithm and structure.
	Handle<Approximator>  approximator = dynamic_cast(Approximator *, m_Algorithm.GetPointer());
	Handle<DecisionTable> table        = dynamic_cast(DecisionTable *, m_Structure.GetPointer());

	// Set decision (approximation target).
	CString decision_text;
	m_ComboDecision.GetWindowText(decision_text);
	int decision_attr  = table->GetDecisionAttribute(m_Masked);
	int decision_value = table->GetDictionaryEntry(decision_attr, decision_text.GetBuffer(0), m_Masked);

	if (!approximator->SetDecisionValue(decision_value))
		Message::Error("Error setting the decision (approximation target).");

	// Set precision level.
	if (!approximator->SetPrecision(m_Precision))
		Message::Error("Error setting the precision level.");

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

BOOL CDialogApproximator::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();
	
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(APPROXIMATOR)) {
		Message::Error("Algorithm not set or of incorrect type.");
		EndDialog(IDABORT);
		return FALSE;
	}

	if (m_Structure.IsNULL() || !m_Structure->IsA(DECISIONTABLE)) {
		Message::Error("Structure not set or of incorrect type.");
		EndDialog(IDABORT);
		return FALSE;
	}

	// Get algorithm to apply.
	Handle<Approximator> approximator = dynamic_cast(Approximator *, m_Algorithm.GetPointer());

	// Get the decision table containing the set to approximate.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, m_Structure.GetPointer());

	// Determine decision attribute.
	int decision_attribute = table->GetDecisionAttribute(m_Masked);

	if (decision_attribute == Undefined::Integer()) {
		Message::Error("The decision table has no decision attribute specified.");
		EndDialog(IDABORT);
		return FALSE;
	}

	Vector(int)   decisions;
	Map(int, int) histogram;

	// Extract all possible decision values.
	table->GetEntries(decisions, decision_attribute, m_Masked, true);

	MathKit::Distribution(histogram, decisions);

	Map(int, int)::const_iterator current = histogram.begin();
	Map(int, int)::const_iterator end     = histogram.end();

	int no_iterations     = 0;
	int initial_selection = 0;

	// Build combo box options.
	while (!(current == end)) {
		int decision_value = (*current).first;
		if (decision_value == approximator->GetDecisionValue())
			initial_selection = no_iterations;
    String name = table->GetDictionaryEntry(decision_attribute, decision_value, m_Masked);
		m_ComboDecision.AddString(name.GetBuffer());
		no_iterations++;
		current++;
	}

	// Set default initial selection.
	m_ComboDecision.SetCurSel(initial_selection);

	// Set default precision level.
	m_Precision = approximator->GetPrecision();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
// Method........: OnButtonPartition
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogApproximator::OnButtonPartition() 
{

	CDialogAttributeSelector dialog;

	dialog.m_Structure = m_Structure;
	dialog.m_Project   = m_Project;
	dialog.m_Algorithm = m_Algorithm;

	if (dialog.DoModal() != IDOK)
		return;
	
}
 
