// dialogrosettamode.cpp : implementation file
//

#include <stdafx.h>
#include <frontend\rosetta.h>
#include <frontend\dialogs\dialogrosettamode.h>
//#include <common\objectmanager.h>
#include <common\modemanager.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogRosettaMode dialog


CDialogRosettaMode::CDialogRosettaMode(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogRosettaMode::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogRosettaMode)
	//}}AFX_DATA_INIT
}


void CDialogRosettaMode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogRosettaMode)
	DDX_Control(pDX, ID_BUTTON_STD_ROSETTA, m_ButtonStdRosetta);
	DDX_Control(pDX, ID_BUTTON_GEN_ROSETTA, m_ButtonGenRosetta);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogRosettaMode, CDialog)
	//{{AFX_MSG_MAP(CDialogRosettaMode)
	ON_BN_CLICKED(ID_BUTTON_GEN_ROSETTA, OnButtonGenRosetta)
	ON_BN_CLICKED(ID_BUTTON_STD_ROSETTA, OnButtonStdRosetta)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogRosettaMode message handlers

BOOL CDialogRosettaMode::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_RosBmp.LoadBitmap(IDB_ROSETTA);
	HBITMAP hBitmap1 = (HBITMAP)m_RosBmp.GetSafeHandle();
	m_ButtonStdRosetta.SetBitmap(hBitmap1);

	m_GenBmp.LoadBitmap(IDB_GENOMIC_ROSETTA);
	HBITMAP hBitmap2 = (HBITMAP)m_GenBmp.GetSafeHandle();
	m_ButtonGenRosetta.SetBitmap(hBitmap2);
		
	return TRUE;
}

void CDialogRosettaMode::OnButtonGenRosetta() 
{
	ModeManager::SetMode(ModeManager::GENOMIC);
	CDialog::OnOK();
}

void CDialogRosettaMode::OnButtonStdRosetta() 
{
	ModeManager::SetMode(ModeManager::ROSETTA);
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////
///// Others

 
