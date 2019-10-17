// dialogodbcdecisiontableimporter.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogodbcdecisiontableimporter.h>
#include <frontend\dialogs\structuredialogs\dialogattrtype.h>

#include <frontend\algorithms\odbcdecisiontableimporter.h>
#include <frontend\algorithms\odbckit.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDialogODBCDecisionTableImporter dialog


CDialogODBCDecisionTableImporter::CDialogODBCDecisionTableImporter(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogODBCDecisionTableImporter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogODBCDecisionTableImporter)
	m_TableInfo = _T("");
	//}}AFX_DATA_INIT

	m_CurrentTable     = 0;
	m_CurrentAttribute = 0;

}


void CDialogODBCDecisionTableImporter::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogODBCDecisionTableImporter)
	DDX_Control(pDX, IDC_LIST_TABLES, m_ListTable);
	DDX_Control(pDX, IDC_LIST_ATTRIBUTES, m_ListAttributes);
	DDX_Text(pDX, IDC_STATIC_TABLEINFO, m_TableInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogODBCDecisionTableImporter, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogODBCDecisionTableImporter)
	ON_BN_CLICKED(IDC_BUTTON_DATABASE, OnButtonDatabase)
	ON_BN_CLICKED(IDC_BUTTON_TYPE, OnButtonType)
	ON_LBN_SELCHANGE(IDC_LIST_TABLES, OnSelchangeListTables)
	ON_LBN_SELCHANGE(IDC_LIST_ATTRIBUTES, OnSelchangeListAttributes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogODBCDecisionTableImporter message handlers

//-------------------------------------------------------------------
// Method........: OnButtonDatabase
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogODBCDecisionTableImporter::OnButtonDatabase() 
{

	// Reset current database connection?
	if (m_Database.IsOpen())
		m_Database.Close();

	m_ListTable.ResetContent();
	m_ListAttributes.ResetContent();
	m_TableInfo = "";

	// Open new database connection.
	if (!m_Database.Open(NULL, FALSE, TRUE, "ODBC;")) {
		Message::Error("Failed to open database.");
		UpdateData(FALSE);
		return;
	}

	// Get names of tables in DB.
	if (!ODBCKit::GetTables(m_Database, m_Tables) || m_Tables.empty()) {
		Message::Error("Failed to get table names from database.");
		UpdateData(FALSE);
		return;
	}

	m_CurrentTable = 0;

	FillTableList();

	// Get columns of current table in DB.
	if (!ODBCKit::GetColumns(m_Database, m_Tables[m_CurrentTable], m_Names, m_Types, m_Scales) || m_Names.empty()) {
		UpdateData(FALSE);
		return;
	}

	m_CurrentAttribute = 0;

	FillAttributeList();
	
}

//-------------------------------------------------------------------
// Method........: OnButtonType
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogODBCDecisionTableImporter::OnButtonType() 
{

	CDialogAttributeType dialog;

	dialog.m_Type     = m_Types[m_CurrentAttribute];
	dialog.m_Exponent = m_Scales[m_CurrentAttribute];
	
	if (dialog.DoModal() != IDOK)
		return;

	m_Types[m_CurrentAttribute]  = dialog.m_Type;
	m_Scales[m_CurrentAttribute] = dialog.m_Exponent;

	// Update list.
	String entry = m_Names[m_CurrentAttribute] + '\t' + Attribute::GetString(m_Types[m_CurrentAttribute]);
	if (m_Types[m_CurrentAttribute] == Attribute::TYPE_FLOAT)
		entry += '(' + String::Format(m_Scales[m_CurrentAttribute]) + ')';

	bool state = m_ListAttributes.GetCheck(m_CurrentAttribute);

	m_ListAttributes.DeleteString(m_CurrentAttribute);
	m_ListAttributes.InsertString(m_CurrentAttribute, entry.GetBuffer());
	m_ListAttributes.SetCheck(m_CurrentAttribute, state);

}

//-------------------------------------------------------------------
// Method........: OnSelchangeListTables
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogODBCDecisionTableImporter::OnSelchangeListTables() 
{

	// Get index of selected table.
	int selected = m_ListTable.GetCurSel();

	if (selected == LB_ERR) {
		m_CurrentTable = 0;
		m_ListAttributes.ResetContent();
		return;
	}

	if (selected == m_CurrentTable)
		return;

	m_CurrentTable = selected;

	// Get columns of current table in DB.
	if (!ODBCKit::GetColumns(m_Database, m_Tables[m_CurrentTable], m_Names, m_Types, m_Scales) || m_Names.empty()) {
		m_ListAttributes.ResetContent();
		m_TableInfo = "";
		UpdateData(FALSE);
		return;
	}

	m_CurrentAttribute = 0;

	FillAttributeList();
	
}

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogODBCDecisionTableImporter::OnOK() 
{

	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(ODBCDECISIONTABLEIMPORTER)) {
		Message::Error("Algorithm not present or of unexpected type.");
		return;
	}

	Handle<ODBCDecisionTableImporter> importer = dynamic_cast(ODBCDecisionTableImporter *, m_Algorithm.GetPointer());

	int i;

	// Build mask.
	Bits selected(m_ListAttributes.GetCount(), true);

	for (i = 0; i < m_ListAttributes.GetCount(); i++)
		selected[i] = m_ListAttributes.GetCheck(i);

	// Set parameters.
	importer->SetDatabase(&m_Database); // NB: Assumes that this dialog lives on!
	                                    // NB: Importer closes database upon completion.
	importer->SetTable(m_Tables[m_CurrentTable]);
	importer->SetSelected(selected);
	importer->SetNames(m_Names);
	importer->SetTypes(m_Types);
	importer->SetScales(m_Scales);
	importer->SetFilename(m_Database.GetDatabaseName().GetBuffer(0));
	
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

BOOL CDialogODBCDecisionTableImporter::OnInitDialog() 
{
	CDialogAlgorithm::OnInitDialog();
	
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(ODBCDECISIONTABLEIMPORTER)) {
		Message::Error("Algorithm not present or of unexpected type.");
		EndDialog(IDABORT);
		return FALSE;
	}

	m_ListTable.ResetContent();
	m_ListAttributes.ResetContent();
	m_TableInfo = "";

	CRect rect;
	m_ListAttributes.GetWindowRect(&rect);
	CFont *pFont = m_ListAttributes.GetFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	int nBaseUnit = lf.lfWidth;
	if (nBaseUnit == 0)
		nBaseUnit = LOWORD(GetDialogBaseUnits());
	int nDialogUnitNumber = (rect.Width() * 4) / nBaseUnit;

	int nTabStop = nDialogUnitNumber / 2;
	m_ListAttributes.SetTabStops(nTabStop);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
// Method........: FillTableList
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogODBCDecisionTableImporter::FillTableList() 
{

	m_ListTable.ResetContent();

	int i;

	// Trin away any quotes that may have been added.
	for (i = 0; i < m_Tables.size(); i++) {
		String name = m_Tables[i];
		name.Trim('\"');
		m_ListTable.AddString(name.GetBuffer());
	}

	m_ListTable.SetCurSel(m_CurrentTable);

}

//-------------------------------------------------------------------
// Method........: FillAttributeList
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogODBCDecisionTableImporter::FillAttributeList()
{

	m_ListAttributes.ResetContent();
	m_TableInfo = "";

	int no_records = ODBCKit::GetNoRecords(m_Database, m_Tables[m_CurrentTable]);

	if (no_records != Undefined::Integer())
		m_TableInfo = String::Format(no_records).GetBuffer() + CString(" records, ") + String::Format(m_Names.size()).GetBuffer() + CString(" fields.");
	else
		m_TableInfo = String::Format(m_Names.size()).GetBuffer() + CString(" fields.");

	int i;

	for (i = 0; i < m_Names.size(); i++) {
		String entry = m_Names[i] + '\t' + Attribute::GetString(m_Types[i]);
		if (m_Types[i] == Attribute::TYPE_FLOAT)
			entry += '(' + String::Format(m_Scales[i]) + ')';
		m_ListAttributes.AddString(entry.GetBuffer());
		m_ListAttributes.SetCheck(i, TRUE);
	}

	m_ListAttributes.SetCurSel(m_CurrentAttribute);

	UpdateData(FALSE);

}

//-------------------------------------------------------------------
// Method........: OnSelchangeListAttributes
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogODBCDecisionTableImporter::OnSelchangeListAttributes() 
{

	// Get index of selected attribute.
	int selected = m_ListAttributes.GetCurSel();

	if (selected == LB_ERR) {
		m_CurrentAttribute = 0;
		return;
	}

	// Update index.
	m_CurrentAttribute = selected;
	
}
 
