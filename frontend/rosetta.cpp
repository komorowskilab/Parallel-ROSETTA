#include <stdafx.h>

#include <frontend\rosetta.h>
#include <frontend\mainfrm.h>
#include <frontend\childfrm.h>
#include <frontend\rosettadoc.h>
#include <frontend\splash.h>

#include <common\configuration.h>
#include <common\modemanager.h>
#include <common\installer.h>

#include <frontend\views\viewproject.h>
#include <frontend\views\viewdecisiontable.h>
#include <frontend\views\viewreducts.h>
#include <frontend\views\viewrules.h>
#include <frontend\views\viewpatterns.h>
#include <frontend\views\viewbatchclassification.h>
#include <frontend\views\viewtextfile.h>
#include <frontend\views\viewpartition.h>
#include <frontend\views\viewapproximation.h>

//Robin Andersson 0406
#include <frontend\dialogs\dialogrosettamode.h>
//
//Robin Andersson 050316
// LL
#include <frontend\dialogs\dialoglicense.h>
#include <kernel\utilities\license.h>
//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRosettaApp

BEGIN_MESSAGE_MAP(CRosettaApp, CWinApp)
//{{AFX_MSG_MAP(CRosettaApp)
ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
// NOTE - the ClassWizard will add and remove mapping macros here.
//	  DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
// Standard file based document commands
ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRosettaApp construction

CRosettaApp::CRosettaApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	
	m_TreeTemplate				  = NULL;
	m_DecisionTableTemplate 	  = NULL;
	m_ProgressTemplate			  = NULL;
	m_ReductsTemplate			  = NULL;
	m_RulesTemplate 			  = NULL;
	m_BatchClassificationTemplate = NULL;
	m_PartitionTemplate 		  = NULL;
	m_ApproximationTemplate 	  = NULL;
	m_TextFileTemplate			  = NULL;
	
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRosettaApp object

CRosettaApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRosettaApp initialization

//-------------------------------------------------------------------
// Method  ......: InitInstance
// Author........: MFC Wizard + Aš
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....: 970316 KMR - Only one documenttempl. registered
//                 040627 RA - Added dialog for mode selection
//===================================================================

BOOL CRosettaApp::InitInstance()
{
	
	{
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);
		
		CWndSplash::EnableSplashScreen(cmdInfo.m_bShowSplash);
	}
	
	SetDialogBkColor();
	
	GXInit();

		if (!License::CheckLicense()){
		return 0;
	};


	InstallStructures();
	InstallAlgorithms();
	
	//Robin Andersson 0406: Added dialog for mode selection
	
	//Set the default application mode
	ModeManager::SetMode(ModeManager::ROSETTA);
	
	//Start the mode selector dialog (Std Rosetta/Genomic Rosetta)
	CDialogRosettaMode modeDialog;
	modeDialog.DoModal();
	
	// Currently does nothing...
	ProcessLICFile();
	
	// Special configuration?
	ProcessCFGFile();
	
	//#ifdef _DEBUG
#if 0
	int no_structures = ObjectManager::GetNoStructures();
	int no_algorithms = ObjectManager::GetNoAlgorithms();
	
	String compstr = "Main file compiled " + String(__TIME__) + " " + String(__DATE__) + ".";
	String inststr = "Prototype installation count: " + String::Format(no_structures) + " structures, " + String::Format(no_algorithms) + " algorithms.";
	
	Message::Information("Debug version.\n\n" + compstr + "\n\n" + inststr);
#endif
	
	AfxEnableControlContainer();
	
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//	of your final executable, you should remove from the following
	//	the specific initialization routines you do not need.
	
#ifdef _AFXDLL
	Enable3dControls(); 		// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)
	
	// Register the application's document templates.  Document templates
	//	serve as the connection between documents, frame windows and views.
	
	m_TreeTemplate = new CMultiDocTemplate(
		IDR_TREEVIEWTYPE,
		RUNTIME_CLASS(CRosettaDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CViewProject));
	AddDocTemplate(m_TreeTemplate);
	
	m_DecisionTableTemplate = new CMultiDocTemplate(
		IDR_DECISIONTABLETYPE,
		RUNTIME_CLASS(CRosettaDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CViewDecisionTable));
	
	// AddDocTemplate(m_DecisionTableTemplate);
	
	m_ReductsTemplate = new CMultiDocTemplate(
		IDR_REDUCTSTYPE,
		RUNTIME_CLASS(CRosettaDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CViewReducts));
	
	// AddDocTemplate(m_ReductsTemplate);
	
	m_RulesTemplate = new CMultiDocTemplate(
		IDR_RULESTYPE,
		RUNTIME_CLASS(CRosettaDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CViewRules));
	
	// AddDocTemplate(m_RulesTemplate);
	
	m_PatternsTemplate = new CMultiDocTemplate(
		IDR_PATTERNSTYPE,
		RUNTIME_CLASS(CRosettaDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CViewPatterns));
	
	// AddDocTemplate(m_PatternsTemplate);
	
	m_BatchClassificationTemplate = new CMultiDocTemplate(
		IDR_CONFUSIONTYPE,
		RUNTIME_CLASS(CRosettaDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CViewBatchClassification));
	
	// AddDocTemplate(m_ConfusionTemplate);
	
	m_PartitionTemplate = new CMultiDocTemplate(
		IDR_PARTITIONTYPE,
		RUNTIME_CLASS(CRosettaDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CViewPartition));
	
	// AddDocTemplate(m_PartitionTemplate);
	
	m_ApproximationTemplate = new CMultiDocTemplate(
		IDR_APPROXIMATIONTYPE,
		RUNTIME_CLASS(CRosettaDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CViewApproximation));
	
	// AddDocTemplate(m_ApproximationTemplate);
	
	m_TextFileTemplate = new CMultiDocTemplate(
		IDR_TEXTFILETYPE,
		RUNTIME_CLASS(CRosettaDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CViewTextFile));
	
	// AddDocTemplate(m_TextFileTemplate);
	
	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	
	// Change the application icon and title if genomic Rosetta mode:
	if (ModeManager::IsMode(ModeManager::GENOMIC)) {
		HICON genIcon = AfxGetApp()->LoadIcon(IDI_GENOMIC);
		pMainFrame->SetWindowText("Genomic Rosetta");
		pMainFrame->SetIcon(genIcon,FALSE);
	}
	
	m_pMainWnd = pMainFrame;
	
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	
	// Aleksander: DON'T display a new MDI child window during startup!!!
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	
	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	
	// TODO: Delay when showing the splash-window at startup
	// If you do not want to wait the extra amount of time as specified in
	// the sleep function, delete the next line.
	//Sleep(5000);
	Sleep(1000);
	
	// TODO: Program initially maximized.
	// If you do not want the program to be maximized as default,
	// use the m_nCmdShow parameter in the ShowWindow function.
	
	// The main window has been initialized, so show and update it.
	//pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	
	pMainFrame->UpdateWindow();
	
	return TRUE;
}


int CRosettaApp::ExitInstance() 
{
	GXTerminate();
	
	ObjectManager::Uninstall(IDENTIFIER);
	//RA 0406: Remove mode settings:
	ModeManager::RemoveMode();
	//RA 0406: Remove nodes for genomic Rosetta
	ModeManager::RemoveNodes();
	
	IdHolder::Unregister();
	
	/*
	delete m_TreeTemplate;
	delete m_DecisionTableTemplate;
	delete m_ProgressTemplate;
	delete m_ReductsTemplate;
	delete m_RulesTemplate;
	delete m_PatternsTemplate;
	delete m_ConfusionTemplate;
	delete m_PartitionTemplate;
	delete m_ApproximationTemplate;
	delete m_TextFileTemplate;
	*/
	
	return CWinApp::ExitInstance();
	
}

//-------------------------------------------------------------------
// Method  ......: InstallStructures
// Author........: Aleksander šhrn
// Date..........: 
// Description...: Adds available structures to the object manager.
// Comments......: 
// Revisions.....:
//===================================================================

void CRosettaApp::InstallStructures()
{
	Installer::InstallStructures();
}	 

//-------------------------------------------------------------------
// Method  ......: InstallAlgorithms
// Author........: Aleksander šhrn
// Date..........: 
// Description...: Adds available algorithms to the object manager.
// Comments......: 
// Revisions.....:
//===================================================================

void CRosettaApp::InstallAlgorithms()
{
	Installer::InstallAlgorithms();
}

//-------------------------------------------------------------------
// Method  ......: ProcessCFGFile
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CRosettaApp::ProcessCFGFile() const
{
	Configuration::Kernel::Load("rosetta.cfg");
#if defined(_ROSGUI)
	Configuration::GUI::Load("rosetta.cfg");
#endif
}

//-------------------------------------------------------------------
// Method  ......: ProcessLICFile
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CRosettaApp::ProcessLICFile() const
{
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString m_Text;
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_Text = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_EDIT_ABOUT, m_Text);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CRosettaApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CRosettaApp commands


BOOL CAboutDlg::OnInitDialog()
{
	
	CDialog::OnInitDialog();
	
	// Version number.
	m_Text = CString("ROSETTA GUI version ") + Configuration::GUI::GetVersion().GetBuffer() + CString("\r\n");
	m_Text += "\r\n";
	
	m_Text += CString("ROSETTA kernel version ") + Configuration::Kernel::GetVersion().GetBuffer() + CString("\r\n");
	m_Text += "\r\n";
	
	// Build stamp.
#if defined(_DEBUG)
	m_Text += "Debug ";
#else
	m_Text += "Release ";
#endif
	m_Text += CString("build ") + Configuration::GetCompilationTime().GetBuffer() + " " + Configuration::GetCompilationDate().GetBuffer();
	m_Text += "\r\n";
#if defined(_GXDLL)
	m_Text += "GX dynamically linked, ";
#else
	m_Text += "GX statically linked, ";
#endif
#if defined(_AFXDLL)
	m_Text += "MFC dynamically linked.";
#else
	m_Text += "MFC statically linked.";
#endif
	m_Text += "\r\n\r\n";
	
	// Version numbers, internal technical stuff.
	m_Text += CString("STL version ") + Configuration::Kernel::GetSTLVersion().GetBuffer();
	m_Text += "\r\n";
	m_Text += CString("Objective Grid version ") + Configuration::GUI::GetGXVersion().GetBuffer();
	m_Text += "\r\n\r\n";
	
	// Copyright stuff.
	m_Text += "Copyright ¿ 1996-2004 Knowledge Systems Group, Dept. of Computer and Information Science, NTNU, Norway. 2005 Jan Komorowski";
	m_Text += "\r\n";
	
	m_Text += "\r\n";
	
	m_Text += "Check out the ROSETTA homepage at http://rosetta.lcb.uu.se/ for updated information.";
	m_Text += "\r\n\r\n";
	
	// Credits.
	m_Text += "Kernel architecture, GUI front-end and computational kernel designed and implemented at the "
		"Knowledge Systems Group, NTNU, Norway by Aleksander šhrn (aleks@idi.ntnu.no). "
		"Additional programming, MFC help and bug reporting from (in alphabetical order) Merete Hvalshagen, "
		"Tor-Kristian Jenssen, J—rn Erlend Nygjerd, Daniel Remmem, Knut Magne Risvik, Dyre Tjeldvoll, Ivan Uthus, "
		"Staal Vinterbo and Thomas ×gotnes.";
	m_Text += "\r\n\r\n";
	
	// Disclaimer.
	m_Text += "This software (including documentation) is provided \"as is\" without warranty of any kind. This "
		"software (or derivative works thereof) may not be sold, rented, leased or licensed to others without "
		"express written consent. Also, this software may not be reverse engineered, decompiled, translated or "
		"disassembled in any way.";
	m_Text += "\r\n\r\n";
	
	// Addresses.
	m_Text += "Formal contact information:";
	m_Text += "\r\n\r\n";
	
	m_Text += "Prof. Jan Komorowski\r\n";
	m_Text += "Head of the Linnaeus Centre of Bioinformatics, Uppsala University\r\n";
	m_Text += "BMC\r\n";
	m_Text += "Box 598\r\n";
	m_Text += "SE-715 24 Uppsala";
	
	m_Text += "\r\n\r\n";
	
	// Misc. debug stuff.
#if defined(_DEBUG)
	m_Text += "Installed structures:\r\n\r\n";
	
	int i;
	
	for (i = 0; i < ObjectManager::GetNoStructures(); i++) {
		m_Text += CString("  ") + IdHolder::GetClassname(ObjectManager::GetEnumeratedStructure(i)->GetId()).GetBuffer() +
			CString(" (")  + IdHolder::GetDescription(ObjectManager::GetEnumeratedStructure(i)->GetId()).GetBuffer() + CString(")\r\n");
	}
	
	m_Text += "\r\n";
	m_Text += "Installed algorithms:\r\n\r\n";
	
	for (i = 0; i < ObjectManager::GetNoAlgorithms(); i++) {
		m_Text += CString("  ") + IdHolder::GetClassname(ObjectManager::GetEnumeratedAlgorithm(i)->GetId()).GetBuffer()   +
			CString(" (")  + IdHolder::GetDescription(ObjectManager::GetEnumeratedAlgorithm(i)->GetId()).GetBuffer() + CString(")\r\n");
	}
#endif
	
	UpdateData(FALSE);
	
	return TRUE;
	
}

BOOL CRosettaApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following line was added by the Splash Screen component.
	CWndSplash::PreTranslateAppMessage(pMsg);
	
	return CWinApp::PreTranslateMessage(pMsg);
}

 
