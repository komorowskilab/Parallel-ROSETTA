// dialogfunctionalclassifiertemplatedefs.cpp : implementation file
//

#include <stdafx.h>
#include <frontend\rosetta.h>
#include <frontend\dialogs\algorithmdialogs\dialogfunctionalclassifiertemplatedefs.h>
#include <kernel\genomics\algorithms\functionalclassifier.h>
#include <kernel\utilities\mathkit.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogFunctionalClassifierTemplateDefs dialog


CDialogFunctionalClassifierTemplateDefs::CDialogFunctionalClassifierTemplateDefs(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogFunctionalClassifierTemplateDefs::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogFunctionalClassifierTemplateDefs)
	//}}AFX_DATA_INIT
}


void CDialogFunctionalClassifierTemplateDefs::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogFunctionalClassifierTemplateDefs)
	DDX_Control(pDX, IDC_STATIC_TO1, m_StaticTo1);
	DDX_Control(pDX, IDC_STATIC_TO2, m_StaticTo2);
	DDX_Control(pDX, IDC_STATIC_TO3, m_StaticTo3);
	DDX_Control(pDX, IDC_STATIC_TO4, m_StaticTo4);
	DDX_Control(pDX, IDC_STATIC_TO5, m_StaticTo5);
	DDX_Control(pDX, IDC_STATIC_TO6, m_StaticTo6);
	DDX_Control(pDX, IDC_STATIC_STEP1, m_StaticStep1);
	DDX_Control(pDX, IDC_STATIC_STEP2, m_StaticStep2);
	DDX_Control(pDX, IDC_STATIC_STEP3, m_StaticStep3);
	DDX_Control(pDX, IDC_STATIC_STEP4, m_StaticStep4);
	DDX_Control(pDX, IDC_STATIC_STEP5, m_StaticStep5);
	DDX_Control(pDX, IDC_STATIC_STEP6, m_StaticStep6);
	DDX_Control(pDX, IDC_STATIC_PARAMSFILE, m_StaticParamsFile);
	DDX_Control(pDX, IDC_EDIT_FILENAME_PARAMS, m_EditFilenameParams);
	DDX_Control(pDX, IDC_EDIT_CONSTANT_THR_FROM, m_EditConstantThrFrom);
	DDX_Control(pDX, IDC_EDIT_CONSTANT_THR_TO, m_EditConstantThrTo);
	DDX_Control(pDX, IDC_EDIT_CONSTANT_THR_STEP, m_EditConstantThrStep);
	DDX_Text(pDX, IDC_EDIT_CONSTANT_THR_FROM, m_ConstantThrFrom);
	DDX_Text(pDX, IDC_EDIT_CONSTANT_THR_TO, m_ConstantThrTo);
	DDX_Text(pDX, IDC_EDIT_CONSTANT_THR_STEP, m_ConstantThrStep);
	DDX_Control(pDX, IDC_SLIDER_CONSTANT_THR_FROM, m_SliderConstantThrFrom);
	DDX_Control(pDX, IDC_SLIDER_CONSTANT_THR_TO, m_SliderConstantThrTo);
	DDX_Control(pDX, IDC_SPIN_CONSTANT_THR_STEP, m_SpinConstantThrStep);
	DDX_Control(pDX, IDC_EDIT_TOLERANCE_THR_FROM, m_EditToleranceThrFrom);
	DDX_Control(pDX, IDC_EDIT_TOLERANCE_THR_TO, m_EditToleranceThrTo);
	DDX_Control(pDX, IDC_EDIT_TOLERANCE_THR_STEP, m_EditToleranceThrStep);
	DDX_Text(pDX, IDC_EDIT_TOLERANCE_THR_FROM, m_ToleranceThrFrom);
	DDX_Text(pDX, IDC_EDIT_TOLERANCE_THR_TO, m_ToleranceThrTo);
	DDX_Text(pDX, IDC_EDIT_TOLERANCE_THR_STEP, m_ToleranceThrStep);
	DDX_Control(pDX, IDC_SLIDER_TOLERANCE_THR_FROM, m_SliderToleranceThrFrom);
	DDX_Control(pDX, IDC_SLIDER_TOLERANCE_THR_TO, m_SliderToleranceThrTo);
	DDX_Control(pDX, IDC_SPIN_TOLERANCE_THR_STEP, m_SpinToleranceThrStep);
	DDX_Control(pDX, IDC_EDIT_CREASETHR_FROM, m_EditCreaseThrFrom);
	DDX_Control(pDX, IDC_EDIT_CREASETHR_TO, m_EditCreaseThrTo);
	DDX_Control(pDX, IDC_EDIT_CREASETHR_STEP, m_EditCreaseThrStep);
	DDX_Text(pDX, IDC_EDIT_CREASETHR_FROM, m_CreaseThrFrom);
	DDX_Text(pDX, IDC_EDIT_CREASETHR_TO, m_CreaseThrTo);
	DDX_Text(pDX, IDC_EDIT_CREASETHR_STEP, m_CreaseThrStep);
	DDX_Control(pDX, IDC_SLIDER_CREASETHR_FROM, m_SliderCreaseThrFrom);
	DDX_Control(pDX, IDC_SLIDER_CREASETHR_TO, m_SliderCreaseThrTo);
	DDX_Control(pDX, IDC_SPIN_CREASETHR_STEP, m_SpinCreaseThrStep);
	DDX_Control(pDX, IDC_EDIT_STARTEND_THR_FROM, m_EditStartEndThrFrom);
	DDX_Control(pDX, IDC_EDIT_STARTEND_THR_TO, m_EditStartEndThrTo);
	DDX_Control(pDX, IDC_EDIT_STARTEND_THR_STEP, m_EditStartEndThrStep);
	DDX_Text(pDX, IDC_EDIT_STARTEND_THR_FROM, m_StartEndThrFrom);
	DDX_Text(pDX, IDC_EDIT_STARTEND_THR_TO, m_StartEndThrTo);
	DDX_Text(pDX, IDC_EDIT_STARTEND_THR_STEP, m_StartEndThrStep);
	DDX_Control(pDX, IDC_SLIDER_STARTEND_THR_FROM, m_SliderStartEndThrFrom);
	DDX_Control(pDX, IDC_SLIDER_STARTEND_THR_TO, m_SliderStartEndThrTo);
	DDX_Control(pDX, IDC_SPIN_STARTEND_THR_STEP, m_SpinStartEndThrStep);
	DDX_Control(pDX, IDC_EDIT_TEMPLATE_LENGTH_FROM, m_EditTemplateLengthFrom);
	DDX_Control(pDX, IDC_EDIT_TEMPLATE_LENGTH_TO, m_EditTemplateLengthTo);
	DDX_Control(pDX, IDC_EDIT_TEMPLATE_LENGTH_STEP, m_EditTemplateLengthStep);
	DDX_Text(pDX, IDC_EDIT_TEMPLATE_LENGTH_FROM, m_TemplateLengthFrom);
	DDX_Text(pDX, IDC_EDIT_TEMPLATE_LENGTH_TO, m_TemplateLengthTo);
	DDX_Text(pDX, IDC_EDIT_TEMPLATE_LENGTH_STEP, m_TemplateLengthStep);
	DDX_Control(pDX, IDC_SLIDER_TEMPLATE_LENGTH_FROM, m_SliderTemplateLengthFrom);
	DDX_Control(pDX, IDC_SLIDER_TEMPLATE_LENGTH_TO, m_SliderTemplateLengthTo);
	DDX_Control(pDX, IDC_SPIN_TEMPLATE_LENGTH_STEP, m_SpinTemplateLengthStep);
	DDX_Control(pDX, IDC_EDIT_CONSTANT_TEMPLATE_LENGTH_FROM, m_EditConstantTemplateLengthFrom);
	DDX_Control(pDX, IDC_EDIT_CONSTANT_TEMPLATE_LENGTH_TO, m_EditConstantTemplateLengthTo);
	DDX_Control(pDX, IDC_EDIT_CONSTANT_TEMPLATE_LENGTH_STEP, m_EditConstantTemplateLengthStep);
	DDX_Text(pDX, IDC_EDIT_CONSTANT_TEMPLATE_LENGTH_FROM, m_ConstantTemplateLengthFrom);
	DDX_Text(pDX, IDC_EDIT_CONSTANT_TEMPLATE_LENGTH_TO, m_ConstantTemplateLengthTo);
	DDX_Text(pDX, IDC_EDIT_CONSTANT_TEMPLATE_LENGTH_STEP, m_ConstantTemplateLengthStep);
	DDX_Control(pDX, IDC_SLIDER_CONSTANT_TEMPLATE_LENGTH_FROM, m_SliderConstantTemplateLengthFrom);
	DDX_Control(pDX, IDC_SLIDER_CONSTANT_TEMPLATE_LENGTH_TO, m_SliderConstantTemplateLengthTo);
	DDX_Control(pDX, IDC_SPIN_CONSTANT_TEMPLATE_LENGTH_STEP, m_SpinConstantTemplateLengthStep);
	DDX_Control(pDX, IDC_BUTTON_BROWSE_PARAMS, m_ButtonBrowseParams);
	DDX_Check(pDX, IDC_CHECK_ITERATE, m_CheckIterate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogFunctionalClassifierTemplateDefs, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogFunctionalClassifierTemplateDefs)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_PARAMS, OnButtonBrowseParams)
	ON_BN_CLICKED(IDC_CHECK_ITERATE, OnCheckIterate)
	ON_EN_UPDATE(IDC_EDIT_CONSTANT_TEMPLATE_LENGTH_FROM, OnUpdateEditConstantTemplateLengthFrom)
	ON_EN_UPDATE(IDC_EDIT_CONSTANT_TEMPLATE_LENGTH_TO, OnUpdateEditConstantTemplateLengthTo)
	ON_EN_UPDATE(IDC_EDIT_CONSTANT_THR_FROM, OnUpdateEditConstantThrFrom)
	ON_EN_UPDATE(IDC_EDIT_CONSTANT_THR_TO, OnUpdateEditConstantThrTo)
	ON_EN_UPDATE(IDC_EDIT_CREASETHR_FROM, OnUpdateEditCreasethrFrom)
	ON_EN_UPDATE(IDC_EDIT_CREASETHR_TO, OnUpdateEditCreasethrTo)
	ON_EN_UPDATE(IDC_EDIT_STARTEND_THR_FROM, OnUpdateEditStartendThrFrom)
	ON_EN_UPDATE(IDC_EDIT_STARTEND_THR_TO, OnUpdateEditStartendThrTo)
	ON_EN_UPDATE(IDC_EDIT_TEMPLATE_LENGTH_FROM, OnUpdateEditTemplateLengthFrom)
	ON_EN_UPDATE(IDC_EDIT_TEMPLATE_LENGTH_TO, OnUpdateEditTemplateLengthTo)
	ON_EN_UPDATE(IDC_EDIT_TOLERANCE_THR_FROM, OnUpdateEditToleranceThrFrom)
	ON_EN_UPDATE(IDC_EDIT_TOLERANCE_THR_TO, OnUpdateEditToleranceThrTo)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CONSTANT_TEMPLATE_LENGTH_FROM, OnCustomdrawSliderConstantTemplateLengthFrom)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CONSTANT_TEMPLATE_LENGTH_TO, OnCustomdrawSliderConstantTemplateLengthTo)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CONSTANT_THR_FROM, OnCustomdrawSliderConstantThrFrom)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CONSTANT_THR_TO, OnCustomdrawSliderConstantThrTo)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CREASETHR_FROM, OnCustomdrawSliderCreasethrFrom)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CREASETHR_TO, OnCustomdrawSliderCreasethrTo)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_STARTEND_THR_FROM, OnCustomdrawSliderStartendThrFrom)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_STARTEND_THR_TO, OnCustomdrawSliderStartendThrTo)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_TEMPLATE_LENGTH_FROM, OnCustomdrawSliderTemplateLengthFrom)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_TEMPLATE_LENGTH_TO, OnCustomdrawSliderTemplateLengthTo)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_TOLERANCE_THR_FROM, OnCustomdrawSliderToleranceThrFrom)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_TOLERANCE_THR_TO, OnCustomdrawSliderToleranceThrTo)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CREASETHR_STEP, OnDeltaposSpinCreasethrStep)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_STARTEND_THR_STEP, OnDeltaposSpinStartendThrStep)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TOLERANCE_THR_STEP, OnDeltaposSpinToleranceThrStep)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CONSTANT_THR_STEP, OnDeltaposSpinConstantThrStep)
	ON_EN_UPDATE(IDC_EDIT_TOLERANCE_THR_STEP, OnUpdateEditToleranceThrStep)
	ON_EN_UPDATE(IDC_EDIT_STARTEND_THR_STEP, OnUpdateEditStartendThrStep)
	ON_EN_UPDATE(IDC_EDIT_CREASETHR_STEP, OnUpdateEditCreasethrStep)
	ON_EN_UPDATE(IDC_EDIT_CONSTANT_THR_STEP, OnUpdateEditConstantThrStep)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogFunctionalClassifierTemplateDefs message handlers

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifierTemplateDefs::OnButtonBrowseParams() 
{
	UpdateData(TRUE);

	CFileDialog fileDlg(TRUE, NULL, "*.*");
  
	if (fileDlg.DoModal() == IDOK) {
		CString fullpath = fileDlg.GetPathName();
		m_EditFilenameParams.SetWindowText(fullpath);
	}

	UpdateData(FALSE);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifierTemplateDefs::OnCheckIterate() 
{
	UpdateData(TRUE);
	
	//Enable or disable items
	m_StaticTo1.EnableWindow(m_CheckIterate);
	m_StaticTo2.EnableWindow(m_CheckIterate);
	m_StaticTo3.EnableWindow(m_CheckIterate);
	m_StaticTo4.EnableWindow(m_CheckIterate);
	m_StaticTo5.EnableWindow(m_CheckIterate);
	m_StaticTo6.EnableWindow(m_CheckIterate);
	m_StaticStep1.EnableWindow(m_CheckIterate);
	m_StaticStep2.EnableWindow(m_CheckIterate);
	m_StaticStep3.EnableWindow(m_CheckIterate);
	m_StaticStep4.EnableWindow(m_CheckIterate);
	m_StaticStep5.EnableWindow(m_CheckIterate);
	m_StaticStep6.EnableWindow(m_CheckIterate);
	m_StaticParamsFile.EnableWindow(m_CheckIterate);
	m_EditFilenameParams.EnableWindow(m_CheckIterate);
	m_EditConstantThrTo.EnableWindow(m_CheckIterate);
	m_EditConstantThrStep.EnableWindow(m_CheckIterate);
	m_SliderConstantThrTo.EnableWindow(m_CheckIterate);
	m_SpinConstantThrStep.EnableWindow(m_CheckIterate);
	m_EditToleranceThrTo.EnableWindow(m_CheckIterate);
	m_EditToleranceThrStep.EnableWindow(m_CheckIterate);
	m_SliderToleranceThrTo.EnableWindow(m_CheckIterate);
	m_SpinToleranceThrStep.EnableWindow(m_CheckIterate);
	m_EditCreaseThrTo.EnableWindow(m_CheckIterate);
	m_EditCreaseThrStep.EnableWindow(m_CheckIterate);
	m_SliderCreaseThrTo.EnableWindow(m_CheckIterate);
	m_SpinCreaseThrStep.EnableWindow(m_CheckIterate);
	m_EditStartEndThrTo.EnableWindow(m_CheckIterate);
	m_EditStartEndThrStep.EnableWindow(m_CheckIterate);
	m_SliderStartEndThrTo.EnableWindow(m_CheckIterate);
	m_SpinStartEndThrStep.EnableWindow(m_CheckIterate);
	m_EditTemplateLengthTo.EnableWindow(m_CheckIterate);
	m_EditTemplateLengthStep.EnableWindow(m_CheckIterate);
	m_SliderTemplateLengthTo.EnableWindow(m_CheckIterate);
	m_SpinTemplateLengthStep.EnableWindow(m_CheckIterate);
	m_EditConstantTemplateLengthTo.EnableWindow(m_CheckIterate);
	m_EditConstantTemplateLengthStep.EnableWindow(m_CheckIterate);
	m_SliderConstantTemplateLengthTo.EnableWindow(m_CheckIterate);
	m_SpinConstantTemplateLengthStep.EnableWindow(m_CheckIterate);
	m_ButtonBrowseParams.EnableWindow(m_CheckIterate);

	UpdateData(FALSE);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifierTemplateDefs::OnUpdateEditField(float &edit, CSliderCtrl &slider, float resolution /*=100.0*/)
{
	UpdateData(TRUE);

	float min_pos = static_cast(float,slider.GetRangeMin()) / resolution;
	float max_pos = static_cast(float,slider.GetRangeMax()) / resolution;

	// Commented out, causes unwanted things...
	/*
	if (edit < min_pos) {
		edit = min_pos;
	}
	else if (edit > max_pos){
		edit = max_pos;
	}
	*/

	if (edit >= min_pos && edit <= max_pos)
		slider.SetPos(MathKit::Round(edit * resolution));
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifierTemplateDefs::OnUpdateEditField(int &edit, CSliderCtrl &slider)
{
	UpdateData(TRUE);
	
	if (edit < slider.GetRangeMin()) {
		edit = slider.GetRangeMin();
	}
	else if (edit > slider.GetRangeMax()){
		edit = slider.GetRangeMax();
	}
	slider.SetPos(edit);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifierTemplateDefs::OnCustomdrawSlider(CSliderCtrl &slider1, CSliderCtrl &slider2, bool low, int &edit)
{
	edit = slider1.GetPos();
	int pos2 = slider2.GetPos();

	if ((low && edit > pos2) ||
		(!low && edit < pos2)) {
	
		pos2 = edit;
		slider2.SetPos(pos2);
	}

	UpdateData(FALSE);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifierTemplateDefs::OnCustomdrawSlider(CSliderCtrl &slider1, CSliderCtrl &slider2, bool low, float &edit, float resolution /*=100.0*/)
{
	edit = static_cast(float,slider1.GetPos()) / resolution;
	float pos2 = static_cast(float,slider2.GetPos()) / resolution;

	if ((low && edit > pos2) ||
		(!low && edit < pos2)) {
	
		pos2 = edit;
		slider2.SetPos(MathKit::Round(pos2 * resolution));
	}

	UpdateData(FALSE);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifierTemplateDefs::OnDeltaposSpinStep(NMHDR* pNMHDR, float &edit, float resolution /*= 100.0*/) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	edit = static_cast(float,pNMUpDown->iPos) / resolution;
	
	UpdateData(FALSE);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifierTemplateDefs::OnUpdateEditStep(CSpinButtonCtrl &spin, float &edit) 
{
	UpdateData(TRUE);
	
	int max;
	int min;
	
	spin.GetRange(min,max);

	int pos = MathKit::Round(edit * 100.0);
	
	if (pos > max) {
		pos = max;
		edit = static_cast(float,pos) / 100.0;
	}
	else if (pos < min) {
		pos = min;
		edit = static_cast(float,pos) / 100.0;
	}

	spin.SetPos(pos);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifierTemplateDefs::OnOK() 
{

	float min = 0.01f;
	float max = 10.0f;
	int min_int = 1;
	int max_int = 10;

	// Check values
	if (m_TemplateLengthFrom < min_int || m_TemplateLengthTo < min_int || m_TemplateLengthStep < min_int ||
		m_TemplateLengthFrom > max_int || m_TemplateLengthTo > max_int || m_TemplateLengthStep > max_int ||
		m_ConstantTemplateLengthFrom < min_int || m_ConstantTemplateLengthTo < min_int || m_ConstantTemplateLengthStep < min_int ||
		m_ConstantTemplateLengthFrom > max_int || m_ConstantTemplateLengthTo > max_int || m_ConstantTemplateLengthStep > max_int ||
		m_CreaseThrFrom < min || m_CreaseThrTo < min || m_CreaseThrStep < min ||
		m_CreaseThrFrom > max || m_CreaseThrTo > max || m_CreaseThrStep > max ||
		m_ConstantThrFrom < min || m_ConstantThrTo < min || m_ConstantThrStep < min ||
		m_ConstantThrFrom > max || m_ConstantThrTo > max || m_ConstantThrStep > max ||
		m_StartEndThrFrom < min || m_StartEndThrTo < min || m_StartEndThrStep < min ||
		m_StartEndThrFrom > max || m_StartEndThrTo > max || m_StartEndThrStep > max ||
		m_ToleranceThrFrom < min || m_ToleranceThrTo < min || m_ToleranceThrStep < min ||
		m_ToleranceThrFrom > max || m_ToleranceThrTo > max || m_ToleranceThrStep > max) {
		
		Message::Error("Bad template parameter settings, \nfloating points below 0.01 or above 10.0 and\ndiscrete values below 1 or above 10 are not allowed.");
		return;
	}

	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(FUNCTIONALCLASSIFIER)) { 
		Message::Error("Algorithm not present or not a FunctionalClassifier");
		EndDialog(IDABORT);
		return;
	}

	Handle <FunctionalClassifier> executor = dynamic_cast(FunctionalClassifier*, m_Algorithm.GetPointer());

	CString filename;
	m_EditFilenameParams.GetWindowText(filename);
	executor->SetParamsLogFile(filename.GetBuffer(0));

	executor->SetIterateCV(m_CheckIterate);
	// Set template parameters
	executor->SetTemplateLength(m_TemplateLengthFrom, m_TemplateLengthTo, m_TemplateLengthStep);
	executor->SetConstantTemplateLength(m_ConstantTemplateLengthFrom, m_ConstantTemplateLengthTo, m_ConstantTemplateLengthStep);
	executor->SetCreaseThr(m_CreaseThrFrom, m_CreaseThrTo, m_CreaseThrStep);
	executor->SetConstantThr(m_ConstantThrFrom, m_ConstantThrTo, m_ConstantThrStep);
	executor->SetStartEndThr(m_StartEndThrFrom, m_StartEndThrTo, m_StartEndThrStep);
	executor->SetToleranceThr(m_ToleranceThrFrom, m_ToleranceThrTo, m_ToleranceThrStep);

	CDialogAlgorithm::OnOK();
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

BOOL CDialogFunctionalClassifierTemplateDefs::OnInitDialog() 
{
	CDialogAlgorithm::OnInitDialog();

	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(FUNCTIONALCLASSIFIER)) { 
		Message::Error("Algorithm not present or not a FunctionalClassifier");
		CDialog::OnCancel();
		return false;
	}

	// Cast to verified type
	Handle<FunctionalClassifier> alg = dynamic_cast(FunctionalClassifier*, m_Algorithm.GetPointer());

	// Init values
	m_CheckIterate = alg->GetIterateCV();
	alg->GetTemplateLength(m_TemplateLengthFrom, m_TemplateLengthTo, m_TemplateLengthStep);
	alg->GetConstantTemplateLength(m_ConstantTemplateLengthFrom, m_ConstantTemplateLengthTo, m_ConstantTemplateLengthStep);
	alg->GetCreaseThr(m_CreaseThrFrom, m_CreaseThrTo, m_CreaseThrStep);
	alg->GetConstantThr(m_ConstantThrFrom, m_ConstantThrTo,	m_ConstantThrStep);
	alg->GetStartEndThr(m_StartEndThrFrom, m_StartEndThrTo,	m_StartEndThrStep);
	alg->GetToleranceThr(m_ToleranceThrFrom, m_ToleranceThrTo, m_ToleranceThrStep);

	m_EditFilenameParams.SetWindowText(alg->GetParamsLogFile().GetBuffer());

	float resolution = 100.0;

	m_SliderConstantTemplateLengthFrom.SetRange(1,10);
	m_SliderConstantTemplateLengthTo.SetRange(1,10);
	m_SliderTemplateLengthFrom.SetRange(1,10);
	m_SliderTemplateLengthTo.SetRange(1,10);
	m_SliderConstantThrFrom.SetRange(1,MathKit::Round(10 * resolution));
	m_SliderConstantThrTo.SetRange(1,MathKit::Round(10 * resolution));
	m_SliderCreaseThrFrom.SetRange(1,MathKit::Round(10 * resolution));
	m_SliderCreaseThrTo.SetRange(1,MathKit::Round(10 * resolution));
	m_SliderStartEndThrFrom.SetRange(1,MathKit::Round(10 * resolution));
	m_SliderStartEndThrTo.SetRange(1,MathKit::Round(10 * resolution));
	m_SliderToleranceThrFrom.SetRange(1,MathKit::Round(10 * resolution));
	m_SliderToleranceThrTo.SetRange(1,MathKit::Round(10 * resolution));

	m_SliderConstantTemplateLengthFrom.SetPos(m_ConstantTemplateLengthFrom);
	m_SliderConstantTemplateLengthTo.SetPos(m_ConstantTemplateLengthTo);
	m_SliderTemplateLengthFrom.SetPos(m_TemplateLengthFrom);
	m_SliderTemplateLengthTo.SetPos(m_TemplateLengthTo);
	m_SliderConstantThrFrom.SetPos(MathKit::Round(m_ConstantThrFrom * resolution));
	m_SliderConstantThrTo.SetPos(MathKit::Round(m_ConstantThrTo * resolution));
	m_SliderCreaseThrFrom.SetPos(MathKit::Round(m_CreaseThrFrom * resolution));
	m_SliderCreaseThrTo.SetPos(MathKit::Round(m_CreaseThrTo * resolution));
	m_SliderStartEndThrFrom.SetPos(MathKit::Round(m_StartEndThrFrom * resolution));
	m_SliderStartEndThrTo.SetPos(MathKit::Round(m_StartEndThrTo * resolution));
	m_SliderToleranceThrFrom.SetPos(MathKit::Round(m_ToleranceThrFrom * resolution));
	m_SliderToleranceThrTo.SetPos(MathKit::Round(m_ToleranceThrTo * resolution));
	
	m_SpinConstantTemplateLengthStep.SetRange(1,10);
	m_SpinTemplateLengthStep.SetRange(1,10);
	m_SpinConstantThrStep.SetRange(1,MathKit::Round(10 * resolution));
	m_SpinCreaseThrStep.SetRange(1,MathKit::Round(10 * resolution));
	m_SpinStartEndThrStep.SetRange(1,MathKit::Round(10 * resolution));
	m_SpinToleranceThrStep.SetRange(1,MathKit::Round(10 * resolution));

	m_SpinConstantTemplateLengthStep.SetPos(MathKit::Round(m_ConstantTemplateLengthStep * resolution));
	m_SpinTemplateLengthStep.SetPos(MathKit::Round(m_TemplateLengthStep * resolution));
	m_SpinConstantThrStep.SetPos(MathKit::Round(m_ConstantThrStep * resolution));
	m_SpinCreaseThrStep.SetPos(MathKit::Round(m_CreaseThrStep * resolution));
	m_SpinStartEndThrStep.SetPos(MathKit::Round(m_StartEndThrStep * resolution));
	m_SpinToleranceThrStep.SetPos(MathKit::Round(m_ToleranceThrStep * resolution));

	//Enable or disable items
	m_StaticTo1.EnableWindow(m_CheckIterate);
	m_StaticTo2.EnableWindow(m_CheckIterate);
	m_StaticTo3.EnableWindow(m_CheckIterate);
	m_StaticTo4.EnableWindow(m_CheckIterate);
	m_StaticTo5.EnableWindow(m_CheckIterate);
	m_StaticTo6.EnableWindow(m_CheckIterate);
	m_StaticStep1.EnableWindow(m_CheckIterate);
	m_StaticStep2.EnableWindow(m_CheckIterate);
	m_StaticStep3.EnableWindow(m_CheckIterate);
	m_StaticStep4.EnableWindow(m_CheckIterate);
	m_StaticStep5.EnableWindow(m_CheckIterate);
	m_StaticStep6.EnableWindow(m_CheckIterate);
	m_StaticParamsFile.EnableWindow(m_CheckIterate);
	m_EditFilenameParams.EnableWindow(m_CheckIterate);
	m_EditConstantThrTo.EnableWindow(m_CheckIterate);
	m_EditConstantThrStep.EnableWindow(m_CheckIterate);
	m_SliderConstantThrTo.EnableWindow(m_CheckIterate);
	m_SpinConstantThrStep.EnableWindow(m_CheckIterate);
	m_EditToleranceThrTo.EnableWindow(m_CheckIterate);
	m_EditToleranceThrStep.EnableWindow(m_CheckIterate);
	m_SliderToleranceThrTo.EnableWindow(m_CheckIterate);
	m_SpinToleranceThrStep.EnableWindow(m_CheckIterate);
	m_EditCreaseThrTo.EnableWindow(m_CheckIterate);
	m_EditCreaseThrStep.EnableWindow(m_CheckIterate);
	m_SliderCreaseThrTo.EnableWindow(m_CheckIterate);
	m_SpinCreaseThrStep.EnableWindow(m_CheckIterate);
	m_EditStartEndThrTo.EnableWindow(m_CheckIterate);
	m_EditStartEndThrStep.EnableWindow(m_CheckIterate);
	m_SliderStartEndThrTo.EnableWindow(m_CheckIterate);
	m_SpinStartEndThrStep.EnableWindow(m_CheckIterate);
	m_EditTemplateLengthTo.EnableWindow(m_CheckIterate);
	m_EditTemplateLengthStep.EnableWindow(m_CheckIterate);
	m_SliderTemplateLengthTo.EnableWindow(m_CheckIterate);
	m_SpinTemplateLengthStep.EnableWindow(m_CheckIterate);
	m_EditConstantTemplateLengthTo.EnableWindow(m_CheckIterate);
	m_EditConstantTemplateLengthStep.EnableWindow(m_CheckIterate);
	m_SliderConstantTemplateLengthTo.EnableWindow(m_CheckIterate);
	m_SpinConstantTemplateLengthStep.EnableWindow(m_CheckIterate);
	m_ButtonBrowseParams.EnableWindow(m_CheckIterate);

	// Update fields
	UpdateData(FALSE);

	return TRUE;
}
 
