// dialogmanualscaler.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogmanualscaler.h>

#include <kernel\algorithms\manualscaler.h>

#include <kernel\structures\decisiontable.h>
#include <kernel\structures\dictionary.h>
#include <kernel\structures\attribute.h>

#include <kernel\basic\interval.h>

#include <common\messagehelper.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogManualScaler dialog


CDialogManualScaler::CDialogManualScaler(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogManualScaler::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogManualScaler)
	//}}AFX_DATA_INIT

	m_Masked = TRUE;

}


void CDialogManualScaler::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogManualScaler)
	DDX_Control(pDX, IDC_BUTTON_DELETEALL, m_ButtonDeleteAll);
	DDX_Control(pDX, IDC_EDIT_VALUE, m_EditValue);
	DDX_Control(pDX, IDC_EDIT_INTERVAL, m_EditInterval);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_ButtonDelete);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_ButtonAdd);
	DDX_Control(pDX, IDC_CHECK_VIADICTIONARY, m_CheckViaDictionary);
	DDX_Control(pDX, IDC_LIST_MAPPING, m_ListMapping);
	DDX_Control(pDX, IDC_COMBO_ATTRIBUTES, m_ComboAttributes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogManualScaler, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogManualScaler)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_CBN_SELCHANGE(IDC_COMBO_ATTRIBUTES, OnSelchangeComboAttributes)
	ON_NOTIFY(NM_CLICK, IDC_LIST_MAPPING, OnClickListMapping)
	ON_BN_CLICKED(IDC_BUTTON_DELETEALL, OnButtonDeleteall)
	ON_BN_CLICKED(IDC_CHECK_VIADICTIONARY, OnCheckViadictionary)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogManualScaler message handlers

//-------------------------------------------------------------------
// Method........: OnButtonAdd
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

void CDialogManualScaler::OnButtonAdd() 
{

	// Get interval.
	CString editintervaltext;
	m_EditInterval.GetWindowText(editintervaltext);
	String intervalstr = editintervaltext.GetBuffer(0);

	// Is the interval invalid?
	if (!Interval(intervalstr).IsValid()) {
		Message::Error(intervalstr + " does not constitute a valid interval.");
		UpdateData(FALSE);
		return;
	}

	// Get value.
	CString editvaluetext;
	m_EditValue.GetWindowText(editvaluetext);
  String dummy = editvaluetext.GetBuffer(0);
	if (!dummy.IsInteger()) {
		Message::Error("Map value must be an integer.");
		UpdateData(FALSE);
		return;
	}
	int value = dummy.GetInteger();

	bool is_present = false;
	bool do_overlap = false;

	int i;

	// Check existing contents of mapping list already.
	for (i = 0; i < m_ListMapping.GetItemCount(); i++) {
		CString itemtext0 = m_ListMapping.GetItemText(i, 0);                // Interval.
		CString itemtext1 = m_ListMapping.GetItemText(i, 1);                // Value.
		if (itemtext0 == editintervaltext) {                                // The entered interval is already present.
			m_ListMapping.SetItemText(i, 1, editvaluetext);                   // Reset the mapping value.
			is_present = true;
			continue;
		}
		Interval interval(intervalstr);
		Interval interval0(itemtext0.GetBuffer(0));
		if (interval.Overlaps(interval0)) { // The new interval overlaps an already present one.
			Message::Warning("Intervals " + intervalstr + " and " + itemtext0.GetBuffer(0) + " overlap.");
			do_overlap = true;
		}
	}

	// Was the interval already present?
	if (is_present || do_overlap) {
		UpdateData(FALSE);
		return;
	}

	// Add the interval/value pair to the mapping list as items/subitems.
	LV_ITEM insertstruct;
  int     index;
  String  label;

	insertstruct.mask = LVIF_TEXT;

	// Insert item.
	label                 = intervalstr;
	insertstruct.pszText  = const_cast(char *, label.GetBuffer());
	insertstruct.iItem    = INT_MAX;
	insertstruct.iSubItem = 0;
	insertstruct.lParam   = reinterpret_cast(LPARAM, insertstruct.pszText);
	index                 = m_ListMapping.InsertItem(&insertstruct);
  
	insertstruct.iItem    = index;
  
	// Insert sub item. 
  label                 = String::Format(value);
	insertstruct.pszText  = const_cast(char *, label.GetBuffer());
	insertstruct.iSubItem = 1;
	insertstruct.lParam   = reinterpret_cast(LPARAM, insertstruct.pszText);
	m_ListMapping.SetItem(&insertstruct);

	// Update data.	
	UpdateData(FALSE);
	
}

//-------------------------------------------------------------------
// Method........: OnButtonDelete
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

void CDialogManualScaler::OnButtonDelete() 
{

	int i;

	bool selected = false;

	// Determine which interval is selected.
	for (i = m_ListMapping.GetItemCount() - 1; i >= 0; i--) {
		if (m_ListMapping.GetItemState(i, LVIS_SELECTED)) {
			m_ListMapping.DeleteItem(i);
			selected = true;
		}
	}

	UpdateData(FALSE);

	if (!selected)
		Message::Error("No intervals selected for deletion.");
	
}

//-------------------------------------------------------------------
// Method........: OnSelchangeComboAttributes
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

void CDialogManualScaler::OnSelchangeComboAttributes() 
{

	BOOL enable_check = FALSE;

	// Get current selection.
	int selection = m_ComboAttributes.GetCurSel();

  // Determine if the selected attribute is a float attribute.
	if (selection != CB_ERR) {
	  if (!m_Structure.IsNULL() && m_Structure->IsA(DECISIONTABLE)) {
			Handle<DecisionTable> table = dynamic_cast(DecisionTable *, m_Structure.GetPointer());
			if (table->IsFloat(selection, m_Masked))
				enable_check = TRUE;
		}
	}

	// Disable/enable interval interpretation check box.
	m_CheckViaDictionary.EnableWindow(enable_check);
	
}

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

BOOL CDialogManualScaler::OnInitDialog() 
{

	int i;

	CDialogAlgorithm::OnInitDialog();

	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(MANUALSCALER)) {
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
	Handle<ManualScaler> scaler = dynamic_cast(ManualScaler *, m_Algorithm.GetPointer());

	// Get decision table to scale.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, m_Structure.GetPointer());

	// Get initial selection.
	int initial_selection = scaler->GetAttribute();

	bool enable_check = false;

	// Build combo box options.
	for (i = 0; i < table->GetNoAttributes(m_Masked); i++) {
		String name = table->GetAttributeName(i, m_Masked);
		if (i == initial_selection)
			enable_check = table->IsFloat(i, m_Masked);
		m_ComboAttributes.AddString(name.GetBuffer());
	}
	m_ComboAttributes.SetCurSel(initial_selection);

	// Set and Disable/enable interval interpretation check box.
	m_CheckViaDictionary.SetCheck(scaler->UseDictionary());
	m_CheckViaDictionary.EnableWindow(enable_check);

	// Add headers to list control.
	CRect listrect;
  m_ListMapping.GetClientRect(listrect);
	int width1 = listrect.Width() / 2;
	int width2 = listrect.Width() - width1;
  m_ListMapping.InsertColumn(0, "Interval", LVCFMT_LEFT, width1, 0);
	m_ListMapping.InsertColumn(1, "Value",    LVCFMT_LEFT, width2, 1);

	// Insert existing intervals into list control.
	LV_ITEM insertstruct;
  int     index;
  String  label;

	insertstruct.mask = LVIF_TEXT;

	// Insert the interval/value pairs as items/subitems.
	for (i = 0; i < scaler->GetNoIntervals(); i++) {

		String formatted;
		if (!scaler->GetInterval(i).Format(formatted))
			formatted = "";

		// Insert item.
		label                   = formatted;
		insertstruct.pszText    = const_cast(char *, label.GetBuffer());
		insertstruct.iItem      = INT_MAX;
		insertstruct.iSubItem   = 0;
		insertstruct.lParam     = reinterpret_cast(LPARAM, insertstruct.pszText);
		index                   = m_ListMapping.InsertItem(&insertstruct);
  
		insertstruct.iItem      = index;
  
		// Insert sub item. 
    label                   = String::Format(scaler->GetValue(i));
		insertstruct.pszText    = const_cast(char *, label.GetBuffer());
		insertstruct.iSubItem   = 1;
		insertstruct.lParam     = reinterpret_cast(LPARAM, insertstruct.pszText);
		m_ListMapping.SetItem(&insertstruct);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

void CDialogManualScaler::OnOK() 
{

	UpdateData(TRUE);

	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(MANUALSCALER)) {
		Message::Error("Algorithm not set or of incorrect type.");
		EndDialog(IDABORT);
		return;
	}

	Handle<ManualScaler> scaler = dynamic_cast(ManualScaler *, m_Algorithm.GetPointer());

	// Get/set attribute index.
	int attribute = m_ComboAttributes.GetCurSel();
	if (attribute == CB_ERR) {
		Message::Error("No attribute selected.");
		EndDialog(IDABORT);
		return;
	}
	scaler->SetAttribute(attribute);

	// Get/set interval interpretation check box.
	bool use_dictionary = m_CheckViaDictionary.IsWindowEnabled() && m_CheckViaDictionary.GetCheck();
	scaler->UseDictionary(use_dictionary);

	int i;

	// Get/set mappings.
	scaler->RemoveAllIntervals();
	for (i = 0; i < m_ListMapping.GetItemCount(); i++) {
		CString itemtext0   = m_ListMapping.GetItemText(i, 0);        // Interval.
		CString itemtext1   = m_ListMapping.GetItemText(i, 1);        // Value.
		String  intervalstr = itemtext0.GetBuffer(0);
		String  value       = itemtext1.GetBuffer(0);
		scaler->AppendInterval(Interval(intervalstr), value.GetInteger());
	}
	
	CDialogAlgorithm::OnOK();

}

//-------------------------------------------------------------------
// Method........: OnClickListMapping
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

void CDialogManualScaler::OnClickListMapping(NMHDR* pNMHDR, LRESULT* pResult) 
{

	int i;

	// Determine which interval was clicked, and move selected pair into edit controls.
	for (i = 0; i < m_ListMapping.GetItemCount(); i++) {
		if (m_ListMapping.GetItemState(i, LVIS_SELECTED)) {
			CString itemtext0 = m_ListMapping.GetItemText(i, 0);  // Interval.
			CString itemtext1 = m_ListMapping.GetItemText(i, 1);  // Value.
			m_EditInterval.SetWindowText(itemtext0);
			m_EditValue.SetWindowText(itemtext1);
			UpdateData(FALSE);
			break;
		}
	}
	
	*pResult = 0;
}

//-------------------------------------------------------------------
// Method........: OnButtonDeleteall
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

void CDialogManualScaler::OnButtonDeleteall() 
{

	m_ListMapping.DeleteAllItems();

	UpdateData(FALSE);
	
}

//-------------------------------------------------------------------
// Method........: OnButtonDeleteall
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

void CDialogManualScaler::OnCheckViadictionary() 
{
	UpdateData(TRUE);
	if (!m_CheckViaDictionary.GetCheck()) {
		String text = "The option to bypass the dictionary when interpreting the intervals\n"
			            "is intended for expert use only. It is therefore recommended that this\n"
							    "option is checked, unless you know what you are doing.\n\n"
								  "Are you really sure you want to bypass the dictionary?";
		if (MessageHelper::Query(text, "Dictionary", MessageHelper::BUTTONS_YESNO) == MessageHelper::RESPONSE_NO) {
			m_CheckViaDictionary.SetCheck(TRUE);
			UpdateData(FALSE);
		}
	}
}
 
