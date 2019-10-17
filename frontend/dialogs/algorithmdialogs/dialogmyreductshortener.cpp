// dialogmyreductshortener.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogmyreductshortener.h>

#include <kernel\algorithms\MyReductshortener.h>

#include <kernel\structures\reducts.h>
#include <kernel\structures\decisiontable.h>
#include <kernel\structures\dictionary.h>

#include <kernel\utilities\mathkit.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogMyReductShortener dialog


CDialogMyReductShortener::CDialogMyReductShortener(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogMyReductShortener::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogMyReductShortener)
	//}}AFX_DATA_INIT

	m_Masked = TRUE;

}


void CDialogMyReductShortener::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogMyReductShortener)
	DDX_Control(pDX, IDC_CHECK_PERCENTAGE, m_CheckPercentage);
	DDX_Control(pDX, IDC_CHECK_ATTRIBUTE_IF, m_CheckAttributeIf);
	DDX_Control(pDX, IDC_CHECK_ATTRIBUTE, m_CheckAttribute);
	DDX_Control(pDX, IDC_COMBO_ATTRIBUTE_IF_2, m_ComboAttributeSecondary);
	DDX_Control(pDX, IDC_EDIT_PERCENTAGE_UPPER, m_EditUpperPercentage);
	DDX_Control(pDX, IDC_EDIT_PERCENTAGE_LOWER, m_EditLowerPercentage);
	DDX_Control(pDX, IDC_CHECK_INVERTLOGIC, m_CheckInvert);
	DDX_Control(pDX, IDC_COMBO_ATTRIBUTE_IF_1, m_ComboAttributePrimary);
	DDX_Control(pDX, IDC_COMBO_ATTRIBUTE, m_ComboAttribute);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogMyReductShortener, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogMyReductShortener)
	ON_BN_CLICKED(IDC_CHECK_ATTRIBUTE, OnCheckAttribute)
	ON_BN_CLICKED(IDC_CHECK_ATTRIBUTE_IF, OnCheckAttributeIf)
	ON_BN_CLICKED(IDC_CHECK_PERCENTAGE, OnCheckPercentage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogMyReductShortener message handlers

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogMyReductShortener::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();

	if (m_Structure.IsNULL() || !m_Structure->IsA(REDUCTS)) {
		Message::Error("Structure not present or not a reduct set.");
		EndDialog(IDABORT);
		return FALSE;
	}

	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(MYREDUCTSHORTENER)) {
		Message::Error("Algorithm not present or not a (native) reduct shortener.");
		EndDialog(IDABORT);
		return FALSE;
	}

	Handle<Reducts>           reducts   = dynamic_cast(Reducts *,           m_Structure.GetPointer());
	Handle<MyReductShortener> shortener = dynamic_cast(MyReductShortener *, m_Algorithm.GetPointer());

	Handle<DecisionTable>     table;

	// Is shortening allowed?
	if (reducts->HasChildren()) {
		Message::Error("Cannot shorten reducts with children.");
		EndDialog(IDABORT);
		return FALSE;
	}

	// Get dictionary associated with originating table.
	table = dynamic_cast(DecisionTable *, reducts->FindParent(DECISIONTABLE, *m_Project));

	if (table == NULL) {
		Message::Error("Cannot access originating decision table.");
		EndDialog(IDABORT);
		return FALSE;
	}

	int i;

	// Fill attribute combo box lists.
  for (i = 0; i < table->GetNoAttributes(m_Masked); i++) {
		String name = table->GetAttributeName(i, m_Masked);
		m_ComboAttribute.AddString(name.GetBuffer());
		m_ComboAttributePrimary.AddString(name.GetBuffer());
		m_ComboAttributeSecondary.AddString(name.GetBuffer());
	}

	// Set initial values, check buttons.
	m_CheckPercentage.SetCheck(shortener->GetShorteningStrategy() & MyReductShortener::SHORTENING_PERCENTAGE);
	m_CheckAttribute.SetCheck(shortener->GetShorteningStrategy() & MyReductShortener::SHORTENING_OCCURRENCE);
	m_CheckAttributeIf.SetCheck(shortener->GetShorteningStrategy() & MyReductShortener::SHORTENING_COMBINEDOCCURRENCE);

	// Enable windows.
	OnCheckPercentage();
	OnCheckAttribute();
	OnCheckAttributeIf();

	// Set initial values, percentage range fields.
	m_EditLowerPercentage.SetWindowText(String::Format(shortener->GetLowerPercentage()).GetBuffer());
	m_EditUpperPercentage.SetWindowText(String::Format(shortener->GetUpperPercentage()).GetBuffer());

	// Set initial values, attribute combo box lists.
	m_ComboAttribute.SetCurSel(shortener->GetAttribute());
	m_ComboAttributePrimary.SetCurSel(shortener->GetPrimaryAttribute());
	m_ComboAttributeSecondary.SetCurSel(shortener->GetSecondaryAttribute());

	// Set initial values, invert logic.
	m_CheckInvert.SetCheck(shortener->InvertLogic());
	
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

void CDialogMyReductShortener::OnOK() 
{

	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(MYREDUCTSHORTENER)) {
		Message::Error("Algorithm not present or not a (simple) reduct shortener.");
		EndDialog(IDABORT);
		return;
	}

	Handle<MyReductShortener> shortener = dynamic_cast(MyReductShortener *, m_Algorithm.GetPointer());

	CString lowerpercentage_tmp;
	CString upperpercentage_tmp;

	// Get contents of percentage range edit controls.
	m_EditLowerPercentage.GetWindowText(lowerpercentage_tmp);
	m_EditUpperPercentage.GetWindowText(upperpercentage_tmp);

	String lowerpercentage = lowerpercentage_tmp.GetBuffer(0);
	String upperpercentage = upperpercentage_tmp.GetBuffer(0);

	if (!lowerpercentage.IsFloat() || !upperpercentage.IsFloat()) {
		Message::Error("Both lower and upper percentages must be floating point values.");
		EndDialog(IDABORT);
		return;
	}

	if ((lowerpercentage.GetFloat() < 0.0) || (lowerpercentage.GetFloat() > 100.0)) {
		Message::Warning("Lower percentage must be in the range [0.0, 100.0], truncating.");
		lowerpercentage = String::Format(MathKit::Truncate(0.0, lowerpercentage.GetFloat(), 100.0));
	}

	if ((upperpercentage.GetFloat() < 0.0) || (upperpercentage.GetFloat() > 100.0)) {
		Message::Warning("Upper percentage must be in the range [0.0, 100.0], truncating.");
		upperpercentage = String::Format(MathKit::Truncate(0.0, upperpercentage.GetFloat(), 100.0));
	}

	if (lowerpercentage.GetFloat() > upperpercentage.GetFloat()) {
		Message::Warning("Lower percentage cannot be above upper percentage, swapping.");
		String tmp = lowerpercentage;
		lowerpercentage = upperpercentage;
		upperpercentage = tmp;
	}

	int strategy = 0;

	// Set shortening strategy.
	if (m_CheckPercentage.GetCheck())
		strategy |= MyReductShortener::SHORTENING_PERCENTAGE;
	if (m_CheckAttribute.GetCheck())
		strategy |= MyReductShortener::SHORTENING_OCCURRENCE;
	if (m_CheckAttributeIf.GetCheck())
		strategy |= MyReductShortener::SHORTENING_COMBINEDOCCURRENCE;

	shortener->SetShorteningStrategy(strategy);

	// Set percentage range.
	shortener->SetLowerPercentage(lowerpercentage.GetFloat());
	shortener->SetUpperPercentage(upperpercentage.GetFloat());

	int attribute;

	// Set attributes.
	attribute = m_ComboAttribute.GetCurSel();
	if (attribute != CB_ERR)
		shortener->SetAttribute(attribute);

	attribute = m_ComboAttributePrimary.GetCurSel();
	if (attribute != CB_ERR)
		shortener->SetPrimaryAttribute(attribute);

	attribute = m_ComboAttributeSecondary.GetCurSel();
	if (attribute != CB_ERR)
		shortener->SetSecondaryAttribute(attribute);

	// Set invert logic flag.
	shortener->InvertLogic(m_CheckInvert.GetCheck());

	CDialogAlgorithm::OnOK();

}

void CDialogMyReductShortener::OnCheckAttribute() 
{
	
	m_ComboAttribute.EnableWindow(m_CheckAttribute.GetCheck());

}

void CDialogMyReductShortener::OnCheckAttributeIf() 
{

	m_ComboAttributePrimary.EnableWindow(m_CheckAttributeIf.GetCheck());
	m_ComboAttributeSecondary.EnableWindow(m_CheckAttributeIf.GetCheck());
	
}

void CDialogMyReductShortener::OnCheckPercentage() 
{

	m_EditLowerPercentage.EnableWindow(m_CheckPercentage.GetCheck());
	m_EditUpperPercentage.EnableWindow(m_CheckPercentage.GetCheck());
	
}
 
