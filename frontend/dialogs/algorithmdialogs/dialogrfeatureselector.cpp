// dialogttest.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogrfeatureselector.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogRFeatureSelector dialog

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........: Daniel Remmem
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 960731
//===================================================================
CDialogRFeatureSelector::CDialogRFeatureSelector(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogRFeatureSelector::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogRFeatureSelector)
	m_Attributes = 1;
	
	
	//}}AFX_DATA_INIT
}


void CDialogRFeatureSelector::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogRFeatureSelector)
	DDX_Control(pDX, IDC_CHECK_APPENDT, m_Append);
	
//	DDX_Control(pDX, IDC_EDIT_TTHRESHOLD, m_EditTThreshold);
	DDX_Control(pDX, IDC_EDIT_MAX_ATTR, m_EditMaxAttr);
	DDX_Control(pDX, IDC_EDITRPATH, m_EditRPath);
		DDX_Control(pDX, IDC_EDITTEMP, m_EditTempPath);
		DDX_Control(pDX, IDC_RBROWSE, m_RBrowse);
		DDX_Control(pDX, IDC_TEMPBROWSE, m_TempBrowse);
	DDX_Control(pDX, IDC_EDITRSCRIPTPATH, m_EditRScriptPath);
		DDX_Control(pDX, IDC_RSCRIPTBROWSE, m_RScriptBrowse);
	


	DDX_Text(pDX, IDC_EDIT_MAX_ATTR, m_Attributes);

	DDX_Text(pDX, IDC_EDITRPATH, m_RPath);
	DDX_Text(pDX, IDC_EDITTEMP, m_TempPath);
	// DDX_Text(pDX, IDC_EDITRSCRIPTPATH, m_RScriptPath);

	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogRFeatureSelector, CDialog)
	//{{AFX_MSG_MAP(CDialogRFeatureSelector)
ON_BN_CLICKED(IDC_RBROWSE, OnRBrowse)
ON_BN_CLICKED(IDC_RSCRIPTBROWSE, OnRScriptBrowse)
ON_BN_CLICKED(IDC_TEMPBROWSE, OnTempBrowse)
ON_CBN_SELCHANGE(IDC_EDITRSCRIPTPATH, OnSelchange)
	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogRFeatureSelector message handlers
//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Daniel Remmem
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 960731
//===================================================================

BOOL CDialogRFeatureSelector::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Check if the algorithm is the correct type.
	if (!m_Algorithm->IsA(RFEATURESELECTOR)) { 
		Message::Error("Algorithm was not t test.");
		CDialog::OnCancel();
		return FALSE;
  }

  
ListAllFiles ();
/*
	m_EditRScriptPath.AddString("Universe");
	m_EditRScriptPath.AddString("Upper approximation");
	m_EditRScriptPath.AddString("Lower approximation");
	m_EditRScriptPath.AddString("Boundary region");
	m_EditRScriptPath.AddString("Outside region");
*/
	m_EditRScriptPath.SetCurSel(0);

/*

	m_ComboClassifier.AddString(description.GetBuffer());
m_EditRScriptPath
	m_ComboClassifier.SetCurSel(m_Selected);



#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <iostream>
using namespace std;

void list_all_files ( const string& sStartDir, list<string>& lstFound) {

   cout << "checking " << sStartDir.c_str () << endl;

   DIR* pDir = opendir ( sStartDir.c_str ());

   if ( !pDir) return false;

   dirent* pEntry;

   while ( pEntry = readdir ( pDir)) {

       cout << "found " << pEntry->d_name << endl;

       if ( DT_DIR & pEntry->d_type && strcmp ( pEntry->d_name, ".") && strcmp ( pEntry->d_name, "..")) {

           string sSubDir = sStartDir + string ( "/") + string ( pEntry->d_name);

           list_all_files ( sSubDir, sFound)) {

       }

       string sFound = sStartDir + string ( "/") + string ( pEntry->d_name);

       lstFound.push_back ( sFound);
   }
}

and use it like

list<string> lstFiles;

list_all_files ( "/home/somedir", lstFiles);






*/





	char *libvar;
	CString libvar2;
   /* Get the value of the LIB environment variable. */


	HKEY hTheKey;
	HKEY hTheSubKey;
	LONG lResult = 0;
	char *szData;
	DWORD dwType, dwLen;
  szData = new char[101];
	dwLen = 100;
  
RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("Software\\R-core"), &hTheKey);

RegOpenKey(hTheKey, "R", &hTheSubKey);
	lResult = RegQueryValueEx(hTheSubKey, "InstallPath", NULL, &dwType, (unsigned char *)szData, &dwLen);

RegCloseKey(hTheSubKey);
RegCloseKey(hTheKey);



	libvar = szData;
	
	if (lResult != ERROR_SUCCESS)

	{ 	libvar = getenv( "RHOME" ); }

	  if( libvar != NULL )
		{
			libvar2 = CString(libvar) + "\\bin\\rterm.exe";
			m_RPath = libvar2; 
			m_EditRPath.SetWindowText(libvar2);

		}

	libvar = getenv( "TEMP" ); 

	  if( libvar != NULL )
		{
			// libvar2 = CString(libvar) + "\\bin\\rterm.exe";
			m_TempPath = libvar; 
			m_EditTempPath.SetWindowText(libvar);

		}



	// Cast to a b tester.
	m_alg = dynamic_cast(RFeatureSelector *, m_Algorithm.GetPointer());
	
	// Initialize checkboxes.
	m_Append.SetCheck(m_alg->AppendAsChildren());
	
	// Initialize editvalues.
	m_Attributes   = m_alg->GetAttributes();
	// m_TThreshold = m_alg->GetThreshold(); 


		


	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: Daniel Remmem
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 960731
//===================================================================
void CDialogRFeatureSelector::OnOK() 
{
	
	UpdateData(TRUE);

	if (!m_alg->SetAttributes(m_Attributes)) {
		Message::Error("Could not set random seed");
		return;
	}


	CString cscriptfilename;
		m_EditRScriptPath.GetWindowText(cscriptfilename);

		// 


		String scriptfilename ;
		scriptfilename=  cscriptfilename ;

		 TCHAR szPath[MAX_PATH];
GetModuleFileName(NULL, szPath, MAX_PATH);

String ourdirectory = szPath;

ourdirectory.Replace("rosetta.exe", "RFiles\\");
	
		if (scriptfilename.Contains('\\'))
		{
					scriptfilename=  cscriptfilename ;
		}
		else
		{
					scriptfilename = ourdirectory + scriptfilename;
		}


		Message::Warning("R Script filename :" + scriptfilename);


	if (!m_alg->SetFilename(scriptfilename ))
	 {
		Message::Error("Could not choose R-script path");
		return;
	}




		CString cfilename;
		m_EditRPath.GetWindowText(cfilename);

		// 
  String filename =  cfilename ;
	
	if (!m_alg->SetRPath(filename ))
	 {
		Message::Error("Could not choose Rterm Path");
		return;
	}


		CString cdirectory;
		m_EditTempPath.GetWindowText(cdirectory);

		
		String fdirectory = cdirectory;
		String f2directory;

		if (fdirectory.EndsWith('\\'))
		{
				f2directory = cdirectory;
		}
		else 
		{
					f2directory = cdirectory + "\\";
		}
	
	if (!m_alg->SetDirectory(f2directory))
	 {
		Message::Error("Could choose work directory");
		return;
	}



	

	if (!m_alg->AppendAsChildren(m_Append.GetCheck())) {
		Message::Error("Could not set the bool value AppendAsChildren");
		return;
	}

	CDialog::OnOK();
}



void CDialogRFeatureSelector::OnRBrowse() 
{
	CString lpszStringBuf;
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, "Executable(*.exe)|*.exe||",this);
	// CFileDialog fileDlg(TRUE, NULL, "*.*");
 m_EditRPath.GetWindowText(lpszStringBuf);
 fileDlg.m_ofn.lpstrInitialDir= lpszStringBuf;
	if (fileDlg.DoModal() == IDOK)
		m_RPath  = fileDlg.GetPathName();
	UpdateData(FALSE);

}

void CDialogRFeatureSelector::OnRScriptBrowse() 
{
	CString lpszStringBuf;
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, "R file(*.R)|*.R||",this);
	// CFileDialog fileDlg(TRUE, NULL, "*.*");
 m_EditRScriptPath.GetWindowText(lpszStringBuf);
 fileDlg.m_ofn.lpstrInitialDir= lpszStringBuf;
	if (fileDlg.DoModal() == IDOK)
		m_EditRScriptPath.AddString(fileDlg.GetPathName());
	UpdateData(FALSE);

}

void CDialogRFeatureSelector::OnTempBrowse() 
{
	 LPMALLOC pMalloc;
    /* Gets the Shell's default allocator */
    if (::SHGetMalloc(&pMalloc) == NOERROR)
    {
        BROWSEINFO bi;
        char pszBuffer[MAX_PATH];
        LPITEMIDLIST pidl;
        // Get help on BROWSEINFO struct - it's got all the bit settings.
        bi.hwndOwner = GetSafeHwnd();
         bi.pidlRoot = NULL;
				


        bi.pszDisplayName = pszBuffer;
        bi.lpszTitle = _T("Select a Temporary Files Directory");
        bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
        bi.lpfn = NULL;
        bi.lParam = 0;
        // This next call issues the dialog box.
        if ((pidl = ::SHBrowseForFolder(&bi)) != NULL)
        {
            if (::SHGetPathFromIDList(pidl, pszBuffer))
            { 
            // At this point pszBuffer contains the selected path */.
                //DoingSomethingUseful(pszBuffer);
								m_TempPath = pszBuffer;
								m_EditTempPath.SetWindowText(pszBuffer);
            }
            // Free the PIDL allocated by SHBrowseForFolder.
            pMalloc->Free(pidl);
        }
        // Release the shell's allocator.
        pMalloc->Release();
    }
 

	/*
	CString lpszStringBuf;
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, "Executable(*.exe)|*.exe||",this);
	// CFileDialog fileDlg(TRUE, NULL, "*.*");
 m_EditTempPath.GetWindowText(lpszStringBuf);
 fileDlg.m_ofn.lpstrInitialDir= lpszStringBuf;
	if (fileDlg.DoModal() == IDOK)
		m_TempPath  = fileDlg.GetPathName();
*/
	UpdateData(FALSE);
 
}


void CDialogRFeatureSelector::ListAllFiles () {

//   cout << "checking " << sStartDir.c_str () << endl;
	Message message; 
	
	WIN32_FIND_DATA FindFileData;
   HANDLE hFind = INVALID_HANDLE_VALUE;
   char DirSpec[MAX_PATH] = "Rfiles\\*.r";  // directory specification
   DWORD dwError;

   hFind = FindFirstFile(DirSpec, &FindFileData);

   if (hFind == INVALID_HANDLE_VALUE) 
   {
		 message.Warning("Unable to find default R-files.");
      // return (-1);
   } 
   else 
   {
		  			m_EditRScriptPath.AddString(FindFileData.cFileName);
// 	m_RScriptPath  = FindFileData.cFileName;
	

		//	message.Warning(FindFileData.cFileName);
      // printf ("First file name is %s\n", FindFileData.cFileName);
    
			
			while (FindNextFile(hFind, &FindFileData) != 0) 
      {
         // printf ("Next file name is %s\n", FindFileData.cFileName);
				 m_EditRScriptPath.AddString(FindFileData.cFileName);
      }
    
      dwError = GetLastError();
      if (dwError == ERROR_NO_MORE_FILES) 
      {
         FindClose(hFind);
      } 
      else 
      {
         printf ("FindNextFile error. Error is %u\n", dwError);
        // return (-1);
      }
			UpdateData(FALSE);
   }

}

void CDialogRFeatureSelector::OnSelchange() 
{
/*
	UpdateData(TRUE);

	// Determine which DT is selected.
	int selected = m_EditRScriptPath.GetCurSel();
	if (selected == CB_ERR) {
		Message::Debug("m_EditRScriptPath::GetCurSel() returned CB_ERR");
		return;
	}

	// m_SelectedDTRG = selected;
	*/
} 
