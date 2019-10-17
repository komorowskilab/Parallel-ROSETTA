// dialogjohnsonreducer.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogjohnsonreducer.h>
#include <frontend\dialogs\algorithmdialogs\dialogjohnsonparameters.h>

#include <kernel\algorithms\johnsonreducer.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogJohnsonReducer dialog

//------------------------------------------------------------------
// Method........: Constructor
// Author........: 
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

CDialogJohnsonReducer::CDialogJohnsonReducer(CWnd* pParent /*=NULL*/)
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

BOOL CDialogJohnsonReducer::OnInitDialog() 
{

	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(JOHNSONREDUCER)) { 
		Message::Error("Algorithm not present or not a JohnsonReducer");
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

bool CDialogJohnsonReducer::HasAdvancedParameters() const
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

void CDialogJohnsonReducer::OnButtonAdvancedparameters() 
{

	CDialogJohnsonParameters dialog;

	dialog.SetAlgorithm(m_Algorithm.GetPointer(), m_Structure.GetPointer(), m_Project.GetPointer());

	dialog.DoModal();

}

 
