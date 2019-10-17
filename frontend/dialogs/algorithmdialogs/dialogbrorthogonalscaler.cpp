// dialogbrorthogonalscaler.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogbrorthogonalscaler.h>
#include <frontend\dialogs\algorithmdialogs\dialogjohnsonparameters.h>

#include <kernel\algorithms\brorthogonalscaler.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogBROrthogonalScaler dialog

//------------------------------------------------------------------
// Method........: Constructor
// Author........: 
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

CDialogBROrthogonalScaler::CDialogBROrthogonalScaler(CWnd* pParent /*=NULL*/)
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

BOOL CDialogBROrthogonalScaler::OnInitDialog() 
{

	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(BRORTHOGONALSCALER)) { 
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

bool CDialogBROrthogonalScaler::HasAdvancedParameters() const
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

void CDialogBROrthogonalScaler::OnButtonAdvancedparameters() 
{

	Handle<BROrthogonalScaler> scaler = dynamic_cast(BROrthogonalScaler *, m_Algorithm.GetPointer());

	CDialogJohnsonParameters dialog;

	JohnsonReducer reducer;

	reducer.ApproximateSolutions(scaler->ApproximateSolutions());
	reducer.SetHittingFraction(scaler->GetHittingFraction());

	dialog.SetAlgorithm(&reducer, m_Structure.GetPointer(), m_Project.GetPointer());

	if (dialog.DoModal() == IDOK) {
		scaler->ApproximateSolutions(reducer.ApproximateSolutions());
		scaler->SetHittingFraction(reducer.GetHittingFraction());
	}

}

 
