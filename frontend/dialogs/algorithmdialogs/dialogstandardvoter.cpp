// dialogstandardvoter.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogstandardvoter.h>
#include <frontend\dialogs\algorithmdialogs\dialogstandardvoterparameters.h>

#include <kernel\algorithms\standardvoter.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogStandardVoter dialog


CDialogStandardVoter::CDialogStandardVoter(CWnd* pParent /*=NULL*/)
	: CDialogVoter(pParent)
{
	//{{AFX_DATA_INIT(CDialogStandardVoter)
	//}}AFX_DATA_INIT

	m_EnableAdvanced = TRUE;
	m_EnableGeneralization = TRUE;

}


/////////////////////////////////////////////////////////////////////////////
// CDialogStandardVoter message handlers

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogStandardVoter::OnInitDialog() 
{
	
	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(STANDARDVOTER)) { 
		Message::Error("Algorithm not present or of non-expected type.");
		EndDialog(IDABORT);
		return FALSE;
  }

	return CDialogVoter::OnInitDialog();

}

//------------------------------------------------------------------
// Method........: HasAdvancedParameters
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Overloaded
// Revisions.....:
//===================================================================

bool CDialogStandardVoter::HasAdvancedParameters() const
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

void CDialogStandardVoter::OnButtonAdvancedparameters() 
{

	CDialogStandardVoterParameters dialog;

	dialog.SetAlgorithm(m_Algorithm.GetPointer(), m_Structure.GetPointer(), m_Project.GetPointer());
  dialog.m_EnableGeneralization = m_EnableGeneralization;
	dialog.DoModal();

}
 
