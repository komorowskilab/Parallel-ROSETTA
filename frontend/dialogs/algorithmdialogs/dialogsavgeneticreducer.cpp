// dialogsavgeneticreducer.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogsavgeneticreducer.h>
#include <frontend\dialogs\algorithmdialogs\dialogsavgeneticparameters.h>

#include <kernel\sav\algorithms\savgeneticreducer.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogSAVGeneticReducer dialog

//------------------------------------------------------------------
// Method........: Constructor
// Author........: 
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

CDialogSAVGeneticReducer::CDialogSAVGeneticReducer(CWnd* pParent /*=NULL*/)
	: CDialogReducer(pParent)
{
}

//------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogSAVGeneticReducer::OnInitDialog() 
{

	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(SAVGENETICREDUCER)) { 
		Message::Error("Algorithm not present or not a SAVGeneticReducer");
		CDialog::OnCancel();
		return FALSE;
  }

	return CDialogReducer::OnInitDialog();

}

//------------------------------------------------------------------
// Method........: HasAdvancedParameters
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Overloaded
// Revisions.....:
//===================================================================

bool CDialogSAVGeneticReducer::HasAdvancedParameters() const
{
	return true;
}

//------------------------------------------------------------------
// Method........: OnButtonAdvancedparameters
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Overloaded
// Revisions.....:
//===================================================================

void CDialogSAVGeneticReducer::OnButtonAdvancedparameters() 
{

	Handle<SAVGeneticReducer> reducer = dynamic_cast(SAVGeneticReducer *, m_Algorithm.GetPointer());

	CDialogSAVGeneticParameters dialog;

	dialog.SetAlgorithm(m_Algorithm.GetPointer(), m_Structure.GetPointer(), m_Project.GetPointer());

	dialog.DoModal();

}

 
