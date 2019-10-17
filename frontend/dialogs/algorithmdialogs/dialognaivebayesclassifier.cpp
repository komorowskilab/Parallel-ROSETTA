// dialognaivebayesclassifier.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialognaivebayesclassifier.h>

#include <kernel\structures\project.h>
#include <kernel\structures\decisiontable.h>

#include <kernel\algorithms\naivebayesclassifier.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogNaiveBayesClassifier dialog


CDialogNaiveBayesClassifier::CDialogNaiveBayesClassifier(CWnd* pParent /*=NULL*/)
	: CDialogGenericList(pParent)
{
	//{{AFX_DATA_INIT(CDialogNaiveBayesClassifier)
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CDialogNaiveBayesClassifier, CDialogGenericList)
	//{{AFX_MSG_MAP(CDialogNaiveBayesClassifier)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogNaiveBayesClassifier message handlers

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogNaiveBayesClassifier::OnOK() 
{

	CDialogGenericList::OnOK();

  if (m_Selection < m_Available.size()) {
	  Handle<NaiveBayesClassifier> nb = dynamic_cast(NaiveBayesClassifier *, m_Algorithm.GetPointer());
	  Handle<DecisionTable> table = dynamic_cast(DecisionTable *, m_Available[m_Selection].GetPointer());
	  nb->SetDecisionTable(table);	
  }

	// Don't let the handles keep stuff on life-support...
	m_Available.erase(m_Available.begin(), m_Available.end());

}

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogNaiveBayesClassifier::OnInitDialog() 
{

	int i;

	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(NAIVEBAYESCLASSIFIER)) {
		Message::Error("Algorithm not present or of non-expected type.");
		EndDialog(IDABORT);
		return FALSE;
  }

	Handle<NaiveBayesClassifier> nb = dynamic_cast(NaiveBayesClassifier *, m_Algorithm.GetPointer());

	// Check if the project is set.
	if (m_Project.IsNULL()) { 
		Message::Error("Project not set.");
		EndDialog(IDABORT);
		return FALSE;
  }
  
	// Get all available decision tables.
	m_Project->GetAllIdentifiers(DECISIONTABLE, m_Available);

	// Initialize stuff.
	m_Header    = "Select a master table:";
	m_Title     = "Naive Bayes";
	m_CanEdit   = false;
	m_CanSelect = true;

	m_Entries.erase(m_Entries.begin(), m_Entries.end());
	m_Entries.reserve(m_Available.size());

	for (i = 0; i < m_Available.size(); i++) {
		m_Entries.push_back(dynamic_cast(const DecisionTable *, m_Available[i].GetPointer())->GetName());
	}

	return CDialogGenericList::OnInitDialog();

}

 
