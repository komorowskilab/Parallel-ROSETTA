// dialogrulesstatistics.cpp : implementation file
//
//Author Ewa Makosa
#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\structuredialogs\dialoggeneralrulesstatistics.h>
#include <frontend\dialogs\structuredialogs\dialogrulesstatistics.h>

#include <kernel\structures\rules.h>

#include <kernel\utilities\mathkit.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogRulesStatistics dialog


CDialogGeneralRulesStatistics::CDialogGeneralRulesStatistics(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogRulesStatistics::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogRulesStatistics)
	m_Size = _T("");
	m_Name = _T("");
	m_MeanStrength = _T("");
	m_StdDevStrength = _T("");
	m_MedianStrength = _T("");
	m_MinimumStrength = _T("");
	m_MaximumStrength = _T("");
	m_NoDeterministic = _T("");
	m_MaximumLength = _T("");
	m_MeanLength = _T("");
	m_MedianLength = _T("");
	m_MinimumLength = _T("");
	m_StdDevLength = _T("");
	m_NoTotalLHSDescriptors = _T("");
	m_NoUniqueLHSDescriptors = _T("");
	//}}AFX_DATA_INIT

}


void CDialogGeneralRulesStatistics::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogRulesStatistics)
	DDX_Control(pDX, IDC_LIST_DESCRIPTOR_STATS, m_DescriptorStats);
	DDX_Text(pDX, IDC_STATIC_SIZE, m_Size);
	DDX_Text(pDX, IDC_STATIC_NAME, m_Name);
	DDX_Text(pDX, IDC_STATIC_MEAN_STRENGTH, m_MeanStrength);
	DDX_Text(pDX, IDC_STATIC_STDDEV_STRENGTH, m_StdDevStrength);
	DDX_Text(pDX, IDC_STATIC_MEDIAN_STRENGTH, m_MedianStrength);
	DDX_Text(pDX, IDC_STATIC_MINIMUM_STRENGTH, m_MinimumStrength);
	DDX_Text(pDX, IDC_STATIC_MAXIMUM_STRENGTH, m_MaximumStrength);
	DDX_Text(pDX, IDC_STATIC_NO_DETERMINISTIC, m_NoDeterministic);
	DDX_Text(pDX, IDC_STATIC_MAXIMUM_LENGTH, m_MaximumLength);
	DDX_Text(pDX, IDC_STATIC_MEAN_LENGTH, m_MeanLength);
	DDX_Text(pDX, IDC_STATIC_MEDIAN_LENGTH, m_MedianLength);
	DDX_Text(pDX, IDC_STATIC_MINIMUM_LENGTH, m_MinimumLength);
	DDX_Text(pDX, IDC_STATIC_STDDEV_LENGTH, m_StdDevLength);
	DDX_Text(pDX, IDC_STATIC_TOTALLHSDESCRIPTORS, m_NoTotalLHSDescriptors);
	DDX_Text(pDX, IDC_STATIC_UNIQUELHSDESCRIPTORS, m_NoUniqueLHSDescriptors);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogGeneralRulesStatistics, CDialog)
	//{{AFX_MSG_MAP(CDialogRulesStatistics)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogRulesStatistics message handlers

//-------------------------------------------------------------------
// Method  ......: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....: 
//===================================================================

BOOL CDialogGeneralRulesStatistics::OnInitDialog() 
{

  CDialog::OnInitDialog();

	// Any rules present?
	if (m_Rules.IsNULL()) {
		Message::Error("No rules present.");
		UpdateData(FALSE);
		return TRUE;
	}

	// Set size and name.
	m_Size = String::Format(m_Rules->GetNoRules()).GetBuffer();
	m_Name = m_Rules->GetName().GetBuffer();

	// Insert list ctrl columns.
	CRect rect;
	m_DescriptorStats.GetClientRect(rect);
	int width1 = 2 * rect.Width() / 7;
	int width2 = rect.Width() / 7;
	int width3 = rect.Width() / 7;
	int width4 = rect.Width() / 7;
	int width5 = rect.Width() / 7;
	int width6 = rect.Width() - width1 - width2 - width3 - width4 - width5;

	m_DescriptorStats.InsertColumn(0, "Descriptor",    LVCFMT_LEFT, width1, 0);
	m_DescriptorStats.InsertColumn(1, "Count",         LVCFMT_LEFT, width2, 1);
	m_DescriptorStats.InsertColumn(2, "%",             LVCFMT_LEFT, width3, 2);
	m_DescriptorStats.InsertColumn(3, "Min. support",  LVCFMT_LEFT, width4, 3);
	m_DescriptorStats.InsertColumn(4, "Mean support",  LVCFMT_LEFT, width5, 4);
	m_DescriptorStats.InsertColumn(5, "Max. support",  LVCFMT_LEFT, width6, 5);

	// Calculate statistics and update data.
	CalculateStatistics();

  // Return TRUE unless you set the focus to a control.  (Exception: OCX property
	// pages should return FALSE).
	return TRUE;  

}

//-------------------------------------------------------------------
// Method  ......: CalculateStatistics
// Author........: Aleksander šhrn
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogGeneralRulesStatistics::CalculateStatistics()
{

	// Any rules present?
	if (m_Rules.IsNULL())
		return;

	// Display a wait cursor.
	CWaitCursor wait;

	int no_rules         = m_Rules->GetNoRules();
	int no_deterministic = 0;

	Vector(int) supports;
	Vector(int) lengths;

	supports.reserve(no_rules * 1.2);
	lengths.reserve(no_rules);

	int i, j;

	// Extract vector.
	for (i = 0; i < no_rules; i++) {
		Handle<Rule> rule = m_Rules->GetRule(i);
		if (rule->IsDeterministic())
			no_deterministic++;
		int no_decision_values = rule->GetNoDecisionValues();
		for (j = 0; j < no_decision_values; j++) {
			int support = rule->GetSupport(j);
			supports.push_back(support);
		}
		lengths.push_back(rule->GetNoConditionDescriptors());
	}

	// Calculate statistics
	float mean_strength     = MathKit::Mean(supports);
	float stddev_strength   = MathKit::StandardDeviation(supports);
	float median_strength   = MathKit::Median(supports);
	int   minimum_strength  = MathKit::Minimum(supports);
	int   maximum_strength  = MathKit::Maximum(supports);

	float mean_length     = MathKit::Mean(lengths);
	float stddev_length   = MathKit::StandardDeviation(lengths);
	float median_length   = MathKit::Median(lengths);
	int   minimum_length  = MathKit::Minimum(lengths);
	int   maximum_length  = MathKit::Maximum(lengths);

	// Move stuff into UI variables.
	m_NoDeterministic = CString(String::Format(no_deterministic).GetBuffer()) + " deterministic";

	// ...Support
  if (mean_strength == Undefined::Float())
		m_MeanStrength = Undefined::String().GetBuffer();
	else
		m_MeanStrength = String::Format(mean_strength).GetBuffer();

  if (stddev_strength == Undefined::Float())
		m_StdDevStrength = Undefined::String().GetBuffer();
	else
		m_StdDevStrength = String::Format(stddev_strength).GetBuffer();

  if (median_strength == Undefined::Float())
		m_MedianStrength = Undefined::String().GetBuffer();
	else
		m_MedianStrength = String::Format(median_strength).GetBuffer();

  if (minimum_strength == Undefined::Integer())
		m_MinimumStrength = Undefined::String().GetBuffer();
	else
		m_MinimumStrength = String::Format(minimum_strength).GetBuffer();

  if (maximum_strength == Undefined::Integer())
		m_MaximumStrength = Undefined::String().GetBuffer();
	else
		m_MaximumStrength = String::Format(maximum_strength).GetBuffer();

	// ...Length
  if (mean_length == Undefined::Float())
		m_MeanLength = Undefined::String().GetBuffer();
	else
		m_MeanLength = String::Format(mean_length).GetBuffer();

  if (stddev_length == Undefined::Float())
		m_StdDevLength = Undefined::String().GetBuffer();
	else
		m_StdDevLength = String::Format(stddev_length).GetBuffer();

  if (median_length == Undefined::Float())
		m_MedianLength = Undefined::String().GetBuffer();
	else
		m_MedianLength = String::Format(median_length).GetBuffer();

  if (minimum_length == Undefined::Integer())
		m_MinimumLength = Undefined::String().GetBuffer();
	else
		m_MinimumLength = String::Format(minimum_length).GetBuffer();

  if (maximum_length == Undefined::Integer())
		m_MaximumLength = Undefined::String().GetBuffer();
	else
		m_MaximumLength = String::Format(maximum_length).GetBuffer();

	// Clear list ctrl..
	m_DescriptorStats.DeleteAllItems();

	bool         rhs = false;
	GeneralRules::DVMap descriptors;

	// Get supports per descriptor.
	if (!m_Rules->GetDescriptorSupports(rhs, descriptors))
		return;

	int no_unique_descriptors = descriptors.size();
	int no_total_descriptors  = 0;

	// Count descriptors and compute support sums.
	GeneralRules::DVMap::iterator iterator = descriptors.begin();

	while (!(iterator == descriptors.end())) {
		no_total_descriptors += (*iterator).second.size();
		iterator++;
	}

	m_NoTotalLHSDescriptors  = String::Format(no_total_descriptors).GetBuffer();
	m_NoUniqueLHSDescriptors = String::Format(no_unique_descriptors).GetBuffer();

	// Get originating table.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, m_Rules->FindParent(DECISIONTABLE));

	if (table == NULL)
		Message::Error("Could not get originating table.");

	bool masked = true;

	// Move histogram stuff into UI list controls.
	m_DescriptorStats.LockWindowUpdate();

	LV_ITEM                      insertstruct;
  int                          index;
  String                       label;

	insertstruct.mask = LVIF_TEXT;

  iterator = descriptors.begin();

	while (!(iterator == descriptors.end())) {
		int attribute           = (*iterator).first.first;
		
		Vector(int) &supports   = (*iterator).second;
		String attribute_name   = (table == NULL) ? String::Format(attribute) : table->GetAttributeName(attribute, masked);
		GeneralRule::ValueSet values  = (*iterator).first.second;
		GeneralRule::ValueSet::iterator values_it = values.begin();
		int value = *values_it;

		String value_name = (table == NULL) ? String::Format(value)     : table->GetDictionaryEntry(attribute, value, masked);	
		values_it++;

		while (!(values_it == values.end())){
			int value = *values_it;
			value_name += " OR ";
			value_name += (table == NULL) ? String::Format(value)     : table->GetDictionaryEntry(attribute, value, masked);	
			values_it++;
		}
		
		// Insert item.
		label                   = "(" + attribute_name + ", " + value_name + ")";
		insertstruct.pszText    = const_cast(char *, label.GetBuffer());
		insertstruct.iItem      = INT_MAX;
		insertstruct.iSubItem   = 0;
		index                   = m_DescriptorStats.InsertItem(&insertstruct);
		insertstruct.iItem      = index;
		// Insert sub items.
		int count               = supports.size();
		label                   = String::Format(count);
		insertstruct.pszText    = const_cast(char *, label.GetBuffer());
		insertstruct.iSubItem   = 1;
		m_DescriptorStats.SetItem(&insertstruct);
		label                   = String::Format(static_cast(float, 100 * count) / no_rules);
		insertstruct.pszText    = const_cast(char *, label.GetBuffer());
		insertstruct.iSubItem   = 2;
		m_DescriptorStats.SetItem(&insertstruct);
		label                   = String::Format(MathKit::Minimum(supports));
		insertstruct.pszText    = const_cast(char *, label.GetBuffer());
		insertstruct.iSubItem   = 3;
		m_DescriptorStats.SetItem(&insertstruct);
		label                   = String::Format(MathKit::Mean(supports));
		insertstruct.pszText    = const_cast(char *, label.GetBuffer());
		insertstruct.iSubItem   = 4;
		m_DescriptorStats.SetItem(&insertstruct);
		label                   = String::Format(MathKit::Maximum(supports));
		insertstruct.pszText    = const_cast(char *, label.GetBuffer());
		insertstruct.iSubItem   = 5;
		m_DescriptorStats.SetItem(&insertstruct);
		// Increment iterator.
		iterator++;
	} 

	m_DescriptorStats.UnlockWindowUpdate();

	// Update data.
	UpdateData(FALSE);

}

//-------------------------------------------------------------------
// Method  ......: SetRules
// Author........: 
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogGeneralRulesStatistics::SetRules(GeneralRules *rules)
{
	m_Rules = rules;
}


 
