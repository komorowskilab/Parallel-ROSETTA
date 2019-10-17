// dialogattributestatistics.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\structuredialogs\dialogattributestatistics.h>

#include <kernel\utilities\mathkit.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogAttributeStatistics dialog

//-------------------------------------------------------------------
// Method........: Constuctor
// Author........: Daniel Remmem
// Date..........: 960726
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================
CDialogAttributeStatistics::CDialogAttributeStatistics(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogAttributeStatistics::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogAttributeStatistics)
	m_Mean = _T("");
	m_Correlation = _T("");
	m_Median = _T("");
	m_StDev = _T("");
	m_Status = _T("");
	m_Type = _T("");
	m_NumberAttributes = _T("");
	m_NumberObjects = _T("");
	m_Unit = _T("");
	m_Name = _T("");
	m_MissingValuesWarning = _T("");
	//}}AFX_DATA_INIT

	m_Masked = FALSE;

}


void CDialogAttributeStatistics::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogAttributeStatistics)
	DDX_Control(pDX, IDC_COMBO_ATTRIBUTE_NAME_CORRELATION, m_NameCorrelation);
	DDX_Control(pDX, IDC_LIST_ATTRIBUTE_NAME_MAIN, m_NameMain);
	DDX_Control(pDX, IDC_LIST_HISTOGRAM, m_Histogram);
	DDX_Text(pDX, IDC_STATIC_MEAN, m_Mean);
	DDX_Text(pDX, IDC_STATIC_CORRELATION, m_Correlation);
	DDX_Text(pDX, IDC_STATIC_MEDIAN, m_Median);
	DDX_Text(pDX, IDC_STATIC_STDDEV, m_StDev);
	DDX_Text(pDX, IDC_STATIC_ATTRIBUTE_STATUS, m_Status);
	DDX_Text(pDX, IDC_STATIC_ATTRIBUTE_TYPE, m_Type);
	DDX_Text(pDX, IDC_STATIC_NO_ATTRIBUTES, m_NumberAttributes);
	DDX_Text(pDX, IDC_STATIC_NO_OBJECTS, m_NumberObjects);
	DDX_Text(pDX, IDC_STATIC_ATTRIBUTE_UNIT, m_Unit);
	DDX_Text(pDX, IDC_STATIC_NAME, m_Name);
	DDX_Text(pDX, IDC_MISSINGVALUES_WARNING, m_MissingValuesWarning);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogAttributeStatistics, CDialog)
	//{{AFX_MSG_MAP(CDialogAttributeStatistics)
	ON_CBN_SELCHANGE(IDC_COMBO_ATTRIBUTE_NAME_CORRELATION, OnSelchangeCorrelation)
	ON_LBN_SELCHANGE(IDC_LIST_ATTRIBUTE_NAME_MAIN, OnSelchangeMain)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogAttributeStatistics message handlers

//-------------------------------------------------------------------
// Method........: SetDecisionTable
// Author........: Daniel Remmem
// Date..........: 960726
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogAttributeStatistics::SetDecisionTable(DecisionTable *table)
{
	m_Table = table;
}

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Daniel Remmem
// Date..........: 960726
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogAttributeStatistics::OnInitDialog() 
{

	CDialog::OnInitDialog();
	
	if (m_Table.IsNULL()) {
		Message::Error("No decisiontable was set.");
		EndDialog(IDABORT);
		return FALSE;
	}
	
	// Get table dimensions.
	int no_objects    = m_Table->GetNoObjects(m_Masked);
	int no_attributes = m_Table->GetNoAttributes(m_Masked);

	if ((no_attributes == 0) || (no_objects == 0)) {
		Message::Error("The decision table is empty.");
		EndDialog(IDABORT);
		return FALSE;
	}
		
	// Set table name and number of objects and attributes.
	m_Name             = m_Table->GetName().GetBuffer();
	m_NumberObjects    = String::Format(no_objects).GetBuffer();
	m_NumberAttributes = String::Format(no_attributes).GetBuffer();
	m_Unit             = m_Table->GetAttributeUnit(m_NoAttribute, m_Masked).GetBuffer();
	m_Type             = Attribute::GetString(m_Table->GetAttributeType(m_NoAttribute, m_Masked)).GetBuffer();
	m_Status           = DecisionTable::GetString(m_Table->GetStatus(m_NoAttribute, m_Masked)).GetBuffer();

	// Set missing values warning.
	if (m_Table->HasMissingEntries(m_Masked))
		m_MissingValuesWarning = "Values are missing.";
	else
		m_MissingValuesWarning = "No missing values";

	int i;

	// Build lists.
	for (i = 0; i < no_attributes; i++) {
		m_NameMain.AddString((m_Table->GetAttributeName(i, m_Masked)).GetBuffer());
		m_NameCorrelation.AddString((m_Table->GetAttributeName(i, m_Masked)).GetBuffer());
	}	

	// Set current selection.
	m_NameMain.SetCurSel(m_NoAttribute);
	
	// Initialize the correlation attribute to the last attribute as default.
	m_NoCorrelation = no_attributes - 1; 
	m_NameCorrelation.SetCurSel(m_NoCorrelation);

	// Initialize the Listcontrol.
	
	CRect ListRect;
	// Determine width of ListCtrl.
  m_Histogram.GetClientRect(ListRect);
	int width1 = ListRect.Width() / 3;
	int width2 = ListRect.Width() / 3;
	int width3 = ListRect.Width() - width1 - width2;

  // Insert the different columns.
  m_Histogram.InsertColumn(0, "Value",      LVCFMT_LEFT, width1, 0);
	m_Histogram.InsertColumn(1, "Count",      LVCFMT_LEFT, width2, 1);
  m_Histogram.InsertColumn(2, "Percentage", LVCFMT_LEFT, width3, 2);
		
	// Calculate the statistics for the chosen attribute / correlationattribute.
	CalculateStatistics();
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: Daniel Remmem
// Date..........: 960726
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================
void CDialogAttributeStatistics::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

//-------------------------------------------------------------------
// Method........: CalculateStatistics
// Author........: Daniel Remmem
// Date..........: 960730
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================
void CDialogAttributeStatistics::CalculateStatistics() 
{

	Vector(float) f1_extract, f2_extract, f3_extract;
	Vector(int)   i_extract;
		
	// Make the cursor into a waitcursor.
	CWaitCursor wait;
	
	// So that the listctrl is not updating while inserting items.
	// (The number of items can get quite large.)
	m_Histogram.LockWindowUpdate();

	// Extract values and calculate statistics.
	if (!m_Table->GetEntries(f1_extract, m_NoAttribute,   m_Masked, false, true) ||
	    !m_Table->GetEntries(f2_extract, f3_extract, m_NoAttribute, m_NoCorrelation, m_Masked, false, true)) {
		m_Mean          = Undefined::String().GetBuffer();
		m_StDev         = Undefined::String().GetBuffer();
		m_Median        = Undefined::String().GetBuffer();
		m_Correlation   = Undefined::String().GetBuffer();
	}
	else {
		float f         = MathKit::Mean(f1_extract);                    // Mean.
		if (f == Undefined::Float())
			m_Mean        = Undefined::String().GetBuffer();
		else
			m_Mean        = String::Format(f).GetBuffer();
		f               = MathKit::StandardDeviation(f1_extract);       // St. dev.
		if (f == Undefined::Float())
			m_StDev       = Undefined::String().GetBuffer();
		else
			m_StDev       = String::Format(f).GetBuffer();
		f               = MathKit::Median(f1_extract);                  // Median.
		if (f == Undefined::Float())
			m_Median      = Undefined::String().GetBuffer();
		else
			m_Median      = String::Format(f).GetBuffer();
		f               = MathKit::PearsonCorrelation(f2_extract, f3_extract); // Correlation.
		if (f == Undefined::Float())
			m_Correlation = Undefined::String().GetBuffer();
		else
			m_Correlation = String::Format(f).GetBuffer();
	}
	
	// Clear current histogram
	m_Histogram.DeleteAllItems();

	// Extract values and acquire histogram.
	Map(int, int) histogram;
	if (!m_Table->GetEntries(i_extract, m_NoAttribute, m_Masked, true) ||
		  !MathKit::Distribution(histogram, i_extract)) {
		// Update data and return.
		UpdateData(FALSE);
		return;
	}
	
	// Move data from histogram into list control.		
	LV_ITEM                 InsertStruct;
  int                     nIndex;
  String                  label;

	// This should be a const_iterator, but VC++ 6.0 won't let me...
	Map(int, int)::iterator it = histogram.begin();

	InsertStruct.mask                     = LVIF_TEXT;

	int no_objects = m_Table->GetNoObjects(m_Masked);

	// Get the different items, put them in the histogram as items/subitems.
	while (!(it == histogram.end())) {

		int value = (*it).first;
		int count = (*it).second;

		// Insert item.
		label                   = m_Table->GetDictionaryEntry(m_NoAttribute, value, m_Masked);
		InsertStruct.pszText    = const_cast(char *, label.GetBuffer());
		InsertStruct.iItem      = INT_MAX;
		InsertStruct.iSubItem   = 0;
		InsertStruct.lParam     = reinterpret_cast(LPARAM, InsertStruct.pszText);
		nIndex                  = m_Histogram.InsertItem(&InsertStruct);
  
		InsertStruct.iItem      = nIndex;
  
		// Insert sub item. 
    label                   = String::Format(count);
		InsertStruct.pszText    = const_cast(char *, label.GetBuffer());
		InsertStruct.iSubItem   = 1;
		InsertStruct.lParam     = reinterpret_cast(LPARAM, InsertStruct.pszText);

		m_Histogram.SetItem(&InsertStruct);

		// Insert sub item. 
    label                   = String::Format(100 * static_cast(float, count) / no_objects);
		InsertStruct.pszText    = const_cast(char *, label.GetBuffer());
		InsertStruct.iSubItem   = 2;
		InsertStruct.lParam     = reinterpret_cast(LPARAM, InsertStruct.pszText);

		m_Histogram.SetItem(&InsertStruct);

		it++;

	}

	m_Histogram.UnlockWindowUpdate();

	// Update data.	
	UpdateData(FALSE);
	
}

//-------------------------------------------------------------------
// Method........: OnSelchangeMain
// Author........: Daniel Remmem
// Date..........: 960730
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogAttributeStatistics::OnSelchangeMain() 
{

	m_NoAttribute = m_NameMain.GetCurSel();

	// Update the type and status of the attribute and calculate the new statistics.
	m_Type         = Attribute::GetString(m_Table->GetAttributeType(m_NoAttribute, m_Masked)).GetBuffer();
	m_Status       = DecisionTable::GetString(m_Table->GetStatus(m_NoAttribute, m_Masked)).GetBuffer();
	m_Unit         = m_Table->GetAttributeUnit(m_NoAttribute, m_Masked).GetBuffer();

	CalculateStatistics();
	
}

//-------------------------------------------------------------------
// Method........: OnSelchangeCorrelation
// Author........: Daniel Remmem
// Date..........: 960730
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogAttributeStatistics::OnSelchangeCorrelation() 
{

	Vector(float) f1_extract, f2_extract;

	// Set the new correlation attribute.
	m_NoCorrelation = m_NameCorrelation.GetCurSel();

	// Extract data and calculate correlation.
	if (!m_Table->GetEntries(f1_extract, f2_extract, m_NoAttribute, m_NoCorrelation, m_Masked, false, true)) {
		m_Correlation = Undefined::String().GetBuffer();
	}
	else {
		float f = MathKit::PearsonCorrelation(f1_extract, f2_extract);
		if (f == Undefined::Float())
			m_Correlation = Undefined::String().GetBuffer();
		else
			m_Correlation = String::Format(f).GetBuffer();
	}
		
	UpdateData(FALSE);

}
 
