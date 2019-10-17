// dialogreductsstatistics.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\structuredialogs\dialogreductsstatistics.h>

#include <kernel\structures\decisiontable.h>
#include <kernel\structures\dictionary.h>
#include <kernel\structures\reduct.h>
#include <kernel\structures\reducts.h>

#include <kernel\utilities\mathkit.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogReductsStatistics dialog


CDialogReductsStatistics::CDialogReductsStatistics(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogReductsStatistics::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogReductsStatistics)
	m_Size = _T("");
	m_Name = _T("");
	m_MeanLength = _T("");
	m_StdDevLength = _T("");
	m_MedianLength = _T("");
	m_MinimumLength = _T("");
	m_MaximumLength = _T("");
	m_MeanStrength = _T("");
	m_StdDevStrength = _T("");
	m_MedianStrength = _T("");
	m_MinimumStrength = _T("");
	m_MaximumStrength = _T("");
	//}}AFX_DATA_INIT

}


void CDialogReductsStatistics::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogReductsStatistics)
	DDX_Control(pDX, IDC_HISTOGRAM_ATTRIBUTE, m_HistogramAttribute);
	DDX_Control(pDX, IDC_HISTOGRAM_LENGTH, m_HistogramLength);
	DDX_Control(pDX, IDC_HISTOGRAM_STRENGTH, m_HistogramStrength);
	DDX_Text(pDX, IDC_STATIC_SIZE, m_Size);
	DDX_Text(pDX, IDC_STATIC_NAME, m_Name);
	DDX_Text(pDX, IDC_STATIC_MEAN_LENGTH, m_MeanLength);
	DDX_Text(pDX, IDC_STATIC_STDDEV_LENGTH, m_StdDevLength);
	DDX_Text(pDX, IDC_STATIC_MEDIAN_LENGTH, m_MedianLength);
	DDX_Text(pDX, IDC_STATIC_MINIMUM_LENGTH, m_MinimumLength);
	DDX_Text(pDX, IDC_STATIC_MAXIMUM_LENGTH, m_MaximumLength);
	DDX_Text(pDX, IDC_STATIC_MEAN_STRENGTH, m_MeanStrength);
	DDX_Text(pDX, IDC_STATIC_STDDEV_STRENGTH, m_StdDevStrength);
	DDX_Text(pDX, IDC_STATIC_MEDIAN_STRENGTH, m_MedianStrength);
	DDX_Text(pDX, IDC_STATIC_MINIMUM_STRENGTH, m_MinimumStrength);
	DDX_Text(pDX, IDC_STATIC_MAXIMUM_STRENGTH, m_MaximumStrength);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogReductsStatistics, CDialog)
	//{{AFX_MSG_MAP(CDialogReductsStatistics)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogReductsStatistics message handlers

//-------------------------------------------------------------------
// Method  ......: OnInitDialog
// Author........: Merete Hvalshagen/Aleksander šhrn
// Date..........: 960420
// Description...:
// Comments......: 
// Revisions.....: Aš 960703 Full rewrite.
//===================================================================

BOOL CDialogReductsStatistics::OnInitDialog() 
{

  CDialog::OnInitDialog();

	// Any reducts present?
	if (m_Reducts.IsNULL()) {
		Message::Error("No reducts present.");
		UpdateData(FALSE);
		return TRUE;
	}

	// Set size and name.
	m_Size = String::Format(m_Reducts->GetNoReducts()).GetBuffer();
	m_Name = m_Reducts->GetName().GetBuffer();

	// Insert listctrl columns.
	CRect rect;
	m_HistogramAttribute.GetClientRect(rect);
	int width1 = rect.Width() / 6;
	int width2 = rect.Width() / 6;
	int width3 = rect.Width() / 6;
	int width4 = rect.Width() / 6;
	int width5 = rect.Width() / 6;
	int width6 = rect.Width() - width1 - width2 - width3 - width4 - width5;

	m_HistogramAttribute.InsertColumn(0, "Attribute",    LVCFMT_LEFT, width1, 0);
	m_HistogramAttribute.InsertColumn(1, "Count",        LVCFMT_LEFT, width2, 1);
	m_HistogramAttribute.InsertColumn(2, "%",            LVCFMT_LEFT, width3, 2);
	m_HistogramAttribute.InsertColumn(3, "Min. support", LVCFMT_LEFT, width4, 3);
	m_HistogramAttribute.InsertColumn(4, "Mean support", LVCFMT_LEFT, width5, 4);
	m_HistogramAttribute.InsertColumn(5, "Max. support", LVCFMT_LEFT, width6, 5);

	m_HistogramLength.GetClientRect(rect);
	width1 = rect.Width() / 2;
	width2 = rect.Width() - width1;

	m_HistogramLength.InsertColumn(0, "Length", LVCFMT_LEFT, width1, 0);
	m_HistogramLength.InsertColumn(1, "Count",  LVCFMT_LEFT, width2, 1);

	m_HistogramStrength.GetClientRect(rect);
	width1 = rect.Width() / 2;
	width2 = rect.Width() - width1;

	m_HistogramStrength.InsertColumn(0, "Support",  LVCFMT_LEFT, width1, 0);
	m_HistogramStrength.InsertColumn(1, "Count",    LVCFMT_LEFT, width2, 1);

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

void CDialogReductsStatistics::CalculateStatistics()
{

	// Any reducts present?
	if (m_Reducts.IsNULL())
		return;

	int no_reducts = m_Reducts->GetNoReducts();

	// Clear current histograms.
	m_HistogramAttribute.DeleteAllItems();
	m_HistogramLength.DeleteAllItems();
	m_HistogramStrength.DeleteAllItems();

	// Display a wait cursor.
	CWaitCursor wait;

	Reducts::IVMap attributes;

	// Get supports per attribute.
	if (!m_Reducts->GetAttributeSupports(attributes))
		return;

	// A convenient typedef.
	typedef Map(int, int) IMap;

	Vector(int) lengths;         
	Vector(int) supports;   
	
	lengths.reserve(no_reducts);
	supports.reserve(no_reducts);

	int i;

	// Extract lengths/supports.
	for (i = 0; i < no_reducts; i++) {
		Handle<Reduct> reduct = m_Reducts->GetReduct(i);
		lengths.push_back(reduct->GetNoAttributes());
		supports.push_back(reduct->GetSupport());
	}

	// Calculate statistics
	float mean_length      = MathKit::Mean(lengths);
	float stddev_length    = MathKit::StandardDeviation(lengths);
	float median_length    = MathKit::Median(lengths);
	int   minimum_length   = MathKit::Minimum(lengths);
	int   maximum_length   = MathKit::Maximum(lengths);

	float mean_support     = MathKit::Mean(supports);
	float stddev_support   = MathKit::StandardDeviation(supports);
	float median_support   = MathKit::Median(supports);
	int   minimum_support  = MathKit::Minimum(supports);
	int   maximum_support  = MathKit::Maximum(supports);

	// Move stuff into UI variables.
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

  if (mean_support == Undefined::Float())
		m_MeanStrength = Undefined::String().GetBuffer();
	else
		m_MeanStrength = String::Format(mean_support).GetBuffer();

  if (stddev_support == Undefined::Float())
		m_StdDevStrength = Undefined::String().GetBuffer();
	else
		m_StdDevStrength = String::Format(stddev_support).GetBuffer();

  if (median_support == Undefined::Float())
		m_MedianStrength = Undefined::String().GetBuffer();
	else
		m_MedianStrength = String::Format(median_support).GetBuffer();

  if (minimum_support == Undefined::Integer())
		m_MinimumStrength = Undefined::String().GetBuffer();
	else
		m_MinimumStrength = String::Format(minimum_support).GetBuffer();

  if (maximum_support == Undefined::Integer())
		m_MaximumStrength = Undefined::String().GetBuffer();
	else
		m_MaximumStrength = String::Format(maximum_support).GetBuffer();

	// Calculate histograms.
	Map(int, int) length_hist;
	Map(int, int) strength_hist;

  if (!MathKit::Distribution(length_hist, lengths) || !MathKit::Distribution(strength_hist, supports))
		return;

	// Get originating table.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, m_Reducts->FindParent(DECISIONTABLE));

	if (table == NULL)
		Message::Error("Could not get originating table.");

	bool masked = true;

	// Move histogram stuff into UI list controls.
	m_HistogramAttribute.LockWindowUpdate();
	m_HistogramLength.LockWindowUpdate();
	m_HistogramStrength.LockWindowUpdate();

	LV_ITEM                  insertstruct;
  int                      index;
  String                   label;
	Reducts::IVMap::iterator iviterator;

	insertstruct.mask = LVIF_TEXT;

  iviterator = attributes.begin();                              // Attribute list ctrl.

	while (!(iviterator == attributes.end())) {
		int attribute           = (*iviterator).first;
		Vector(int) &supports2  = (*iviterator).second;
		String attribute_name   = (table == NULL) ? String::Format(attribute) : table->GetAttributeName(attribute, masked);
		// Insert item.
		label                   = attribute_name;
		insertstruct.pszText    = const_cast(char *, label.GetBuffer());
		insertstruct.iItem      = INT_MAX;
		insertstruct.iSubItem   = 0;
		index                   = m_HistogramAttribute.InsertItem(&insertstruct);
		insertstruct.iItem      = index;
		// Insert sub items.
		int count               = supports2.size();
		label                   = String::Format(count);
		insertstruct.pszText    = const_cast(char *, label.GetBuffer());
		insertstruct.iSubItem   = 1;
		m_HistogramAttribute.SetItem(&insertstruct);
		label                   = String::Format(static_cast(float, 100 * count) / no_reducts);
		insertstruct.pszText    = const_cast(char *, label.GetBuffer());
		insertstruct.iSubItem   = 2;
		m_HistogramAttribute.SetItem(&insertstruct);
		label                   = String::Format(MathKit::Minimum(supports2));
		insertstruct.pszText    = const_cast(char *, label.GetBuffer());
		insertstruct.iSubItem   = 3;
		m_HistogramAttribute.SetItem(&insertstruct);
		label                   = String::Format(MathKit::Mean(supports2));
		insertstruct.pszText    = const_cast(char *, label.GetBuffer());
		insertstruct.iSubItem   = 4;
		m_HistogramAttribute.SetItem(&insertstruct);
		label                   = String::Format(MathKit::Maximum(supports2));
		insertstruct.pszText    = const_cast(char *, label.GetBuffer());
		insertstruct.iSubItem   = 5;
		m_HistogramAttribute.SetItem(&insertstruct);
		// Increment iterator.
		iviterator++;
	}  

	// This should be a const_iterator, but VC++ 6.0 won't let me...
	Map(int, int)::iterator it = length_hist.begin();                                 // Length histogram.

	while (!(it == length_hist.end())) {
		int length = (*it).first;
		int count  = (*it).second;
		// Insert item.
		label                   = String::Format(length);
		insertstruct.pszText    = const_cast(char *, label.GetBuffer());
		insertstruct.iItem      = INT_MAX;
		insertstruct.iSubItem   = 0;
		index                   = m_HistogramLength.InsertItem(&insertstruct);
		insertstruct.iItem      = index;
		// Insert sub items.
		label                   = String::Format(count);
		insertstruct.pszText    = const_cast(char *, label.GetBuffer());
		insertstruct.iSubItem   = 1;
		m_HistogramLength.SetItem(&insertstruct);
		// Increment iterator.
		it++;
	}  

	it = strength_hist.begin();                                                       // Support histogram.

	while (!(it == strength_hist.end())) {
		int support  = (*it).first;
		int count    = (*it).second;
		// Insert item.
		label                   = String::Format(support);
		insertstruct.pszText    = const_cast(char *, label.GetBuffer());
		insertstruct.iItem      = INT_MAX;
		insertstruct.iSubItem   = 0;
		index                   = m_HistogramStrength.InsertItem(&insertstruct);
		insertstruct.iItem      = index;
		// Insert sub items.
		label                   = String::Format(count);
		insertstruct.pszText    = const_cast(char *, label.GetBuffer());
		insertstruct.iSubItem   = 1;
		m_HistogramStrength.SetItem(&insertstruct);
		// Increment iterator.
		it++;
	}  

	m_HistogramAttribute.UnlockWindowUpdate();
	m_HistogramLength.UnlockWindowUpdate();
	m_HistogramStrength.UnlockWindowUpdate();

	// Update data.
	UpdateData(FALSE);

}

//-------------------------------------------------------------------
// Method  ......: SetReducts
// Author........: 
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogReductsStatistics::SetReducts(Reducts *reducts)
{
	m_Reducts = reducts;
}

 
