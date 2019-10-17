// dialogobjectselector.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogobjectselector.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogObjectSelector dialog


CDialogObjectSelector::CDialogObjectSelector(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogObjectSelector::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogObjectSelector)
	//}}AFX_DATA_INIT
}


void CDialogObjectSelector::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogObjectSelector)
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_EditFilename);
	DDX_Control(pDX, IDC_COMBO_VALUE, m_ComboValue);
	DDX_Control(pDX, IDC_COMBO_INDEX, m_ComboIndex);
	DDX_Control(pDX, IDC_COMBO_ATTRIBUTE, m_ComboAttribute);
	DDX_Control(pDX, IDC_BUTTON_BROWSE, m_ButtonBrowse);
	DDX_Control(pDX, IDC_RADIO_ALL, m_RadioAll);
	DDX_Control(pDX, IDC_RADIO_INDEX, m_RadioIndex);
	DDX_Control(pDX, IDC_RADIO_VALUE, m_RadioValue);
	DDX_Control(pDX, IDC_RADIO_FILE, m_RadioFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogObjectSelector, CDialog)
	//{{AFX_MSG_MAP(CDialogObjectSelector)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_CBN_SELCHANGE(IDC_COMBO_ATTRIBUTE, OnSelchangeComboAttribute)
	ON_BN_CLICKED(IDC_RADIO_ALL, OnRadioAll)
	ON_BN_CLICKED(IDC_RADIO_FILE, OnRadioFile)
	ON_BN_CLICKED(IDC_RADIO_INDEX, OnRadioIndex)
	ON_BN_CLICKED(IDC_RADIO_VALUE, OnRadioValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogObjectSelector message handlers

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

BOOL CDialogObjectSelector::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_Table.IsNULL()) {
		Message::Error("Decision table not set.");
		EndDialog(IDABORT);
		return FALSE;
	}

	// Set radio button states.
	m_RadioAll.SetCheck(m_Selector.GetSelection() == ObjectSelector::SELECTION_ALL);
	m_RadioIndex.SetCheck(m_Selector.GetSelection() == ObjectSelector::SELECTION_INDEX);
	m_RadioValue.SetCheck(m_Selector.GetSelection() == ObjectSelector::SELECTION_VALUE);
	m_RadioFile.SetCheck(m_Selector.GetSelection() == ObjectSelector::SELECTION_FILE);

	// Fill index combo (unsorted).
	int no_objects = m_Table->GetNoObjects(true);

	int initial_selection = 0;
	int i;

	for (i = 0; i < no_objects; i++) {
		m_ComboIndex.AddString(String::Format(i).GetBuffer());
		if (m_Selector.GetIndex() == i)
			initial_selection = i;
	}

	m_ComboIndex.SetCurSel(initial_selection);

	// Fill attribute combo (unsorted).
	int no_attributes = m_Table->GetNoAttributes(false);

	initial_selection = 0;

	for (i = 0; i < no_attributes; i++) {
		String name = m_Table->GetAttributeName(i, false);
		m_ComboAttribute.AddString(name.GetBuffer());
		if (name == m_Selector.GetAttributeName())
			initial_selection = i;
	}

	// Set attribute combo selection.
	m_ComboAttribute.SetCurSel(initial_selection);

	// Fill value combo.
	UpdateValueCombo(m_ComboAttribute, m_ComboValue);

	// Set filename.
	m_EditFilename.SetWindowText(m_Selector.GetFilename().GetBuffer());

	// Enable/disable stuff.
	if (m_RadioAll.GetCheck()) 
		OnRadioAll();
	else if (m_RadioIndex.GetCheck())
		OnRadioIndex();
	else if (m_RadioValue.GetCheck())
		OnRadioValue();
	else if (m_RadioFile.GetCheck())
		OnRadioFile();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
// Method........: OnButtonBrowse
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogObjectSelector::OnButtonBrowse() 
{

	CFileDialog fileDlg(TRUE, NULL, "*.*");
  
	if (fileDlg.DoModal() == IDOK) {
    CString fullpath = fileDlg.GetPathName();
		m_EditFilename.SetWindowText(fullpath);
	}

	UpdateData(FALSE);
	
}

//-------------------------------------------------------------------
// Method........: OnSelchangeComboAttribute
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogObjectSelector::OnSelchangeComboAttribute() 
{

	UpdateData(TRUE);

	// Update value combo.
	UpdateValueCombo(m_ComboAttribute, m_ComboValue);
		
	UpdateData(FALSE);
	
}

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogObjectSelector::OnRadioAll() 
{

	// Enable/disable.
	m_EditFilename.EnableWindow(FALSE);
	m_ButtonBrowse.EnableWindow(FALSE);
	m_ComboIndex.EnableWindow(FALSE);
	m_ComboAttribute.EnableWindow(FALSE);
	m_ComboValue.EnableWindow(FALSE);

	UpdateData(FALSE);
	
}

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogObjectSelector::OnRadioFile() 
{

	// Enable/disable.
	m_EditFilename.EnableWindow(TRUE);
	m_ButtonBrowse.EnableWindow(TRUE);
	m_ComboIndex.EnableWindow(FALSE);
	m_ComboAttribute.EnableWindow(FALSE);
	m_ComboValue.EnableWindow(FALSE);

	UpdateData(FALSE);

}

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogObjectSelector::OnRadioIndex() 
{

	// Enable/disable.
	m_EditFilename.EnableWindow(FALSE);
	m_ButtonBrowse.EnableWindow(FALSE);
	m_ComboIndex.EnableWindow(TRUE);
	m_ComboAttribute.EnableWindow(FALSE);
	m_ComboValue.EnableWindow(FALSE);

	UpdateData(FALSE);

}

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogObjectSelector::OnRadioValue() 
{

	// Enable/disable.
	m_EditFilename.EnableWindow(FALSE);
	m_ButtonBrowse.EnableWindow(FALSE);
	m_ComboIndex.EnableWindow(FALSE);
	m_ComboAttribute.EnableWindow(TRUE);
	m_ComboValue.EnableWindow(TRUE);

	UpdateData(FALSE);
	
}

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogObjectSelector::OnOK() 
{

	UpdateData(TRUE);

	// Set object selection.
	if (m_RadioAll.GetCheck())
		m_Selector.SetSelection(ObjectSelector::SELECTION_ALL);
	else if (m_RadioValue.GetCheck())
		m_Selector.SetSelection(ObjectSelector::SELECTION_VALUE);
	else if (m_RadioFile.GetCheck())
		m_Selector.SetSelection(ObjectSelector::SELECTION_FILE);
	else if (m_RadioIndex.GetCheck())
		m_Selector.SetSelection(ObjectSelector::SELECTION_INDEX);

	CString filename;

	// Get selection filename.
	m_EditFilename.GetWindowText(filename);

	// Set selection filename.
	m_Selector.SetFilename(filename.GetBuffer(0));

	CString attribute_name;
	CString value_name;

	// Get attribute/value.
	m_ComboAttribute.GetWindowText(attribute_name);
	m_ComboValue.GetWindowText(value_name);

	/*
	// Lookup integral values.
	int attribute = table->GetAttributeIndex(attribute_name.GetBuffer(0), true, false);
	int value     = table->GetDictionaryEntry(attribute, value_name.GetBuffer(0), false);
	*/

	// Set attribute/name.
	m_Selector.SetAttributeName(attribute_name.GetBuffer(0));
	m_Selector.SetValueName(value_name.GetBuffer(0));

	// Get index.
	int index = m_ComboIndex.GetCurSel();
	if (index == CB_ERR)
		index = 0;

	// Set index.
	m_Selector.SetIndex(index);
	
	CDialog::OnOK();
}

//-------------------------------------------------------------------
// Method........: UpdateValueCombo
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogObjectSelector::UpdateValueCombo(const CComboBox &attribute_combo, CComboBox &value_combo)
{

	bool masked = false;

	CString attribute_name;

	// Get attribute index.
	m_ComboAttribute.GetWindowText(attribute_name);
	int attribute = m_Table->GetAttributeIndex(attribute_name.GetBuffer(0), true, masked);

	Vector(int) values;
	Vector(int) frequencies; // Dummy, not in use.

	// Get value set of selected attribute.
	m_Table->GetValueSet(values, frequencies, attribute, masked);

	// Clear current contents of value combo box.
	value_combo.ResetContent();

	int i;

	// Fill value combo box.
	for (i = 0; i < values.size(); i++) {
    String name = m_Table->GetDictionaryEntry(attribute, values[i], masked);
		value_combo.AddString(name.GetBuffer());
	}

	// Get current value selection.
	String current = m_Selector.GetValueName();

	int initial_selection = 0;

	// Deduce index of selection.
	for (i = 0; i < value_combo.GetCount(); i++) {
		CString lbtext;
		value_combo.GetLBText(i, lbtext);
		if (lbtext == current.GetBuffer()) {
			initial_selection = i;
			break;
		}
	}

	// Set selection.
	m_ComboValue.SetCurSel(initial_selection);

}
 
