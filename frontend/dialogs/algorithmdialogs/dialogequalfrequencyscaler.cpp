// dialogequalfrequencyscaler.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogequalfrequencyscaler.h>
#include <frontend\dialogs\algorithmdialogs\dialogequalfrequencyscalerparameters.h>

#include <kernel\algorithms\equalfrequencyscaler.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogEqualFrequencyScaler dialog

//------------------------------------------------------------------
// Method........: Constructor
// Author........: 
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

CDialogEqualFrequencyScaler::CDialogEqualFrequencyScaler(CWnd* pParent /*=NULL*/)
	: CDialogOrthogonalScaler(pParent)
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

BOOL CDialogEqualFrequencyScaler::OnInitDialog() 
{

	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(EQUALFREQUENCYSCALER)) { 
		Message::Error("Algorithm not present or of wrong type.");
		CDialog::OnCancel();
		return FALSE;
  }

	return CDialogOrthogonalScaler::OnInitDialog();

}

//------------------------------------------------------------------
// Method........: HasAdvancedParameters
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Overloaded
// Revisions.....:
//===================================================================

bool CDialogEqualFrequencyScaler::HasAdvancedParameters() const
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

void CDialogEqualFrequencyScaler::OnButtonAdvancedparameters() 
{

	Handle<EqualFrequencyScaler> scaler = dynamic_cast(EqualFrequencyScaler *, m_Algorithm.GetPointer());

	CDialogEqualFrequencyScalerParameters dialog;

	dialog.SetAlgorithm(m_Algorithm.GetPointer(), m_Structure.GetPointer(), m_Project.GetPointer());

	dialog.DoModal();

}

 
