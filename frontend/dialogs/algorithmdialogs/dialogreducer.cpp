// dialogreducer.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogreducer.h>
#include <frontend\dialogs\algorithmdialogs\dialogobjectselector.h>

#include <kernel\algorithms\reducer.h>

#include <common\messagehelper.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogReducer dialog


CDialogReducer::CDialogReducer(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogReducer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogReducer)
	m_Filename = _T("");
	m_BRTThreshold = 0.1f;
	//}}AFX_DATA_INIT
}


void CDialogReducer::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogReducer)
	DDX_Control(pDX, IDC_GROUP_USEIDG, m_GroupUseIDG);
	DDX_Control(pDX, IDC_GROUP_PRECOMPUTEMATRIX, m_GroupPrecomputeMatrix);
	DDX_Control(pDX, IDC_CHECK_MODULO_DECISION, m_CheckModuloDecision);
	DDX_Control(pDX, IDC_BUTTON_OBJECTSELECTION, m_ButtonObjectSelection);
	DDX_Control(pDX, IDC_RADIO_DISCERNIBILITY_FULL, m_RadioDiscernibilityFull);
	DDX_Control(pDX, IDC_RADIO_DISCERNIBILITY_OBJECT, m_RadioDiscernibilityObject);
	DDX_Control(pDX, IDC_CHECK_USEIDG, m_CheckUseIDG);
	DDX_Control(pDX, IDC_CHECK_USEBRT, m_CheckUseBRT);
	DDX_Control(pDX, IDC_CHECK_PRECOMPUTEMATRIX, m_CheckPrecomputeMatrix);
	DDX_Control(pDX, IDC_STATIC_IDGTEXT, m_StaticIDGText);
	DDX_Control(pDX, IDC_STATIC_PRECOMPUTETEXT, m_StaticPrecomputeText);
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_EditFilename);
	DDX_Control(pDX, IDC_EDIT_THRESHOLD, m_EditBRTThreshold);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_Filename);
	DDX_Control(pDX, IDC_BUTTON_BROWSE, m_ButtonBrowse);
	DDX_Control(pDX, IDC_BUTTON_ADVANCEDPARAMS, m_ButtonAdvancedParameters);
	DDX_Text(pDX, IDC_EDIT_THRESHOLD, m_BRTThreshold);
	DDV_MinMaxFloat(pDX, m_BRTThreshold, 0.0f, 0.5f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogReducer, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogReducer)
	ON_BN_CLICKED(IDC_RADIO_DISCERNIBILITY_FULL, OnRadioDiscernibilityFull)
	ON_BN_CLICKED(IDC_RADIO_DISCERNIBILITY_OBJECT, OnRadioDiscernibilityObject)
	ON_BN_CLICKED(IDC_BUTTON_OBJECTSELECTION, OnButtonObjectselection)
	ON_BN_CLICKED(IDC_CHECK_MODULO_DECISION, OnCheckModuloDecision)
	ON_BN_CLICKED(IDC_CHECK_USEIDG, OnCheckUseIDG)
	ON_BN_CLICKED(IDC_CHECK_USEBRT, OnCheckUseBRT)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_ADVANCEDPARAMS, OnButtonAdvancedparameters)
	ON_BN_CLICKED(IDC_CHECK_PRECOMPUTEMATRIX, OnCheckPrecomputematrix)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogReducer message handlers

//------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogReducer::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();
	
	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(REDUCER)) { 
		Message::Error("Algorithm not present or not a Reducer");
		CDialog::OnCancel();
		return FALSE;
  }

	// Cast to verified type.
	Handle<Reducer> reducer = dynamic_cast(Reducer *, m_Algorithm.GetPointer());

	// Set window title.
	SetWindowText(IdHolder::GetDescription(reducer->GetId()).GetBuffer());

	// Initialize radio buttons.
	m_RadioDiscernibilityFull.SetCheck(reducer->GetDiscernibility() == Reduct::DISCERNIBILITY_FULL);
	m_RadioDiscernibilityObject.SetCheck(reducer->GetDiscernibility() == Reduct::DISCERNIBILITY_OBJECT);

	// Initialize modulo stuff.
	m_CheckModuloDecision.SetCheck(reducer->ModuloDecision());
	m_CheckUseBRT.SetCheck(reducer->UseBRT());
	if (HasBRTSupport())
		m_BRTThreshold = reducer->GetBRTPrecision();

	// Initialize object selection button.
	m_ButtonObjectSelection.EnableWindow(reducer->GetDiscernibility() == Reduct::DISCERNIBILITY_OBJECT);

	// Initialize IDG stuff.
	m_CheckUseIDG.SetCheck(reducer->UseIDG());
	m_Filename = reducer->GetIDGFilename().GetBuffer();

	// Initialize precompute stuff.
	m_CheckPrecomputeMatrix.SetCheck(reducer->PrecomputeMatrix());

	// Initialize advanced stuff.
	m_CheckUseBRT.EnableWindow(HasBRTSupport() && reducer->ModuloDecision());
	m_EditBRTThreshold.EnableWindow(HasBRTSupport() && reducer->ModuloDecision() && reducer->UseBRT());

	m_GroupUseIDG.EnableWindow(HasIDGSupport());
	m_CheckUseIDG.EnableWindow(HasIDGSupport());
	m_StaticIDGText.EnableWindow(HasIDGSupport());

	m_GroupPrecomputeMatrix.EnableWindow(HasPrecomputeSupport());
	m_CheckPrecomputeMatrix.EnableWindow(HasPrecomputeSupport());
	m_StaticPrecomputeText.EnableWindow(HasPrecomputeSupport());

	m_EditFilename.EnableWindow(HasIDGSupport() && reducer->UseIDG());
	m_ButtonBrowse.EnableWindow(HasIDGSupport() && reducer->UseIDG());

	m_ButtonAdvancedParameters.EnableWindow(HasAdvancedParameters());

	// Update data fields.
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogReducer::OnOK() 
{

	UpdateData(TRUE);

	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(REDUCER)) { 
		Message::Error("Algorithm not present or not a Reducer");
		EndDialog(IDABORT);
		return;
  }

	// Cast to verified type.
	Handle<Reducer> reducer = dynamic_cast(Reducer *, m_Algorithm.GetPointer());

	// Set discernibility type.
	if (m_RadioDiscernibilityFull.GetCheck())
		reducer->SetDiscernibility(Reduct::DISCERNIBILITY_FULL);
	else if (m_RadioDiscernibilityObject.GetCheck())
		reducer->SetDiscernibility(Reduct::DISCERNIBILITY_OBJECT);
	else
		Message::Warning("Discernibility type not set.");

	// Set modulo decision flag.
	reducer->ModuloDecision(m_CheckModuloDecision.GetCheck());

	// Set BRT stuff.
	reducer->UseBRT(m_CheckUseBRT.GetCheck());
	reducer->SetBRTPrecision(m_BRTThreshold);

	// Set IDG stuff.
	reducer->UseIDG(m_CheckUseIDG.GetCheck());
	reducer->SetIDGFilename(m_Filename.GetBuffer(0));

	// Set precompute flag.
	reducer->PrecomputeMatrix(m_CheckPrecomputeMatrix.GetCheck());
	
	CDialogAlgorithm::OnOK();

}

//------------------------------------------------------------------
// Method........: OnRadioDiscernibilityFull
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogReducer::OnRadioDiscernibilityFull() 
{

	m_ButtonObjectSelection.EnableWindow(FALSE);
	
	// Hack: The !#&%"%± resource should do this...
	m_RadioDiscernibilityFull.SetCheck(TRUE);
	m_RadioDiscernibilityObject.SetCheck(FALSE);

}

//------------------------------------------------------------------
// Method........: OnRadioDiscernibilityObject
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogReducer::OnRadioDiscernibilityObject() 
{

	m_ButtonObjectSelection.EnableWindow(TRUE);
	
	// Hack: The !#&%"%± resource should do this...
	m_RadioDiscernibilityFull.SetCheck(FALSE);
	m_RadioDiscernibilityObject.SetCheck(TRUE);

}

//------------------------------------------------------------------
// Method........: OnButtonObjectselection
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogReducer::OnButtonObjectselection() 
{

	Handle<Reducer> reducer = dynamic_cast(Reducer *, m_Algorithm.GetPointer());

	CDialogObjectSelector dialog;

	// Prepare object selection dialog.
	dialog.m_Selector      = reducer->GetObjectSelector();
	dialog.m_Table         = dynamic_cast(DecisionTable *, m_Structure.GetPointer());

	// Set object selection parameters.
	if (dialog.DoModal() == IDOK)
		reducer->SetObjectSelector(dialog.m_Selector);

}

//------------------------------------------------------------------
// Method........: OnCheckUseIDG
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogReducer::OnCheckUseIDG() 
{
	UpdateData(TRUE);
	m_EditFilename.EnableWindow(m_CheckUseIDG.GetCheck());
	m_ButtonBrowse.EnableWindow(m_CheckUseIDG.GetCheck());
}

//------------------------------------------------------------------
// Method........: OnCheckUseBRT
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogReducer::OnCheckUseBRT() 
{
	UpdateData(TRUE);
	m_EditBRTThreshold.EnableWindow(m_CheckUseBRT.GetCheck());
}

//------------------------------------------------------------------
// Method........: OnCheckModuloDecision
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogReducer::OnCheckModuloDecision() 
{
	UpdateData(TRUE);
	m_CheckUseBRT.EnableWindow(HasBRTSupport() && m_CheckModuloDecision.GetCheck());
	m_EditBRTThreshold.EnableWindow(HasBRTSupport() && m_CheckModuloDecision.GetCheck() && m_CheckUseBRT.GetCheck());
}

//------------------------------------------------------------------
// Method........: OnButtonBrowse
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogReducer::OnButtonBrowse() 
{
	CFileDialog fileDlg(TRUE, NULL, "*.*");
	if (fileDlg.DoModal() == IDOK)
		m_Filename = fileDlg.GetPathName();
	UpdateData(FALSE);
}

//------------------------------------------------------------------
// Method........: HasPrecomputeSupport
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Overload this for reducers with no such support.
// Revisions.....:
//===================================================================

bool CDialogReducer::HasPrecomputeSupport() const
{
	return true;
}

//------------------------------------------------------------------
// Method........: HasIDGSupport
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Overload this for reducers with no IDG support.
// Revisions.....:
//===================================================================

bool CDialogReducer::HasIDGSupport() const
{
	return true;
}

//------------------------------------------------------------------
// Method........: HasBRTSupport
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Overload this for reducers with no BRT support.
// Revisions.....:
//===================================================================

bool CDialogReducer::HasBRTSupport() const
{
	return true;
}

//------------------------------------------------------------------
// Method........: HasAdvancedParameters
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Overload this for reducers with special options.
// Revisions.....:
//===================================================================

bool CDialogReducer::HasAdvancedParameters() const
{
	return false;
}

//------------------------------------------------------------------
// Method........: OnButtonAdvancedparameters
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Overload this for reducers with special options.
// Revisions.....:
//===================================================================

void CDialogReducer::OnButtonAdvancedparameters() 
{
}

//------------------------------------------------------------------
// Method........: OnCheckPrecomputematrix
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogReducer::OnCheckPrecomputematrix() 
{

	UpdateData(TRUE);

	if (m_CheckPrecomputeMatrix.GetCheck()) {
		String text = "The option to precompute all relevant entries in the discernibility matrix\n"
			            "prior to simplification and reduction of the discernibility function may\n"
							    "give either a decrease or a slight increase in speed, depending on the data.\n"
									"However, this option will definitely require larger memory resources.\n"
									"\n"
									"This option is only really relevant if combined with approximate solutions\n"
									"in a hitting set framework, as it affects how the weights associated with\n"
									"each sum in the discernibility function are computed. Nevertheless, the results\n"
									"obtained when not precomputing the entries are often still acceptable.\n"
									"\n"
								  "Are you really sure you want to precompute the matrix entries?";
		if (MessageHelper::Query(text, "Precompute", MessageHelper::BUTTONS_YESNO) == MessageHelper::RESPONSE_NO) {
			m_CheckPrecomputeMatrix.SetCheck(FALSE);
			UpdateData(FALSE);
		}
	}
	
}
 
