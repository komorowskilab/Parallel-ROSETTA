// algorithmdialog.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

#include <kernel\algorithms\algorithm.h>

#include <common\objectmanager.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogAlgorithm dialog


CDialogAlgorithm::CDialogAlgorithm(UINT DialogID, CWnd* pParent /*=NULL*/)
	//: CDialog(CDialogAlgorithm::IDD, pParent)
  : CDialog(DialogID, pParent)
{
	//{{AFX_DATA_INIT(CDialogAlgorithm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
}


void CDialogAlgorithm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogAlgorithm)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogAlgorithm, CDialog)
	//{{AFX_MSG_MAP(CDialogAlgorithm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogAlgorithm message handlers


void CDialogAlgorithm::SetAlgorithm(Algorithm *algorithm, Structure *structure, Project *project)
{
	m_Algorithm = algorithm;
	m_Structure = structure;
	m_Project   = project;
}

 
