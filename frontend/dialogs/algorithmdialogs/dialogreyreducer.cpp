// dialogreyreducer.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogreyreducer.h>
//#include <frontend\dialogs\algorithmdialogs\dialogreyparameters.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogReyReducer dialog

//------------------------------------------------------------------
// Method........: Constructor
// Author........: 
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

CDialogReyReducer::CDialogReyReducer(CWnd* pParent /*=NULL*/)
	: CDialogReducer(pParent)
{
}

//------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogReyReducer::OnInitDialog() 
{

	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(REYREDUCER)) { 
		Message::Error("Algorithm not present or not a ReyReducer");
		CDialog::OnCancel();
		return FALSE;
  }

	return CDialogReducer::OnInitDialog();

}

//------------------------------------------------------------------
// Method........: HasAdvancedParameters
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: Overloaded
// Revisions.....:
//===================================================================

bool CDialogReyReducer::HasAdvancedParameters() const
{
	return false;
}

//------------------------------------------------------------------
// Method........: OnButtonAdvancedparameters
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: Overloaded, Inactive
// Revisions.....: 
//===================================================================

//void CDialogReyReducer::OnButtonAdvancedparameters() 
//{

	//CDialogReyParameters dialog;

	//dialog.SetAlgorithm(m_Algorithm.GetPointer(), m_Structure.GetPointer(), m_Project.GetPointer());

	//dialog.DoModal();

//}
 
